# 初审演示视频脚本

## 录制节奏

建议控制在 4 到 6 分钟：

1. 开场和任务目标：20 秒。
2. 硬件、SSH 和 NPU 环境：60 秒。
3. Task3 / ONNX 基线：70 秒。
4. Task2 / RKNN 板端结果：120 秒。
5. 风险、边界和下一步：40 秒。

录屏时不要直接滚完整 RKNN 日志。先展示包含 `direction=LEFT/RIGHT`、`rknn_perf_run_us`、`api_version`、`drv_version` 的关键行，再解释 runtime 版本警告。

## 1. 开场

说明 Proj57 目标：把 ACT 动作预测模型部署到 StarryOS、QEMU 和 RK3588 等平台，完成模型推理并输出动作方向。

说明奖项路径：任务三 QEMU 是基础保底，任务三加任务二 RK3588 是当前冲击方向，任务一 SG2002 是最高难度资源受限目标。

## 2. 展示硬件

展示 Orange Pi 5 Plus 开发板，说明它使用 RK3588，带 NPU，是任务二目标平台。

## 3. 展示 SSH 登录

展示 SSH 到 `192.168.0.104`，执行：

```bash
uname -a
cat /etc/os-release
cat /proc/device-tree/model
free -h
```

解释系统为 Ubuntu 22.04.5 LTS，aarch64，内存约 4GB。

## 4. 展示 NPU 环境

执行：

```bash
ls -l /dev/dri/by-path | grep npu
ls -l /usr/lib/librknnrt.so
dmesg | grep -i rknpu | tail
```

说明 NPU 驱动和 RKNN runtime 已存在。

## 5. 展示仓库和已有成果

展示：

```bash
tree -L 2
cat docs/logs/starryos_task3_outputs.txt
```

说明任务三 QEMU/StarryOS CPU 路线已有 LEFT/RIGHT 正确结果。

## 6. 展示 ONNX 正确性和性能

展示：

```bash
python tools/check_onnx.py
type artifacts\baseline\local_infer_output.txt
type artifacts\baseline\local_benchmark.json
```

说明当前路线和结果：

```text
PyTorch checkpoint -> ONNX -> ONNX Runtime / RKNN -> action -> LEFT/RIGHT
frame_000000.jpg -> LEFT   expected: LEFT
frame_000227.jpg -> RIGHT  expected: RIGHT
```

## 7. 展示 RK3588/NPU 进度

展示：

```bash
type docs\initial_review\rknn_result.md
Select-String artifacts\rknn\runtime_log_frame000_nhwc.txt -Pattern "api_version|drv_version|rknn_perf_run_us|wall_ms|first_step_denorm"
Select-String artifacts\rknn\runtime_log_frame227_nhwc.txt -Pattern "api_version|drv_version|rknn_perf_run_us|wall_ms|first_step_denorm"
```

说明 RKNN 转换和板端 runtime 已跑通：

```text
build/act_rk3588_fp.rknn
frame_000000 -> LEFT
frame_000227 -> RIGHT
```

说明 RKNN runtime 耗时：

```text
frame_000000: rknn_perf_run_us=34968
frame_000227: rknn_perf_run_us=25340
```

同时说明板端 runtime API 1.5.2 与 RKNN Toolkit2 2.3.2 不完全匹配，会打印 `invalid run task counter` 警告；但 `rknn_run`、输出、耗时和 LEFT/RIGHT 方向都返回。后续应升级 RKNN runtime 以做更正式的稳定性测试。

说明当前工程边界：板端 C++ runtime 读取的是预处理后的 `image.bin/state.bin/latent.bin`，还不是直接 `jpg -> preprocess -> infer`。

## 8. 总结

强调当前成果：

- 板子已 SSH 登录并确认 NPU 环境。
- Task3 CPU baseline 已有可展示数据。
- ONNX 导出和本地正确性验证已跑通。
- Task2 RK3588/RKNN/NPU 已跑通两个代表样例。
- 后续重点是升级 runtime、扩展样例和完善 CLI。
