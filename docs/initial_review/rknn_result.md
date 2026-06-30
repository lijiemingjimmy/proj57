# RKNN / NPU 结果

## 板端状态

- SSH IP: `192.168.0.104`
- 板型：Orange Pi 5 Plus
- 系统：Ubuntu 22.04.5 LTS
- 架构：aarch64
- 内核：`5.10.0-1012-rockchip`
- 内存：3.8 GiB
- 根分区：约 15 GB，剩余约 9.0 GB

## NPU 驱动与 Runtime

板端已发现 NPU DRM 节点：

```text
/dev/dri/card1
/dev/dri/renderD129
/dev/dri/by-path/platform-fdab0000.npu-card
/dev/dri/by-path/platform-fdab0000.npu-render
```

板端已安装 RKNN runtime 库和头文件：

```text
/usr/lib/librknnrt.so
/usr/local/include/rknn/rknn_api.h
/usr/local/include/rknn/rknn_matmul_api.h
```

`dmesg` 中确认 RKNPU 初始化：

```text
RKNPU fdab0000.npu: RKNPU: rknpu iommu is enabled, using iommu mode
[drm] Initialized rknpu 0.9.6 20240322 for fdab0000.npu on minor 1
```

## RKNN 转换

已在 WSL Ubuntu 24.04 的 x86_64 Python 环境中完成 ONNX 到 RKNN 转换。

关键版本：

```text
rknn-toolkit2 2.3.2
torch 2.4.0+cpu
onnx 1.16.2
numpy 1.26.4
scipy 1.15.3
setuptools 80.9.0
```

产物：

```text
build/act_rk3588_fp.rknn
size: 101,709,294 bytes, about 97.0 MiB
```

转换日志：

```text
artifacts/rknn/wsl_convert_real_retry3.log
```

## 板端 RKNN Runtime

已新增并编译最小 C++ runtime：

```text
tools/rknn_runtime_infer.cpp
```

板端运行目录：

```text
/home/ubuntu/proj57_rknn_run
```

编译命令：

```bash
g++ -O2 -std=c++17 rknn_runtime_infer.cpp \
  -I/usr/local/include/rknn -L/usr/lib -lrknnrt \
  -o rknn_runtime_infer
```

运行命令：

```bash
LD_LIBRARY_PATH=/usr/lib:/lib ./rknn_runtime_infer \
  act_rk3588_fp.rknn image.bin state.bin latent.bin \
  action_q01.bin action_q99.bin
```

## 正确性与性能

`frame_000000`：

```text
rknn_perf_run_us=34968
wall_ms=34.998
first_step_denorm_left=-0.001526
right=0.007373
direction=LEFT
```

`frame_000227`：

```text
rknn_perf_run_us=25340
wall_ms=25.364
first_step_denorm_left=0.006128
right=-0.000488
direction=RIGHT
```

结论：两个代表样例的 RKNN/NPU 方向与 ONNX Runtime 基线一致。

板端 runtime 日志：

```text
artifacts/rknn/runtime_log_frame000_nhwc.txt
artifacts/rknn/runtime_log_frame227_nhwc.txt
```

## 当前风险

板端 RKNN runtime 版本较旧：

```text
api_version=1.5.2 (2023-08-23)
drv_version=0.9.6
```

运行时会打印如下警告：

```text
failed to submit!, invalid run task counter: 0 >= 0
please try updating to the latest version of the toolkit2 and runtime
```

尽管有该警告，`rknn_run`、`rknn_outputs_get`、性能查询和输出结果均返回，两个方向样例均正确。后续建议升级板端 RKNN runtime，使其与 RKNN Toolkit2 2.3.2 对齐，再做更正式的稳定性测试。
