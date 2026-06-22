# Proj57 任务三：StarryOS/QEMU 上的 ACT 模型推理部署报告

## 1. 任务目标

本次任务目标是完成 Proj57 任务三：在 **StarryOS + QEMU** 环境中，使用 **CPU 后端**运行 ACT（Action Chunking Transformer）模型推理，并输出动作预测结果。

本次实现的核心流程如下：

```text
PyTorch checkpoint
    -> 导出 ONNX 模型
    -> ONNX Runtime Python 验证
    -> ONNX Runtime C++ 本机验证
    -> StarryOS/QEMU riscv64 环境运行
    -> 输出 left_vel / right_vel / gripper_target
    -> 判断 LEFT / RIGHT 方向
```

最终结果表明：ACT 模型能够在 StarryOS/QEMU 中通过 ONNX Runtime 成功运行，且左右转方向判断与参考结果一致。

---

## 2. 完成情况

```text
[完成] PyTorch notebook 参考推理
[完成] PyTorch checkpoint 导出为 ONNX
[完成] ONNX Runtime Python 验证
[完成] ONNX Runtime C++ 本机验证
[完成] StarryOS/QEMU riscv64 启动
[完成] riscv64 hello world 用户程序运行
[完成] StarryOS 内通过 apk 安装 onnxruntime / onnxruntime-dev
[完成] StarryOS 内安装 g++ / make
[完成] StarryOS 内编译 C++ 推理程序 task3_act
[完成] StarryOS 内运行 ACT ONNX 推理
[完成] frame_000000 -> LEFT
[完成] frame_000227 -> RIGHT
```

代表性样例：

```text
frame_000000.jpg -> LEFT
frame_000227.jpg -> RIGHT
```

---

## 3. 模型与数据说明

原始仓库提供了 ACT 模型定义、训练好的 checkpoint 以及示例数据集。

主要文件结构如下：

```text
output/
├── dataset/
│   ├── data/
│   ├── meta/stats.json
│   └── videos/observation.images.fpv/chunk-000/frame_*.jpg
└── train/
    └── model.pt
```

关键文件：

```text
output/train/model.pt
output/dataset/meta/stats.json
output/dataset/videos/observation.images.fpv/chunk-000/frame_000000.jpg
output/dataset/videos/observation.images.fpv/chunk-000/frame_000227.jpg
```

其中：

```text
frame_000000.jpg 预期方向：LEFT
frame_000227.jpg 预期方向：RIGHT
```

---

## 4. ONNX 导出

原始模型文件为：

```text
output/train/model.pt
```

将其导出为：

```text
build/act.onnx
```

导出命令：

```bash
python tools/export_act_onnx.py
```

导出结果：

```text
ONNX 导出完成：build/act.onnx
模型大小：约 193.2 MB
```

导出的 ONNX 模型输入为：

```text
image  : [1, 3, 224, 224]
state  : [1, 2]
latent : [1, 32]
```

输出为：

```text
action : [1, 8, 3]
```

其中每一步 action 的三个维度为：

```text
[left_vel, right_vel, gripper_target]
```

---

## 5. ONNX Runtime Python 验证

导出 ONNX 后，首先使用 Python 版 ONNX Runtime 进行验证，确保 ONNX 模型没有导出错误。

运行命令：

```bash
python tools/check_onnx.py
```

验证目标：

```text
frame_000000.jpg -> LEFT
frame_000227.jpg -> RIGHT
```

该步骤用于确认：ONNX 模型在方向判断上与 PyTorch notebook 参考结果一致。

---

## 6. C++ 本机验证

随后将推理流程改写为 C++ 程序：

```text
src/task3_main.cpp
```

为了降低 StarryOS 端调试难度，当前版本先使用预处理后的二进制输入文件：

```text
image.bin
state.bin
latent.bin
action_q01.bin
action_q99.bin
```

生成输入文件命令：

```bash
python tools/dump_cpp_inputs.py --frame 227
```

C++ 程序执行逻辑：

```text
1. 读取 image.bin / state.bin / latent.bin
2. 加载 act.onnx
3. 调用 ONNX Runtime CPU 后端推理
4. 对 action 进行反归一化
5. 输出 8 步 action chunk
6. 根据第 0 步 left_vel / right_vel 判断方向
```

方向判断规则：

```text
left_vel < right_vel  -> LEFT
left_vel > right_vel  -> RIGHT
```

本机 C++ 验证结果：

```text
frame_000000 -> LEFT
frame_000227 -> RIGHT
```

---

## 7. StarryOS/QEMU 环境搭建

StarryOS 使用 QEMU riscv64 平台运行。

在 Apple Silicon Mac 上，使用 Docker 的 `linux/amd64` 模式运行 StarryOS 构建环境：

```bash
docker pull --platform linux/amd64 ghcr.io/arceos-org/arceos-build

docker run --platform linux/amd64 -it --rm \
  -v "$(pwd)":/workspace \
  -w /workspace \
  ghcr.io/arceos-org/arceos-build
```

在 Docker 容器内构建并启动 StarryOS：

```bash
make ARCH=riscv64 rootfs
make ARCH=riscv64 build
make ARCH=riscv64 run
```

成功启动后显示：

```text
Welcome to Starry OS!
starry:~#
```

启动信息中目标平台为：

```text
arch = riscv64
platform = riscv64-qemu-virt
target = riscv64gc-unknown-none-elf
```

---

## 8. StarryOS 内安装依赖

StarryOS 内使用 `apk` 安装 ONNX Runtime 和 C++ 编译器：

```bash
apk update
apk add onnxruntime onnxruntime-dev
apk add g++ make
```

实际安装到的包包括：

```text
onnxruntime
onnxruntime-dev
```

C++ 编译器版本：

```text
g++ (Alpine 15.2.0) 15.2.0
```

ONNX Runtime 相关库：

```text
/usr/lib/libonnxruntime.so
/usr/lib/libonnxruntime.so.1
/usr/lib/libonnxruntime.so.1.23.0
/usr/lib/libonnxruntime_providers_shared.so
```

---

## 9. 文件传输方式

由于 StarryOS 运行在 QEMU 中，本次使用 Docker 容器内的 HTTP server 将文件传入 StarryOS。

Docker 容器内启动 HTTP server：

```bash
cd /workspace
python3 -m http.server 8000 > /tmp/proj57_http.log 2>&1 &
```

StarryOS 内通过如下地址访问 Docker 容器：

```text
http://10.0.2.2:8000
```

在 StarryOS 内下载文件：

```bash
mkdir proj57
cd proj57

URL=http://10.0.2.2:8000

wget "$URL/task3_main.cpp" -O task3_main.cpp
wget "$URL/act.onnx" -O act.onnx
wget "$URL/image.bin" -O image.bin
wget "$URL/state.bin" -O state.bin
wget "$URL/latent.bin" -O latent.bin
wget "$URL/action_q01.bin" -O action_q01.bin
wget "$URL/action_q99.bin" -O action_q99.bin
```

---

## 10. StarryOS 内编译 C++ 推理程序

在 StarryOS 内查找 ONNX Runtime 头文件路径：

```bash
ORT_HEADER=$(find /usr/include -name "onnxruntime_cxx_api.h" | head -1)
ORT_INCLUDE_DIR=$(dirname "$ORT_HEADER")
```

编译 C++ 推理程序：

```bash
g++ -std=c++17 -O2 task3_main.cpp \
  -I"$ORT_INCLUDE_DIR" \
  -L/usr/lib \
  -lonnxruntime \
  -pthread \
  -o task3_act
```

编译结果：

```text
task3_act = 26.8 KB
```

运行命令：

```bash
export LD_LIBRARY_PATH=/usr/lib:$LD_LIBRARY_PATH
./task3_act
```

---

## 11. StarryOS/QEMU 推理结果

### 11.1 样例一：frame_000227

输入样例：

```text
frame_000227.jpg
```

预期方向：

```text
RIGHT
```

StarryOS/QEMU 输出：

```text
step 0:
left_vel  = +0.00611769
right_vel = -0.000454509
gripper   ≈ 0
direction = RIGHT

infer_ms = 18442.2
```

结果：

```text
PASS
```

---

### 11.2 样例二：frame_000000

输入样例：

```text
frame_000000.jpg
```

预期方向：

```text
LEFT
```

StarryOS/QEMU 输出：

```text
step 0:
left_vel  = -0.0015857
right_vel = +0.0072386
gripper   ≈ 0
direction = LEFT

infer_ms = 18657.4
```

结果：

```text
PASS
```

---

## 12. 文件大小与资源信息

StarryOS 内工作目录文件大小如下：

```text
act.onnx          193.2 MB
task3_act          26.8 KB
task3_main.cpp      5.1 KB
image.bin         588.0 KB
state.bin             8 B
latent.bin          128 B
action_q01.bin       12 B
action_q99.bin       12 B
```

推理耗时：

```text
frame_000227: 18442.2 ms
frame_000000: 18657.4 ms
```

运行时峰值内存占用：

```
454 MB
```

---

## 13. 运行时 warning 说明

推理时出现如下 warning：

```text
Unimplemented syscall: riscv_hwprobe
```

这是 ONNX Runtime / libc 尝试探测 RISC-V CPU 特性时触发的 syscall。StarryOS 当前未实现该 syscall，但该 warning 不影响推理。

ONNX Runtime 还输出：

```text
Unknown CPU vendor. cpuinfo_vendor value: 0
```

这是因为 ONNX Runtime 未能识别 StarryOS/QEMU 暴露的 CPU vendor 信息。该 warning 同样不影响推理。

从实际结果看，ONNX Runtime 能够成功创建 session 并完成 inference。

---

## 14. 当前实现限制

当前 C++ 程序读取的是预处理后的二进制 tensor：

```text
image.bin
state.bin
latent.bin
```

因此当前版本验证的是：

```text
预处理后的 tensor 输入
    -> ONNX Runtime C++ 推理
    -> action 输出
    -> LEFT / RIGHT 方向判断
```

当前版本尚未在 C++ 内部直接完成 JPEG 解码、resize 和 normalize。后续更完整的版本可以扩展为：

```text
frame_*.jpg
    -> C++ JPEG 解码
    -> resize 到 224 x 224
    -> RGB normalize
    -> ONNX Runtime 推理
    -> action 反归一化
```

---

## 15. 结论

本次任务三的核心推理链路已经跑通。

ACT 模型从 PyTorch checkpoint 成功导出为 ONNX，并通过 ONNX Runtime 在 C++ 程序中完成推理。该 C++ 程序进一步在 StarryOS/QEMU riscv64 环境中成功编译和运行。

最终两个代表性样例方向判断均正确：

```text
frame_000000 -> LEFT
frame_000227 -> RIGHT
```

这说明部署后的 StarryOS/QEMU 推理流程能够保持原模型的基本决策方向，任务三核心要求已经完成。
