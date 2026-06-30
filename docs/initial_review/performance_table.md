# 性能与资源数据表

| 平台 | 后端 | 状态 | 模型大小 | 推理耗时 | 内存 | 结果 |
|---|---|---|---:|---:|---:|---|
| StarryOS/QEMU riscv64 | ONNX Runtime CPU | 已有报告跑通 | 193.2 MB | 18.4-18.7 s | 454 MB 峰值 | LEFT/RIGHT 正确 |
| Windows 本机 | ONNX Runtime CPU | 已跑通 | 193.25 MB | 25.91-26.78 ms | 未单独采集 | LEFT/RIGHT 正确 |
| Orange Pi 5 Plus RK3588 | RKNN/NPU | 已跑通 | 97.0 MiB | 25.34-35.00 ms | 板端 3.8 GiB | LEFT/RIGHT 正确 |

## 代表性输出

来自本地 ONNX Runtime 结果：

```text
frame_000000:
left=-0.001586
right=+0.007239
direction=LEFT
avg_ms=26.778

frame_000227:
left=+0.006118
right=-0.000454
direction=RIGHT
avg_ms=25.909
```

来自 Orange Pi 5 Plus RK3588 / RKNN runtime 结果：

```text
frame_000000:
left=-0.001526
right=+0.007373
direction=LEFT
rknn_perf_run_us=34968
wall_ms=34.998

frame_000227:
left=+0.006128
right=-0.000488
direction=RIGHT
rknn_perf_run_us=25340
wall_ms=25.364
```

来自已有 StarryOS/QEMU 结果：

```text
frame_000000:
left_vel=-0.0015857
right_vel=+0.0072386
direction=LEFT
infer_ms=18657.4

frame_000227:
left_vel=+0.00611769
right_vel=-0.000454509
direction=RIGHT
infer_ms=18442.2
```
