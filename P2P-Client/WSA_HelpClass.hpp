#pragma once

#include <WinSock2.h>

class WSAHelp
{
public:
	WSAHelp(BYTE minorVer = 2, BYTE majorVer = 2)
	{
		StartUp(minorVer, majorVer);
	}
	~WSAHelp()
	{
		CleanUp();
	}

	BOOL StartUp(BYTE minorVer = 2, BYTE majorVer = 2)
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(minorVer, majorVer), &wsaData) != 0)
		{
			return false;
		}

		if (LOBYTE(wsaData.wVersion) != minorVer || HIBYTE(wsaData.wVersion) != majorVer)
		{
			return false;
		}

		return true;
	}

	BOOL CleanUp(VOID)
	{
		return WSACleanup() == 0;
	}
};