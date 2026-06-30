# Proj57 初审技术文档草稿

## 1. 赛题背景与项目目标

Proj57 是全国大学生计算机系统能力大赛 OS 功能挑战赛道题目，完整题名为“面向具身智能的国产操作系统 ACT 模型适配与实时推理优化”。赛题要求使用统一 ACT 模型，在 StarryOS、QEMU 和指定国产化嵌入式平台上完成推理部署，并记录正确性、资源占用、延迟和工程可复现性。

赛题任务分为三档：

- 任务一：在荔枝派 SG2002（256 MB）上跑通 ACT 推理，是最高难度资源受限目标。
- 任务二：在香橙派 RK3588（4 GB）上跑通 ACT 推理，是当前硬件 NPU 适配主线。
- 任务三：在 QEMU 中跑通 ACT 模型推理，是基础保底和可复现基线。

奖项路径可概括为：任务三对应基础达标；任务三加任务二对应进阶能力；任务一对应高难挑战；任务一和任务二都完成并具备良好工程实现与优化表现，则对应综合优秀。更完整的零基础说明见 `docs/initial_review/competition_context.md`。

Proj57 的目标是在 StarryOS 和嵌入式硬件平台上部署 ACT（Action Chunking Transformer）模型，完成图像与状态输入到动作输出的推理链路。ACT 模型输入前视图像和当前状态，输出未来 8 步动作，每步包含：

```text
left_vel, right_vel, gripper_target
```

评审重点是推理方向正确和工程链路可复现，不要求优化后数值 bit-exact 完全一致。

## 2. 当前任务二目标

任务二面向 RK3588 平台。本次实际硬件为 Orange Pi 5 Plus，芯片为 Rockchip RK3588。目标是在该平台上确认系统、NPU 驱动、RKNN runtime，并推进 ONNX -> RKNN -> NPU 推理。

## 3. 已完成工作总览

| 工作项 | 状态 | 证据 |
|---|---|---|
| SSH 登录 Orange Pi | 完成 | `docs/initial_review/board_inventory.md` |
| 板型和系统确认 | 完成 | Orange Pi 5 Plus / Ubuntu 22.04.5 / aarch64 |
| NPU 驱动探测 | 完成 | `docs/initial_review/board_npu_probe.md` |
| RKNN runtime 探测 | 完成 | `/usr/lib/librknnrt.so` 和 RKNN headers 存在 |
| 模型权重下载 | 完成 | `output/train/model.pt`，202,962,639 bytes |
| ONNX 导出 | 完成 | `build/act.onnx`，202,633,726 bytes |
| ONNX Runtime 校验 | 完成 | `artifacts/onnx/check_output.txt` |
| 本地 benchmark | 完成 | `artifacts/baseline/local_benchmark.json` |
| RKNN 转换 | 完成 | `build/act_rk3588_fp.rknn`，101,709,294 bytes |
| 比赛背景整理 | 完成 | `docs/initial_review/competition_context.md` |
| WSL RKNN 工具链 | 完成 | `rknn-toolkit2 2.3.2`，日志见 `artifacts/rknn/` |
| 板端 RKNN runtime | 完成 | `tools/rknn_runtime_infer.cpp`，LEFT/RIGHT 正确 |

## 4. 系统环境与硬件信息

Orange Pi 信息：

```text
IP: 192.168.0.104
Board: Orange Pi 5 Plus
OS: Ubuntu 22.04.5 LTS
Kernel: 5.10.0-1012-rockchip
Arch: aarch64
CPU: Cortex-A55 + Cortex-A76, 8 cores
Memory: 3.8 GiB
Root disk: /dev/mmcblk1p2, 15 GB total, 9.0 GB available
```

NPU/RKNN 环境：

```text
/dev/dri/by-path/platform-fdab0000.npu-card
/dev/dri/by-path/platform-fdab0000.npu-render
/usr/lib/librknnrt.so
/usr/local/include/rknn/rknn_api.h
/usr/local/include/rknn/rknn_matmul_api.h
```

内核日志显示：

```text
RKNPU fdab0000.npu: RKNPU: rknpu iommu is enabled, using iommu mode
[drm] Initialized rknpu 0.9.6 20240322 for fdab0000.npu on minor 1
```

## 5. 模型推理链路

当前已完成链路：

```text
PyTorch checkpoint
  -> tools/export_act_onnx.py
  -> build/act.onnx
  -> tools/check_onnx.py
  -> ONNX Runtime CPU inference
  -> action output
  -> LEFT/RIGHT decision
```

任务二 RK3588/NPU 已完成链路：

```text
build/act.onnx
  -> tools/convert_act_rknn.py
  -> build/act_rk3588_fp.rknn
  -> tools/rknn_runtime_infer.cpp
  -> Orange Pi / librknnrt.so
  -> action output
  -> denormalize
  -> LEFT/RIGHT decision
```

模型输入：

```text
image  : [1, 3, 224, 224]
state  : [1, 2]
latent : [1, 32]
```

模型输出：

```text
action : [1, 8, 3]
```

方向判断：

```text
left_vel < right_vel -> LEFT
left_vel > right_vel -> RIGHT
```

## 6. 正确性结果

ONNX Runtime 校验结果：

```text
frame_000000.jpg -> LEFT   expected: LEFT
frame_000227.jpg -> RIGHT  expected: RIGHT
```

关键输出：

```text
frame_000000:
left_vel  = -0.001586
right_vel = +0.007239
direction = LEFT

frame_000227:
left_vel  = +0.006118
right_vel = -0.000454
direction = RIGHT
```

RKNN/NPU 板端校验结果：

```text
frame_000000:
left_vel  = -0.001526
right_vel = +0.007373
direction = LEFT

frame_000227:
left_vel  = +0.006128
right_vel = -0.000488
direction = RIGHT
```

## 7. 性能数据

本地 Windows ONNX Runtime CPU benchmark，5 次运行平均：

| 样例 | 方向 | 平均耗时 |
|---|---|---:|
| `frame_000000` | LEFT | 26.78 ms |
| `frame_000227` | RIGHT | 25.91 ms |

Orange Pi 5 Plus RK3588 / RKNN runtime 结果：

| 样例 | 方向 | RKNN 耗时 | wall time |
|---|---|---:|---:|
| `frame_000000` | LEFT | 34.97 ms | 35.00 ms |
| `frame_000227` | RIGHT | 25.34 ms | 25.36 ms |

已有 StarryOS/QEMU 报告数据：

| 平台 | 后端 | 模型大小 | 推理耗时 | 峰值内存 |
|---|---|---:|---:|---:|
| StarryOS/QEMU riscv64 | ONNX Runtime CPU | 193.2 MB | 18.4-18.7 s | 454 MB |

当前生成文件：

```text
output/train/model.pt        202,962,639 bytes
build/act.onnx               202,633,726 bytes
build/act_rk3588_fp.rknn     101,709,294 bytes
build/cpp_test/image.bin         602,112 bytes
```

## 8. 工程工作与原创性

本次冲刺完成的工程工作：

- 自动扫描局域网候选 SSH 设备并识别 Orange Pi。
- 通过 SSH 采集板端系统、磁盘、内存、NPU 和 RKNN runtime 信息。
- 修复 ONNX 导出流程中的两个工程问题：
  - 避免导出时下载 torchvision ResNet18 预训练权重。
  - 自动创建 `build/` 输出目录。
- 下载最小模型与样例数据。
- 复现 ONNX Runtime Python 推理结果。
- 生成本地 benchmark JSON 和文本输出。
- 新增 RKNN 转换脚本 `tools/convert_act_rknn.py`。
- 在 WSL x86_64 Linux 环境中安装 RKNN Toolkit2 2.3.2 并完成 ONNX -> RKNN 转换。
- 新增板端 C++ RKNN runtime `tools/rknn_runtime_infer.cpp`。
- 将 `.rknn` 模型和输入 tensor 复制到 Orange Pi，调用 `/usr/lib/librknnrt.so` 跑通两个样例。
- 整理初审文档、演示脚本、性能表和工作日志。

## 9. 当前问题与分析

RKNN/NPU 已形成端到端闭环，但仍有工程风险需要说明：

- Windows Python 3.11 不能直接安装 `rknn-toolkit2`，实际转换环境切换到 WSL Ubuntu 24.04 / x86_64。
- RKNN Toolkit2 2.3.2 需要匹配较旧的 `onnx` API，因此将 `onnx` 固定到 1.16.2。
- 板端 RKNN runtime API 版本为 1.5.2，驱动版本为 0.9.6，运行时会打印 `invalid run task counter` 警告。
- 尽管存在 runtime 版本警告，`rknn_run`、`rknn_outputs_get` 和性能查询均返回结果，两个样例方向与 ONNX Runtime 基线一致。
- 后续正式优化前应升级板端 RKNN runtime，使其与 RKNN Toolkit2 2.3.2 对齐，并做更多样例的稳定性测试。

## 10. 下一步计划

1. 升级或替换板端 RKNN runtime，消除 `invalid run task counter` 警告。
2. 将 C++ runtime 扩展为更完整的 CLI，支持直接读取 JPEG、预处理、推理和后处理。
3. 增加更多 LEFT/RIGHT 样例回归，记录精度漂移和性能波动。
4. 将 RKNN runtime 路线整理为可复现脚本，方便评委和队友复测。
5. 继续评估模型裁剪、量化和任务一 SG2002 方向。

## 11. AI 工具使用说明

本项目冲刺过程中使用 AI 辅助完成：

- 需求拆解和时间盒计划。
- 仓库结构分析和文档整理。
- SSH 自动化采集命令设计。
- ONNX 导出问题定位。
- benchmark 脚本和初审文档草稿生成。

人工/队伍工作包括：

- 提供开发板、账号信息和比赛上下文。
- 运行并验证真实 Orange Pi 板端环境。
- 提供和维护队伍仓库。
- 后续需要人工录制演示视频并确认提交材料。
