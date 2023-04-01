#pragma once
#include "common.h"

struct message_header
{
	int id;
	uint32_t size = 0; // Using uint32_t instead of size_t because size_t may not be the same size on different systems
};

struct message
{
	message_header header{};
	std::vector<uint8_t> body;

	size_t bodySize() const;
	
	void log();

	// Overloads to make it so you can use stream operators with the message

// Push any POD-like data into the message buffer
	template <typename DataType>
	friend message& operator << (message& msg, const DataType& data)
	{
		// Make sure that the data being pushed is trivially copyable
		static_assert(std::is_standard_layout<DataType>::value, "Data is not trivially copyable!");

		// Cache current size of buffer, as this is the point at which the data will be inserted
		size_t i = msg.body.size();

		// Make sure there's enough room for the type
		msg.body.resize(msg.body.size() + sizeof(DataType));

		// This seems janky. I have no idea if this is even remotely safe.
		// Physically copy the data from the user's variable to the end of the buffer
		std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

		msg.header.size = (uint32_t)msg.bodySize();

		// Return a reference to the message so `msg << x << y` is possible
		return msg;
	}

	// Pops any POD-like data from the message buffer
	template <typename DataType>
	friend message& operator >> (message& msg, DataType& data)
	{
		// Make sure that the data being popped is trivially copyable
		static_assert(std::is_standard_layout<DataType>::value, "Data is not trivially copyable!");

		// Cache the location where the data will be popped from
		size_t i = msg.body.size() - sizeof(DataType);

		// This seems janky. I have no idea if this is even remotely safe.
		// Physically copy the data from the end of the buffer to the user's variable
		std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

		// Shrink the vector to remove the popped bytes. This shouldn't cause a reallocation
		msg.body.resize(i);

		msg.header.size = (uint32_t)msg.bodySize();

		// Return a reference to the message so `msg >> y >> x` is possible
		return msg;
	}
};

class Connection;

struct owned_message
{
	int remoteID = -1;
	message msg;

	void log();
};