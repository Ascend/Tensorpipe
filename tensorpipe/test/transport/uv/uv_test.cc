/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <tensorpipe/test/transport/uv/uv_test.h>

namespace {

UVTransportTestHelper helper;

} // namespace

INSTANTIATE_TEST_CASE_P(Uv, TransportTest, ::testing::Values(&helper));
