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

#include <tensorpipe/channel/context_impl_boilerplate.h>
#include <tensorpipe/common/deferred_executor.h>
#include <tensorpipe/common/device.h>

namespace tensorpipe_npu {
namespace channel {
namespace basic {

class ChannelImpl;

class ContextImpl final
    : public ContextImplBoilerplate<ContextImpl, ChannelImpl> {
 public:
  static std::shared_ptr<ContextImpl> create();

  explicit ContextImpl(
      std::unordered_map<Device, std::string> deviceDescriptors);

  std::shared_ptr<Channel> createChannel(
      std::vector<std::shared_ptr<transport::Connection>> connections,
      Endpoint endpoint);

  // Implement the DeferredExecutor interface.
  bool inLoop() const override;
  void deferToLoop(std::function<void()> fn) override;

 protected:
  // Implement the entry points called by ContextImplBoilerplate.
  void handleErrorImpl() override;
  void joinImpl() override;

 private:
  OnDemandDeferredExecutor loop_;
};

} // namespace basic
} // namespace channel
} // namespace tensorpipe_npu
