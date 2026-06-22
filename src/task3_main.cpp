#include <onnxruntime_cxx_api.h>

#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>


static std::vector<float> read_f32(const std::string& path, size_t n) {
    std::vector<float> data(n);

    std::ifstream fin(path, std::ios::binary);
    if (!fin) {
        throw std::runtime_error("cannot open file: " + path);
    }

    fin.read(reinterpret_cast<char*>(data.data()), n * sizeof(float));

    if (fin.gcount() != static_cast<std::streamsize>(n * sizeof(float))) {
        throw std::runtime_error("bad file size or bad read: " + path);
    }

    return data;
}


static float denorm(float x, float q01, float q99) {
    float denom = q99 - q01;
    if (denom == 0.0f) {
        denom = 1e-8f;
    }
    return (x + 1.0f) * 0.5f * denom + q01;
}


static std::string direction_from_lr(float left, float right) {
    if (left < right) {
        return "LEFT";
    } else if (left > right) {
        return "RIGHT";
    } else {
        return "STRAIGHT";
    }
}


int main() {
    try {
        const std::string model_path = "act.onnx";

        std::cout << "loading input files...\n";

        std::vector<float> image = read_f32("image.bin", 1 * 3 * 224 * 224);
        std::vector<float> state = read_f32("state.bin", 1 * 2);
        std::vector<float> latent = read_f32("latent.bin", 1 * 32);

        std::vector<float> action_q01 = read_f32("action_q01.bin", 3);
        std::vector<float> action_q99 = read_f32("action_q99.bin", 3);

        std::cout << "creating ONNX Runtime session...\n";

        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "proj57_task3");
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

        Ort::Session session(env, model_path.c_str(), session_options);

        Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(
            OrtArenaAllocator,
            OrtMemTypeDefault
        );

        std::array<int64_t, 4> image_shape = {1, 3, 224, 224};
        std::array<int64_t, 2> state_shape = {1, 2};
        std::array<int64_t, 2> latent_shape = {1, 32};

        Ort::Value image_tensor = Ort::Value::CreateTensor<float>(
            memory_info,
            image.data(),
            image.size(),
            image_shape.data(),
            image_shape.size()
        );

        Ort::Value state_tensor = Ort::Value::CreateTensor<float>(
            memory_info,
            state.data(),
            state.size(),
            state_shape.data(),
            state_shape.size()
        );

        Ort::Value latent_tensor = Ort::Value::CreateTensor<float>(
            memory_info,
            latent.data(),
            latent.size(),
            latent_shape.data(),
            latent_shape.size()
        );

        const char* input_names[] = {"image", "state", "latent"};
        const char* output_names[] = {"action"};

        std::vector<Ort::Value> input_tensors;
        input_tensors.emplace_back(std::move(image_tensor));
        input_tensors.emplace_back(std::move(state_tensor));
        input_tensors.emplace_back(std::move(latent_tensor));

        std::cout << "running inference...\n";

        auto t0 = std::chrono::high_resolution_clock::now();

        std::vector<Ort::Value> output_tensors = session.Run(
            Ort::RunOptions{nullptr},
            input_names,
            input_tensors.data(),
            input_tensors.size(),
            output_names,
            1
        );

        auto t1 = std::chrono::high_resolution_clock::now();

        float* action = output_tensors[0].GetTensorMutableData<float>();

        std::cout << "full 8-step action chunk:\n";

        for (int step = 0; step < 8; ++step) {
            float left_norm = action[step * 3 + 0];
            float right_norm = action[step * 3 + 1];
            float grip_norm = action[step * 3 + 2];

            float left = denorm(left_norm, action_q01[0], action_q99[0]);
            float right = denorm(right_norm, action_q01[1], action_q99[1]);
            float grip = denorm(grip_norm, action_q01[2], action_q99[2]);

            std::string dir = direction_from_lr(left, right);

            std::cout
                << "step " << step
                << ": left_vel=" << left
                << ", right_vel=" << right
                << ", gripper=" << grip
                << ", direction=" << dir
                << "\n";
        }

        float first_left = denorm(action[0], action_q01[0], action_q99[0]);
        float first_right = denorm(action[1], action_q01[1], action_q99[1]);

        std::string first_dir = direction_from_lr(first_left, first_right);

        double infer_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

        std::cout << "====================================\n";
        std::cout << "first step direction: " << first_dir << "\n";
        std::cout << "infer_ms: " << infer_ms << "\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}
