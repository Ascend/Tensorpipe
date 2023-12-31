/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/transport/uv/error.h>

#include <tensorpipe/transport/uv/uv.h>

namespace tensorpipe_npu {
namespace transport {
namespace uv {

std::string UVError::what() const {
  return formatUvError(error_);
}

std::string NoAddrFoundError::what() const {
  return "no address found";
}

} // namespace uv
} // namespace transport
} // namespace tensorpipe_npu
