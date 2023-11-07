#pragma once

#include "Network.h"

class Client
{
public:
	static bool Connect(std::string domain, std::string port);
	static bool Disconnect();
	static bool Listen(std::function<void(std::string)> callback);
	static bool Send(std::string data);

private:
	static int m_FuncResult;
	static SOCKET m_Socket;
	static char m_ReceiveBuffer[RECEIVE_BUFFER_LENGTH];
};