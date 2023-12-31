/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <string>

#include <tensorpipe/channel/context.h>
#include <tensorpipe/transport/context.h>

namespace tensorpipe_npu {
namespace benchmark {

enum class TensorType {
  kCpu,
  kNpu,
};

struct Options {
  std::string mode; // server or client
  std::string transport; // shm or uv
  std::string channel; // basic
  std::string address; // address for listen or connect
  int numRoundTrips{0}; // number of write/read pairs
  size_t numPayloads{0};
  size_t payloadSize{0};
  size_t numTensors{0};
  size_t tensorSize{0};
  TensorType tensorType{TensorType::kCpu};
  size_t metadataSize{0};
  size_t npuSyncPeriod{1};
};

struct Options parseOptions(int argc, char** argv);

} // namespace benchmark
} // namespace tensorpipe_npu
