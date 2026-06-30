# Proj57 初审冲刺进展报告

## 已完成

- 成功通过 SSH 登录 Orange Pi。
- 补充比赛背景、任务分档、奖项路径和评审关注点，见 `competition_context.md`。
- 确认板子 IP 为 `192.168.0.104`。
- 确认板型为 Orange Pi 5 Plus。
- 确认系统为 Ubuntu 22.04.5 LTS / aarch64 / Rockchip 5.10 内核。
- 确认 NPU 驱动存在，RKNN runtime 库和头文件已安装。
- 整理队伍仓库结构和任务要求。
- 下载最小数据文件 `stats.json`、`frame_000000.jpg`、`frame_000227.jpg`。
- 完成 `model.pt` 下载。
- 完成 ONNX 导出，生成 `build/act.onnx`。
- 完成 ONNX Runtime 校验，两个代表性样例方向正确。
- 完成本地 ONNX Runtime benchmark。
- 新增 RKNN 转换脚本 `tools/convert_act_rknn.py`。
- 打通 WSL/Linux RKNN Toolkit2 路线，生成 `build/act_rk3588_fp.rknn`。
- 新增 C++ RKNN runtime `tools/rknn_runtime_infer.cpp`。
- 将 `.rknn` 模型和输入 tensor 上传到 Orange Pi。
- 在 Orange Pi 上使用 `/usr/lib/librknnrt.so` 跑通两个代表样例：
  - `frame_000000 -> LEFT`
  - `frame_000227 -> RIGHT`

## 主要发现

- 当前队伍仓库已经完成任务三 StarryOS/QEMU CPU 推理链路的报告和代码。
- 本次已在本地重新复现 ONNX 导出和 ONNX Runtime 推理。
- 本次已完成任务二 RK3588/RKNN/NPU 的最小闭环。
- Orange Pi 板端可直接编译 C++ 程序并链接 RKNN runtime。

## 当前风险

- 板端 RKNN runtime API 1.5.2 与转换工具 RKNN Toolkit2 2.3.2 版本不完全匹配，运行时会打印 `invalid run task counter` 警告。
- 目前验证了两个代表性样例，还需要扩展更多样例和稳定性测试。
- C++ runtime 当前读取预处理后的 tensor bin，尚未直接支持 JPEG 输入和完整预处理。

## 下一步

1. 整理 RKNN/NPU 运行日志到初审材料。
2. 升级或替换板端 RKNN runtime，消除版本警告。
3. 扩展 C++ runtime，支持直接 JPEG 预处理和更多测试样例。
4. 录制初审视频，展示 ONNX 基线和 RK3588/NPU 结果。
