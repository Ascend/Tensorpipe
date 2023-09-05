/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <tensorpipe/test/channel/channel_test_npu.h>

#include <gmock/gmock.h>

using namespace tensorpipe;
using namespace tensorpipe::channel;


class ReceiverWaitsForStartEventTest: public ClientServerChannelTestCase
{
static constexpr size_t kSize = 1024;


void server(std::shared_ptr < Channel > channel)
override
{
	TP_NPU_CHECK(aclrtSetDevice(0));
	aclrtStream 	sendStream;

	TP_NPU_CHECK(aclrtCreateStreamWithConfig(&sendStream, 0, (ACL_STREAM_FAST_LAUNCH | ACL_STREAM_FAST_SYNC)));
	void *			ptr;

	TP_NPU_CHECK(aclrtMalloc(&ptr, kSize, ACL_MEM_MALLOC_HUGE_FIRST));

	// Delay sendStream with computations on buffer.
	// slowKernel(ptr, kSize, sendStream);
	// Set buffer to target value.
	TP_NPU_CHECK(aclrtMemsetAsync(ptr, kSize, 0x42, kSize, sendStream));

	// Perform send and wait for completion.
	auto			sendPromise = std::make_shared < std::promise < tensorpipe::Error >> ();
	auto			sendFuture = sendPromise->get_future();

	channel->send(NPUBuffer {.ptr = ptr, 
		.stream 		= sendStream, 
		}, 
		kSize, 
		[sendPromise { std::move(sendPromise) }] (const tensorpipe::Error & error) { sendPromise->set_value(error);
		 });

	Error			sendError = sendFuture.get();

	EXPECT_FALSE(sendError) << sendError.what();
	TP_NPU_CHECK(aclrtFree(ptr));

	this->peers_->done(PeerGroup::kServer);
	this->peers_->join(PeerGroup::kServer);
}


void client(std::shared_ptr < Channel > channel)
override
{
	TP_NPU_CHECK(aclrtSetDevice(0));
	aclrtStream 	recvStream;

	TP_NPU_CHECK(aclrtCreateStreamWithConfig(&recvStream, 0, (ACL_STREAM_FAST_LAUNCH | ACL_STREAM_FAST_SYNC)));
	void *			ptr;

	TP_NPU_CHECK(aclrtMalloc(&ptr, kSize, ACL_MEM_MALLOC_HUGE_FIRST));

	// Perform recv and wait for completion.
	auto			recvPromise = std::make_shared < std::promise < tensorpipe::Error >> ();
	auto			recvFuture = recvPromise->get_future();

	channel->recv(NPUBuffer {.ptr = ptr, 
		.stream 		= recvStream, 
		}, 
		kSize, 
		[recvPromise { std::move(recvPromise) }] (const tensorpipe::Error & error) { recvPromise->set_value(error);
		 });

	Error			recvError = recvFuture.get();

	EXPECT_FALSE(recvError) << recvError.what();

	std::array < uint8_t, kSize > data;
	TP_NPU_CHECK(aclrtSynchronizeStream(recvStream));
	TP_NPU_CHECK(aclrtMemcpy(data.data(), kSize, ptr, kSize, ACL_MEMCPY_HOST_TO_HOST));
	EXPECT_THAT(data, ::testing::Each(0x42));
	TP_NPU_CHECK(aclrtFree(ptr));

	this->peers_->done(PeerGroup::kClient);
	this->peers_->join(PeerGroup::kClient);
}


};


CHANNEL_TEST(NPUChannelTestSuite, ReceiverWaitsForStartEvent);


class SendOffsetAllocationTest: public ClientServerChannelTestCase
{




public:

static constexpr int kDataSize = 256;
static constexpr int kOffset = 128;


void server(std::shared_ptr < Channel > channel)
override
{
	// Initialize with sequential values.
	void *			ptr;

	TP_NPU_CHECK(aclrtMalloc(&ptr, kOffset + kDataSize, ACL_MEM_MALLOC_HUGE_FIRST));

	// Set buffer to target value.
	TP_NPU_CHECK(aclrtMemset(ptr, kOffset + kDataSize, 0xff, kOffset));

	TP_NPU_CHECK(aclrtMemset(static_cast < uint8_t * > (ptr) +kOffset, kOffset + kDataSize, 0x42, kDataSize));

	// Perform send and wait for completion.
	std::future < Error > sendFuture = sendWithFuture(channel, 
		NPUBuffer {.ptr = static_cast < uint8_t * > (ptr) +kOffset }, 
		kDataSize);
	Error			sendError = sendFuture.get();

	EXPECT_FALSE(sendError) << sendError.what();

	this->peers_->done(PeerGroup::kServer);
	this->peers_->join(PeerGroup::kServer);
}


void client(std::shared_ptr < Channel > channel)
override
{
	std::unique_ptr < DataWrapper > wrappedData = helper_->makeDataWrapper(kDataSize);

	// Perform recv and wait for completion.
	std::future < Error > recvFuture = recvWithFuture(channel, *wrappedData);
	Error			recvError = recvFuture.get();

	EXPECT_FALSE(recvError) << recvError.what();

	// Validate contents of vector.
	EXPECT_THAT(wrappedData->unwrap(), ::testing::Each(0x42));

	this->peers_->done(PeerGroup::kClient);
	this->peers_->join(PeerGroup::kClient);
}


};


CHANNEL_TEST(NPUChannelTestSuite, SendOffsetAllocation);

