/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>
#include <vector>

#include <tensorpipe/channel/context.h>
#include <tensorpipe/transport/context.h>

namespace tensorpipe_npu {
namespace channel {
namespace mpt {

std::shared_ptr<Context> create(
    std::vector<std::shared_ptr<transport::Context>> contexts,
    std::vector<std::shared_ptr<transport::Listener>> listeners);

} // namespace mpt
} // namespace channel
} // namespace tensorpipe_npu
