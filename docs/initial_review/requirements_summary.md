# Proj57 任务要求摘要

## 任务目标

Proj57 要求使用组织方提供的 ACT 模型，在 StarryOS 和指定嵌入式平台上完成推理部署，并输出动作预测结果。ACT 输入图像和机器人状态，输出多步未来动作。

## 任务分层

- 任务一：荔枝派 SG2002，256MB，TPU，C/C++/Rust。
- 任务二：香橙派 RK3588，4/8GB，NPU，C/C++/Rust。
- 任务三：QEMU/StarryOS，CPU，C/C++/Rust。

当前 6 小时冲刺重点是任务二 RK3588 板端验证，同时保留任务三 QEMU/CPU 已完成结果作为初审展示底座。

## 模型输入输出

- `image`: `[1, 3, 224, 224]`
- `state`: `[1, 2]`
- `latent`: `[1, 32]`
- `action`: `[1, 8, 3]`

每一步 action 为 `[left_vel, right_vel, gripper_target]`。方向判断采用：

```text
left_vel < right_vel -> LEFT
left_vel > right_vel -> RIGHT
```

## 评审关注点

- 第一优先级是推理正确性：预处理、推理、后处理链路完整，方向与参考一致。
- 第二优先级是工程质量：资源占用、推理速度、稳定性、代码质量、文档可复现。
- 文档需要包含模型大小、可执行文件大小、运行内存、推理耗时、左右转标志性数据。
- 需要说明原创工作和 AI 工具使用情况。

## 当前已知成果

- 队伍仓库已有任务三 StarryOS/QEMU CPU 推理报告。
- 已记录 `frame_000000 -> LEFT`，`frame_000227 -> RIGHT`。
- 已记录 ONNX 模型约 193.2 MB，QEMU/StarryOS 单次推理约 18.4 到 18.7 秒，峰值内存约 454 MB。
- 本次冲刺已完成任务二 RK3588 最小闭环：ONNX -> RKNN 转换成功，并在 Orange Pi RK3588 上通过 C++ `librknnrt` 跑通两个代表样例。
- RKNN 板端结果：`frame_000000 -> LEFT`，`frame_000227 -> RIGHT`，单次 `rknn_run` 约 25 到 35 ms。

## 当前风险

- 当前仓库默认不带 `output/` 和 `build/` 大文件。
- C++ 程序当前读取预处理后的 tensor bin，尚未端到端处理 JPEG。
- 板端 RKNN runtime API 1.5.2 与 RKNN Toolkit2 2.3.2 不完全匹配，运行时会打印版本警告。
- 后续更换模型、量化或扩大样例时仍需关注算子支持、内存占用和精度漂移。
