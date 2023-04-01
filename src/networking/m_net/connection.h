#pragma once
#include "common.h"
#include "tsqueue.h"
#include "message.h"

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	Connection(asio::io_context& context, tsqueue<owned_message>& qMessagesIn, asio::ip::tcp::socket* socket = nullptr) : asioContext(context), qMessagesIn(qMessagesIn)
	{
		if (!socket)
			this->socket = new asio::ip::tcp::socket(context);
		else
			this->socket = socket;
	};

	virtual ~Connection();


	bool Connect(const std::string& host, const uint16_t port);
	bool Disconnect();
	bool IsConnected() const;

	void StartListening();
	
	bool Send(const message& msg);

	asio::ip::tcp::endpoint GetRemoteEndpoint();
	int GetID();
	void SetID(int newID);

private:
	// Prime ASIO to write a message
	void AsyncWriteMessage();

	// Prime ASIO to write a message body
	void AsyncWriteMessageBody();

	// Prime ASIO to read a message
	void AsyncReadMessage();

	// Prime ASIO to read a message body
	void AsyncReadMessageBody();

	asio::ip::tcp::socket* socket;
	int id = -1;

	// Temporary incoming message cache 
	message msgTemporaryIn;

	// This is shared across the entire ASIO instance, otherwise things will break in the case of more than 1 connection
	asio::io_context& asioContext;

	tsqueue<message> qMessagesOut;

	// This is expected to be global to all connections, and it is expected to be provided by the owner of this connection
	tsqueue<owned_message>& qMessagesIn;
};