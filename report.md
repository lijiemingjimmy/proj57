# Proj57 技术报告

## 1. 目标描述

Proj57 的目标是将统一 ACT 动作预测模型适配到 StarryOS/QEMU 与国产化嵌入式平台上，完成推理部署并输出预期动作数据。本项目重点关注：

- Task3：在 QEMU/StarryOS CPU 环境中完成 ACT 推理，作为基础保底链路。
- Task2：在 Orange Pi 5 Plus / RK3588 平台上完成 ACT 推理，验证 RKNN/NPU 路线。
- 后续 Task1：面向 SG2002 256 MB 资源约束平台继续进行模型裁剪、量化和部署优化。

ACT 模型输入前视图像、机器人状态和 latent 向量，输出未来 8 步动作。每步动作包含 `left_vel`、`right_vel` 和 `gripper_target`。本项目使用第一步动作判断方向：`left_vel < right_vel` 为 LEFT，`left_vel > right_vel` 为 RIGHT。

## 2. 比赛题目分析和相关资料调研

赛题要求使用组织方提供的统一 ACT 模型，在不同操作系统和嵌入式平台上完成推理。评价重点首先是推理正确性，其次是工程质量、资源占用、性能数据和可复现性。

我们调研并使用了以下技术路线：

- ACT / PyTorch：作为原始模型定义和 checkpoint 来源。
- ONNX Runtime：用于导出后模型的 CPU 基线验证，以及 QEMU/StarryOS 路线对照。
- RKNN Toolkit2：用于将 ONNX 转换为 RK3588 可运行的 RKNN 模型。
- RKNN Runtime C API：用于 Orange Pi RK3588 板端 C++ 推理。
- StarryOS/QEMU 既有结果：作为 Task3 的基础验证和初赛保底数据。

## 3. 系统框架设计

整体链路如下：

```text
model.pt
  -> export_act_onnx.py
  -> build/act.onnx
  -> check_onnx.py 验证 ONNX 基线
  -> convert_act_rknn.py
  -> build/act_rk3588_fp.rknn
  -> Orange Pi RK3588 / librknnrt.so / C++ runtime
  -> action[1,8,3]
  -> LEFT / RIGHT
```

系统分为四层：

1. 模型层：ACT PyTorch 模型和 checkpoint。
2. 中间表示层：ONNX 模型，用于跨平台验证和转换。
3. 平台转换层：RKNN Toolkit2 将 ONNX 转为 RK3588 RKNN 模型。
4. 板端运行层：C++ 程序调用 `librknnrt.so` 完成推理并进行后处理。

## 4. 开发计划

开发按先保底、再上板、再工程化的顺序推进：

1. 复现队伍已有 Task3 / ONNX / QEMU 结果。
2. 下载最小模型权重和两个代表性样例帧。
3. 修复 ONNX 导出流程并生成 `build/act.onnx`。
4. 用 ONNX Runtime 验证 `frame_000000 -> LEFT` 和 `frame_000227 -> RIGHT`。
5. 在 WSL Linux 环境安装 RKNN Toolkit2 并完成 ONNX 到 RKNN 转换。
6. 编写板端 C++ RKNN runtime，复制模型和输入到 Orange Pi。
7. 在 RK3588 板端运行两个代表样例，记录方向和耗时。
8. 整理初赛技术文档、视频脚本、演示 PDF 和证据日志。

## 5. 比赛过程中的重要进展

本次初赛阶段已完成以下关键进展：

- 成功 SSH 登录 Orange Pi 5 Plus，确认系统为 Ubuntu 22.04.5 LTS，架构为 aarch64，内存约 3.8 GiB。
- 确认板端存在 NPU DRM 节点、`/usr/lib/librknnrt.so` 和 RKNN C 头文件。
- 修复 ONNX 导出脚本，避免导出时下载 torchvision ResNet18 预训练权重，并自动创建输出目录。
- 成功生成 `build/act.onnx`，大小 202,633,726 bytes。
- ONNX Runtime 基线验证通过：
  - `frame_000000.jpg -> LEFT`
  - `frame_000227.jpg -> RIGHT`
- 在 WSL Ubuntu 24.04 / x86_64 环境中安装 RKNN Toolkit2 2.3.2，完成 ONNX 到 RKNN 转换。
- 生成 `build/act_rk3588_fp.rknn`，大小 101,709,294 bytes。
- 新增 `tools/rknn_runtime_infer.cpp`，在 Orange Pi RK3588 上调用 RKNN runtime 完成推理。
- 板端 RKNN 结果：
  - `frame_000000 -> LEFT`，`rknn_perf_run_us=34968`，`wall_ms=34.998`
  - `frame_000227 -> RIGHT`，`rknn_perf_run_us=25340`，`wall_ms=25.364`

## 6. 系统测试情况

### 6.1 ONNX 正确性测试

使用 `tools/check_onnx.py` 对两个代表性样例进行测试：

| 样例 | ONNX 输出 | 预期 | 结果 |
|---|---|---|---|
| `frame_000000.jpg` | LEFT | LEFT | 通过 |
| `frame_000227.jpg` | RIGHT | RIGHT | 通过 |

本地 Windows ONNX Runtime CPU benchmark：

| 样例 | 平均耗时 |
|---|---:|
| `frame_000000` | 26.778 ms |
| `frame_000227` | 25.909 ms |

### 6.2 RK3588 板端测试

板端环境：

- Board：Orange Pi 5 Plus
- SoC：RK3588
- OS：Ubuntu 22.04.5 LTS
- Arch：aarch64
- RKNN runtime API：1.5.2
- Driver：0.9.6

板端 RKNN 测试：

| 样例 | RKNN 输出 | 耗时 | 结果 |
|---|---|---:|---|
| `frame_000000` | LEFT | 34.998 ms | 通过 |
| `frame_000227` | RIGHT | 25.364 ms | 通过 |

### 6.3 QEMU/StarryOS 既有结果

队伍已有 Task3 报告记录：

- `frame_000000 -> LEFT`
- `frame_000227 -> RIGHT`
- 单次推理约 18.4 到 18.7 秒
- 峰值内存约 454 MB

## 7. 遇到的主要问题和解决方法

### 7.1 ONNX 导出时依赖网络下载

问题：原模型构造中使用 torchvision ResNet18 默认权重，导出时可能尝试下载预训练权重。

解决：将 ResNet18 初始化改为 `weights=None`。比赛 checkpoint 会覆盖模型权重，因此不影响最终推理结果，同时减少网络依赖。

### 7.2 Windows 环境无法直接安装 RKNN Toolkit2

问题：RKNN Toolkit2 主要面向 Linux x86_64 转换环境，Windows Python 安装不稳定。

解决：切换到 WSL Ubuntu 24.04 / x86_64，安装 `rknn-toolkit2 2.3.2`，并固定兼容的 `onnx 1.16.2`。

### 7.3 板端 RKNN runtime 版本警告

问题：板端 RKNN runtime API 为 1.5.2，转换工具为 2.3.2，运行时会打印 `invalid run task counter` 相关警告。

解决：保留完整日志并验证 `rknn_run`、输出获取、性能查询均能返回结果。当前两个代表样例方向正确。后续计划升级或替换板端 RKNN runtime，使其与转换工具版本对齐。

### 7.4 当前 C++ runtime 还不是完整 JPEG 端到端输入

问题：当前 C++ RKNN runtime 读取的是预处理后的 tensor bin，而不是直接读取 JPEG。

解决：初赛阶段先完成模型推理最小闭环，文档中明确说明该边界。后续将加入 JPEG 解码、resize、normalize 和完整 CLI 参数。

## 8. 分工和协作

- 李捷铭：队长，负责队伍组织、仓库维护、既有 Task3 路线推进与整体方向协调。
- 朱屹帆：参与 RK3588 板端环境连接、初赛材料整理、文档和演示录制。
- 杨若朝：参与模型适配、结果分析和文档协作。

本次冲刺中使用 AI 辅助完成代码整理、脚本生成、文档汇总和日志分析；关键结果均基于真实命令输出、板端日志和本地文件。

## 9. 提交仓库目录和文件描述

关键目录和文件如下：

```text
README.md
report.md
tools/export_act_onnx.py
tools/convert_act_rknn.py
tools/rknn_runtime_infer.cpp
docs/initial_review/
artifacts/onnx/check_output.txt
artifacts/rknn/runtime_log_frame000_nhwc.txt
artifacts/rknn/runtime_log_frame227_nhwc.txt
artifacts/baseline/local_benchmark.json
```

说明：

- `README.md`：提交入口说明、视频链接和材料索引。
- `report.md`：本技术报告。
- `tools/export_act_onnx.py`：导出 ONNX。
- `tools/convert_act_rknn.py`：转换 RKNN。
- `tools/rknn_runtime_infer.cpp`：RK3588 板端 C++ 推理程序。
- `docs/initial_review/`：技术文档、演示 PDF、讲稿、截图和性能表。
- `artifacts/`：ONNX/RKNN 运行日志和性能数据。

## 10. 比赛收获

本项目完成了从模型导出、跨平台基线验证、RKNN 转换到 RK3588 板端推理的完整最小闭环。通过本次实践，我们加深了对以下内容的理解：

- ACT 模型输入输出结构和动作后处理方式。
- ONNX 在跨平台模型验证中的作用。
- RKNN Toolkit2 与 RKNN runtime 的分工。
- 嵌入式 AI 部署中模型格式、runtime 版本、算子支持和资源约束之间的关系。
- 初赛提交中自解释文档、可复现日志和真实性证据的重要性。

当前任务二已经跑通两个代表样例，后续将继续完善 runtime 版本对齐、端到端输入处理、更多样例回归和面向 SG2002 的资源优化。
