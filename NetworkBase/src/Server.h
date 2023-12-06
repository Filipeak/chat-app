#pragma once

#include "Network.h"
#include "NetworkCommands.h"

#include <functional>
#include <thread>
#include <mutex>
#include <string>
#include <vector>

struct ClientData
{
	SOCKET socket;
	int id;
	std::thread t;
	std::string host;
};

class Server
{
public:
	static bool Bind(std::string port);
	static bool Dispose();
	static bool Listen(std::function<void(int, NetworkCommand)> onReceivedMessage, std::function<void(int)> onClientJoined, std::function<void(int)> onClientLeft);
	static bool Broadcast(NetworkCommand command);
	static bool SendDM(NetworkCommand command, int clientId);

	static std::string GetClientHost(int clientId);

private:
	static int m_FuncResult;
	static SOCKET m_Socket;
	static std::mutex m_ClientMutex;
	static int m_ClientIdNumber;
	static std::vector<ClientData> m_Clients;

	static void ClientListen(int clientId, SOCKET socket, std::function<void(int, NetworkCommand)> onReceivedMessage, std::function<void(int)> onClientLeft);
};