import argparse
import json
from pathlib import Path

import numpy as np
import torch
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]

CKPT_PATH = ROOT / "output/train/model.pt"
STATS_PATH = ROOT / "output/dataset/meta/stats.json"
VIDEO_DIR = ROOT / "output/dataset/videos/observation.images.fpv/chunk-000"

OUT_DIR = ROOT / "build/cpp_test"
OUT_DIR.mkdir(parents=True, exist_ok=True)


def preprocess_image(path: Path) -> np.ndarray:
    img = Image.open(path).convert("RGB")
    img = img.resize((224, 224), Image.Resampling.BILINEAR)

    arr = np.asarray(img).astype(np.float32) / 255.0
    arr = arr.transpose(2, 0, 1)

    mean = np.array([0.485, 0.456, 0.406], dtype=np.float32).reshape(3, 1, 1)
    std = np.array([0.229, 0.224, 0.225], dtype=np.float32).reshape(3, 1, 1)

    arr = (arr - mean) / std
    return arr[None, :, :, :].astype(np.float32)


def normalize_state(state, stats):
    q01 = np.array(stats["observation.state"]["q01"], dtype=np.float32)
    q99 = np.array(stats["observation.state"]["q99"], dtype=np.float32)

    state = np.array(state, dtype=np.float32)
    denom = q99 - q01
    denom = np.where(denom == 0, 1e-8, denom)

    state_norm = 2 * (state - q01) / denom - 1
    return state_norm[None, :].astype(np.float32)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--frame", type=int, required=True, help="frame index, e.g. 0 or 227")
    args = parser.parse_args()

    frame_name = f"frame_{args.frame:06d}.jpg"
    frame_path = VIDEO_DIR / frame_name

    if not frame_path.exists():
        raise FileNotFoundError(f"frame 不存在: {frame_path}")

    with open(STATS_PATH, "r") as f:
        stats = json.load(f)

    ckpt = torch.load(CKPT_PATH, map_location="cpu", weights_only=False)

    if "inference_latent_mu" in ckpt:
        latent = ckpt["inference_latent_mu"].reshape(1, -1).numpy().astype(np.float32)
    else:
        latent = np.zeros((1, 32), dtype=np.float32)

    image = preprocess_image(frame_path)
    state = normalize_state([0.0, 0.0], stats)

    action_q01 = np.array(stats["action"]["q01"], dtype=np.float32)
    action_q99 = np.array(stats["action"]["q99"], dtype=np.float32)

    image.tofile(OUT_DIR / "image.bin")
    state.tofile(OUT_DIR / "state.bin")
    latent.tofile(OUT_DIR / "latent.bin")
    action_q01.tofile(OUT_DIR / "action_q01.bin")
    action_q99.tofile(OUT_DIR / "action_q99.bin")

    print(f"frame: {frame_path}")
    print(f"dumped to: {OUT_DIR}")
    print(f"image shape: {image.shape}")
    print(f"state shape: {state.shape}")
    print(f"latent shape: {latent.shape}")


if __name__ == "__main__":
    main()
