"""
Convert ACT ONNX model to RKNN for RK3588.

This script is intended for an x86_64 Linux RKNN-Toolkit2 environment.
It is not expected to run on the Windows host unless a compatible
rknn-toolkit2 wheel is installed.
"""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
ONNX_PATH = ROOT / "build/act.onnx"
RKNN_PATH = ROOT / "build/act_rk3588_fp.rknn"


def main():
    try:
        from rknn.api import RKNN
    except Exception as exc:
        raise RuntimeError(
            "rknn-toolkit2 is not installed in this Python environment. "
            "Install Rockchip RKNN-Toolkit2 on x86_64 Linux, then rerun."
        ) from exc

    if not ONNX_PATH.exists():
        raise FileNotFoundError(f"ONNX model not found: {ONNX_PATH}")

    RKNN_PATH.parent.mkdir(parents=True, exist_ok=True)

    rknn = RKNN(verbose=True)
    try:
        ret = rknn.config(target_platform="rk3588")
        if ret != 0:
            raise RuntimeError(f"rknn.config failed: {ret}")

        ret = rknn.load_onnx(model=str(ONNX_PATH))
        if ret != 0:
            raise RuntimeError(f"rknn.load_onnx failed: {ret}")

        # First pass uses floating point to reduce quantization variables.
        ret = rknn.build(do_quantization=False)
        if ret != 0:
            raise RuntimeError(f"rknn.build failed: {ret}")

        ret = rknn.export_rknn(str(RKNN_PATH))
        if ret != 0:
            raise RuntimeError(f"rknn.export_rknn failed: {ret}")

        print(f"RKNN exported: {RKNN_PATH}")
    finally:
        rknn.release()


if __name__ == "__main__":
    main()
