#include <iostream>
#include "main.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Thread.h"

#pragma comment(lib, "ws2_32.lib")

#define MAX_THREADS 3

sf::Vector2f position(100, 100);


void handleClient(SOCKET server)
{
	sockaddr_in clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	char buffer[1024];

	while (true)
	{
		int bytesReceived = recvfrom(server, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddr, &clientAddrSize);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cout << "Erreur recvfrom : " << WSAGetLastError() << std::endl;
			continue;
		}

		buffer[bytesReceived] = '\0';

		std::string data(buffer);
		size_t commaPos = data.find(',');
		float x = std::stof(data.substr(0, commaPos));
		float y = std::stof(data.substr(commaPos + 1));

		position.x = x;
		position.y = y;
		std::cout << "x: " << x << ", y: " << y << std::endl;
		//Sleep(100);
	}
}

int main()
{
	/// AFFICHAGE
	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Server");
	sf::CircleShape ball(25.f);
	ball.setFillColor(sf::Color::Red);
	ball.setPosition(position);
	sf::Clock clock;

	/// SERVER
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "Erreur initialisation WinSock : " << WSAGetLastError();
		return -1;
	}

	SOCKET server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server == INVALID_SOCKET)
	{
		std::cout << "Erreur initialisation socket : " << WSAGetLastError();
		return -2;
	}

	const unsigned short port = 9999;
	sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	int res = bind(server, (sockaddr*)&addr, sizeof(addr));
	if (res != 0)
	{
		std::cout << "Erreur bind : " << WSAGetLastError();
		return -3;
	}

	std::cout << "Serveur demarre sur le port " << port << std::endl;

	/// THREADS
	Thread* thread1 = new Thread();
	thread1->Start([server]() {handleClient(server); });

	/// AFFICHAGE
	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
		}

		ball.setPosition(position);

		window.clear();
		window.draw(ball);
		window.display();
	}


	closesocket(server);
	WSACleanup();
    return 0;
}
