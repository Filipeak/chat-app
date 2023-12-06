#include "Client.h"
#include <ws2tcpip.h>

bool Client::Connect(std::string host, std::string port)
{
	m_Host = host;
	m_Port = port;

	NETWORK_LOG("Connecting...");

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	m_FuncResult = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);

	if (m_FuncResult != 0)
	{
		NETWORK_LOG_ERR_CODE("getaddrinfo", m_FuncResult);

		WSACleanup();

		return false;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		m_Socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (m_Socket == INVALID_SOCKET)
		{
			NETWORK_LOG_ERR_WIN("socket");

			WSACleanup();

			return false;
		}

		m_FuncResult = connect(m_Socket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (m_FuncResult == SOCKET_ERROR)
		{
			closesocket(m_Socket);

			m_Socket = INVALID_SOCKET;

			continue;
		}

		break;
	}

	freeaddrinfo(result);

	if (m_Socket == INVALID_SOCKET)
	{
		NETWORK_LOG_ERR("Unable to connect to server!");

		WSACleanup();

		return false;
	}

	NETWORK_LOG("Successfully connected to server!");

	return true;
}

bool Client::Disconnect()
{
	m_FuncResult = shutdown(m_Socket, SD_SEND);

	if (m_FuncResult == SOCKET_ERROR)
	{
		NETWORK_LOG_ERR_WIN("shutdown");
	}

	closesocket(m_Socket);
	WSACleanup();

	NETWORK_LOG("Disconnected");

	return true;
}

bool Client::Listen(std::function<void(NetworkCommand)> callback)
{
	NETWORK_LOG("Listening on client...");

	char receiveBuffer[512];

	do
	{
		ZeroMemory(receiveBuffer, sizeof(receiveBuffer));

		m_FuncResult = recv(m_Socket, receiveBuffer, sizeof(receiveBuffer), 0);

		if (m_FuncResult > 0)
		{
			NETWORK_LOG_ARG("Bytes received: ", m_FuncResult);

			if (callback)
			{
				callback(NetworkCommands::Deserialize(receiveBuffer, m_FuncResult));
			}
		}
		else if (m_FuncResult == 0)
		{
			NETWORK_LOG("Connection closed");

			return false;
		}
		else
		{
			NETWORK_LOG_ERR_WIN("recv");

			return false;
		}

	} while (m_FuncResult > 0);

	return true;
}

bool Client::Send(NetworkCommand command)
{
	std::vector<char> data = NetworkCommands::Serialize(command);

	m_FuncResult = send(m_Socket, data.data(), (int)data.size(), 0);

	if (m_FuncResult == SOCKET_ERROR)
	{
		NETWORK_LOG_ERR_WIN("send");

		closesocket(m_Socket);
		WSACleanup();

		return false;
	}

	NETWORK_LOG_ARG("Bytes sent: ", m_FuncResult);

	return true;
}

std::string Client::GetHost()
{
	return m_Host;
}

std::string Client::GetPort()
{
	return m_Port;
}