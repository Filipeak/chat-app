#pragma once

#include "Network.h"

#include <vector>

struct ClientData
{
	SOCKET socket;
	std::thread t;
};

class Server
{
public:
	static bool Bind(std::string port);
	static bool Dispose();
	static bool Listen(std::function<void(std::string)> onReceivedMessage);
	static bool Broadcast(std::string data);

private:
	static int m_FuncResult;
	static SOCKET m_Socket;
	static std::mutex m_ClientMutex;
	static std::vector<ClientData> m_Clients;

	static void ClientListen(SOCKET socket, std::function<void(std::string)> callback);
};