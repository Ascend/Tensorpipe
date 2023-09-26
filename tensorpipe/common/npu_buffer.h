/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <tensorpipe/common/device.h>
#include "third_party/acl/inc/acl/acl_base.h"
#include "third_party/acl/inc/acl/acl_rt.h"
#include "third_party/acl/inc/acl/acl.h"


namespace tensorpipe_npu {

struct NPUBuffer {
  void* ptr{nullptr};
  aclrtStream stream;

  Device getDevice() const;
};

} // namespace tensorpipe_npu

