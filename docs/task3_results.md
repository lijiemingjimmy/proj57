# Proj57 Task 3: ACT Inference on StarryOS/QEMU

## 1. Overview

This project completes **Task 3** of Proj57: running ACT model inference on **StarryOS/QEMU** using the **CPU** backend.

The core pipeline is:

```text
PyTorch checkpoint
    -> ONNX export
    -> ONNX Runtime validation
    -> C++ inference program
    -> StarryOS/QEMU riscv64 execution
    -> left/right action decision
```

The current implementation successfully runs ACT inference inside StarryOS/QEMU and produces correct direction decisions for representative left-turn and right-turn samples.

## 2. Current Status

Completed:

```text
[OK] PyTorch notebook reference inference
[OK] PyTorch checkpoint exported to ONNX
[OK] ONNX Runtime Python validation
[OK] ONNX Runtime C++ validation on host machine
[OK] StarryOS/QEMU riscv64 boot
[OK] riscv64 hello-world user program execution in StarryOS
[OK] ONNX Runtime installed inside StarryOS through apk
[OK] C++ ACT inference program compiled inside StarryOS
[OK] ACT inference executed inside StarryOS/QEMU
[OK] LEFT/RIGHT direction decisions match reference behavior
```

Representative results:

```text
frame_000000 -> LEFT
frame_000227 -> RIGHT
```

## 3. Repository Layout

Main files added for Task 3:

```text
tools/
├── export_act_onnx.py       # Export PyTorch ACT checkpoint to ONNX
├── check_onnx.py            # Validate ONNX model using Python ONNX Runtime
└── dump_cpp_inputs.py       # Dump preprocessed tensors for C++ inference

src/
└── task3_main.cpp           # C++ ONNX Runtime inference program

build/
├── act.onnx                 # Exported ONNX model
└── cpp_test/
    ├── task3_act            # Host-side C++ executable
    ├── act.onnx
    ├── image.bin
    ├── state.bin
    ├── latent.bin
    ├── action_q01.bin
    └── action_q99.bin

docs/
└── task3_results.md         # Task 3 experimental results
```

## 4. Model and Data

The original repository provides:

```text
output/
├── dataset/
│   ├── data/
│   ├── meta/stats.json
│   └── videos/observation.images.fpv/chunk-000/frame_*.jpg
└── train/
    └── model.pt
```

Important files:

```text
output/train/model.pt
output/dataset/meta/stats.json
output/dataset/videos/observation.images.fpv/chunk-000/frame_000000.jpg
output/dataset/videos/observation.images.fpv/chunk-000/frame_000227.jpg
```

The two representative samples are:

```text
frame_000000.jpg -> expected LEFT
frame_000227.jpg -> expected RIGHT
```

## 5. ONNX Export

The PyTorch checkpoint is exported to ONNX:

```text
output/train/model.pt -> build/act.onnx
```

Run:

```bash
python tools/export_act_onnx.py
```

Expected output:

```text
ONNX 导出完成: build/act.onnx
文件大小: approximately 193 MB
```

The exported ONNX model uses explicit inputs:

```text
image  : [1, 3, 224, 224]
state  : [1, 2]
latent : [1, 32]
```

Output:

```text
action : [1, 8, 3]
```

Each action step contains:

```text
[left_vel, right_vel, gripper_target]
```

## 6. ONNX Runtime Python Validation

Run:

```bash
python tools/check_onnx.py
```

Expected behavior:

```text
frame_000000.jpg -> LEFT
frame_000227.jpg -> RIGHT
```

This step verifies that the exported ONNX model preserves the PyTorch reference direction behavior.

## 7. C++ Host Validation

First dump preprocessed input tensors:

```bash
python tools/dump_cpp_inputs.py --frame 227
```

Then build and run the C++ program on the host machine.

Example on macOS with Homebrew ONNX Runtime:

```bash
brew install onnxruntime

export ORT_PREFIX="$(brew --prefix onnxruntime)"
export ORT_INCLUDE_DIR="$(dirname "$(find "$ORT_PREFIX/include" -name onnxruntime_cxx_api.h | head -1)")"

clang++ -std=c++17 -O2 src/task3_main.cpp \
  -I"$ORT_INCLUDE_DIR" \
  -L"$ORT_PREFIX/lib" \
  -lonnxruntime \
  -Wl,-rpath,"$ORT_PREFIX/lib" \
  -o build/cpp_test/task3_act
```

Run:

```bash
cd build/cpp_test
./task3_act
```

Host-side C++ validation results:

```text
frame_000000 -> LEFT
frame_000227 -> RIGHT
```

## 8. StarryOS/QEMU Setup

Clone and build StarryOS:

```bash
git clone --recursive https://github.com/Starry-OS/StarryOS.git
cd StarryOS
```

On Apple Silicon Mac, the official Docker image should be run as `linux/amd64`:

```bash
docker pull --platform linux/amd64 ghcr.io/arceos-org/arceos-build

docker run --platform linux/amd64 -it --rm \
  -v "$(pwd)":/workspace \
  -w /workspace \
  ghcr.io/arceos-org/arceos-build
```

Inside the Docker container:

```bash
make ARCH=riscv64 rootfs
make ARCH=riscv64 build
make ARCH=riscv64 run
```

Successful boot shows:

```text
Welcome to Starry OS!
starry:~#
```

## 9. Transfer Files into StarryOS

A simple HTTP server was used to transfer files from the Docker container to the StarryOS guest.

Inside the Docker container:

```bash
cd /workspace
python3 -m http.server 8000 > /tmp/proj57_http.log 2>&1 &
```

Copy files from the host Proj57 repository into the Docker container:

```bash
docker cp /Users/lijieming/do_something/proj57/src/task3_main.cpp <container_id>:/workspace/task3_main.cpp

docker cp /Users/lijieming/do_something/proj57/build/cpp_test/act.onnx <container_id>:/workspace/act.onnx
docker cp /Users/lijieming/do_something/proj57/build/cpp_test/image.bin <container_id>:/workspace/image.bin
docker cp /Users/lijieming/do_something/proj57/build/cpp_test/state.bin <container_id>:/workspace/state.bin
docker cp /Users/lijieming/do_something/proj57/build/cpp_test/latent.bin <container_id>:/workspace/latent.bin
docker cp /Users/lijieming/do_something/proj57/build/cpp_test/action_q01.bin <container_id>:/workspace/action_q01.bin
docker cp /Users/lijieming/do_something/proj57/build/cpp_test/action_q99.bin <container_id>:/workspace/action_q99.bin
```

Inside StarryOS:

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

## 10. Install ONNX Runtime and Compiler in StarryOS

Inside StarryOS:

```bash
apk update
apk add onnxruntime onnxruntime-dev
apk add g++ make
```

Verify:

```bash
apk info | grep onnxruntime
g++ --version
find /usr/include -name "onnxruntime_cxx_api.h"
find /usr/lib -name "libonnxruntime*"
```

Observed packages:

```text
onnxruntime
onnxruntime-dev
```

Observed compiler:

```text
g++ (Alpine 15.2.0) 15.2.0
```

Observed ONNX Runtime libraries:

```text
/usr/lib/libonnxruntime.so
/usr/lib/libonnxruntime.so.1
/usr/lib/libonnxruntime.so.1.23.0
/usr/lib/libonnxruntime_providers_shared.so
```

## 11. Compile C++ Inference Program in StarryOS

Inside StarryOS:

```bash
ORT_HEADER=$(find /usr/include -name "onnxruntime_cxx_api.h" | head -1)
ORT_INCLUDE_DIR=$(dirname "$ORT_HEADER")

g++ -std=c++17 -O2 task3_main.cpp \
  -I"$ORT_INCLUDE_DIR" \
  -L/usr/lib \
  -lonnxruntime \
  -pthread \
  -o task3_act
```

Executable size:

```text
task3_act = 26.8K
```

Run:

```bash
export LD_LIBRARY_PATH=/usr/lib:$LD_LIBRARY_PATH
./task3_act
```

## 12. StarryOS/QEMU Inference Results

### 12.1 Right-turn sample: frame_000227

Input was generated from:

```text
frame_000227.jpg
```

StarryOS output:

```text
step 0:
left_vel  = +0.00611769
right_vel = -0.000454509
gripper   ≈ 0
direction = RIGHT

infer_ms = 18442.2
```

Result:

```text
PASS
```

### 12.2 Left-turn sample: frame_000000

Input was generated from:

```text
frame_000000.jpg
```

StarryOS output:

```text
step 0:
left_vel  = -0.0015857
right_vel = +0.0072386
gripper   ≈ 0
direction = LEFT

infer_ms = 18657.4
```

Result:

```text
PASS
```

## 13. File Sizes

Inside StarryOS:

```text
act.onnx          193.2M
task3_act          26.8K
task3_main.cpp      5.1K
image.bin         588.0K
state.bin             8B
latent.bin          128B
action_q01.bin       12B
action_q99.bin       12B
```

## 14. Runtime Warnings

During execution, StarryOS prints:

```text
Unimplemented syscall: riscv_hwprobe
```

ONNX Runtime also prints:

```text
Unknown CPU vendor. cpuinfo_vendor value: 0
```

These warnings do not stop inference. ONNX Runtime successfully creates the inference session and runs the model.

## 15. Limitations

The current C++ program reads preprocessed binary tensors:

```text
image.bin
state.bin
latent.bin
```

Therefore, the current implementation verifies the core inference pipeline:

```text
preprocessed tensor input
    -> ONNX Runtime C++ inference
    -> action output
    -> direction decision
```

It does not yet perform full JPEG preprocessing inside C++.

A more complete version should implement:

```text
frame_*.jpg
    -> C++ JPEG decoding
    -> resize to 224 x 224
    -> RGB normalization
    -> ONNX Runtime inference
    -> action denormalization
```

## 16. AI Usage Statement

AI assistance was used during the development process for:

```text
1. understanding the task requirements and repository structure
2. designing the deployment pipeline
3. writing ONNX export and validation scripts
4. writing the C++ ONNX Runtime inference program
5. debugging StarryOS/QEMU execution steps
6. organizing the final documentation
```

Human work included:

```text
1. running the notebook reference inference
2. executing ONNX export and validation
3. compiling and testing the C++ program
4. booting StarryOS/QEMU
5. installing ONNX Runtime inside StarryOS
6. running final inference experiments
7. verifying LEFT/RIGHT correctness
```

## 17. Conclusion

Task 3 core inference was successfully completed.

The ACT model was exported from PyTorch to ONNX, loaded by ONNX Runtime in a C++ program, compiled and executed inside StarryOS/QEMU on riscv64 CPU.

Both representative direction samples are correct:

```text
frame_000000 -> LEFT
frame_000227 -> RIGHT
```

This confirms that the deployed inference pipeline preserves the intended left/right action behavior on StarryOS/QEMU.
