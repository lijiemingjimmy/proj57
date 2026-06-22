import sys
from pathlib import Path

import torch
import torch.nn as nn

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from act.configuration_act import ACTConfig
from act.modeling_act import ACTModel


class ACTONNXWrapper(nn.Module):
    """
    把原来的 ACTModel 包一层，改成更适合 ONNX 导出的形式。

    原始模型 forward:
        images, state, action_target=None, infer_cvae=True

    我们导出的 ONNX 模型:
        image, state, latent -> action

    这样做的原因：
        1. ONNX 推理时不应该有随机 latent 采样
        2. C++ 里显式传入 latent 更稳定
    """

    def __init__(self, model: ACTModel):
        super().__init__()
        self.m = model

    def forward(self, image, state, latent):
        # image:  [1, 3, 224, 224]
        # state:  [1, 2]
        # latent: [1, 32]

        batch_size = image.shape[0]

        # 1. 图像编码
        vision_features = self.m.vision_encoder(image)

        # 2. 状态编码
        state_features = self.m.state_encoder(state)

        # 3. latent 编码
        latent_features = self.m.latent_proj(latent).unsqueeze(1)

        # 4. 拼接 encoder 输入
        encoder_in = torch.cat(
            [latent_features, state_features, vision_features],
            dim=1,
        )

        # 5. 位置编码
        seq_len = encoder_in.shape[1]
        pos_embed = self.m.encoder_pos_embed.weight[:seq_len].unsqueeze(0)

        # 6. Transformer Encoder
        encoder_out = self.m.encoder(encoder_in, pos_embed=pos_embed)

        # 7. Transformer Decoder
        decoder_pos_embed = self.m.decoder_pos_embed.weight.unsqueeze(0).expand(
            batch_size, -1, -1
        )

        decoder_in = torch.zeros(
            batch_size,
            self.m.config.action_chunk_size,
            self.m.config.hidden_dim,
            device=image.device,
        ) + decoder_pos_embed

        decoder_out = self.m.decoder(
            decoder_in,
            encoder_out,
            decoder_pos_embed=decoder_pos_embed,
            encoder_pos_embed=pos_embed,
        )

        # 8. 输出动作
        action = self.m.action_head(decoder_out)

        return action


def main():
    ckpt_path = ROOT / "output/train/model.pt"
    out_path = ROOT / "build/act.onnx"

    if not ckpt_path.exists():
        raise FileNotFoundError(f"模型不存在: {ckpt_path}")

    print(f"加载 PyTorch checkpoint: {ckpt_path}")
    ckpt = torch.load(ckpt_path, map_location="cpu", weights_only=False)

    config_dict = ckpt.get("config", {})
    print("模型 config:")
    for k, v in config_dict.items():
        print(f"  {k}: {v}")

    config = ACTConfig(**config_dict)

    model = ACTModel(config)
    model.load_state_dict(ckpt["model_state_dict"])
    model.eval()

    wrapper = ACTONNXWrapper(model).eval()

    image = torch.randn(1, 3, 224, 224, dtype=torch.float32)
    state = torch.zeros(1, config.state_dim, dtype=torch.float32)

    if "inference_latent_mu" in ckpt:
        latent = ckpt["inference_latent_mu"].reshape(1, -1).float()
        print("使用 checkpoint 里的 inference_latent_mu 作为导出 latent")
    else:
        latent = torch.zeros(1, config.latent_dim, dtype=torch.float32)
        print("checkpoint 里没有 inference_latent_mu，使用全零 latent")

    print("开始导出 ONNX...")
    print(f"image shape:  {tuple(image.shape)}")
    print(f"state shape:  {tuple(state.shape)}")
    print(f"latent shape: {tuple(latent.shape)}")

    torch.onnx.export(
        wrapper,
        (image, state, latent),
        str(out_path),
        input_names=["image", "state", "latent"],
        output_names=["action"],
        opset_version=17,
        do_constant_folding=True,
        dynamo=False,
    )

    print(f"ONNX 导出完成: {out_path}")
    print(f"文件大小: {out_path.stat().st_size / 1024 / 1024:.2f} MB")


if __name__ == "__main__":
    main()
