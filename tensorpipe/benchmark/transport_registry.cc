/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/benchmark/transport_registry.h>

#include <tensorpipe/tensorpipe.h>

TP_DEFINE_SHARED_REGISTRY(
    TensorpipeTransportRegistry,
    tensorpipe_npu::transport::Context);

// IBV

#if TENSORPIPE_HAS_IBV_TRANSPORT
std::shared_ptr<tensorpipe_npu::transport::Context> makeIbvContext() {
  return tensorpipe_npu::transport::ibv::create();
}

TP_REGISTER_CREATOR(TensorpipeTransportRegistry, ibv, makeIbvContext);
#endif // TENSORPIPE_HAS_IBV_TRANSPORT

// SHM

#if TENSORPIPE_HAS_SHM_TRANSPORT
std::shared_ptr<tensorpipe_npu::transport::Context> makeShmContext() {
  return tensorpipe_npu::transport::shm::create();
}

TP_REGISTER_CREATOR(TensorpipeTransportRegistry, shm, makeShmContext);
#endif // TENSORPIPE_HAS_SHM_TRANSPORT

// UV

std::shared_ptr<tensorpipe_npu::transport::Context> makeUvContext() {
  return tensorpipe_npu::transport::uv::create();
}

TP_REGISTER_CREATOR(TensorpipeTransportRegistry, uv, makeUvContext);

void validateTransportContext(
    std::shared_ptr<tensorpipe_npu::transport::Context> context) {
  if (!context) {
    auto keys = TensorpipeTransportRegistry().keys();
    std::cout
        << "The transport you passed in is not supported. The following transports are valid: ";
    for (const auto& key : keys) {
      std::cout << key << ", ";
    }
    std::cout << "\n";
    exit(EXIT_FAILURE);
  }
}
