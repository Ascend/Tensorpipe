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

#include <tensorpipe/common/error.h>

namespace tensorpipe_npu {
namespace transport {

class ContextClosedError final : public BaseError {
 public:
  ContextClosedError() {}

  std::string what() const override;
};

class ListenerClosedError final : public BaseError {
 public:
  ListenerClosedError() {}

  std::string what() const override;
};

class ConnectionClosedError final : public BaseError {
 public:
  ConnectionClosedError() {}

  std::string what() const override;
};

class ContextNotViableError final : public BaseError {
 public:
  ContextNotViableError() {}

  std::string what() const override;
};

} // namespace transport
} // namespace tensorpipe_npu
