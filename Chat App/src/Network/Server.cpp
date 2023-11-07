#include "Server.h"

int Server::m_FuncResult;
SOCKET Server::m_Socket;
SOCKET Server::m_ClientSocket;
char Server::m_ReceiveBuffer[RECEIVE_BUFFER_LENGTH];

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

bool Server::Listen(std::function<void(std::string)> callback)
{
	if (listen(m_Socket, SOMAXCONN) == SOCKET_ERROR)
	{
		NETWORK_LOG_ERR_WIN("listen");

		closesocket(m_Socket);
		WSACleanup();

		return false;
	}

	NETWORK_LOG("Waiting for clients...");

	m_ClientSocket = accept(m_Socket, NULL, NULL);

	if (m_ClientSocket == INVALID_SOCKET)
	{
		NETWORK_LOG_ERR_WIN("accept");

		closesocket(m_ClientSocket);
		WSACleanup();

		return false;
	}

	NETWORK_LOG("Client connected!");

	NETWORK_LOG("Listening...");

	do
	{
		m_FuncResult = recv(m_ClientSocket, m_ReceiveBuffer, RECEIVE_BUFFER_LENGTH, 0);

		if (m_FuncResult > 0)
		{
			NETWORK_LOG_ARG("Bytes received: ", m_FuncResult);

			callback(m_ReceiveBuffer);
		}
		else if (m_FuncResult == 0)
		{
			NETWORK_LOG("Connection closed");
		}
		else
		{
			NETWORK_LOG_ERR_WIN("recv");

			return false;
		}

	} while (m_FuncResult > 0);

	return true;
}

bool Server::Send(std::string data)
{
	m_FuncResult = send(m_ClientSocket, data.c_str(), (int)data.size(), 0);

	if (m_FuncResult == SOCKET_ERROR)
	{
		NETWORK_LOG_ERR_WIN("send");

		closesocket(m_ClientSocket);
		WSACleanup();

		return false;
	}

	NETWORK_LOG_ARG("Bytes sent: ", m_FuncResult);

	return true;
}