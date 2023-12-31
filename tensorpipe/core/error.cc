/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/core/error.h>

#include <sstream>

namespace tensorpipe_npu {

std::string LogicError::what() const {
  std::ostringstream ss;
  ss << "logic error: " << reason_;
  return ss.str();
}

std::string ContextClosedError::what() const {
  return "context closed";
}

std::string ListenerClosedError::what() const {
  return "listener closed";
}

std::string PipeClosedError::what() const {
  return "pipe closed";
}

} // namespace tensorpipe_npu
