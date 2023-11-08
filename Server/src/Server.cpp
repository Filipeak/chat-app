#include "Server.h"

int Server::m_FuncResult;
SOCKET Server::m_Socket;
std::mutex Server::m_ClientMutex;
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
	m_ClientMutex.lock();

	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		m_FuncResult = shutdown(m_Clients[i].socket, SD_SEND);

		if (m_FuncResult == SOCKET_ERROR)
		{
			NETWORK_LOG_ERR_WIN("shutdown");
		}

		closesocket(m_Clients[i].socket);
		WSACleanup();

		m_Clients[i].t.join();

		NETWORK_LOG("Disconnected");
	}

	m_ClientMutex.unlock();

	return true;
}

bool Server::Listen(std::function<void(std::string)> onReceivedMessage)
{
	if (listen(m_Socket, SOMAXCONN) == SOCKET_ERROR)
	{
		NETWORK_LOG_ERR_WIN("listen");

		closesocket(m_Socket);
		WSACleanup();

		return false;
	}

	NETWORK_LOG("Listening...");
	NETWORK_LOG("Waiting for clients...");

	while (true)
	{
		SOCKET clientSocket = accept(m_Socket, NULL, NULL);

		if (clientSocket == INVALID_SOCKET)
		{
			NETWORK_LOG_ERR_WIN("accept");

			closesocket(clientSocket);
			WSACleanup();

			continue;
		}

		std::thread th(ClientListen, clientSocket, onReceivedMessage);

		m_ClientMutex.lock();
		m_Clients.push_back({ clientSocket, std::move(th) });
		m_ClientMutex.unlock();

		NETWORK_LOG("Client connected!");
	}

	return true;
}

bool Server::Broadcast(std::string data)
{
	m_ClientMutex.lock();

	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		m_FuncResult = send(m_Clients[i].socket, data.c_str(), (int)data.size(), 0);

		if (m_FuncResult == SOCKET_ERROR)
		{
			NETWORK_LOG_ERR_WIN("send");

			closesocket(m_Clients[i].socket);
			WSACleanup();
		}

		NETWORK_LOG_ARG("Bytes sent: ", m_FuncResult);
	}

	m_ClientMutex.unlock();

	return true;
}

void Server::ClientListen(SOCKET socket, std::function<void(std::string)> callback)
{
	char receiveBuffer[RECEIVE_BUFFER_LENGTH];

	do
	{
		ZeroMemory(receiveBuffer, sizeof(receiveBuffer));

		m_FuncResult = recv(socket, receiveBuffer, RECEIVE_BUFFER_LENGTH, 0);

		if (m_FuncResult > 0)
		{
			NETWORK_LOG_ARG("Bytes received: ", m_FuncResult);

			callback(receiveBuffer);
		}
		else if (m_FuncResult == 0)
		{
			NETWORK_LOG("Connection closed");
		}
		else
		{
			NETWORK_LOG_ERR_WIN("recv");
		}

	} while (m_FuncResult > 0);
}