#pragma once

#include <iostream>
#include <mutex>
#include <winsock2.h>

extern std::mutex g_LogMutex;

#define NETWORK_LOG_ENABLE

#ifdef NETWORK_LOG_ENABLE
#define NETWORK_LOG_THREAD_GUARD() std::lock_guard<std::mutex> __guard(g_LogMutex)
#define NETWORK_LOG(msg) {NETWORK_LOG_THREAD_GUARD(); std::cout << "[INFO]: " << msg << std::endl;}
#define NETWORK_LOG_ARG(msg, arg) {NETWORK_LOG_THREAD_GUARD(); std::cout << "[INFO]: " << msg << arg << std::endl;}
#define NETWORK_LOG_ERR(msg) {NETWORK_LOG_THREAD_GUARD(); std::cout << "[ERROR]: " << msg << std::endl;}
#define NETWORK_LOG_ERR_CODE(functionName, err) {NETWORK_LOG_THREAD_GUARD(); std::cout  << "[WINSOCK ERROR]: " << functionName << "() failed with error: " << err << std::endl;}
#define NETWORK_LOG_ERR_WIN(functionName) NETWORK_LOG_ERR_CODE(functionName, WSAGetLastError())
#else
#define NETWORK_LOG_THREAD_GUARD()
#define NETWORK_LOG(msg)
#define NETWORK_LOG_ARG(msg, arg)
#define NETWORK_LOG_ERR(msg)
#define NETWORK_LOG_ERR_CODE(functionName, err)
#define NETWORK_LOG_ERR_WIN(functionName)
#endif

class Network
{
public:
	static bool Init();
};