/**
* RESOURCES:
*  - https://learn.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock
* 
* TODO:
*  - Multiple clients - Disconnecting, Sending
*  - Threading - Input, Server
*/

#include "Network/Network.h"
#include "Network/Client.h"
#include "Network/Server.h"

#define HOSTNAME "localhost"
#define PORT "27015"
#define SERVER_BUILD 0

void OnListen(std::string s)
{
	std::cout << " > " << s << std::endl;
}

int main()
{
	Network::Init();

#if !SERVER_BUILD
	if (Client::Connect(HOSTNAME, PORT))
	{
		Client::Listen(OnListen);
		Client::Disconnect();
	}
#else
	if (Server::Bind(PORT))
	{
		Server::Listen(OnListen);
	}
#endif

	return 0;
}