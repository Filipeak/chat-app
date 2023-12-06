#pragma once

#include <string>
#include <vector>

#define NETWORK_COMMAND_USER_MESSAGE 'a'
#define NETWORK_COMMAND_USER_JOIN 'b'

#define NETWORK_COMMAND_USER_MESSAGE_ 'c'
#define NETWORK_COMMAND_USER_JOIN_ 'd'
#define NETWORK_COMMAND_USER_LEAVE_ 'e'

struct NetworkCommand
{
	char type;
	std::string payload;
};

class NetworkCommands
{
public:
	static std::vector<char> Serialize(NetworkCommand cmd);
	static NetworkCommand Deserialize(const char* buffer, int len);
};