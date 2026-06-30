# Proj57 初审冲刺工作日志

## 2026-06-30

### 已完成

- 克隆并检查 `chenlongos/proj57` 上游仓库和当前队伍仓库 `lijiemingjimmy/proj57`。
- 梳理赛题目标、三个任务、奖项路径和评审关注点，形成 `competition_context.md`。
- 确认队伍仓库已有任务三 QEMU/StarryOS CPU 推理代码和报告材料。
- 自动扫描局域网 SSH 设备，定位 Orange Pi 开发板地址为 `192.168.0.104`。
- 通过 SSH 登录板端，采集板型、系统、内核、CPU、内存、磁盘和网络信息。
- 确认板端为 Orange Pi 5 Plus，系统为 Ubuntu 22.04.5 LTS，架构为 aarch64。
- 确认板端存在 RK3588 NPU DRM 节点、RKNPU 内核日志、`librknnrt.so` 和 RKNN 头文件。
- 下载最小验证数据：`stats.json`、`frame_000000.jpg`、`frame_000227.jpg`。
- 下载 ACT checkpoint：`output/train/model.pt`。
- 修复 ONNX 导出流程：
  - 避免导出阶段联网下载 torchvision ResNet18 预训练权重。
  - 自动创建 `build/` 输出目录。
- 导出 ONNX 模型：`build/act.onnx`。
- 运行 ONNX Runtime 正确性验证：
  - `frame_000000.jpg -> LEFT`，符合预期。
  - `frame_000227.jpg -> RIGHT`，符合预期。
- 运行本地 ONNX Runtime CPU benchmark：
  - `frame_000000` 平均约 26.78 ms。
  - `frame_000227` 平均约 25.91 ms。
- 生成 C++/Runtime 输入 tensor 二进制样例，位于 `build/cpp_test/`。
- 新增 RKNN 转换脚本 `tools/convert_act_rknn.py`。
- 在 WSL Ubuntu 24.04 中完成 RKNN Toolkit2 2.3.2 环境配置。
- 解决 RKNN 转换环境中的关键版本问题：
  - Windows Python 无法安装 `rknn-toolkit2`，改用 WSL x86_64 Linux。
  - 使用 CPU 版 `torch 2.4.0`。
  - 将 `onnx` 固定到 1.16.2 以兼容 `onnx.mapping`。
  - 使用 `numpy 1.26.4`、`scipy 1.15.3`、`setuptools 80.9.0`。
- 成功生成 `build/act_rk3588_fp.rknn`，大小 101,709,294 bytes。
- 新增 `tools/rknn_runtime_infer.cpp`，在 Orange Pi 上编译并调用 `/usr/lib/librknnrt.so`。
- 板端 RKNN/NPU 推理结果：
  - `frame_000000`：LEFT，`rknn_perf_run_us=34968`。
  - `frame_000227`：RIGHT，`rknn_perf_run_us=25340`。
- 生成初审技术文档、性能表、进度报告、演示脚本、PPT 和两份 PDF。

### 当前风险

- 板端 RKNN runtime 版本较旧，会打印 `invalid run task counter` 警告，需要后续升级或替换 runtime。
- 当前 C++ runtime 读取的是预处理后的 tensor bin，尚未直接支持 JPEG 输入。
- 当前只验证了两个代表性样例，还需要更多样例和稳定性测试。
- 尚未录制初审演示视频。

### 下一步优先级

1. 用当前文档和日志录制初审视频，说明已完成成果、性能数据、当前问题和下一步计划。
2. 升级或替换板端 RKNN runtime，消除版本警告。
3. 扩展 C++ runtime，支持 JPEG 预处理和更多样例回归。
4. 继续评估任务一 SG2002 的模型压缩与运行时裁剪路线。

### 敏感信息处理

- SSH 密码只在临时命令内存中使用，不写入仓库、文档或日志。
- 板端同步状态只记录用户、IP 和目标目录，不记录密码。
