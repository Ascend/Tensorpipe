/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <numeric>

#include <tensorpipe/channel/basic/factory.h>
#include <tensorpipe/channel/npu_basic/factory.h>
#include <tensorpipe/test/channel/channel_test_npu.h>

namespace {

class NPUBasicChannelTestHelper : public NPUChannelTestHelper {
 protected:
  std::shared_ptr<tensorpipe::channel::Context> makeContextInternal(
      std::string id) override {
    auto cpuContext = tensorpipe::channel::basic::create();
    auto context =
        tensorpipe::channel::npu_basic::create(std::move(cpuContext));
    context->setId(std::move(id));
    return context;
  }

 public:
  std::shared_ptr<PeerGroup> makePeerGroup() override {
    return std::make_shared<ProcessPeerGroup>();
  }
};

NPUBasicChannelTestHelper helper;

class NPUBasicChannelTestSuite : public ChannelTestSuite {};

} // namespace

class CannotCommunicateCpuToCpuTest : public ChannelTestCase {
 public:
  void run(ChannelTestHelper* /* unused */) override {
    ProcessPeerGroup pg;
    pg.spawn(
        [&]() {
          auto cpuContext = tensorpipe::channel::basic::create();
          auto ctx =
              tensorpipe::channel::npu_basic::create(std::move(cpuContext));
          auto deviceDescriptors = ctx->deviceDescriptors();
          auto it = deviceDescriptors.find(
              tensorpipe::Device{tensorpipe::kCpuDeviceType, 0});
          EXPECT_FALSE(it == deviceDescriptors.end());
          auto descriptor = it->second;
          EXPECT_FALSE(ctx->canCommunicateWithRemote(descriptor, descriptor));
        },
        [&]() {
          // Do nothing.
        });
  }
};

CHANNEL_TEST(NPUBasicChannelTestSuite, CannotCommunicateCpuToCpu);

INSTANTIATE_TEST_CASE_P(
    NPUBasic,
    ChannelTestSuite,
    ::testing::Values(&helper));

INSTANTIATE_TEST_CASE_P(
    NPUBasic,
    NPUChannelTestSuite,
    ::testing::Values(&helper));


INSTANTIATE_TEST_CASE_P(
    NPUBasic,
    NPUBasicChannelTestSuite,
    ::testing::Values(&helper));

