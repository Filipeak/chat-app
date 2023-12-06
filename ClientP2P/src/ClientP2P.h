#pragma once

#include <Network.h>
#include <NetworkCommands.h>
#include <Server.h>
#include <Client.h>

#include <functional>
#include <thread>
#include <mutex>
#include <string>
#include <vector>

#define CLIENTP2P_MSG_CMD 'b'
#define CLIENTP2P_JOIN_CMD 'c'
#define CLIENTP2P_LEAVE_CMD 'd'

#define CLIENTP2P_JOIN_CMD_ 'e'
#define CLIENTP2P_IPS_CMD_ 'a'

struct ClientP2PData
{
	Client c;
	std::thread t;
	int serverClientId;
};

class ClientP2P
{
public:
	static bool Bind(std::string port, std::function<void(NetworkCommand)> onMessage);
	static bool Dispose();
	static bool AddPeer(std::string host, std::string port, int id);
	static bool Send(NetworkCommand command);

private:
	static int m_FuncResult;
	static SOCKET m_Socket;
	static std::mutex m_ServerMutex;
	static std::thread m_ServerListenThread;
	static std::string m_ServerPort;
	static std::vector<ClientP2PData> m_Clients;
	static std::function<void(NetworkCommand)> m_OnMessage;

	static void ServerListen();
	static void ClientListen(Client c);

	static void OnServerMessage(int clientId, NetworkCommand cmd);
	static void OnClientMessage(NetworkCommand cmd);
	static void OnClientJoined(int clientId);
	static void OnClientLeft(int clientId);
};