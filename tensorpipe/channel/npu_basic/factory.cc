/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/channel/npu_basic/factory.h>

#include <tensorpipe/channel/context_boilerplate.h>
#include <tensorpipe/channel/npu_basic/channel_impl.h>
#include <tensorpipe/channel/npu_basic/context_impl.h>

namespace tensorpipe {
namespace channel {
namespace npu_basic {

std::shared_ptr<Context> create(std::shared_ptr<Context> cpuContext) {
    return std::make_shared<ContextBoilerplate<ContextImpl, ChannelImpl>>(std::move(cpuContext)); 
}

} // namespace npu_basic
} // namespace channel
} // namespace tensorpipe