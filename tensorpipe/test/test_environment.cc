/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/test/test_environment.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <tensorpipe/common/npu.h>
#include <tensorpipe/common/defs.h>
#include <unistd.h>


int TestEnvironment::numNPUDevices() {
  static int count = -1;
  if (count == -1) {
  	pid_t pid = fork();
    TP_THROW_SYSTEM_IF(pid < 0, errno) << "Failed to fork";
    if (pid == 0) {
      uint32_t res;
      TP_NPU_CHECK(aclrtGetDeviceCount(&res));
      std::exit((int)res);
    } else {
      int status;
      TP_THROW_SYSTEM_IF(waitpid(pid, &status, 0) < 0, errno)
          << "Failed to wait for child process";
      TP_THROW_ASSERT_IF(!WIFEXITED(status));
      count = WEXITSTATUS(status);
    }
  }

  return count;
}

