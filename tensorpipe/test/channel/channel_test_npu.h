/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include <tensorpipe/common/npu.h>
#include <tensorpipe/common/npu_buffer.h>
#include <tensorpipe/test/channel/channel_test.h>

class NPUDataWrapper : public DataWrapper {
 public:
  // Non-copyable.
  NPUDataWrapper(const NPUDataWrapper&) = delete;
  NPUDataWrapper& operator=(const NPUDataWrapper&) = delete;
  // Non-movable.
  NPUDataWrapper(NPUDataWrapper&& other) = delete;
  NPUDataWrapper& operator=(NPUDataWrapper&& other) = delete;

  explicit NPUDataWrapper(size_t length) : length_(length) {
    if (length_ > 0) {
      TP_NPU_CHECK(aclrtSetDevice(0));
      TP_NPU_CHECK(aclrtCreateStreamWithConfig(&stream_, 0, (ACL_STREAM_FAST_LAUNCH | ACL_STREAM_FAST_SYNC)));
      TP_NPU_CHECK(aclrtMalloc(&npuPtr_, length_, ACL_MEM_MALLOC_HUGE_FIRST));
    }
  }

  explicit NPUDataWrapper(std::vector<uint8_t> v) : NPUDataWrapper(v.size()) {
    if (length_ > 0) {
	  size_t destMax=sizeof(npuPtr_);
      TP_NPU_CHECK(aclrtMemcpyAsync(npuPtr_, destMax, v.data(), length_, ACL_MEMCPY_HOST_TO_HOST, stream_));
    }
  }

  tensorpipe::Buffer buffer() const override {
    return tensorpipe::NPUBuffer{
        .ptr = npuPtr_,
        .stream = stream_,
    };
  }

  size_t bufferLength() const override {
    return length_;
  }

  std::vector<uint8_t> unwrap() override {
    std::vector<uint8_t> v(length_);
    if (length_ > 0) {
      TP_NPU_CHECK(aclrtSynchronizeStream(stream_));
      TP_NPU_CHECK(aclrtMemcpy(v.data(), length_, npuPtr_, length_, ACL_MEMCPY_HOST_TO_HOST));
    }
    return v;
  }

  ~NPUDataWrapper() override {
    if (length_ > 0) {
      TP_NPU_CHECK(aclrtFree(npuPtr_));
      TP_NPU_CHECK(aclrtDestroyStream(stream_));
    }
  }

 private:
  void* npuPtr_{nullptr};
  size_t length_{0};
  aclrtStream stream_;
};

class NPUChannelTestHelper : public ChannelTestHelper {
 public:
  std::unique_ptr<DataWrapper> makeDataWrapper(size_t length) override {
    return std::make_unique<NPUDataWrapper>(length);
  }

  std::unique_ptr<DataWrapper> makeDataWrapper(
      std::vector<uint8_t> v) override {
    return std::make_unique<NPUDataWrapper>(std::move(v));
  }
};

class NPUChannelTestSuite
    : public ::testing::TestWithParam<NPUChannelTestHelper*> {};


