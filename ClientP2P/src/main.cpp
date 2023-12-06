#include <Network.h>
#include "ClientP2P.h"

#include <thread>
#include <mutex>

static std::mutex s_Mutex;
static bool m_Finished;

static void InputThread()
{
	while (!m_Finished)
	{
		std::string s;
		std::getline(std::cin, s);

		ClientP2P::Send({ CLIENTP2P_MSG_CMD , s });
	}
}

static void OnMessage(NetworkCommand cmd)
{
	if (cmd.type == CLIENTP2P_MSG_CMD)
	{
		std::cout << " > " << cmd.payload << std::endl;
	}
	else if (cmd.type == CLIENTP2P_JOIN_CMD)
	{
		std::cout << "[SERVER] New user has joined!" << std::endl;
	}
	else if (cmd.type == CLIENTP2P_LEAVE_CMD)
	{
		std::cout << "[SERVER] A user has left!" << std::endl;
	}
}

int main()
{
	Network::Init();

	std::string port;
	std::cout << "Enter port: ";
	std::getline(std::cin, port);

	std::string connectPort;
	std::cout << "Enter port to start connection: ";
	std::getline(std::cin, connectPort);
	
	if (ClientP2P::Bind(port, OnMessage))
	{
		if (connectPort != "")
		{
			ClientP2P::AddPeer("127.0.0.1", connectPort, -1);
		}

		std::thread t(InputThread);

		t.join();

		std::lock_guard<std::mutex> guard(s_Mutex);

		m_Finished = true;

		ClientP2P::Dispose();
	}
}