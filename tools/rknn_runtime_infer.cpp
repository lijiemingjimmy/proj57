#include <rknn_api.h>

#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::vector<uint8_t> read_file(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    throw std::runtime_error("failed to open: " + path);
  }
  in.seekg(0, std::ios::end);
  const auto size = in.tellg();
  in.seekg(0, std::ios::beg);
  std::vector<uint8_t> data(static_cast<size_t>(size));
  if (!data.empty()) {
    in.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
  }
  return data;
}

std::vector<uint8_t> nchw_fp32_to_nhwc_fp32(const std::vector<uint8_t>& nchw_bytes,
                                            int channels,
                                            int height,
                                            int width) {
  const size_t elems = static_cast<size_t>(channels) * height * width;
  if (nchw_bytes.size() != elems * sizeof(float)) {
    throw std::runtime_error("unexpected NCHW image byte size");
  }
  const auto* src = reinterpret_cast<const float*>(nchw_bytes.data());
  std::vector<uint8_t> out(nchw_bytes.size());
  auto* dst = reinterpret_cast<float*>(out.data());
  for (int h = 0; h < height; ++h) {
    for (int w = 0; w < width; ++w) {
      for (int c = 0; c < channels; ++c) {
        const size_t src_idx = static_cast<size_t>(c) * height * width + h * width + w;
        const size_t dst_idx = (static_cast<size_t>(h) * width + w) * channels + c;
        dst[dst_idx] = src[src_idx];
      }
    }
  }
  return out;
}

void check_ret(int ret, const std::string& what) {
  if (ret != RKNN_SUCC) {
    throw std::runtime_error(what + " failed, ret=" + std::to_string(ret));
  }
}

void print_attr(const char* kind, const rknn_tensor_attr& attr) {
  std::cout << kind << "[" << attr.index << "] name=" << attr.name << " dims=";
  for (uint32_t i = 0; i < attr.n_dims; ++i) {
    std::cout << (i == 0 ? "" : "x") << attr.dims[i];
  }
  std::cout << " elems=" << attr.n_elems << " size=" << attr.size
            << " fmt=" << get_format_string(attr.fmt)
            << " type=" << get_type_string(attr.type)
            << " qnt=" << get_qnt_type_string(attr.qnt_type) << "\n";
}

std::vector<uint8_t>* choose_input(const rknn_tensor_attr& attr,
                                   std::vector<uint8_t>& image,
                                   std::vector<uint8_t>& state,
                                   std::vector<uint8_t>& latent) {
  const uint32_t fp32_bytes = attr.n_elems * sizeof(float);
  std::string name(attr.name);
  if (name.find("image") != std::string::npos || fp32_bytes == image.size()) {
    return &image;
  }
  if (name.find("state") != std::string::npos || fp32_bytes == state.size()) {
    return &state;
  }
  if (name.find("latent") != std::string::npos || fp32_bytes == latent.size()) {
    return &latent;
  }
  return nullptr;
}

bool is_image_input(const rknn_tensor_attr& attr, const std::vector<uint8_t>& image) {
  const uint32_t fp32_bytes = attr.n_elems * sizeof(float);
  std::string name(attr.name);
  return name.find("image") != std::string::npos || fp32_bytes == image.size();
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 7) {
    std::cerr << "usage: " << argv[0]
              << " <model.rknn> <image.bin> <state.bin> <latent.bin>"
              << " <action_q01.bin> <action_q99.bin>\n";
    return 2;
  }

  try {
    auto model = read_file(argv[1]);
    auto image = read_file(argv[2]);
    auto state = read_file(argv[3]);
    auto latent = read_file(argv[4]);
    auto image_nhwc = nchw_fp32_to_nhwc_fp32(image, 3, 224, 224);
    auto action_q01_bytes = read_file(argv[5]);
    auto action_q99_bytes = read_file(argv[6]);
    if (action_q01_bytes.size() < 3 * sizeof(float) ||
        action_q99_bytes.size() < 3 * sizeof(float)) {
      throw std::runtime_error("action q01/q99 files must contain at least three float32 values");
    }
    const auto* action_q01 = reinterpret_cast<const float*>(action_q01_bytes.data());
    const auto* action_q99 = reinterpret_cast<const float*>(action_q99_bytes.data());

    std::cout << "model_bytes=" << model.size() << "\n";
    std::cout << "image_bytes=" << image.size() << " state_bytes=" << state.size()
              << " latent_bytes=" << latent.size() << "\n";

    rknn_context ctx = 0;
    check_ret(rknn_init(&ctx, model.data(), static_cast<uint32_t>(model.size()),
                        RKNN_FLAG_COLLECT_PERF_MASK, nullptr),
              "rknn_init");

    rknn_sdk_version version;
    std::memset(&version, 0, sizeof(version));
    if (rknn_query(ctx, RKNN_QUERY_SDK_VERSION, &version, sizeof(version)) == RKNN_SUCC) {
      std::cout << "api_version=" << version.api_version << "\n";
      std::cout << "drv_version=" << version.drv_version << "\n";
    }

    rknn_input_output_num io_num;
    std::memset(&io_num, 0, sizeof(io_num));
    check_ret(rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num)),
              "rknn_query in/out num");
    std::cout << "n_input=" << io_num.n_input << " n_output=" << io_num.n_output << "\n";

    std::vector<rknn_tensor_attr> input_attrs(io_num.n_input);
    std::vector<rknn_input> inputs(io_num.n_input);
    for (uint32_t i = 0; i < io_num.n_input; ++i) {
      std::memset(&input_attrs[i], 0, sizeof(rknn_tensor_attr));
      input_attrs[i].index = i;
      check_ret(rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &input_attrs[i], sizeof(rknn_tensor_attr)),
                "rknn_query input attr");
      print_attr("input", input_attrs[i]);

      auto* buf = choose_input(input_attrs[i], image_nhwc, state, latent);
      if (buf == nullptr) {
        throw std::runtime_error("could not match input tensor: " + std::string(input_attrs[i].name));
      }

      std::memset(&inputs[i], 0, sizeof(rknn_input));
      inputs[i].index = i;
      inputs[i].buf = buf->data();
      inputs[i].size = static_cast<uint32_t>(buf->size());
      inputs[i].pass_through = 0;
      inputs[i].type = RKNN_TENSOR_FLOAT32;
      // The dumped image tensor is NCHW float32; convert it to NHWC because the
      // converted RKNN model exposes its image input as NHWC.
      if (is_image_input(input_attrs[i], image_nhwc)) {
        inputs[i].fmt = RKNN_TENSOR_NHWC;
      } else {
        inputs[i].fmt = input_attrs[i].fmt == RKNN_TENSOR_UNDEFINED ? RKNN_TENSOR_NCHW : input_attrs[i].fmt;
      }
    }

    std::vector<rknn_tensor_attr> output_attrs(io_num.n_output);
    for (uint32_t i = 0; i < io_num.n_output; ++i) {
      std::memset(&output_attrs[i], 0, sizeof(rknn_tensor_attr));
      output_attrs[i].index = i;
      check_ret(rknn_query(ctx, RKNN_QUERY_OUTPUT_ATTR, &output_attrs[i], sizeof(rknn_tensor_attr)),
                "rknn_query output attr");
      print_attr("output", output_attrs[i]);
    }

    check_ret(rknn_inputs_set(ctx, io_num.n_input, inputs.data()), "rknn_inputs_set");

    const auto t0 = std::chrono::steady_clock::now();
    check_ret(rknn_run(ctx, nullptr), "rknn_run");

    std::vector<rknn_output> outputs(io_num.n_output);
    for (uint32_t i = 0; i < io_num.n_output; ++i) {
      std::memset(&outputs[i], 0, sizeof(rknn_output));
      outputs[i].index = i;
      outputs[i].want_float = 1;
      outputs[i].is_prealloc = 0;
    }
    check_ret(rknn_outputs_get(ctx, io_num.n_output, outputs.data(), nullptr),
              "rknn_outputs_get");
    const auto t1 = std::chrono::steady_clock::now();

    rknn_perf_run perf;
    std::memset(&perf, 0, sizeof(perf));
    if (rknn_query(ctx, RKNN_QUERY_PERF_RUN, &perf, sizeof(perf)) == RKNN_SUCC) {
      std::cout << "rknn_perf_run_us=" << perf.run_duration << "\n";
    }
    const double wall_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    std::cout << "wall_ms=" << std::fixed << std::setprecision(3) << wall_ms << "\n";

    for (uint32_t i = 0; i < io_num.n_output; ++i) {
      const auto* data = static_cast<const float*>(outputs[i].buf);
      const uint32_t elems = output_attrs[i].n_elems;
      std::cout << "output[" << i << "] raw_first_values=";
      const uint32_t limit = elems < 24 ? elems : 24;
      for (uint32_t j = 0; j < limit; ++j) {
        std::cout << (j == 0 ? "" : ",") << std::setprecision(6) << data[j];
      }
      std::cout << "\n";
      if (elems >= 3) {
        float denorm[3];
        for (int j = 0; j < 3; ++j) {
          denorm[j] = (data[j] + 1.0f) * 0.5f * (action_q99[j] - action_q01[j]) + action_q01[j];
        }
        const char* dir = denorm[0] < denorm[1] ? "LEFT" : "RIGHT";
        std::cout << "first_step_denorm_left=" << std::setprecision(6) << denorm[0]
                  << " right=" << denorm[1]
                  << " grip=" << denorm[2]
                  << " direction=" << dir << "\n";
      }
    }

    rknn_outputs_release(ctx, io_num.n_output, outputs.data());
    rknn_destroy(ctx);
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
}
