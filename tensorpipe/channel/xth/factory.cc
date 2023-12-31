/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/channel/xth/factory.h>

#include <tensorpipe/channel/context_boilerplate.h>
#include <tensorpipe/channel/xth/channel_impl.h>
#include <tensorpipe/channel/xth/context_impl.h>

namespace tensorpipe_npu {
namespace channel {
namespace xth {

std::shared_ptr<Context> create() {
  return std::make_shared<ContextBoilerplate<ContextImpl, ChannelImpl>>();
}

} // namespace xth
} // namespace channel
} // namespace tensorpipe_npu
