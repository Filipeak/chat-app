#pragma once

#include "Network.h"
#include "NetworkCommands.h"

#include <functional>
#include <string>

class Client
{
public:
	Client() : m_FuncResult(0), m_Socket(0) {}

	bool Connect(std::string host, std::string port);
	bool Disconnect();
	bool Listen(std::function<void(NetworkCommand)> callback);
	bool Send(NetworkCommand command);

	std::string GetHost();
	std::string GetPort();

private:
	int m_FuncResult;
	SOCKET m_Socket;
	std::string m_Host;
	std::string m_Port;
};