/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstddef>

namespace tensorpipe_npu {
namespace channel {
namespace npu_basic {

// FIXME Avoid this anonymous namespace and use inline variables in C++-17.
namespace {

// Define all three (redundant) values to make them explicit and avoid
// misunderstandings due to miscalculations.
static constexpr size_t kStagingAreaSize = 16 * 1024 * 1024;
static constexpr size_t kSlotSize = 1024 * 1024;
static constexpr size_t kNumSlots = 16;

static_assert(kStagingAreaSize == kSlotSize * kNumSlots, "");

} // namespace

} // namespace npu_basic
} // namespace channel
} // namespace tensorpipe_npu

