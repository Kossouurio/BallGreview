#include <iostream>
#include "main.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
    /// SERVER
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        std::cout << "Erreur creation socket : " << WSAGetLastError() << std::endl;
        return 0;
    }

    sockaddr_in server;
    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0)
    {
        std::cout << "Erreur inet_pton : " << WSAGetLastError() << std::endl;
        return 0;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(9999);

    /// AFFICHAGE
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Client");
    sf::CircleShape ball(25.f);
    ball.setFillColor(sf::Color::Green);
    ball.setPosition({ 100.f, 100.f });
    sf::Vector2f direction(100.f, 100.f);

    sf::Clock clock;

    while (window.isOpen())
    {
        std::cout << "x: " << ball.getPosition().x << ", y: " << ball.getPosition().y << std::endl;

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        sf::Time deltaTime = clock.restart();
        ball.move(direction * deltaTime.asSeconds());

        if (ball.getPosition().x + ball.getRadius() * 2 > window.getSize().x || ball.getPosition().x < 0)
        {
            direction.x = -direction.x;
        }
        if (ball.getPosition().y + ball.getRadius() * 2 > window.getSize().y || ball.getPosition().y < 0)
        {
            direction.y = -direction.y;
        }

        sf::Vector2f position = ball.getPosition();
        std::string message = std::to_string(position.x) + "," + std::to_string(position.y);
        sendto(sock, message.c_str(), message.size(), 0, (sockaddr*)&server, sizeof(server));

        window.clear();
        window.draw(ball);
        window.display();
    }

    WSACleanup();
    return 0;
}
