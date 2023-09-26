/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <tensorpipe/benchmark/registry.h>
#include <tensorpipe/transport/context.h>

TP_DECLARE_SHARED_REGISTRY(
    TensorpipeTransportRegistry,
    tensorpipe_npu::transport::Context);

void validateTransportContext(
    std::shared_ptr<tensorpipe_npu::transport::Context> context);
