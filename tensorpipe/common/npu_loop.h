/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <list>
#include <mutex>
#include <thread>

#include <tensorpipe/common/error_macros.h>

#include "third_party/acl/inc/acl/acl_base.h"
#include "third_party/acl/inc/acl/acl_rt.h"
#include "third_party/acl/inc/acl/acl.h"


namespace tensorpipe_npu {

class NPULoop {
  struct Operation {
    std::function<void(const Error&)> callback;
    Error error;
  };

 public:
  NPULoop();

  ~NPULoop();

  void join();
  void close();

  void addCallback(
      int device,
      aclrtStream stream,
      std::function<void(const Error&)> callback);

 private:
  std::thread thread_;
  std::deque<Operation> operations_;
  std::mutex mutex_;
  std::condition_variable cv_;
  uint64_t pendingOperations_{0};

  bool closed_{false};
  std::atomic<bool> joined_{false};

  void processCallbacks();

  // Proxy static method for npuStreamAddCallback(), which does not accept
  // lambdas.
  static void  runNPUCallback(
      aclrtStream stream,
      aclError npuError,
      void* callbackPtr);
};

} // namespace tensorpipe_npu
