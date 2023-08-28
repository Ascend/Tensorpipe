# Copyright (c) 2023 Huawei Technologies Co., Ltd
# Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

# List of source files that we need to build tensorpipe_test executable.
set(TP_TEST_SRCS)

# TP_TEST_LINK_LIBRARIES is list of dependent libraries to be linked
set(TP_TEST_LINK_LIBRARIES)

# TP_TEST_INCLUDE_DIRS is list of include path to be used
set(TP_TEST_INCLUDE_DIRS)

# TP_TEST_COMPILE_DEFS is list of compile definitions to be used
set(TP_TEST_COMPILE_DEFS)

list(APPEND TP_TEST_SRCS
  test.cc
  test_environment.cc
  transport/connection_test.cc
  transport/context_test.cc
  transport/listener_test.cc
  transport/uv/uv_test.cc
  transport/uv/context_test.cc
  transport/uv/loop_test.cc
  transport/uv/connection_test.cc
  transport/uv/sockaddr_test.cc
  core/context_test.cc
  core/listener_test.cc
  core/pipe_test.cc
  channel/basic/basic_test.cc
  channel/xth/xth_test.cc
  channel/mpt/mpt_test.cc
  channel/channel_test.cc
  channel/channel_test_cpu.cc
  common/system_test.cc
  common/defs_test.cc
  )

add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/googletest
  ${PROJECT_BINARY_DIR}/third_party/googletest EXCLUDE_FROM_ALL)

list(APPEND TP_TEST_LINK_LIBRARIES
  tensorpipe
  uv::uv
  gmock
  gtest_main)

add_executable(tensorpipe_test ${TP_TEST_SRCS})

# Add all the dependent link libraries to the tensorpipe_test target
target_link_libraries(tensorpipe_test PRIVATE ${TP_TEST_LINK_LIBRARIES})