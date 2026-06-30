# Proj57 仓库地图

## 仓库来源

- 主仓库：`https://github.com/chenlongos/proj57`
- 队伍仓库：`https://github.com/lijiemingjimmy/proj57`
- 本地队伍仓库：`C:\Users\Laptop\Pictures\main\team-proj57`

## 关键目录

- `act/`: ACT PyTorch 模型定义、配置和数据集代码。
- `tools/`: ONNX 导出、ONNX 校验、C++ 输入 dump 工具。
- `src/`: C++ ONNX Runtime 推理程序。
- `docs/`: 任务三结果文档和日志。
- `docs/initial_review/`: 本次初审冲刺新增材料。
- `artifacts/`: 本次冲刺生成的日志和实验产物。

## 当前新增文件

- `tools/export_act_onnx.py`: PyTorch checkpoint 导出 ONNX。
- `tools/check_onnx.py`: ONNX Runtime Python 校验。
- `tools/dump_cpp_inputs.py`: 生成 `image.bin/state.bin/latent.bin/action_q*.bin`。
- `src/task3_main.cpp`: C++ ONNX Runtime 推理程序。
- `Report_Task3.md`: 中文任务三报告。
- `docs/task3_results.md`: 英文任务三结果文档。
- `docs/logs/starryos_task3_outputs.txt`: StarryOS/QEMU 简短输出日志。

## 必需但仓库默认缺失的大文件

- `output/train/model.pt`
- `output/dataset/meta/stats.json`
- `output/dataset/videos/observation.images.fpv/chunk-000/frame_000000.jpg`
- `output/dataset/videos/observation.images.fpv/chunk-000/frame_000227.jpg`
- `build/act.onnx`
- `build/cpp_test/*.bin`

## 数据来源

- 模型：HuggingFace `bobodai/proj57_model`
- 数据集：HuggingFace dataset `bobodai/proj57_dataset`

## 当前可复现路径

1. 下载 `model.pt`、`stats.json` 和代表性图像帧。
2. 运行 `python tools/export_act_onnx.py` 生成 `build/act.onnx`。
3. 运行 `python tools/check_onnx.py` 验证 `frame_000000` 与 `frame_000227`。
4. 运行 `python tools/dump_cpp_inputs.py --frame 0/227` 生成 C++ 输入。
5. 编译并运行 `src/task3_main.cpp`。

## RKNN 当前状态

本次冲刺已补齐任务二最小闭环：

- `tools/convert_act_rknn.py`: ONNX -> RKNN 转换脚本。
- `tools/rknn_runtime_infer.cpp`: RK3588 板端 C++ RKNN runtime 程序。
- `build/act_rk3588_fp.rknn`: 已生成 RK3588 FP 模型，约 97.0 MiB。
- `artifacts/rknn/runtime_log_frame000_nhwc.txt`: `frame_000000 -> LEFT`。
- `artifacts/rknn/runtime_log_frame227_nhwc.txt`: `frame_000227 -> RIGHT`。

当前剩余工程边界：

- C++ runtime 读取预处理后的 tensor bin，尚未直接处理 JPEG。
- 板端 RKNN runtime API 1.5.2 与转换工具 2.3.2 不完全匹配，运行时会打印版本警告。
- 还需要更多样例回归和更完整的命令行封装。
