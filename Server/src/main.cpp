#include "Network.h"
#include "Server.h"

static void OnListen(std::string s)
{
	Server::Broadcast(s);
}

int main()
{
	Network::Init();

	if (Server::Bind("1337"))
	{
		Server::Listen(OnListen);
		Server::Dispose();
	}

	return 0;
}