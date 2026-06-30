# Proj57 初赛演示逐页讲稿

建议总时长：5 到 7 分钟。录制时只展示 `proj57_task2_demo_slides.pdf`，不需要现场切终端。

## 第 1 页：标题页

各位老师好，我们汇报的是 OS 功能挑战赛道 Proj57，题目是“面向具身智能的国产操作系统 ACT 模型适配与实时推理优化”。

我们的团队成员包括李捷铭，队长，来自清华大学，以及朱屹帆和杨若朝。

这次汇报的核心结论是：我们已经完成了从 PyTorch checkpoint 到 ONNX，再到 RKNN 模型，最后在 Orange Pi RK3588 板端通过 C++ RKNN runtime 输出动作方向的最小闭环。也就是说，这不是只停留在方案设计，而是已经有板端真实运行日志和性能数据。

## 第 2 页：本次展示要证明什么

Proj57 的目标是把统一的 ACT 动作预测模型部署到 StarryOS/QEMU 和嵌入式平台上，完成推理并输出动作预测结果。

任务三是 QEMU 和 StarryOS CPU 路线，是初赛保底链路。任务二是 Orange Pi RK3588 和 NPU 路线，是我们当前重点推进的硬件适配主线。

这一版展示主要证明一件事：任务二的 RK3588 最小推理闭环已经实际跑通。我们会展示板端系统信息、NPU 和 RKNN runtime 环境、RKNN 模型运行结果，以及和 ONNX 基线一致的 LEFT/RIGHT 输出。

## 第 3 页：我们实际完成的链路

这页是整体技术链路。

首先，我们使用赛题提供的 ACT 模型权重 `model.pt`。然后把 PyTorch checkpoint 导出为 ONNX，得到 `act.onnx`。接着在 WSL 的 Linux 环境中使用 `rknn-toolkit2` 把 ONNX 转成 RK3588 可用的 RKNN 模型，文件名是 `act_rk3588_fp.rknn`。

板端运行部分不是 Python，而是 C++ 程序调用 RKNN runtime，也就是 `/usr/lib/librknnrt.so`。模型输出是未来 8 步 action，每一步包含 `left_vel`、`right_vel` 和 `gripper_target`。我们取第一步动作，用左轮速度和右轮速度比较来判断方向。左轮速度小于右轮速度时判断为 LEFT，反过来判断为 RIGHT。

## 第 4 页：板端证据 1，Orange Pi 5 Plus 实物与系统

这页展示的是我们实际使用的板子和板端系统信息。

左边是 Orange Pi 5 Plus 开发板实物照片。右边是从板子上通过 SSH 采集的终端输出。可以看到，`/proc/device-tree/model` 显示板型是 Orange Pi 5 Plus。系统是 Ubuntu 22.04.5 LTS，架构是 aarch64，内核是 Rockchip 5.10 系列。

内存这里显示总量约 3.8 GiB，这和任务二 RK3588 4G 平台的资源条件匹配。这个环境足够支撑我们做 ACT 模型在 RK3588 上的 RKNN 推理验证。

## 第 5 页：板端证据 2，NPU 节点与 RKNN Runtime

这页展示的是 RK3588 的 NPU 和 RKNN runtime 环境。

终端输出里可以看到 `/dev/dri/by-path` 下有 `platform-fdab0000.npu-card` 和 `platform-fdab0000.npu-render`，说明系统里能看到 NPU 对应的 DRM 节点。

同时，板子上存在 `/usr/lib/librknnrt.so`，并且 `/usr/local/include/rknn` 目录下有 `rknn_api.h` 和 `rknn_matmul_api.h`。这说明板端具备编译和运行 RKNN C/C++ 程序的基本条件。

下面的 `RKNPU: invalid rknpu task number` 是运行过程中观察到的 runtime/driver 版本相关 warning。我们后面会解释，它目前没有阻止 `rknn_run` 返回输出和性能数据，但属于后续需要对齐版本解决的问题。

## 第 6 页：ONNX 基线，先证明模型本身输出正确

在做 RKNN 前，我们先做 ONNX 基线验证。原因是 RKNN 模型不要求和 ONNX 做 bit-level 完全一致，但方向判断必须和 ONNX 基线一致。

我们已经用 ONNX Runtime 跑过两个代表样例。`frame_000000.jpg` 输出 LEFT，符合预期；`frame_000227.jpg` 输出 RIGHT，也符合预期。

本机 Windows 上，ONNX Runtime CPU 两个样例的平均推理时间大约是 25.91 到 26.78 毫秒。队伍已有的 QEMU/StarryOS 报告结果中，单次推理约 18.4 到 18.7 秒，峰值内存约 454 MB。

所以 ONNX 基线在这里有两个作用：第一，它证明模型和预处理、后处理逻辑是正确的；第二，它作为 RKNN 板端输出 LEFT/RIGHT 的对照标准。

## 第 7 页：RKNN 转换结果

这页展示 RKNN 转换的环境和模型产物。

因为 Windows Python 环境不能直接稳定安装 `rknn-toolkit2`，所以我们切换到了 WSL Ubuntu 24.04 的 x86_64 Linux 环境。转换环境里使用的是 `rknn-toolkit2 2.3.2`，`torch 2.4.0+cpu`，以及 `onnx 1.16.2`。

模型大小方面，原始 PyTorch 权重 `model.pt` 是 202,962,639 字节；导出的 ONNX 模型 `act.onnx` 是 202,633,726 字节；转换后的 RKNN 模型 `act_rk3588_fp.rknn` 是 101,709,294 字节，大约 97 MiB。

这部分对应的工程文件是 `tools/convert_act_rknn.py`，板端 C++ 推理程序是 `tools/rknn_runtime_infer.cpp`。

## 第 8 页：RKNN 板端运行结果，两个代表样例

这页是任务二最关键的证据。

上半部分是 `frame_000000` 的板端 RKNN 运行结果。可以看到 RKNN runtime 的 `api_version` 是 1.5.2，驱动版本 `drv_version` 是 0.9.6。`rknn_perf_run_us` 是 34968 微秒，墙钟时间 `wall_ms` 是 34.998 毫秒。第一步动作反归一化后，left 是 -0.001526，right 是 0.007373，所以方向判断为 LEFT。

下半部分是 `frame_000227`。同样是在板端日志里得到的结果，`rknn_perf_run_us` 是 25340 微秒，`wall_ms` 是 25.364 毫秒。第一步动作反归一化后，left 是 0.006128，right 是 -0.000488，所以方向判断为 RIGHT。

这两个方向和 ONNX 基线一致。因此我们认为任务二的 RK3588 RKNN 最小推理闭环已经实际跑通。

## 第 9 页：性能与资源数据

这页把三条链路放在一起对比。

第一行是 QEMU/StarryOS 的 ONNX CPU 路线，已有报告中单次推理约 18.4 到 18.7 秒。它的意义是任务三保底和 StarryOS 方向的基础验证。

第二行是 Windows 本机 ONNX Runtime CPU，我们本地复现了两个样例，耗时大约 25.91 到 26.78 毫秒。

第三行是 Orange Pi RK3588 的 RKNN/NPU 路线，也就是任务二当前结果。RKNN 模型大小约 97 MiB，两个样例板端耗时约 25.34 到 35.00 毫秒，LEFT/RIGHT 方向正确。

这里要强调的是：这不是最终性能优化结果，而是初赛阶段可以展示的真实板端运行数据。

## 第 10 页：任务二现在到底完成到什么程度

这页专门澄清我们完成到什么程度，避免过度表述。

左边是可以确认完成的部分：ONNX 模型导出已经完成；RKNN 模型转换已经完成；RK3588 板端 C++ runtime 已经跑通；两个代表样例方向正确；模型大小和推理耗时已经记录。

右边是仍需完善的部分。当前 C++ runtime 的输入是预处理后的 tensor bin，还不是直接从 JPEG 图片开始做完整预处理。板端 runtime 仍然有版本 warning。验证样例目前是两个代表样例，后续需要扩展更多样例和稳定性测试。

所以我们最稳妥的表述是：任务二 RK3588 平台 ACT 推理最小闭环已经跑通，后续进入工程化和稳定性优化阶段。

## 第 11 页：本次新增工程产物

这页展示我们本次冲刺新增和整理的工程产物。

第一，我们修复了 ONNX 导出流程。原代码会尝试下载 torchvision 的 ResNet18 预训练权重，但比赛 checkpoint 会覆盖模型权重，所以这里改成不下载预训练权重，避免网络依赖。导出脚本也补了自动创建 `build/` 目录。

第二，我们新增了 RKNN 转换脚本 `tools/convert_act_rknn.py`。

第三，我们新增了板端 C++ RKNN runtime 程序 `tools/rknn_runtime_infer.cpp`，它负责加载 RKNN 模型，设置三个输入，调用 `rknn_run`，取输出并做动作反归一化和方向判断。

第四，我们采集并整理了板端证据，包括系统信息、NPU 节点、RKNN runtime、运行日志、性能表和演示材料。板端同步目录是 `/home/ubuntu/proj57_initial_review`。

## 第 12 页：下一步计划

下一步主要有四件事。

第一，对齐 RKNN Toolkit2 和板端 RKNN runtime 版本，消除目前的 `invalid run task counter` 警告。

第二，把 C++ runtime 扩展成完整命令行工具，支持直接读取 JPEG 图片，完成 resize、normalize、推理和动作后处理。这样就能从原始图像直接得到方向输出。

第三，增加更多 LEFT/RIGHT 样例回归，记录精度漂移、耗时波动和内存占用，而不是只看两个代表样例。

第四，面向任务一 SG2002 的 256 MB 内存限制，继续探索模型裁剪、量化和算子拆分。任务一的资源约束更强，需要进一步压缩模型和运行时开销。

## 第 13 页：总结

最后总结一下。

我们已经完成了从 `model.pt` 到 `act.onnx`，再到 `act_rk3588_fp.rknn`，最后在 Orange Pi RK3588 上通过 C++ RKNN runtime 输出动作方向的最小闭环。

任务三结果提供了保底正确性和 ONNX 对照。任务二已经有真实板端运行日志和性能数据，两个代表样例分别输出 LEFT 和 RIGHT，并且方向与 ONNX 基线一致。

当前主要风险也已经明确：一是 runtime 版本 warning，二是输入还不是端到端 JPEG，三是样例数量还需要扩展。后续我们会围绕这些问题继续推进工程化和稳定性优化。

我的汇报到这里，谢谢各位老师。

## 录制提醒

- 只展示 PDF 即可，不要现场切终端，避免网络或板子状态影响录制。
- 第 8 页是任务二最关键证据，建议多停留 20 秒。
- 第 10 页要主动说明边界，不要说“任务二完全工程化结束”。
- 如果被问“是不是只说能跑”，回答：不是，已经在 Orange Pi RK3588 上跑出 `LEFT/RIGHT`，但当前是最小闭环，还在补端到端 JPEG 和版本对齐。
