#include "message.h"
#include "connection.h"

size_t message::bodySize() const
{
	return body.size();
}

void message::log()
{
	logger.log("ID: " + std::to_string(header.id) + " Size: " + std::to_string(header.size));
}

void owned_message::log()
{
	logger.log("ID: " + std::to_string(msg.header.id) + "Owner: " + std::to_string(remoteID) + " Size: " + std::to_string(msg.header.size));
}