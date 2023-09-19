/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/common/npu_buffer.h>

#include <tensorpipe/common/npu.h>
#include <tensorpipe/common/defs.h>
#include <tensorpipe/common/device_id.h>

namespace tensorpipe {

Device NPUBuffer::getDevice() const {
  static NPULib npuLib = []() {
    Error error;
    NPULib lib;
    std::tie(error, lib) = NPULib::create();
    TP_THROW_ASSERT_IF(error)
        << "Cannot get NPU device for pointer because lib could not be loaded: "
        << error.what();
    return lib;
  }();

  return Device{kNpuDeviceType, getDeviceId()};
}

} // namespace tensorpipe

