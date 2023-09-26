/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <deque>
#include <memory>
#include <string>

#include <tensorpipe/channel/channel_impl_boilerplate.h>
#include <tensorpipe/common/allocator.h>
#include <tensorpipe/common/npu.h>
#include <tensorpipe/common/npu_buffer.h>
#include <tensorpipe/common/npu_loop.h>
#include <tensorpipe/common/state_machine.h>

namespace tensorpipe_npu {
namespace channel {
namespace npu_basic {

class ContextImpl;

struct ChunkSendOperation {
  enum State {
    UNINITIALIZED,
    ALLOCATING_CPU_BUFFER,
    COPYING_FROM_GPU_TO_CPU,
    INVOKED_CALLBACK,
    SENDING_CPU_BUFFER,
    FINISHED
  };

  // Fields used by the state machine
  uint64_t sequenceNumber{0};
  State state{UNINITIALIZED};

  // Arguments at creation
  uint64_t bufferSequenceNumber{0};
  bool isCpuBuffer{false};
  void* devicePtr{nullptr};
  size_t chunkId{0};
  size_t numChunks{0};
  size_t length{0};
  std::function<void(const Error&)> callback;

  // For NPU buffers
  aclrtStream stream;
  int deviceIdx{0};

  // Data collected during processing
  std::shared_ptr<uint8_t> tmpBuffer;

  // Progress flags
  bool doneAllocatingCpuStagingBuffer{false};
  bool doneCopyingFromNpuToCpu{false};
  bool doneSendingCpuBuffer{false};
};

struct ChunkRecvOperation {
  enum State {
    UNINITIALIZED,
    READING_READY_TO_SEND,
    ALLOCATING_CPU_BUFFER,
    RECEIVING_CPU_BUFFER,
    COPYING_FROM_CPU_TO_GPU,
    COPYING_FROM_CPU_TO_GPU_AND_INVOKED_CALLBACK,
    FINISHED
  };

  // Fields used by the state machine
  uint64_t sequenceNumber{0};
  State state{UNINITIALIZED};

  // Arguments at creation
  uint64_t bufferSequenceNumber{0};
  bool isCpuBuffer{false};
  void* devicePtr{nullptr};
  size_t chunkId{0};
  size_t numChunks{0};
  size_t length{0};
  std::function<void(const Error&)> callback;

  // For NPU buffers
  aclrtStream stream;
  int deviceIdx{0};

  // Data collected during processing
  std::shared_ptr<uint8_t> tmpBuffer;

  // Progress flags
  bool doneReadingReadyToSend{false};
  bool doneAllocatingCpuStagingBuffer{false};
  bool doneReceivingCpuBuffer{false};
  bool doneCopyingFromCpuToNpu{false};
};

class ChannelImpl final
    : public ChannelImplBoilerplate<ContextImpl, ChannelImpl> {
 public:
  ChannelImpl(
      ConstructorToken token,
      std::shared_ptr<ContextImpl> context,
      std::string id,
      std::shared_ptr<transport::Connection> connection,
      std::shared_ptr<Channel> cpuChannel,
      NPULoop& npuLoop);

 protected:
  // Implement the entry points called by ChannelImplBoilerplate.
  void initImplFromLoop() override;
  void sendImplFromLoop(
      uint64_t sequenceNumber,
      Buffer buffer,
      size_t length,
      TSendCallback callback) override;
  void recvImplFromLoop(
      uint64_t sequenceNumber,
      Buffer buffer,
      size_t length,
      TRecvCallback callback) override;
  void handleErrorImpl() override;
  void setIdImpl() override;

 private:
  const std::shared_ptr<transport::Connection> connection_;
  const std::shared_ptr<Channel> cpuChannel_;
  NPULoop& npuLoop_;

  // A sequence number for the chunks.
  uint64_t nextChunkBeingSent_{0};
  uint64_t nextChunkBeingReceived_{0};

  OpsStateMachine<ChannelImpl, ChunkSendOperation> chunkSendOps_{
      *this,
      &ChannelImpl::advanceChunkSendOperation};
  using ChunkSendOpIter = decltype(chunkSendOps_)::Iter;
  OpsStateMachine<ChannelImpl, ChunkRecvOperation> chunkRecvOps_{
      *this,
      &ChannelImpl::advanceChunkRecvOperation};
  using ChunkRecvOpIter = decltype(chunkRecvOps_)::Iter;

  // State machines for send and recv ops.
  void advanceChunkSendOperation(
      ChunkSendOpIter opIter,
      ChunkSendOperation::State prevOpState);
  void advanceChunkRecvOperation(
      ChunkRecvOpIter opIter,
      ChunkRecvOperation::State prevOpState);

  // Actions (i.e., methods that begin a state transition).
  // For send operations:
  void allocateSendCpuBuffer(ChunkSendOpIter opIter);
  void copyFromNpuToCpu(ChunkSendOpIter opIter);
  void callSendCallback(ChunkSendOpIter opIter);
  void sendCpuBuffer(ChunkSendOpIter opIter);
  void writeReadyToSend(ChunkSendOpIter opIter);
  void returnSendCpuBuffer(ChunkSendOpIter opIter);
  // For recv operations:
  void readReadyToSend(ChunkRecvOpIter opIter);
  void allocateRecvCpuBuffer(ChunkRecvOpIter opIter);
  void receiveCpuBuffer(ChunkRecvOpIter opIter);
  void copyFromCpuToNpu(ChunkRecvOpIter opIter);
  void callRecvCallback(ChunkRecvOpIter opIter);
  void returnRecvCpuBuffer(ChunkRecvOpIter opIter);

  void NPUCopy(
      void* dst,
      const void* src,
      size_t length,
      int deviceIdx,
      aclrtStream stream,
      std::function<void(const Error&)> callback,
      aclrtMemcpyKind copy_kind);
};

} // namespace npu_basic
} // namespace channel
} // namespace tensorpipe_npu

