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

#include <tensorpipe/transport/error.h>

namespace tensorpipe_npu {
namespace transport {
namespace uv {

class UVError final : public BaseError {
 public:
  explicit UVError(int error) : error_(error) {}

  std::string what() const override;

 private:
  int error_;
};

class NoAddrFoundError final : public BaseError {
 public:
  NoAddrFoundError() {}

  std::string what() const override;
};

} // namespace uv
} // namespace transport
} // namespace tensorpipe_npu
