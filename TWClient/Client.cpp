#include "Client.h"
#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <sstream>

DWORD pida;
HWND hWnd;
HANDLE pHandle;

Client::Client(int argc, char **argv, int PORT)
{
	WSADATA wsaData;
	hWnd = FindWindowA(0, ("Island"));
	GetWindowThreadProcessId(hWnd, &pida);
	pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pida);



	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("[BLAD INICJOWANIA WINSOCK]: %d\n", iResult);
		exit(0);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	std::string ipAdr;
	std::cout << "Podaj adres IP: ";
	std::cin >> ipAdr;


	iResult = getaddrinfo(ipAdr.c_str(), std::to_string(PORT).c_str(), &hints, &result);
	if (iResult != 0) {
		printf("[BLAD POBIERANIA ADRESU IP]: %d\n", iResult);
		WSACleanup();
	}

	clientPtr = this;
}

int str_length;


bool Client::Connect()
{
	bool connected = false;
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		ServerConnection = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ServerConnection == INVALID_SOCKET) {
			printf("[PODCZAS TWORZENIA SOCKETU WYSTAPIL BLAD]: %ld\n", WSAGetLastError());
			WSACleanup();
			return false;
		}

		iResult = connect(ServerConnection, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			std::cout << "[NIE MOZNA POLACZYC Z SERWEREM]: " << WSAGetLastError() << std::endl;
			closesocket(ServerConnection);
			ServerConnection = INVALID_SOCKET;
			continue;
		}
		else
		{
			connected = true;
			break;

		}
	}

	return connected;
}

float OutputCoords[3];


bool Client::ProcessPacket(PACKET packetType)
{

	switch (packetType)
	{
	case P_ChatMessage:
	{	 
		std::string message;
		if (!GetString(message))
			return false;

		std::istringstream ss(message);
		std::copy(
			std::istream_iterator <float>(ss),
			std::istream_iterator <float>(),
			OutputCoords
		);
		//debug
		std::cout << OutputCoords[0] << "/" << OutputCoords[1] << "/" << OutputCoords[2] << std::endl;

		WriteProcessMemory(pHandle, (void*)0x00E16C64, &OutputCoords[0], sizeof(OutputCoords[0]), 0);
		WriteProcessMemory(pHandle, (void*)0x00E16C68, &OutputCoords[1], sizeof(OutputCoords[1]), 0);
		WriteProcessMemory(pHandle, (void*)0x00E16C6C, &OutputCoords[2], sizeof(OutputCoords[2]), 0);
		break;
	}

	case P_DirectMessage:
	{
		bool value;
		if (!GetBool(value))
		DM_Failed = value;
		break;
	}
	default:
		std::cout << "[NIEROZPOZNANY PAKIET]: " << packetType << std::endl;
	}
	return true;
}

void Client::ClientHandler()
{
	PACKET packetType;
	while (true)
	{
		if (!clientPtr->GetPacketType(packetType))
			break;
		if (!clientPtr->ProcessPacket(packetType))
			break;
	}

	//tutaj blad podczas laczenia mozna dac

	std::cout << "[UTRACONO POLACZENIE Z SERWEREM]" << std::endl;
	if (clientPtr->CloseConnection())
	{
		std::cout << "[POPRAWNIE ZAMKNIETO SOCKET]" << std::endl;
	}
}

bool Client::SendString(const std::string& value)
{
	if (!SendPacketType(P_ChatMessage))
		return false;

	int bufferLength = value.size();
	if (!SendInt(bufferLength))
		return false;

	int returnCheck = send(ServerConnection, value.c_str(), bufferLength, NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::SendDirectMessage(const std::string& value)
{
	if (!SendPacketType(P_DirectMessage))
		return false;

	int bufferLength = value.size();
	if (!SendInt(bufferLength))
		return false;

	int returnCheck = send(ServerConnection, value.c_str(), bufferLength, NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::SendInt(const int& value) const
{
	int returnCheck = send(ServerConnection, (char*)&value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::GetInt(int& value)
{
	int returnCheck = recv(ServerConnection, (char*)& value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::SendPacketType(const PACKET& packetType)
{
	int returnCheck = send(ServerConnection, (char*)& packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::GetPacketType(PACKET& packetType)
{
	int returnCheck = recv(ServerConnection, (char*)& packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::GetString(std::string& value)
{
	int bufferLength = 0;
	if (!GetInt(bufferLength))
		return false;

	char* buffer = new char[bufferLength + 1];
	buffer[bufferLength] = '\0';

	int returnCheck = recv(ServerConnection, buffer, bufferLength, NULL);
	value = buffer;
	delete[] buffer;

	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::SendBool(bool value)
{
	int returnCheck = send(ServerConnection, (char*)& value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false; 

	return true; 
}

bool Client::GetBool(bool& value)
{
	int returnCheck = recv(ServerConnection, (char*)& value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::CloseConnection()
{
	if (closesocket(ServerConnection == SOCKET_ERROR))
	{
		if (WSAGetLastError() == WSAENOTSOCK)
			return true;

		std::cout << "[NIE MOZNA ZAMKNAC SOCKETU]: " << std::to_string (WSAGetLastError()) << std::endl;
		return false;
	}

	return true;
}
