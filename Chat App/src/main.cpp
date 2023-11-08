/**
* RESOURCES:
*  - https://learn.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock
* 
* TODO:
*  - Commands
*  - Handle spaces
*  - Handle names
*  - Handle disconnects (removing from list)
*/

#include "Network.h"
#include "Client.h"

static std::mutex s_Mutex;
static bool m_Finished;

static void OnListen(std::string s)
{
	std::cout << " > " << s << std::endl;
}

static void InputThread()
{
	bool isDone = false;

	while (!isDone)
	{
		std::string s;
		std::cin >> s;

		Client::Send(s);

		s_Mutex.lock();
		isDone = m_Finished;
		s_Mutex.unlock();
	}
}

static void ListenThread()
{
	Client::Listen(OnListen);
	Client::Disconnect();

	s_Mutex.lock();
	m_Finished = true;
	s_Mutex.unlock();
}

int main()
{
	Network::Init();

	if (Client::Connect("localhost", "1337"))
	{
		std::thread t1(InputThread);
		std::thread t2(ListenThread);

		t1.join();
		t2.join();
	}

	return 0;
}