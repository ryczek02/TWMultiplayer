#include "Server.h"

#define SERVER_IP "127.0.0.1"

Server::Server(int PORT)
{
	WSADATA wsaData;

	ListenSocket = INVALID_SOCKET;

	printf("[ZAINICJOWANO WINSOCK]\n");

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("Blad utworzenia WSAStartup: %d\n", iResult);
		exit(0);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(SERVER_IP, std::to_string(PORT).c_str(), &hints, &result);
	if (iResult != 0)
	{
		printf("Pobieranie adresu: %d\n", iResult);
		WSACleanup();
		exit(0);
	}

	printf("[POBRANO ZADANY ADRES: %s]\n", SERVER_IP);

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("Blad socketu: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(0);
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Blad bindowania: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(0);
	}

	freeaddrinfo(result);

	printf("[UTWORZONO SOCKET NASLUCHUJACY]\n");

	//listen for incoming connection
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("[BLAD NASLUCHIWANIA]: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(0);
	}
	printf("[SERWER NASLUCHUJE NA IP: %s:%i]\n", SERVER_IP, PORT);
	serverPtr = this;
}

bool Server::ListenForNewConnection()
{
	SOCKET ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET)
	{
		printf("[BLAD AKCEPTACJI KLIENTA]: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	else // sucessfull connection
	{
		std::cout << "[UZYTKOWNIK PODLACZONY]" << std::endl;
		std::cout << "[OCZEKIWANIE NA WYBRANIE NICKU]" << std::endl;
		Connections[ConnectionCounter] = ClientSocket;
		Getusername(ConnectionCounter);

		connectionThreads[ConnectionCounter] = std::thread(ClientHandler, ConnectionCounter);
		ConnectionCounter++;
		return true;
	}
}

bool Server::SendInt(int id, int value)
{
	int returnCheck = send(Connections[id], (char*)&value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::GetInt(int id, int& value)
{
	int returnCheck = recv(Connections[id], (char*)&value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::SendBool(int id, bool value)
{
	int returnCheck = send(Connections[id], (char*)&value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::GetBool(int id, bool& value)
{
	int returnCheck = recv(Connections[id], (char*)&value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::SendPacketType(int id, const PACKET& packetType)
{
	int returnCheck = send(Connections[id], (char*)&packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::GetPacketType(int id, PACKET& packetType)
{
	int returnCheck = recv(Connections[id], (char*)&packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::SendString(int id, const std::string& value)
{
	if (!SendPacketType(id, P_ChatMessage))
		return false;

	int bufferLength = value.size();
	if (!SendInt(id, bufferLength))
		return false;

	int returnCheck = send(Connections[id], value.c_str(), bufferLength, NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::GetString(int id, std::string& value)
{
	int bufferLength = 0;
	if (!GetInt(id, bufferLength))
		return false;

	char* buffer = new char[bufferLength + 1];

	int returnCheck = recv(Connections[id], buffer, bufferLength, NULL);
	buffer[bufferLength] = '\0';
	value = buffer;
	delete[] buffer;

	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::ProcessPacket(int index, PACKET packetType)
{
	switch (packetType)
	{
	case P_ChatMessage:
	{
		std::string message;
		if (!GetString(index, message))
			return false;
		for (int i = 0; i < ConnectionCounter; i++)
		{
			if (i == index)
				continue;
			//pakiet z nickiem
			//std::string newMessage = usernames[index]+ ": " + message;
			if (!SendString(i, message))
				std::cout << "[NIE MOZNA WYSLAC WIADOMOSCI: " << index << " DO " << i << "]" << std::endl;
		}

		std::cout << "[ODEBRANO PAKIET OD ID = " << index << " ]" << std::endl;
		break;
	}

	case P_DirectMessage:
	{
		std::cout << "[WIADOMOSC BEZPOSREDNIA]: " << std::endl;
		std::string user;
		std::string message;

		std::string value;

		int usernameIndex = -1;
		bool userExists = false;

		//get user
		if (!GetString(index, value))
			return false;

		int val = 0;
		//get desired user
		while (value[val] != ' ')
		{
			user += value[val];
			val++;
		}

		//Check if user Exists
		for (int i = 0; i < ConnectionCounter; i++)
		{
			if (usernames[i] == user)
			{
				userExists = true;
				usernameIndex = i;
				break;
			}
		}

		if (userExists)
		{
			//get message
			for (int i = val; i < value.size(); i++)
			{
				message += value[i];
			}
		}

		SendPacketType(index, P_DirectMessage);
		SendBool(index, userExists);

		if (userExists)
		{
			std::string fullMessage = "[BEZPOSREDNI PAKIET OD]: " + usernames[index] + ": " + message;

			SendString(usernameIndex, fullMessage);
		}

		break;
	}

	default:
		std::cout << "[NIEROZPOZNANY PAKIET]: " << packetType << std::endl;
		break;
	}
	return true;
}

bool Server::CloseConnection(int index)
{
	if (closesocket(Connections[index]) == SOCKET_ERROR)
	{
		std::cout << "[BLAD ZAMYKANIA SOCKETU]: " << WSAGetLastError() << std::endl;
		return false;
	}

	return true;
}

void Server::Getusername(int index)
{
	serverPtr->usernames.push_back("");
	PACKET packetType;

	//Get Username
	bool usernameSaved = true;
	do
	{
		usernameSaved = true;
		if (!serverPtr->GetPacketType(index, packetType))
		{
			std::cout << "[NIE MOZNA POBRAC NAZWY UZYTKOWNIKA]" << std::endl;
			break;
		}

		if (!packetType == P_ChatMessage)
		{
			std::cout << "[POBIERANIE NAZWY UZYTKOWNIKA NIE JEST WIADOMOSCIA]" << std::endl;
			break;
		}

		std::string userName;
		serverPtr->GetString(index, userName);
		/*
		for each (std::string var in serverPtr->usernames)
		{
			if (var == userName)
			{
				usernameSaved = false;
				break;
			}
		}
		*/
		if (usernameSaved)
		{
			serverPtr->usernames[index] = userName;
			std::cout << "[NAZWA UZYTKOWAIKA: " << userName << " ZAPISANA NA " << index << "]" << std::endl;
		}
		serverPtr->SendBool(index, usernameSaved);

	} while (!usernameSaved);
}

//Bulk of work
void Server::ClientHandler(int index)
{
	PACKET packetType;
	while (true)
	{
		//Receive Messages
		if (!serverPtr->GetPacketType(index, packetType))
			break;
		if (!serverPtr->ProcessPacket(index, packetType))
			break;
	}

	std::cout << "[UTRACONO POLACZENIE Z ID = " << index << "]" << std::endl;
	serverPtr->CloseConnection(index);
}
