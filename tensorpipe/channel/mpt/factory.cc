/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/channel/mpt/factory.h>

#include <tensorpipe/channel/context_boilerplate.h>
#include <tensorpipe/channel/mpt/channel_impl.h>
#include <tensorpipe/channel/mpt/context_impl.h>

namespace tensorpipe_npu {
namespace channel {
namespace mpt {

std::shared_ptr<Context> create(
    std::vector<std::shared_ptr<transport::Context>> contexts,
    std::vector<std::shared_ptr<transport::Listener>> listeners) {
  return std::make_shared<ContextBoilerplate<ContextImpl, ChannelImpl>>(
      std::move(contexts), std::move(listeners));
}

} // namespace mpt
} // namespace channel
} // namespace tensorpipe_npu
