#include <iostream>
#include <vector>
#include <thread>
#include <WinSock2.h>

#define BUFFER_SIZE 2048

#pragma comment(lib, "ws2_32.lib")
#pragma warning (disable:4996)

char* Calculate(std::vector<wchar_t>* str);
void ClientThread(SOCKET clientSocket, SOCKADDR_IN clientAddrIn, std::vector<std::thread*>* clientList);

int main()
{
	std::vector<std::thread*> clientList;

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "[Error] WSAStartup error." << std::endl;

		return 1;
	}

	SOCKET serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (serverSocket == INVALID_SOCKET)
	{
		std::cout << "[Error] socket error." << std::endl;

		return 1;
	}

	SOCKADDR_IN serverAddrIn;

	memset(&serverAddrIn, 0, sizeof(serverAddrIn));

	serverAddrIn.sin_family = AF_INET;
	serverAddrIn.sin_port = htons(2694);
	serverAddrIn.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(serverSocket, (SOCKADDR*)&serverAddrIn, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		std::cout << "[Error] bind error." << std::endl;

		return 1;
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "[Error] listen error." << std::endl;

		return 1;
	}

	std::cout << "[Log] Started server." << std::endl;

	while (1)
	{
		SOCKADDR_IN clientAddrIn;

		int addrLength = sizeof(SOCKADDR_IN);

		SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddrIn, &addrLength);

		clientList.push_back(new std::thread(ClientThread, clientSocket, clientAddrIn, &clientList));
	}

	if (clientList.size() > 0)
	{
		for (auto ptr = clientList.begin(); ptr < clientList.end(); ptr++)
		{
			(*ptr)->join();
		}
	}

	closesocket(serverSocket);
	WSACleanup();

	return 0;
}

char* Calculate(std::vector<wchar_t>* str)
{
	int p = 0;
	wchar_t out[BUFFER_SIZE + 2];
	wchar_t* result = new wchar_t[str->size()];

	memset(result, 0, sizeof(result));

	std::cout << "[Log] ";

	for (int n = 0; n < (str->size() / BUFFER_SIZE) + 1; n++)
	{
		int size = str->size();

		if (size > BUFFER_SIZE)
		{
			if (str->size() < (n + 1) * BUFFER_SIZE)
			{
				size = str->size() % BUFFER_SIZE;
			}
			else
			{
				size = BUFFER_SIZE;
			}
		}

		for (int i = 0; i < size; i++, p++)
		{
			out[i] = *(str->begin() + p);

			if (out[i] == '\0')
			{
				out[i] = ' ';
			}

			*(result + p) = out[i];
		}

		out[size] = '\0';

		wprintf(L"%s", out);
	}

	int a = 0;
	int b = 0;
	char resultValue[BUFFER_SIZE];
	wchar_t operatorType;
	wchar_t* ptr;

	memset(resultValue, 0, sizeof(resultValue));

	ptr = wcstok(result, L" ");
	a = _wtoi(ptr);
	ptr = wcstok(nullptr, L" ");
	operatorType = ptr[0];
	ptr = wcstok(nullptr, L" ");
	b = _wtoi(ptr);

	delete[] result;

	switch (operatorType)
	{
	case '+':
		sprintf(resultValue, "%d", a + b);

		break;
	case '-':
		sprintf(resultValue, "%d", a - b);

		break;
	case 'x':
		sprintf(resultValue, "%d", a * b);

		break;
	case '¡À':
		sprintf(resultValue, "%d", a / b);

		break;
	}

	std::cout << "= " << resultValue << std::endl;

	return resultValue;
}

void ClientThread(SOCKET clientSocket, SOCKADDR_IN clientAddrIn, std::vector<std::thread*>* clientList)
{
	std::cout << "[Log] Client connected IP : " << inet_ntoa(clientAddrIn.sin_addr) << ":" << ntohs(clientAddrIn.sin_port) << std::endl;

	std::vector<wchar_t> buffer;

	wchar_t x;

	while (1)
	{
		if (recv(clientSocket, (char*)&x, sizeof(x), 0) == SOCKET_ERROR)
		{
			std::cout << "[Error] recv error." << std::endl;

			break;
		}

		if (buffer.size() > 0 && x == '=')
		{
			if (*buffer.begin() == 'e' && *(buffer.begin() + 1) == 'x' && *(buffer.begin() + 2) == 'i' && *(buffer.begin() + 3) == 't')
			{
				break;
			}

			char* data = Calculate(&buffer);

			send(clientSocket, data, strlen(data) + 1, 0);

			buffer.clear();

			continue;
		}

		buffer.push_back(x);
	}

	closesocket(clientSocket);

	std::cout << "[Log] Client disconnected IP : " << inet_ntoa(clientAddrIn.sin_addr) << ":" << ntohs(clientAddrIn.sin_port) << std::endl;

	for (auto ptr = clientList->begin(); ptr < clientList->end(); ptr++)
	{
		if ((*ptr)->get_id() == std::this_thread::get_id())
		{
			clientList->erase(ptr);

			break;
		}
	}
}