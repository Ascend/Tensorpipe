/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/common/npu_loop.h>

#include <tensorpipe/common/npu.h>
#include <tensorpipe/common/system.h>

namespace tensorpipe {

namespace {

struct NPUCallback {
  NPULoop& loop;
  std::function<void(const Error&)> callback;

  NPUCallback(NPULoop& loop, std::function<void(const Error&)> callback)
      : loop(loop), callback(std::move(callback)) {}
};

class NPULoopClosedError final : public BaseError {
  std::string what() const override {
    return "NPU loop already closed";
  }
};

} // namespace

NPULoop::NPULoop() {
  thread_ = std::thread([this]() {
    setThreadName("TP_NPU_callback_loop");
    processCallbacks();
  });
}

NPULoop::~NPULoop() {
  join();
}

void NPULoop::join() {
  close();

  if (!joined_.exchange(true)) {
    thread_.join();
  }
}

void NPULoop::close() {
  std::unique_lock<std::mutex> lock(mutex_);
  if (closed_) {
    return;
  }
  closed_ = true;
  cv_.notify_all();
}

void NPULoop::processCallbacks() {
  for (;;) {
    std::deque<Operation> operations;
    {
      std::unique_lock<std::mutex> lock(mutex_);

      if (operations_.empty()) {
        if (closed_ && pendingOperations_ == 0) {
          break;
        } else {
          cv_.wait(lock);
        }
      }

      std::swap(operations, operations_);
      pendingOperations_ -= operations.size();
    }

    for (auto& op : operations) {
      op.callback(op.error);
    }
  }
}

void NPULoop::addCallback(
    int device,
    aclrtStream stream,
    std::function<void(const Error&)> callback) {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    if (closed_) {
      callback(TP_CREATE_ERROR(NPULoopClosedError));
      return;
    }
    ++pendingOperations_;
  }

  auto npuCallback =
      std::make_unique<NPUCallback>(*this, std::move(callback));
  NPUDeviceGuard guard(device);
  //TP_NPU_CHECK(npuStreamAddCallback(stream, runNPUCallback, npuCallback.release(), 0));
}

void  NPULoop::runNPUCallback(
    aclrtStream /* unused */,
    aclError npuError,
    void* callbackPtr) {
  std::unique_ptr<NPUCallback> npuCallback(reinterpret_cast<NPUCallback*>(callbackPtr));
  NPULoop& loop = npuCallback->loop;
  {
    std::unique_lock<std::mutex> lock(loop.mutex_);
    auto error = Error::kSuccess;
    if (npuError != 0) {
      error = TP_CREATE_ERROR(NPUError, npuError);
    }
    loop.operations_.push_back(
        {std::move(npuCallback->callback), std::move(error)});
    loop.cv_.notify_all();
  }
  npuCallback.reset();
}

} // namespace tensorpipe

