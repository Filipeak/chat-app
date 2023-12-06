#include <Network.h>
#include <Client.h>

#include <thread>
#include <mutex>

static std::mutex s_Mutex;
static Client s_Client;
static bool m_Finished;

static void OnListen(NetworkCommand cmd)
{
	if (cmd.type == NETWORK_COMMAND_USER_MESSAGE_)
	{
		std::cout << " > " << cmd.payload << std::endl;
	}
	else if (cmd.type == NETWORK_COMMAND_USER_JOIN_)
	{
		std::cout << "[SERVER] User: " << cmd.payload << " has joined!" << std::endl;
	}
	else if (cmd.type == NETWORK_COMMAND_USER_LEAVE_)
	{
		std::cout << "[SERVER] User: " << cmd.payload << " has left!" << std::endl;
	}
}

static void InputThread()
{
	while (!m_Finished)
	{
		std::string s;
		std::getline(std::cin, s);

		s_Client.Send({ NETWORK_COMMAND_USER_MESSAGE , s });
	}
}

static void ListenThread()
{
	s_Client.Listen(OnListen);
	s_Client.Disconnect();

	std::lock_guard<std::mutex> guard(s_Mutex);

	m_Finished = true;
}

int main()
{
	Network::Init();

	std::string nickname;
	std::cout << "Enter nickname: ";
	std::getline(std::cin, nickname);

	if (s_Client.Connect("127.0.0.1", "1337"))
	{
		s_Client.Send({ NETWORK_COMMAND_USER_JOIN , nickname });

		std::thread t1(InputThread);
		std::thread t2(ListenThread);

		t1.join();
		t2.join();
	}
}