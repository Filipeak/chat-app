#include <Network.h>
#include <Server.h>

#include <unordered_map>
#include <mutex>

static std::mutex s_Mutex;
static std::unordered_map<int, std::string> s_Users;

static void OnListen(int clientId, NetworkCommand cmd)
{
	if (cmd.type == NETWORK_COMMAND_USER_MESSAGE)
	{
		if (s_Users.find(clientId) != s_Users.end())
		{
			Server::Broadcast({ NETWORK_COMMAND_USER_MESSAGE_ , s_Users[clientId] + ": " + cmd.payload });
		}
		else
		{
			Server::Broadcast({ NETWORK_COMMAND_USER_MESSAGE_ , "Anonymous: " + cmd.payload });
		}
	}
	else if (cmd.type == NETWORK_COMMAND_USER_JOIN)
	{
		std::lock_guard<std::mutex> guard(s_Mutex);

		s_Users[clientId] = cmd.payload;

		Server::Broadcast({ NETWORK_COMMAND_USER_JOIN_ , cmd.payload });
	}
}

static void OnClientLeft(int clientId)
{
	Server::Broadcast({ NETWORK_COMMAND_USER_LEAVE_ , s_Users[clientId] });

	std::lock_guard<std::mutex> guard(s_Mutex);

	s_Users.erase(clientId);
}

int main()
{
	Network::Init();

	if (Server::Bind("1337"))
	{
		Server::Listen(OnListen, nullptr, OnClientLeft);
		Server::Dispose();
	}
}