/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/transport/ibv/factory.h>

#include <tensorpipe/transport/context_boilerplate.h>
#include <tensorpipe/transport/ibv/connection_impl.h>
#include <tensorpipe/transport/ibv/context_impl.h>
#include <tensorpipe/transport/ibv/listener_impl.h>

namespace tensorpipe_npu {
namespace transport {
namespace ibv {

std::shared_ptr<Context> create() {
  return std::make_shared<
      ContextBoilerplate<ContextImpl, ListenerImpl, ConnectionImpl>>();
}

} // namespace ibv
} // namespace transport
} // namespace tensorpipe_npu
