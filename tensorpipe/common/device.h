/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

namespace tensorpipe_npu {

const std::string kCpuDeviceType{"cpu"};
const std::string kNpuDeviceType{"npu"};

struct Device {
  std::string type;
  int index;

  // This pointless constructor is needed to work around a bug in GCC 5.5 (and
  // possibly other versions). It appears to be needed in the nop types that
  // are used inside nop::Optional.
  Device() {}

  Device(std::string type, int index) : type(std::move(type)), index(index) {}

  std::string toString() const {
    std::stringstream ss;
    ss << type << ":" << index;
    return ss.str();
  }

  bool operator==(const Device& other) const {
    return type == other.type && index == other.index;
  }
};

} // namespace tensorpipe_npu

namespace std {

template <>
struct hash<::tensorpipe_npu::Device> {
  size_t operator()(const ::tensorpipe_npu::Device& device) const noexcept {
    return std::hash<std::string>{}(device.toString());
  }
};

template <>
struct hash<std::pair<::tensorpipe_npu::Device, ::tensorpipe_npu::Device>> {
  size_t operator()(const std::pair<::tensorpipe_npu::Device, ::tensorpipe_npu::Device>&
                        p) const noexcept {
    size_t h1 = std::hash<::tensorpipe_npu::Device>{}(p.first);
    size_t h2 = std::hash<::tensorpipe_npu::Device>{}(p.second);
    // Shifting one hash to avoid collisions between (a, b) and (b, a).
    return h1 ^ (h2 << 1);
  }
};

} // namespace std
