/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <signal.h>

// One-time init to use EPIPE errors instead of SIGPIPE
namespace {

struct Initializer {
  explicit Initializer() {
    signal(SIGPIPE, SIG_IGN);
  }
};

Initializer initializer;

} // namespace
