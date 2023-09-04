/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <tensorpipe/channel/context_impl_boilerplate.h>
#include <tensorpipe/common/allocator.h>
#include <tensorpipe/common/npu.h>
#include <tensorpipe/common/npu_buffer.h>
#include <tensorpipe/common/npu_lib.h>
#include <tensorpipe/common/npu_loop.h>
#include <tensorpipe/common/deferred_executor.h>
#include <tensorpipe/common/device.h>
#include <tensorpipe/common/optional.h>

namespace tensorpipe {
namespace channel {
namespace npu_basic {

class ChannelImpl;

class ContextImpl final
    : public ContextImplBoilerplate<ContextImpl, ChannelImpl> {
 public:
  static std::shared_ptr<ContextImpl> create(
      std::shared_ptr<Context> cpuContext);

  ContextImpl(
      NPULib npuLib,
      std::shared_ptr<Context> cpuContext,
      std::unordered_map<Device, std::string> deviceDescriptors);

  std::shared_ptr<Channel> createChannel(
      std::vector<std::shared_ptr<transport::Connection>> connections,
      Endpoint endpoint);

  size_t numConnectionsNeeded() const override;

  bool canCommunicateWithRemote(
      const std::string& localDeviceDescriptor,
      const std::string& remoteDeviceDescriptor) const override;

  const NPULib& getNPULib();
  Allocator& getNPUHostSendAllocator(int deviceIdx);
  Allocator& getNPUHostRecvAllocator(int deviceIdx);

  // Implement the DeferredExecutor interface.
  bool inLoop() const override;
  void deferToLoop(std::function<void()> fn) override;

 protected:
  // Implement the entry points called by ContextImplBoilerplate.
  void handleErrorImpl() override;
  void joinImpl() override;
  void setIdImpl() override;

 private:
  OnDemandDeferredExecutor loop_;

  const NPULib npuLib_;

  const std::shared_ptr<Context> cpuContext_;
  // TODO: Lazy initialization of cuda loop.
  NPULoop npuLoop_;

  struct NPUHostAllocator {
    NPUPinnedBuffer buffer;
    Allocator allocator;
  };
  optional<NPUHostAllocator> npuHostSendAllocator_;
  optional<NPUHostAllocator> npuHostRecvAllocator_;
};

} // namespace npu_basic
} // namespace channel
} // namespace tensorpipe

