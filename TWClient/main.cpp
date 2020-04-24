#include "Client.h"
#include "PACKETENUM.h"
#include <Windows.h>
#include <string.h>
DWORD pid;
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")




Client* myClient;

#define DEFAULT_PORT 21370
#define MP_TICKRATE 128

std::string coords;

void SecureUsername()
{
	bool usernameAccepted = false;
	do
	{
		std::cout << "NICK: ";
		std::string username;
		std::getline(std::cin, username);
		myClient->SendString(username);

		myClient->GetBool(usernameAccepted);
		if (!usernameAccepted)
			std::cout << "Nick zajety" << std::endl;

	} while (!usernameAccepted);
}

int main(int argc, char **argv)
{
	/*
	HWND hWnd = FindWindowA(0, ("Island"));


	GetWindowThreadProcessId(hWnd, &pid);
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);



	if (hWnd != NULL)
	{
		std::cout << "===========================================\n";
		std::cout << "Tajemnicza Wyspa.exe [PROCESS ID = " << hWnd << "]\n";
		std::cout << "===========================================\n";
	}
	else {
		std::cout << "NIE MOZNA ZAINICJALOZIOWAC PROCESU Z GRA.";
		getchar();
		return 1;
	}
	*/
	myClient = new Client(argc, argv, DEFAULT_PORT);

	if (!myClient->Connect())
	{
		system("pause");
		return 1;
	}
	SecureUsername();
	myClient->StartSubRoutine();


	float posXts, posYts, posZts;
	while (true)
	{
		/*
		ReadProcessMemory(pHandle, (void*)0x00E16974, &posXts, sizeof(posXts), 0);
		ReadProcessMemory(pHandle, (void*)0x00E16978, &posYts, sizeof(posYts), 0);
		ReadProcessMemory(pHandle, (void*)0x00E1697C, &posZts, sizeof(posZts), 0);
		std::string posXtsRQ = std::to_string(posXts);
		std::string posYtsRQ = std::to_string(posYts);
		std::string posZtsRQ = std::to_string(posZts);
		std::string position = posXtsRQ + " " + posYtsRQ + " " + posZtsRQ;
		*/
		!myClient->SendString("xD");
		Sleep(1000/MP_TICKRATE);
	}
	system("pause");
	WSACleanup();
	system("pause");
	return 0;
}