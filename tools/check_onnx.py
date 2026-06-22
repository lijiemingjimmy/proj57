import json
from pathlib import Path

import numpy as np
import onnx
import onnxruntime as ort
import torch
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]

ONNX_PATH = ROOT / "build/act.onnx"
CKPT_PATH = ROOT / "output/train/model.pt"
STATS_PATH = ROOT / "output/dataset/meta/stats.json"

FRAME_0 = ROOT / "output/dataset/videos/observation.images.fpv/chunk-000/frame_000000.jpg"
FRAME_227 = ROOT / "output/dataset/videos/observation.images.fpv/chunk-000/frame_000227.jpg"


def preprocess_image(path: Path) -> np.ndarray:
    img = Image.open(path).convert("RGB")
    img = img.resize((224, 224), Image.Resampling.BILINEAR)

    arr = np.asarray(img).astype(np.float32) / 255.0
    arr = arr.transpose(2, 0, 1)  # HWC -> CHW

    mean = np.array([0.485, 0.456, 0.406], dtype=np.float32).reshape(3, 1, 1)
    std = np.array([0.229, 0.224, 0.225], dtype=np.float32).reshape(3, 1, 1)

    arr = (arr - mean) / std
    return arr[None, :, :, :].astype(np.float32)  # [1, 3, 224, 224]


def normalize_state(state, stats):
    q01 = np.array(stats["observation.state"]["q01"], dtype=np.float32)
    q99 = np.array(stats["observation.state"]["q99"], dtype=np.float32)

    state = np.array(state, dtype=np.float32)
    denom = q99 - q01
    denom = np.where(denom == 0, 1e-8, denom)

    state_norm = 2 * (state - q01) / denom - 1
    return state_norm[None, :].astype(np.float32)


def denormalize_action(action, stats):
    q01 = np.array(stats["action"]["q01"], dtype=np.float32)
    q99 = np.array(stats["action"]["q99"], dtype=np.float32)

    denom = q99 - q01
    denom = np.where(denom == 0, 1e-8, denom)

    return (action + 1) / 2 * denom + q01


def direction_from_lr(left, right):
    if left < right:
        return "LEFT"
    elif left > right:
        return "RIGHT"
    else:
        return "STRAIGHT"


def run_one(sess, image_path, stats, latent):
    image = preprocess_image(image_path)
    state = normalize_state([0.0, 0.0], stats)

    output = sess.run(
        ["action"],
        {
            "image": image,
            "state": state,
            "latent": latent,
        },
    )[0]

    # output shape: [1, 8, 3]
    action_chunk = output[0]
    action_chunk_denorm = denormalize_action(action_chunk, stats)

    first = action_chunk_denorm[0]
    left, right, grip = first.tolist()
    direction = direction_from_lr(left, right)

    print("=" * 60)
    print(f"image: {image_path.name}")
    print(f"first step:")
    print(f"  left_vel  = {left:+.6f}")
    print(f"  right_vel = {right:+.6f}")
    print(f"  gripper   = {grip:+.6f}")
    print(f"  direction = {direction}")

    print("full 8-step action chunk:")
    for i, a in enumerate(action_chunk_denorm):
        l, r, g = a.tolist()
        d = direction_from_lr(l, r)
        print(f"  step {i}: left={l:+.6f}, right={r:+.6f}, grip={g:+.6f}, dir={d}")

    return direction


def main():
    if not ONNX_PATH.exists():
        raise FileNotFoundError(f"ONNX 不存在: {ONNX_PATH}")

    print(f"检查 ONNX 文件: {ONNX_PATH}")
    onnx_model = onnx.load(str(ONNX_PATH))
    onnx.checker.check_model(onnx_model)
    print("ONNX checker: OK")

    print("创建 ONNX Runtime session...")
    sess = ort.InferenceSession(
        str(ONNX_PATH),
        providers=["CPUExecutionProvider"],
    )

    print("ONNX inputs:")
    for i in sess.get_inputs():
        print(f"  {i.name}: shape={i.shape}, type={i.type}")

    print("ONNX outputs:")
    for o in sess.get_outputs():
        print(f"  {o.name}: shape={o.shape}, type={o.type}")

    with open(STATS_PATH, "r") as f:
        stats = json.load(f)

    ckpt = torch.load(CKPT_PATH, map_location="cpu", weights_only=False)
    if "inference_latent_mu" in ckpt:
        latent = ckpt["inference_latent_mu"].reshape(1, -1).numpy().astype(np.float32)
        print("使用 checkpoint 里的 inference_latent_mu")
    else:
        latent = np.zeros((1, 32), dtype=np.float32)
        print("没有 inference_latent_mu，使用全零 latent")

    d0 = run_one(sess, FRAME_0, stats, latent)
    d227 = run_one(sess, FRAME_227, stats, latent)

    print("=" * 60)
    print("summary:")
    print(f"  frame_000000.jpg -> {d0}   expected: LEFT")
    print(f"  frame_000227.jpg -> {d227} expected: RIGHT")

    if d0 == "LEFT" and d227 == "RIGHT":
        print("ONNX Runtime Python 验证通过")
    else:
        print("方向和预期不完全一致，需要继续排查")


if __name__ == "__main__":
    main()
