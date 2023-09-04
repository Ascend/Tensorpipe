/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <iomanip>
#include <ios>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "third_party/acl/inc/acl/acl_base.h"
#include "third_party/acl/inc/acl/acl_rt.h"
#include "third_party/acl/inc/acl/acl.h"


#include <tensorpipe/common/npu_lib.h>
#include <tensorpipe/common/defs.h>
#include <tensorpipe/common/device.h>
#include <tensorpipe/common/error.h>
#include <tensorpipe/common/strings.h>

#define TP_NPU_CHECK(a)                            \
  do {                                             \
    auto error = (a);                              \
    TP_THROW_ASSERT_IF(0 != error)                 \
        << __TP_EXPAND_OPD(a) << " " << " ("       \
        << aclGetRecentErrMsg() << ")";            \
  } while (false)

namespace tensorpipe {

class NPUError final : public BaseError {
 public:
  explicit NPUError(aclError error) : error_(error) {}

  std::string what() const override {
    return std::string(aclGetRecentErrMsg());
  }

 private:
  aclError error_;
};

class NPUDeviceGuard {
 public:
  NPUDeviceGuard() = delete;
  NPUDeviceGuard(const NPUDeviceGuard&) = delete;
  NPUDeviceGuard(NPUDeviceGuard&&) = delete;
  NPUDeviceGuard& operator=(const NPUDeviceGuard&) = delete;
  NPUDeviceGuard& operator=(NPUDeviceGuard&&) = delete;

  explicit NPUDeviceGuard(int device) {
    TP_NPU_CHECK(aclrtGetDevice(&device_));
    TP_NPU_CHECK(aclrtSetDevice(device));
  }

  ~NPUDeviceGuard() {
    TP_NPU_CHECK(aclrtSetDevice(device_));
  }

 private:
  int device_;
};

class NPUEvent {
 public:
  NPUEvent() = delete;
  NPUEvent(const NPUEvent&) = delete;
  NPUEvent(NPUEvent&&) = delete;
  NPUEvent& operator=(const NPUEvent&) = delete;
  NPUEvent& operator=(NPUEvent&&) = delete;

  explicit NPUEvent(int device, bool interprocess = false)
      : deviceIdx_(device) {
    NPUDeviceGuard guard(deviceIdx_);
    
    TP_NPU_CHECK(aclrtCreateEvent(&ev_));
  }

  explicit NPUEvent(int device)
      : deviceIdx_(device) {
    // It could crash if we don't set device when creating events from handles
    NPUDeviceGuard guard(deviceIdx_);
  }

  void record(aclrtStream stream) {
    NPUDeviceGuard guard(deviceIdx_);
    TP_NPU_CHECK(aclrtRecordEvent(ev_, stream));
  }

  void wait(aclrtStream stream, int device) {
    NPUDeviceGuard guard(device);
    TP_NPU_CHECK(aclrtStreamWaitEvent(stream, ev_));
  }

  bool query() const {
    NPUDeviceGuard guard(deviceIdx_);
	aclrtEventRecordedStatus status = ACL_EVENT_RECORDED_STATUS_NOT_READY;
    aclError res = aclrtQueryEventStatus(ev_, &status);
    TP_NPU_CHECK(res);
    return true;
  }

  aclrtEvent raw() {
    return ev_;
  }


  ~NPUEvent() {
    NPUDeviceGuard guard(deviceIdx_);
    TP_NPU_CHECK(aclrtDestroyEvent(ev_));
  }

 private:
  aclrtEvent ev_;
  int deviceIdx_;
};


class NPUPinnedMemoryDeleter {
 public:
  explicit NPUPinnedMemoryDeleter(int deviceIdx) : deviceIdx_(deviceIdx) {}

  void operator()(uint8_t* ptr) {
    NPUDeviceGuard guard(deviceIdx_);
    TP_NPU_CHECK(aclrtFreeHost(ptr));
  }

 private:
  const int deviceIdx_;
};

using NPUPinnedBuffer = std::unique_ptr<uint8_t[], NPUPinnedMemoryDeleter>;

inline NPUPinnedBuffer makeNPUPinnedBuffer(size_t length, int deviceIdx) {
  NPUDeviceGuard guard(deviceIdx);
  void* ptr;
  TP_NPU_CHECK(aclrtMallocHost(&ptr, length));
  return NPUPinnedBuffer((uint8_t *)ptr, NPUPinnedMemoryDeleter(deviceIdx));
}

class NPUDeviceBuffer {
 public:
  NPUDeviceBuffer() = default;

  NPUDeviceBuffer(size_t length, int deviceIdx) {
    NPUDeviceGuard guard(deviceIdx);
    void* ptr;
    TP_NPU_CHECK(aclrtMalloc(&ptr, length, ACL_MEM_MALLOC_HUGE_FIRST));
    ptr_ = {(uint8_t *)ptr, Deleter{deviceIdx}};
  }

  uint8_t* ptr() const {
    return ptr_.get();
  }

  int deviceIdx() const {
    return ptr_.get_deleter().deviceIdx;
  }

  void reset() {
    ptr_.reset();
  }

 private:
  struct Deleter {
    int deviceIdx;

    void operator()(uint8_t* ptr) {
      NPUDeviceGuard guard(deviceIdx);
      TP_NPU_CHECK(aclrtFree(ptr));
    }
  };

  std::unique_ptr<uint8_t[], Deleter> ptr_;
};


inline std::vector<Device> getNPUDevices(const NPULib& npuLib) {
  int deviceCount;
  uint32_t count;
  TP_NPU_CHECK(aclrtGetDeviceCount(&count));
  deviceCount = (int)count;
  std::vector<Device> result(deviceCount);
  for (int devIdx = 0; devIdx < deviceCount; ++devIdx) {
    result[devIdx] = Device{kNpuDeviceType, devIdx};
  }

  return result;
}

} // namespace tensorpipe

