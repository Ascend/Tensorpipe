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

if(DTP_TEST_NPU)
  list(APPEND TP_TEST_LINK_LIBRARIES ${NPU_LIBRARIES}) 
  list(APPEND TP_TEST_INCLUDE_DIRS ${NPU_INCLUDE_DIRS})

  list(APPEND TP_TEST_SRCS 
    channel/channel_test_npu.cc
    channel/npu_basic/npu_basic_test.cc
    common/npu_test.cc)  
endif()

if(TP_ENABLE_SHM)
  list(APPEND TP_TEST_SRCS
    common/epoll_loop_test.cc
    common/ringbuffer_test.cc
    common/shm_ringbuffer_test.cc
    common/shm_segment_test.cc
    transport/shm/reactor_test.cc
    transport/shm/connection_test.cc
    transport/shm/listener_test.cc
    transport/shm/sockaddr_test.cc
    transport/shm/shm_test.cc
    )
endif()

if(TP_ENABLE_CMA)
  list(APPEND TP_TEST_SRCS
    channel/cma/cma_test.cc
    )
  add_subdirectory(channel/cma)
endif()

if(TP_ENABLE_IBV)
  list(APPEND TP_TEST_SRCS
    common/epoll_loop_test.cc
    common/ringbuffer_test.cc
    transport/ibv/sockaddr_test.cc
    transport/ibv/context_test.cc
    transport/ibv/ibv_test.cc
    transport/ibv/connection_test.cc
    )
endif()

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

if(DTP_TEST_NPU)
  set(TORCHNPU_THIRD_PARTY_ROOT "${PROJECT_SOURCE_DIR}/third_party") 
  link_directories(${TORCHNPU_THIRD_PARTY_ROOT}/acl/libs) 
  target_link_libraries(tensorpipe_test PUBLIC ${TORCHNPU_THIRD_PARTY_ROOT}/acl/libs/libhccl.so) 
  target_link_libraries(tensorpipe_test PUBLIC ${TORCHNPU_THIRD_PARTY_ROOT}/acl/libs/libascendcl.so) 
  target_link_libraries(tensorpipe_test PUBLIC ${TORCHNPU_THIRD_PARTY_ROOT}/acl/libs/libacl_op_compiler.so) 
  target_link_libraries(tensorpipe_test PUBLIC ${TORCHNPU_THIRD_PARTY_ROOT}/acl/libs/libge_runner.so) 
  target_link_libraries(tensorpipe_test PUBLIC ${TORCHNPU_THIRD_PARTY_ROOT}/acl/libs/libgraph.so) 
  target_link_libraries(tensorpipe_test PUBLIC ${TORCHNPU_THIRD_PARTY_ROOT}/acl/libs/libacl_tdt_channel.so)
endif()
