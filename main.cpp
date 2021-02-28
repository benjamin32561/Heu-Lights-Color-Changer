#pragma comment (lib, "ws2_32.lib")

#include "WinHttpHandler.h"
#include <chrono>
#include <thread>

int main()
{
	WSAInitializer init;
	
	while (true)
	{
		All::send();
		std::this_thread::sleep_for(std::chrono::milliseconds(45));
	}

	return 0;
}
