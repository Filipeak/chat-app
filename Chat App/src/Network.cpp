#include "Network.h"

bool Network::Init()
{
	WSADATA wsaData;

	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (res != 0)
	{
		NETWORK_LOG_ERR_CODE("WSAStartup", res);

		return false;
	}

	NETWORK_LOG("Successfully initialized Winsock!");

	return true;
}