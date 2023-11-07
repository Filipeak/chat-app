#pragma once

#include "Network.h"

class Server
{
public:
	static bool Bind(std::string port);
	static bool Listen(std::function<void(std::string)> callback);
	static bool Send(std::string data);

private:
	static int m_FuncResult;
	static SOCKET m_Socket;
	static SOCKET m_ClientSocket;
	static char m_ReceiveBuffer[RECEIVE_BUFFER_LENGTH];
};