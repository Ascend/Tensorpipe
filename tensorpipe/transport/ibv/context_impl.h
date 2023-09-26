/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <tuple>

#include <tensorpipe/common/epoll_loop.h>
#include <tensorpipe/transport/context_impl_boilerplate.h>
#include <tensorpipe/transport/ibv/reactor.h>

namespace tensorpipe_npu {
namespace transport {
namespace ibv {

class ConnectionImpl;
class ListenerImpl;

class ContextImpl final
    : public ContextImplBoilerplate<ContextImpl, ListenerImpl, ConnectionImpl> {
 public:
  static std::shared_ptr<ContextImpl> create();

  ContextImpl(IbvLib ibvLib, IbvDeviceList deviceList);

  // Implement the DeferredExecutor interface.
  bool inLoop() const override;
  void deferToLoop(std::function<void()> fn) override;

  void registerDescriptor(
      int fd,
      int events,
      std::shared_ptr<EpollLoop::EventHandler> h);

  void unregisterDescriptor(int fd);

  Reactor& getReactor();

 protected:
  // Implement the entry points called by ContextImplBoilerplate.
  void handleErrorImpl() override;
  void joinImpl() override;

 private:
  Reactor reactor_;
  EpollLoop loop_{this->reactor_};
};

} // namespace ibv
} // namespace transport
} // namespace tensorpipe_npu
