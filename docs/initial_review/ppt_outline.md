# Proj57 初赛演示 PPT 大纲

## 第 1 页：项目目标与演示主线

- 题目：面向具身智能的国产操作系统 ACT 模型适配与实时推理优化。
- 目标：把 ACT 动作预测模型部署到 StarryOS/QEMU 与 RK3588 等平台，完成可复现推理。
- 演示主线：阶段总览、真实证据、性能数据、当前风险、下一步计划。

## 第 2 页：赛题任务与当前策略

- 任务三：QEMU/StarryOS 跑通 ACT 推理，是初赛保底链路。
- 任务二：RK3588/Orange Pi 5 Plus 跑通 ACT 推理，是当前硬件适配主线。
- 任务一：SG2002 256 MB 平台资源约束最强，作为后续高难优化方向。
- 当前策略：任务三保底，任务二 RKNN/NPU 已形成最小闭环，后续继续提升稳定性和工程化。

## 第 3 页：阶段总览

- 已完成：模型权重下载、ONNX 导出、ONNX Runtime 正确性验证、本地 benchmark。
- 已完成：Orange Pi 5 Plus 板端登录、系统信息采集、NPU 节点与 RKNN runtime 探测。
- 已完成：ONNX 到 RKNN 转换，生成 `build/act_rk3588_fp.rknn`。
- 已完成：C++ RKNN runtime 上板运行，两个代表样例方向正确。

## 第 4 页：模型与推理链路

- 输入：前视图像、机器人状态、latent 向量。
- 输出：未来 8 步动作，每步包含 `left_vel`、`right_vel`、`gripper_target`。
- ONNX 链路：`model.pt -> act.onnx -> ONNX Runtime -> action -> LEFT/RIGHT`。
- RKNN 链路：`act.onnx -> act_rk3588_fp.rknn -> librknnrt -> RK3588/NPU -> action -> LEFT/RIGHT`。

## 第 5 页：真实证据 1 - ONNX 正确性

- 已生成 ONNX 模型：`build/act.onnx`，约 193.25 MB。
- 校验命令：`python tools/check_onnx.py`。
- 样例 1：`frame_000000.jpg -> LEFT`，符合预期。
- 样例 2：`frame_000227.jpg -> RIGHT`，符合预期。
- 证据文件：`artifacts/onnx/check_output.txt`、`docs/initial_review/baseline_result.md`。

## 第 6 页：真实证据 2 - RK3588 板端环境

- 开发板：Orange Pi 5 Plus，RK3588，aarch64。
- 系统：Ubuntu 22.04.5 LTS，Rockchip 5.10 内核。
- 内存：约 3.8 GiB，可支撑任务二验证。
- 已发现 NPU DRM 节点和 RKNPU 内核初始化日志。
- 已确认存在 `/usr/lib/librknnrt.so` 与 RKNN 头文件。

## 第 7 页：真实证据 3 - RKNN/NPU 运行结果

- RKNN 模型：`build/act_rk3588_fp.rknn`，约 97.0 MiB。
- C++ runtime：`tools/rknn_runtime_infer.cpp`。
- `frame_000000`：LEFT，`rknn_perf_run_us=34968`。
- `frame_000227`：RIGHT，`rknn_perf_run_us=25340`。
- 证据文件：`artifacts/rknn/runtime_log_frame000_nhwc.txt`、`runtime_log_frame227_nhwc.txt`。

## 第 8 页：性能与资源数据表

| 平台 | 后端 | 状态 | 模型大小 | 推理耗时 | 结果 |
|---|---|---|---:|---:|---|
| StarryOS/QEMU riscv64 | ONNX Runtime CPU | 已有报告跑通 | 193.2 MB | 18.4-18.7 s | LEFT/RIGHT 正确 |
| Windows 本机 | ONNX Runtime CPU | 已复现 | 193.25 MB | 25.91-26.78 ms | LEFT/RIGHT 正确 |
| Orange Pi 5 Plus RK3588 | RKNN/NPU | 已跑通 | 97.0 MiB | 25.34-35.00 ms | LEFT/RIGHT 正确 |

## 第 9 页：当前工程产物

- 模型权重：`output/train/model.pt`，202,962,639 bytes。
- ONNX 模型：`build/act.onnx`，202,633,726 bytes。
- RKNN 模型：`build/act_rk3588_fp.rknn`，101,709,294 bytes。
- RKNN 转换脚本：`tools/convert_act_rknn.py`。
- RKNN runtime：`tools/rknn_runtime_infer.cpp`。
- 初审材料：技术文档、进度报告、演示脚本、性能表、PPT、PDF。

## 第 10 页：当前问题与风险

- 板端 RKNN runtime API 1.5.2 与 RKNN Toolkit2 2.3.2 不完全匹配。
- 运行时会打印 `invalid run task counter` 警告，但仍能返回输出和耗时。
- 当前 C++ runtime 使用预处理后的 tensor bin，尚未直接支持 JPEG 输入。
- 当前只验证两个代表样例，仍需更多样例和长时间稳定性测试。

## 第 11 页：初赛演示视频安排

- 开场：说明赛题目标、三档任务和当前冲刺路线。
- 硬件：展示 Orange Pi 5 Plus，并说明 RK3588/NPU 目标。
- 板端证据：展示系统信息、内存、NPU 节点、RKNN runtime。
- 模型证据：展示 ONNX 正确性输出、RKNN/NPU 输出和性能表。
- 结尾：说明 runtime 版本警告、后续升级计划和任务一方向。

## 第 12 页：总结页

- 任务三链路已有正确性和性能证据，可作为初赛保底。
- 任务二已完成 RK3588/RKNN/NPU 最小闭环。
- 当前重点从“能不能跑”转为“runtime 版本对齐、稳定性和工程化”。
- 初赛展示重点：真实证据清晰、数据表完整、问题边界明确、后续路径可执行。
