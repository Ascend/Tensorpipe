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

class LogicError final : public BaseError {
 public:
  explicit LogicError(std::string reason) : reason_(std::move(reason)) {}

  std::string what() const override;

 private:
  const std::string reason_;
};

class ContextClosedError final : public BaseError {
 public:
  explicit ContextClosedError() {}

  std::string what() const override;
};

class ListenerClosedError final : public BaseError {
 public:
  explicit ListenerClosedError() {}

  std::string what() const override;
};

class PipeClosedError final : public BaseError {
 public:
  explicit PipeClosedError() {}

  std::string what() const override;
};

} // namespace tensorpipe_npu
