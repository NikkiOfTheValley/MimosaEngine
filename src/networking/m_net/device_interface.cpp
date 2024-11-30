#include "device_interface.h"

DeviceInterface::DeviceInterface(asio::io_context* context)
{
	this->context = context;
}

// Creates a new Connection and returns its ID (used to access the Connection)
int DeviceInterface::Connect(const std::string& host, const uint16_t port)
{
	Connection* connection = new Connection(*context, qMessagesIn);
	connection->Connect(host, port);
	connection->SetID(lastID + 1);
	connections.push_back(connection);
	return connection->GetID();
}

void DeviceInterface::Disconnect(int connectionID)
{
	for (auto& c : connections)
	{
		if (c->GetID() == connectionID)
			c->Disconnect();
	}
}

// Allows connections on the specified port
void DeviceInterface::AcceptConnectionsOnPort(const uint16_t port)
{
	acceptor = new asio::ip::tcp::acceptor(*context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));

	AsyncAcceptConnection(port);
}

void DeviceInterface::AsyncAcceptConnection(const uint16_t port)
{
	acceptor->async_accept(
		[this, port](std::error_code ec, asio::ip::tcp::socket socket)
		{
			if (!ec)
			{
				asio::ip::tcp::socket* copiedSocket = new asio::ip::tcp::socket(std::move(socket));
				// Make a new Connection that corresponds to the provided socket
				Connection* connection = new Connection(*context, qMessagesIn, copiedSocket);
				connection->StartListening();
				connection->SetID(lastID + 1);
				lastID = connection->GetID();
				connections.push_back(connection);
				logger.log("A new connection was made");

				// Prime ASIO to accept a new connection, this fixes a bug where only one connection can be made to a given host
				AsyncAcceptConnection(port);
			}
			else
			{
				logger.err("A new connection was attempted, but caused an error: " + ec.message());
			}
		});
}

// If this device is connected to at least one other device, returns true
bool DeviceInterface::IsConnectedToAny()
{
	return !connections.empty();
}

bool DeviceInterface::Send(const message& msg, int connectionID)
{

	for (auto& c : connections)
	{
		if (c->GetID() == connectionID)
		{
			return c->Send(msg);
		}
	}
	return false;
}

Connection* DeviceInterface::GetConnection(int id)
{
	for (auto& c : connections)
	{
		if (c->GetID() == id)
		{
			return c;
		}
	}
	return nullptr;
}