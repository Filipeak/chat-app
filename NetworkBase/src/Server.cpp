#include "Server.h"
#include <ws2tcpip.h>

int Server::m_FuncResult;
SOCKET Server::m_Socket;
std::mutex Server::m_ClientMutex;
int Server::m_ClientIdNumber;
std::vector<ClientData> Server::m_Clients;

bool Server::Bind(std::string port)
{
	NETWORK_LOG("Binding server...");

	struct addrinfo* result = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	m_FuncResult = getaddrinfo(NULL, port.c_str(), &hints, &result);

	if (m_FuncResult != 0)
	{
		NETWORK_LOG_ERR_CODE("getaddrinfo", m_FuncResult);

		WSACleanup();

		return false;
	}

	m_Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (m_Socket == INVALID_SOCKET)
	{
		NETWORK_LOG_ERR_WIN("socket");

		freeaddrinfo(result);
		WSACleanup();

		return false;
	}

	m_FuncResult = bind(m_Socket, result->ai_addr, (int)result->ai_addrlen);

	if (m_FuncResult == SOCKET_ERROR)
	{
		NETWORK_LOG_ERR_WIN("bind");

		freeaddrinfo(result);
		closesocket(m_Socket);
		WSACleanup();

		return false;
	}

	freeaddrinfo(result);

	NETWORK_LOG("Successfully bound server!");

	return true;
}

bool Server::Dispose()
{
	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		m_FuncResult = shutdown(m_Clients[i].socket, SD_SEND);

		if (m_FuncResult == SOCKET_ERROR)
		{
			NETWORK_LOG_ERR_WIN("shutdown");
		}

		closesocket(m_Clients[i].socket);

		m_Clients[i].t.join();

		NETWORK_LOG_ARG("Disconnected client: ", m_Clients[i].id);
	}

	m_Clients.clear();

	closesocket(m_Socket);
	WSACleanup();

	NETWORK_LOG("Server has been disposed!");

	return true;
}

bool Server::Listen(std::function<void(int, NetworkCommand)> onReceivedMessage, std::function<void(int)> onClientJoined, std::function<void(int)> onClientLeft)
{
	if (listen(m_Socket, SOMAXCONN) == SOCKET_ERROR)
	{
		NETWORK_LOG_ERR_WIN("listen");

		closesocket(m_Socket);
		WSACleanup();

		return false;
	}

	NETWORK_LOG("Listening on server...");
	NETWORK_LOG("Waiting for clients...");

	while (true)
	{
		struct sockaddr clientAddr;
		int addrLen = sizeof(clientAddr);
		SOCKET clientSocket = accept(m_Socket, &clientAddr, &addrLen);

		if (clientSocket == INVALID_SOCKET)
		{
			NETWORK_LOG_ERR_WIN("accept");

			closesocket(m_Socket);
			WSACleanup();

			return false;
		}

		struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&clientAddr;
		struct in_addr ipAddr = pV4Addr->sin_addr;
		char host[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &ipAddr, host, INET_ADDRSTRLEN);

		m_ClientIdNumber++;

		std::thread th(ClientListen, m_ClientIdNumber, clientSocket, onReceivedMessage, onClientLeft);

		std::lock_guard<std::mutex> guard(m_ClientMutex);

		m_Clients.push_back({ clientSocket, m_ClientIdNumber, std::move(th), host });

		if (onClientJoined)
		{
			onClientJoined(m_ClientIdNumber);
		}

		NETWORK_LOG("Client connected!");
	}

	return true;
}

bool Server::Broadcast(NetworkCommand command)
{
	std::vector<char> data = NetworkCommands::Serialize(command);

	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		m_FuncResult = send(m_Clients[i].socket, data.data(), (int)data.size(), 0);

		NETWORK_LOG_ARG("Sending data to client: ", m_Clients[i].id);

		if (m_FuncResult == SOCKET_ERROR)
		{
			NETWORK_LOG_ERR_WIN("send");
		}
		else
		{
			NETWORK_LOG_ARG("Bytes sent: ", m_FuncResult);
		}
	}

	return true;
}

bool Server::SendDM(NetworkCommand command, int clientId)
{
	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i].id == clientId)
		{
			std::vector<char> data = NetworkCommands::Serialize(command);

			m_FuncResult = send(m_Clients[i].socket, data.data(), (int)data.size(), 0);

			NETWORK_LOG_ARG("Sending data to client: ", m_Clients[i].id);

			if (m_FuncResult == SOCKET_ERROR)
			{
				NETWORK_LOG_ERR_WIN("send");
			}
			else
			{
				NETWORK_LOG_ARG("Bytes sent: ", m_FuncResult);
			}

			return true;
		}
	}

	return false;
}

std::string Server::GetClientHost(int clientId)
{
	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i].id == clientId)
		{
			return m_Clients[i].host;
		}
	}

	return "invalid";
}

void Server::ClientListen(int clientId, SOCKET socket, std::function<void(int, NetworkCommand)> onReceivedMessage, std::function<void(int)> onClientLeft)
{
	int funcResult = 0;
	char receiveBuffer[512];

	do
	{
		ZeroMemory(receiveBuffer, sizeof(receiveBuffer));

		funcResult = recv(socket, receiveBuffer, sizeof(receiveBuffer), 0);

		if (funcResult > 0)
		{
			NETWORK_LOG_ARG("Bytes received: ", funcResult);

			if (onReceivedMessage)
			{
				onReceivedMessage(clientId, NetworkCommands::Deserialize(receiveBuffer, funcResult));
			}
		}
		else if (funcResult == 0)
		{
			NETWORK_LOG_ARG("Connection closed for client: ", clientId);
		}
		else
		{
			NETWORK_LOG_ARG("Error occured while listening for client: ", clientId);
			NETWORK_LOG_ERR_WIN("recv");
		}

	} while (funcResult > 0);

	if (onClientLeft)
	{
		onClientLeft(clientId);
	}

	std::lock_guard<std::mutex> guard(m_ClientMutex);	

	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i].id == clientId)
		{
			closesocket(m_Clients[i].socket);

			m_Clients[i].t.detach();
			m_Clients.erase(m_Clients.begin() + i);

			break;
		}
	}	
}