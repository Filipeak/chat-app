#include "ClientP2P.h"

int ClientP2P::m_FuncResult;
SOCKET ClientP2P::m_Socket;
std::mutex ClientP2P::m_ServerMutex;
std::thread ClientP2P::m_ServerListenThread;
std::string ClientP2P::m_ServerPort;
std::vector<ClientP2PData> ClientP2P::m_Clients;
std::function<void(NetworkCommand)> ClientP2P::m_OnMessage;

static std::vector<std::string> _StringSplit(std::string s, std::string delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();

	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));

	return res;
}

bool ClientP2P::Bind(std::string port, std::function<void(NetworkCommand)> onMessage)
{
	if (!Server::Bind(port))
	{
		return false;
	}

	m_ServerPort = port;
	m_ServerListenThread = std::thread(ServerListen);
	m_OnMessage = onMessage;

	return true;
}

bool ClientP2P::Dispose()
{
	m_ServerListenThread.join();

	NETWORK_LOG("Disposing P2P Client...");

	for (auto& client : m_Clients)
	{
		client.t.join();

		if (!client.c.Disconnect())
		{
			return false;
		}
	}

	return Server::Dispose();
}

bool ClientP2P::AddPeer(std::string host, std::string port, int id)
{
	std::lock_guard<std::mutex> guard(m_ServerMutex);

	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		if ((m_Clients[i].c.GetHost() == host && m_Clients[i].c.GetPort() == port) || m_Clients[i].serverClientId == id)
		{
			NETWORK_LOG_ARG("Peer is already on list: ", host + ":" + port);

			return false;
		}
	}

	NETWORK_LOG_ARG("Adding peer: ", host + ":" + port);

	Client c;

	if (c.Connect(host, port))
	{
		std::thread t(ClientListen, c);

		m_Clients.push_back({ c, std::move(t), id });

		if (id == -1)
		{
			return c.Send({ CLIENTP2P_JOIN_CMD_ , m_ServerPort });
		}
		else
		{
			std::string payload = "";

			payload += m_ServerPort + "!";

			for (size_t i = 0; i < m_Clients.size(); i++)
			{
				if (m_Clients[i].c.GetHost() != host || m_Clients[i].c.GetPort() != port)
				{
					payload += m_Clients[i].c.GetHost() + ":" + m_Clients[i].c.GetPort() + "|";
				}
			}

			return c.Send({ CLIENTP2P_IPS_CMD_, payload });
		}
	}
}

bool ClientP2P::Send(NetworkCommand command)
{
	if (m_OnMessage)
	{
		m_OnMessage(command);
	}

	return Server::Broadcast(command);
}

void ClientP2P::ServerListen()
{
	Server::Listen(OnServerMessage, OnClientJoined, OnClientLeft);
}

void ClientP2P::ClientListen(Client c)
{
	c.Listen(OnClientMessage);
}

void ClientP2P::OnServerMessage(int clientId, NetworkCommand cmd)
{
	if (cmd.type == CLIENTP2P_JOIN_CMD_)
	{
		std::string clientHost = Server::GetClientHost(clientId);
		std::string clientPort = cmd.payload;

		AddPeer(clientHost, clientPort, clientId);
	}
	else if (cmd.type == CLIENTP2P_IPS_CMD_)
	{
		std::vector<std::string> data = _StringSplit(cmd.payload, "!");

		std::string host = Server::GetClientHost(clientId);
		std::string port = data[0];

		for (size_t i = 0; i < m_Clients.size(); i++)
		{
			if (m_Clients[i].c.GetHost() == host && m_Clients[i].c.GetPort() == port)
			{
				std::lock_guard<std::mutex> guard(m_ServerMutex);

				m_Clients[i].serverClientId = clientId;

				break;
			}
		}

		std::vector<std::string> hosts = _StringSplit(data[1], "|");

		NETWORK_LOG("Got list with new hosts: ");

		for (size_t i = 0; i < hosts.size() - 1; i++)
		{
			NETWORK_LOG_ARG(" - Host: ", hosts[i]);

			std::vector<std::string> data = _StringSplit(hosts[i], ":");

			std::string host = data[0];
			std::string port = data[1];

			AddPeer(host, port, -1);
		}
	}
}

void ClientP2P::OnClientMessage(NetworkCommand cmd)
{
	if (m_OnMessage)
	{
		m_OnMessage(cmd);
	}
}

void ClientP2P::OnClientJoined(int clientId)
{
	if (m_OnMessage)
	{
		m_OnMessage({ CLIENTP2P_JOIN_CMD , "" });
	}
}

void ClientP2P::OnClientLeft(int clientId)
{
	if (m_OnMessage)
	{
		m_OnMessage({ CLIENTP2P_LEAVE_CMD , "" });
	}

	std::lock_guard<std::mutex> guard(m_ServerMutex);

	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i].serverClientId == clientId)
		{
			m_Clients[i].t.detach();

			m_Clients.erase(m_Clients.begin() + i);

			break;
		}
	}
}