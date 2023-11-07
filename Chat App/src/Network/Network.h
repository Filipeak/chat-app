#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define RECEIVE_BUFFER_LENGTH 512

#define NETWORK_LOG(msg) std::cout << "[INFO]: " << msg << std::endl
#define NETWORK_LOG_ARG(msg, arg) std::cout << "[INFO]: " << msg << arg << std::endl
#define NETWORK_LOG_ERR(msg) std::cout << "[ERROR]: " << msg << std::endl
#define NETWORK_LOG_ERR_CODE(functionName, err) std::cout  << "[WINSOCK ERROR]: " << functionName << "() failed with error: " << err << std::endl
#define NETWORK_LOG_ERR_WIN(functionName) NETWORK_LOG_ERR_CODE(functionName, WSAGetLastError())

class Network
{
public:
	static bool Init();
};