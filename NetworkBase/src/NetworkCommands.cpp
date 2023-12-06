#include "NetworkCommands.h"

std::vector<char> NetworkCommands::Serialize(NetworkCommand cmd)
{
	std::vector<char> data;

	data.push_back(cmd.type);
	
	for (size_t i = 0; i < cmd.payload.size() + 1; i++)
	{
		data.push_back(cmd.payload[i]);
	}

	return data;
}

NetworkCommand NetworkCommands::Deserialize(const char* buffer, int len)
{
	NetworkCommand cmd {};

	cmd.type = buffer[0];
	cmd.payload = buffer + 1;

	return cmd;
}