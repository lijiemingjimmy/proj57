# C++ / Runtime Plan

## Current C++ Status

The repository already contains `src/task3_main.cpp`, a minimal ONNX Runtime C++ inference program. It reads preprocessed binary tensors:

```text
act.onnx
image.bin
state.bin
latent.bin
action_q01.bin
action_q99.bin
```

It outputs the full 8-step action chunk and derives LEFT/RIGHT from the first step.

## Current Limitation

The C++ program does not decode JPEG, resize, normalize, or read raw dataset metadata directly. Those steps are currently handled by `tools/dump_cpp_inputs.py`.

## Short-Term Plan

For initial review, keep the C++ program as a reproducible inference core and document this limitation clearly.

For the next implementation phase:

1. Add command-line arguments:

```text
./task3_act --model act.onnx --image image.bin --state state.bin --latent latent.bin
```

2. Add timing and structured output JSON.
3. Add optional JPEG preprocessing with a small image library.
4. Extend the RKNN Runtime C++ variant with a cleaner CLI and more sample regression.

## RKNN Runtime Direction

The Orange Pi already has:

```text
/usr/lib/librknnrt.so
/usr/local/include/rknn/rknn_api.h
```

The RKNN Runtime C++ variant has been added as `tools/rknn_runtime_infer.cpp`. It loads `act_rk3588_fp.rknn`, prepares three float inputs, runs `rknn_run`, and postprocesses the `[1,8,3]` output exactly like the ONNX program.

Current board results:

```text
frame_000000 -> LEFT   rknn_perf_run_us=34968
frame_000227 -> RIGHT  rknn_perf_run_us=25340
```

The remaining RKNN issue is not correctness but environment polish: the board runtime API is 1.5.2 while the conversion tool is 2.3.2, so the runtime prints version warnings even though outputs and timing are returned.
