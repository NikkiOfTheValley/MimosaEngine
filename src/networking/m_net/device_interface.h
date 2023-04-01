#pragma once
#include "common.h"
#include "message.h"
#include "tsqueue.h"
#include "connection.h"

// This class represents this computer on the network. It handles the Connections & message passing.
// Only one of these should exist per application.
class DeviceInterface
{
public:
	DeviceInterface(asio::io_context* context);

	// Creates a new Connection and returns its ID (used to access the Connection)
	int Connect(const std::string& host, const uint16_t port);

	void Disconnect(int connectionID);

	// Allows connections on the specified port
	void AcceptConnectionsOnPort(const uint16_t port);

	// If this device is connected to at least one other device, returns true
	bool IsConnectedToAny();

	bool Send(const message& msg, int connectionID);

	Connection* GetConnection(int id);

private:
	void AsyncAcceptConnection(const uint16_t port);

	asio::io_context* context;

	asio::ip::tcp::acceptor* acceptor;

	// A Device can have any number of Connections
	std::vector<Connection*> connections;

	// A cache of the last ID used for a Connection
	int lastID = -1;
public:
	tsqueue<owned_message> qMessagesIn;

};