/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/benchmark/channel_registry.h>

#include <tensorpipe/tensorpipe.h>

TP_DEFINE_SHARED_REGISTRY(
    TensorpipeChannelRegistry,
    tensorpipe::channel::Context);

// BASIC

std::shared_ptr<tensorpipe::channel::Context> makeBasicChannel() {
  return tensorpipe::channel::basic::create();
}

TP_REGISTER_CREATOR(TensorpipeChannelRegistry, basic, makeBasicChannel);

// CMA

#if TENSORPIPE_HAS_CMA_CHANNEL
std::shared_ptr<tensorpipe::channel::Context> makeCmaChannel() {
  return tensorpipe::channel::cma::create();
}

TP_REGISTER_CREATOR(TensorpipeChannelRegistry, cma, makeCmaChannel);
#endif // TENSORPIPE_HAS_CMA_CHANNEL

// MPT

std::shared_ptr<tensorpipe::channel::Context> makeMptChannel() {
  throw std::runtime_error("mtp channel requires arguments");
}

TP_REGISTER_CREATOR(TensorpipeChannelRegistry, mpt, makeMptChannel);

// XTH

std::shared_ptr<tensorpipe::channel::Context> makeXthChannel() {
  return tensorpipe::channel::xth::create();
}

TP_REGISTER_CREATOR(TensorpipeChannelRegistry, xth, makeXthChannel);

void validateChannelContext(
    std::shared_ptr<tensorpipe::channel::Context> context) {
  if (!context) {
    auto keys = TensorpipeChannelRegistry().keys();
    std::cout
        << "The channel you passed in is not supported. The following channels are valid: ";
    for (const auto& key : keys) {
      std::cout << key << ", ";
    }
    std::cout << "\n";
    exit(EXIT_FAILURE);
  }
}
