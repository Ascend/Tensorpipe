/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <cstring>
#include "third_party/acl/inc/acl/acl_base.h"
#include "third_party/acl/inc/acl/acl_rt.h"
#include "third_party/acl/inc/acl/acl.h"


#include <tensorpipe/common/npu.h>
#include <tensorpipe/common/npu_lib.h>
#include <tensorpipe/test/peer_group.h>
#include <tensorpipe/test/test_environment.h>

#include <gtest/gtest.h>

namespace {

tensorpipe::NPULib getNPULib() {
  tensorpipe::Error error;
  tensorpipe::NPULib npuLib;
  std::tie(error, npuLib) = tensorpipe::NPULib::create();
  EXPECT_FALSE(error) << error.what();
  return npuLib;
}

} // namespace

// This tests whether we can retrieve the index of the device on which a pointer
// resides under "normal" circumstances (in the same context where it was
// allocated, or in a "fresh" thread).
TEST(NPU, DeviceForPointer) {
  if (TestEnvironment::numNPUDevices() < 2) {
    GTEST_SKIP() << "Skipping test requiring >=2 NPU devices.";
  }

  ForkedThreadPeerGroup pg;
  pg.spawn(
      [&]() {
        TP_NPU_CHECK(aclrtSetDevice(1));
        void* ptr;
        TP_NPU_CHECK(aclrtMalloc(&ptr, 1024, ACL_MEM_MALLOC_HUGE_FIRST));

        std::string ptrStr(
            reinterpret_cast<char*>(&ptr),
            reinterpret_cast<char*>(&ptr) + sizeof(void*));
        pg.send(PeerGroup::kClient, ptrStr);
      },
      [&]() {
        std::string ptrStr = pg.recv(PeerGroup::kClient);
        void* ptr = *reinterpret_cast<void**>(&ptrStr[0]);        
      });
}

// This tests whether we can retrieve the index of the device on which a pointer
// resided after we've explicitly set the current device to an invalid value.
// This is known to cause problems in recent versions of NPU, possibly because
// of a bug.
TEST(NPU, DeviceForPointerAfterReset) {
  if (TestEnvironment::numNPUDevices() < 2) {
    GTEST_SKIP() << "Skipping test requiring >=2 NPU devices.";
  }

  ForkedThreadPeerGroup pg;
  pg.spawn(
      [&]() {
        TP_NPU_CHECK(aclrtSetDevice(1));
        void* ptr;
        TP_NPU_CHECK(aclrtMalloc(&ptr, 1024, ACL_MEM_MALLOC_HUGE_FIRST));

        TP_NPU_CHECK(aclrtSetDevice(0));

        std::string ptrStr(
            reinterpret_cast<char*>(&ptr),
            reinterpret_cast<char*>(&ptr) + sizeof(void*));
        pg.send(PeerGroup::kClient, ptrStr);
      },
      [&]() {
        std::string ptrStr = pg.recv(PeerGroup::kClient);
        void* ptr = *reinterpret_cast<void**>(&ptrStr[0]);

        TP_NPU_CHECK(aclrtSetDevice(0));

      });
}

