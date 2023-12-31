/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/channel/npu_basic/context_impl.h>

#include <functional>
#include <memory>
#include <utility>

#include <tensorpipe/channel/npu_basic/channel_impl.h>
#include <tensorpipe/channel/npu_basic/constants.h>
#include <tensorpipe/channel/helpers.h>
#include <tensorpipe/common/npu.h>
#include <tensorpipe/common/nop.h>

namespace tensorpipe_npu {
namespace channel {
namespace npu_basic {

namespace {

struct DeviceDescriptor {
  std::string deviceType;
  std::string descriptor;
  NOP_STRUCTURE(DeviceDescriptor, deviceType, descriptor);
};

DeviceDescriptor deserializeDeviceDescriptor(
    const std::string& deviceDescriptor) {
  NopHolder<DeviceDescriptor> nopHolder;
  loadDescriptor(nopHolder, deviceDescriptor);
  return std::move(nopHolder.getObject());
}

} // namespace

std::shared_ptr<ContextImpl> ContextImpl::create(
    std::shared_ptr<Context> cpuContext) {
  Error error;
  NPULib npuLib;
  std::tie(error, npuLib) = NPULib::create();
  if (error) {
    TP_VLOG(5)
        << "NPU basic channel is not viable because lib could not be loaded: "
        << error.what();
    return nullptr;
  }

  if (cpuContext->deviceDescriptors().count(Device{kCpuDeviceType, 0}) == 0) {
    TP_THROW_ASSERT() << "NPU basic channel needs a CPU channel";

    return nullptr;
  }

  if (!cpuContext->isViable()) {
    return nullptr;
  }

  std::unordered_map<Device, std::string> deviceDescriptors;
  // NOTE: Assume there is only one CPU.
  TP_DCHECK_EQ(
      cpuContext->deviceDescriptors().count(Device{kCpuDeviceType, 0}), 1);
  const auto cpuDeviceDescriptor =
      cpuContext->deviceDescriptors().begin()->second;

  NopHolder<DeviceDescriptor> nopHolder;
  DeviceDescriptor& deviceDescriptor = nopHolder.getObject();
  deviceDescriptor.descriptor = cpuDeviceDescriptor;

  deviceDescriptor.deviceType = kCpuDeviceType;
  deviceDescriptors[Device{kCpuDeviceType, 0}] = saveDescriptor(nopHolder);
  for (const auto& device : getNPUDevices(npuLib)) {
    deviceDescriptor.deviceType = kNpuDeviceType;
    deviceDescriptors[device] = saveDescriptor(nopHolder);
  }

  return std::make_shared<ContextImpl>(
      std::move(npuLib), std::move(cpuContext), std::move(deviceDescriptors));
}

ContextImpl::ContextImpl(
    NPULib npuLib,
    std::shared_ptr<Context> cpuContext,
    std::unordered_map<Device, std::string> deviceDescriptors)
    : ContextImplBoilerplate<ContextImpl, ChannelImpl>(
          std::move(deviceDescriptors)),
      npuLib_(std::move(npuLib)),
      cpuContext_(std::move(cpuContext)) {}

std::shared_ptr<Channel> ContextImpl::createChannel(
    std::vector<std::shared_ptr<transport::Connection>> connections,
    Endpoint endpoint) {
  TP_DCHECK_EQ(numConnectionsNeeded(), connections.size());
  auto conn = std::move(connections.back());
  connections.pop_back();
  auto cpuChannel =
      cpuContext_->createChannel(std::move(connections), endpoint);
  return createChannelInternal(
      std::move(conn), std::move(cpuChannel), npuLoop_);
}

size_t ContextImpl::numConnectionsNeeded() const {
  return 1 + cpuContext_->numConnectionsNeeded();
}

bool ContextImpl::canCommunicateWithRemote(
    const std::string& localDeviceDescriptor,
    const std::string& remoteDeviceDescriptor) const {
  DeviceDescriptor nopLocalDeviceDescriptor =
      deserializeDeviceDescriptor(localDeviceDescriptor);
  DeviceDescriptor nopRemoteDeviceDescriptor =
      deserializeDeviceDescriptor(remoteDeviceDescriptor);

  // Prevent NpuBasic from being mistakenly used for CPU to CPU transfers, as
  // there are always better options.
  if (nopLocalDeviceDescriptor.deviceType == kCpuDeviceType &&
      nopRemoteDeviceDescriptor.deviceType == kCpuDeviceType) {
    return false;
  }

  return nopLocalDeviceDescriptor.descriptor ==
      nopRemoteDeviceDescriptor.descriptor;
}

const NPULib& ContextImpl::getNPULib() {
  return npuLib_;
}

Allocator& ContextImpl::getNPUHostSendAllocator(int deviceIdx) {
  if (!npuHostSendAllocator_.has_value()) {
    NPUPinnedBuffer buffer = makeNPUPinnedBuffer(kStagingAreaSize, deviceIdx);
    uint8_t* ptr = buffer.get();
    npuHostSendAllocator_.emplace(NPUHostAllocator{
        std::move(buffer), Allocator(ptr, kNumSlots, kSlotSize)});
  }

  return npuHostSendAllocator_->allocator;
}

Allocator& ContextImpl::getNPUHostRecvAllocator(int deviceIdx) {
  if (!npuHostRecvAllocator_.has_value()) {
    NPUPinnedBuffer buffer = makeNPUPinnedBuffer(kStagingAreaSize, deviceIdx);
    uint8_t* ptr = buffer.get();
    npuHostRecvAllocator_.emplace(NPUHostAllocator{
        std::move(buffer), Allocator(ptr, kNumSlots, kSlotSize)});
  }

  return npuHostRecvAllocator_->allocator;
}

void ContextImpl::handleErrorImpl() {
  if (cpuContext_ != nullptr) {
    cpuContext_->close();
  }
  npuLoop_.close();

  if (npuHostSendAllocator_.has_value()) {
    npuHostSendAllocator_->allocator.close();
  }
  if (npuHostRecvAllocator_.has_value()) {
    npuHostRecvAllocator_->allocator.close();
  }
}

void ContextImpl::joinImpl() {
  if (cpuContext_ != nullptr) {
    cpuContext_->join();
  }
  npuLoop_.join();
}

bool ContextImpl::inLoop() const {
  return loop_.inLoop();
};

void ContextImpl::deferToLoop(std::function<void()> fn) {
  loop_.deferToLoop(std::move(fn));
};

void ContextImpl::setIdImpl() {
  cpuContext_->setId(id_ + ".cpu");
}

} // namespace npu_basic
} // namespace channel
} // namespace tensorpipe_npu

