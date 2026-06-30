# Proj57 - ACT 模型适配与实时推理优化

本仓库为 OS 功能挑战赛道 Proj57 队伍提交材料，目标是在 StarryOS/QEMU 与国产化嵌入式平台上完成 ACT 模型推理部署，并输出动作方向结果。

## 视频链接

演示/讲解视频：

https://cloud.tsinghua.edu.cn/d/e38ac87c52e843f08342/

## 提交材料

- 技术报告：[report.md](./report.md)
- 任务二演示 PDF：[docs/initial_review/proj57_task2_demo_slides.pdf](./docs/initial_review/proj57_task2_demo_slides.pdf)
- 演讲稿：[docs/initial_review/proj57_task2_demo_speech_script.md](./docs/initial_review/proj57_task2_demo_speech_script.md)
- RKNN 结果说明：[docs/initial_review/rknn_result.md](./docs/initial_review/rknn_result.md)
- 性能表：[docs/initial_review/performance_table.md](./docs/initial_review/performance_table.md)

## 当前完成情况

- Task3：已有 QEMU/StarryOS CPU 路线结果，两个代表样例方向正确。
- Task2：已完成 RK3588 / Orange Pi 5 Plus 上的 RKNN 最小推理闭环。
- ONNX 基线：`frame_000000 -> LEFT`，`frame_000227 -> RIGHT`。
- RKNN 板端：`frame_000000 -> LEFT`，`frame_000227 -> RIGHT`。

## 关键工程文件

- `tools/export_act_onnx.py`：导出 ACT ONNX 模型。
- `tools/convert_act_rknn.py`：将 ONNX 转换为 RK3588 RKNN 模型。
- `tools/rknn_runtime_infer.cpp`：Orange Pi RK3588 板端 C++ RKNN runtime 推理程序。
- `docs/initial_review/`：初赛文档、PPT/PDF、演示脚本和证据截图。
- `artifacts/`：ONNX、RKNN、板端运行日志和性能结果。

## 重要说明

当前 Task2 是 RK3588 平台的 ACT 推理最小闭环：模型已转换为 RKNN，并在 Orange Pi RK3588 上通过 C++ `librknnrt` 实际运行，两个代表样例方向与 ONNX 基线一致。

仍需继续完善的部分包括：直接 JPEG 输入预处理、RKNN runtime 版本警告处理、更多样例回归和更完整的稳定性测试。
