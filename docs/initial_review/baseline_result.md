# Baseline / ONNX 当前结果

## 已确认

- 仓库已有 Task3 ONNX/CPU 路线代码。
- `tools/export_act_onnx.py` 可将 `output/train/model.pt` 导出为 `build/act.onnx`。
- `tools/check_onnx.py` 可用 ONNX Runtime 校验两个代表性样例。
- `src/task3_main.cpp` 可读取预处理后的 tensor bin 并输出 8 步 action。

## 下载状态

HuggingFace 直链可达：

- `model.pt`: 约 202,962,639 bytes
- `stats.json`: 651 bytes
- `frame_000000.jpg`: 1,864 bytes
- `frame_000227.jpg`: 1,886 bytes

当前已成功下载：

- `output/dataset/meta/stats.json`
- `output/dataset/videos/observation.images.fpv/chunk-000/frame_000000.jpg`
- `output/dataset/videos/observation.images.fpv/chunk-000/frame_000227.jpg`
- `output/train/model.pt`

模型文件大小：202,962,639 bytes。

## ONNX 导出状态

已成功生成：

```text
build/act.onnx
```

ONNX 文件大小：202,633,726 bytes，约 193.25 MB。

导出日志：

```text
artifacts/onnx/export_log.txt
```

## ONNX Runtime 校验状态

已成功运行：

```text
python tools/check_onnx.py
```

结果：

```text
frame_000000.jpg -> LEFT   expected: LEFT
frame_000227.jpg -> RIGHT  expected: RIGHT
ONNX Runtime Python 验证通过
```

完整输出：

```text
artifacts/onnx/check_output.txt
```

## 本地 Benchmark

本地 Windows ONNX Runtime CPU，5 次运行平均：

```text
frame_000000: avg_ms=26.778, direction=LEFT
frame_000227: avg_ms=25.909, direction=RIGHT
```

产物：

```text
artifacts/baseline/local_infer_output.txt
artifacts/baseline/local_benchmark.json
```

## 基线结论

ONNX baseline 作为 RKNN 结果的正确性对照和任务三保底证据：

- ONNX 模型大小约 193.2 MB。
- QEMU/StarryOS 单次推理约 18.4 到 18.7 秒。
- 峰值内存约 454 MB。
- `frame_000000 -> LEFT`，`frame_000227 -> RIGHT`。

当前 RKNN 板端已跑通两个代表样例，但仍保留 ONNX baseline 用于解释模型链路、对照方向正确性，并在 runtime 版本警告排查时作为回归基准。
