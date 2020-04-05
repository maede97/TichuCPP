#include <string>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <TGUI/TGUI.hpp>

#include <TichuLib/Utils.h>
#include <TichuLib/Player.h>

// hide client window
#define _WIN32_WINNT 0x0500
#include <windows.h>


enum class GAMESTATE {
	LOGIN,
	PLAYING
};

class Client {
public:
	Client() {
	}

	void connect(std::string ip, int port, std::string username) {
		sf::Socket::Status status = socket.connect(ip, port);
		while (status != sf::Socket::Done) {
			status = socket.connect(ip, port);
		}
		std::cout << "Connected to server" << std::endl;
		sf::Packet packet;

		packet << username; // store username in this packet
		if (socket.send(packet)) {
			std::cout << "Could not send username, exit" << std::endl;
			return;
		}

		socket.setBlocking(false); // set to not be blocking!
	}
	~Client() {

	}

	void setPlayer(Tichu::Player* player) {
		this->player = player;
	}

	/* should be called every frame/loop */
	void mainloop(sf::RenderWindow& window) {
		sf::Packet packet;
		socket.receive(packet);
		if (packet.getDataSize() == 0) {
			return;
		}
		// process packet
		int i;
		packet >> i;
		Tichu::PACKET_TYPES pack_type = Tichu::PACKET_TYPES(i);

		switch (pack_type)
		{
		case Tichu::PACKET_TYPES::USERS: {
			// read all user names
			std::cout << "Connected Users:" << std::endl;
			for (int i = 0; i < 4; i++) {
				std::string user;
				packet >> user;
				std::cout << user << std::endl;
			}
			break;
		}
		case Tichu::PACKET_TYPES::CARDS: {
			int amount;
			packet >> amount;
			int c, h;
			for (int i = 0; i < amount; i++) {
				packet >> c >> h;
				player->addCard(Tichu::Card(std::pair<int, int>(c, h)));
			}

			std::cout << "I have those cards:" << std::endl;
			for (auto& ca : player->getCards()) {
				std::cout << " - " << ca.getCardShortage().first << "/" << ca.getCardShortage().second << std::endl;
			}

			break;
		}
		default:
			break;
		}
	}

private:
	sf::TcpSocket socket;
	std::string username;
	Tichu::Player* player = nullptr;
};

void login(tgui::EditBox::Ptr ip, tgui::EditBox::Ptr port, tgui::EditBox::Ptr username, Client* client, Tichu::Player* player, GAMESTATE* state) {
	try {
		player->setName(username->getText().toAnsiString());
		client->connect(ip->getText().toAnsiString(), std::stoi(port->getText().toAnsiString()), player->getName());
		client->setPlayer(player);

		*state = GAMESTATE::PLAYING;
	}
	catch (std::invalid_argument e) {
		std::cout << "Wrong argument given. Check port.";
	}
}

int main()
{
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);


	GAMESTATE state = GAMESTATE::LOGIN;

	Client* client = new Client();
	Tichu::Player* player = new Tichu::Player();

	sf::RenderWindow window(sf::VideoMode(800, 600), "Tichu!");

	tgui::Gui gui(window);
	auto editBoxIP = tgui::EditBox::create();
	editBoxIP->setSize({ "66.67%", "12.5%" });
	editBoxIP->setPosition({ "16.67%", "14%" });
	editBoxIP->setDefaultText("192.168.1.125");
	editBoxIP->setText("192.168.1.125");
	editBoxIP->setCaretPosition(0);
	gui.add(editBoxIP);

	auto editBoxPort = tgui::EditBox::create();
	editBoxPort->setSize({ "66.67%", "12.5%" });
	editBoxPort->setPosition({ "16.67%", "32%" });
	editBoxPort->setDefaultText("5000");
	editBoxPort->setText("5000");
	editBoxPort->setCaretPosition(0);
	gui.add(editBoxPort);

	auto editBoxUsername = tgui::EditBox::create();
	editBoxUsername->setSize({ "66.67%", "12.5%" });
	editBoxUsername->setPosition({ "16.67%", "50%" });
	editBoxUsername->setDefaultText("Username");
	gui.add(editBoxUsername);

	auto button = tgui::Button::create("Login");
	button->setSize({ "50%", "16.67%" });
	button->setPosition({ "25%", "70%" });
	gui.add(button);

	button->connect("pressed", login, editBoxIP, editBoxPort, editBoxUsername, client, player, &state);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::Resized) {
				window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));
				if (state == GAMESTATE::LOGIN)
					gui.setView(window.getView());
			}

			if (state == GAMESTATE::LOGIN)
				gui.handleEvent(event);
		}

		window.clear(sf::Color(100, 255, 100)); // use some kind of green

			if (state == GAMESTATE::LOGIN) {
				gui.draw();
			}

		// invoke mainloop on client
		if (state == GAMESTATE::PLAYING) {
			client->mainloop(window);
		}

		window.display();
	}

	delete client;
	delete player;

	return 0;
}