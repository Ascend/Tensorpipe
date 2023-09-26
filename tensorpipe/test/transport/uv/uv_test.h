/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <tensorpipe/test/transport/transport_test.h>
#include <tensorpipe/transport/uv/factory.h>

class UVTransportTestHelper : public TransportTestHelper {
 protected:
  std::shared_ptr<tensorpipe_npu::transport::Context> getContextInternal()
      override {
    return tensorpipe_npu::transport::uv::create();
  }

 public:
  std::string defaultAddr() override {
    return "127.0.0.1";
  }
};
