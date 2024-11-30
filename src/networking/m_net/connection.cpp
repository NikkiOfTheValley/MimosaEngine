#include "connection.h"

Connection::~Connection()
{
	if (IsConnected())
		Disconnect();
}


bool Connection::Connect(const std::string& host, const uint16_t port)
{
	asio::error_code ec;
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address(host, ec), port);
	if (!ec)
	{
		socket->open(asio::ip::tcp::endpoint::protocol_type::v4());
		socket->async_connect(endpoint,
			[this, host](std::error_code ec)
			{
				if (!ec)
				{
					// Prime ASIO to read incoming messages
					AsyncReadMessage();
				}
				else
				{
					logger.err("Error while attempting to connect to " + host + ": " + ec.message());
				}
			});
		return true;
	}
	else
	{
		logger.err("Error while attempting to connect to " + host + ": " + ec.message());
		return false;
	}
}

bool Connection::Disconnect()
{
	socket->close();
	delete socket;
	return true;
}

bool Connection::IsConnected() const
{
	return socket->is_open();
}

void Connection::StartListening()
{
	// Prime ASIO to read incoming messages
	AsyncReadMessage();
}

bool Connection::Send(const message& msg)
{
	asio::post(asioContext,
		[this, msg]()
		{
			// If the outgoing queue has a message in it, then it is
			// being asynchronously written to.

			// If it is, this message cannot be written right now.
			// If it isn't, then we can start the process of writing this message and all other messages in the queue

			// Either way, push the current message into the outgoing queue, as the async function that's currently
			// in progress will check the outgoing message queue and will write this message anyways
			bool writingMessage = !qMessagesOut.empty<message>();
			qMessagesOut.push_back(msg);
			if (!writingMessage)
			{
				AsyncWriteMessage();
			}
		});
	return true;
}

asio::ip::tcp::endpoint Connection::GetRemoteEndpoint()
{
	return socket->remote_endpoint();
}

int Connection::GetID()
{
	return id;
}

void Connection::SetID(int newID)
{
	// If ID has not been set already, this function can set it
	if (id == -1)
		id = newID;
	else
		logger.err("Connection ID has already been set");
}

// Prime ASIO to write a message
void Connection::AsyncWriteMessage()
{
	asio::async_write(*socket, asio::buffer(&qMessagesOut.front<message>().header, sizeof(message_header)),
		[this](asio::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				// Check if message has a body
				if (qMessagesOut.front<message>().body.size() > 0)
				{
					// If it does, prime ASIO to write it
					AsyncWriteMessageBody();
				}
				else
				{
					// If it doesn't, remove the current message and write
					// the rest of the messages in the outgoing queue

					qMessagesOut.pop_front<message>();

					// If the queue still has messages in it, send them
					if (!qMessagesOut.empty<message>())
					{
						AsyncWriteMessage();
					}
				}
			}
			else
			{
				// ASIO failed to write the message, for simplicity we'll not bother with trying to figure out why
				logger.err("Failed to write header! Connection " + std::to_string(id));
				logger.err(ec.message());
				socket->close();
			}
		});
}

// Prime ASIO to write a message body
void Connection::AsyncWriteMessageBody()
{
	// This function is only called from AsyncWriteMessage(), which writes the header, and checks if a body exists for this message,
	// so no body size checks are required

	asio::async_write(*socket, asio::buffer(qMessagesOut.front<message>().body.data(), qMessagesOut.front<message>().body.size()),
		[this](asio::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				// Remove message, it isn't needed anymore because asio::async_write() wrote the message body,
				// and the message header was written by AsyncWriteMessage()
				qMessagesOut.pop_front<message>();

				// If the queue still has messages in it, send them
				if (!qMessagesOut.empty<message>())
				{
					AsyncWriteMessage();
				}
			}
			else
			{
				// ASIO failed to write the message, for simplicity we'll not bother with trying to figure out why
				logger.err("Failed to write body! Connection " + std::to_string(id));
				socket->close();
			}
		});
}

// Prime ASIO to read a message
void Connection::AsyncReadMessage()
{
	asio::async_read(*socket, asio::buffer(&msgTemporaryIn.header, sizeof(message_header)),
		[this](asio::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				// Message header has been read, check if this message includes a body
				if (msgTemporaryIn.header.size > 0)
				{
					// If it does, allocate space in the message's body
					msgTemporaryIn.body.resize(msgTemporaryIn.header.size);

					// And then read the body
					AsyncReadMessageBody();
				}
				else
				{
					// If it doesn't, add the message to the incoming queue as it has been fully recieved
					qMessagesIn.push_back<owned_message>({ GetID(), msgTemporaryIn });

					// Then prime ASIO to read any new messages after this one
					AsyncReadMessage();
				}
			}
			else
			{
				// ASIO failed to read the message, for simplicity we'll not bother with trying to figure out why
				logger.err("Failed to read header! Connection " + std::to_string(id));
				socket->close();
			}
		});
}

// Prime ASIO to read a message body
void Connection::AsyncReadMessageBody()
{
	asio::async_read(*socket, asio::buffer(msgTemporaryIn.body.data(), msgTemporaryIn.body.size()),
		[this](asio::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				// Body has been recieved, add to incoming message queue
				qMessagesIn.push_back<owned_message>({ GetID(), msgTemporaryIn });

				AsyncReadMessage();
			}
			else
			{
				// ASIO failed to read the message, for simplicity we'll not bother with trying to figure out why
				logger.err("Failed to read body! Connection " + std::to_string(id));
				socket->close();
			}
		});
}