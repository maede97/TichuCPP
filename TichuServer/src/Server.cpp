#include <SFML/Network.hpp>
#include <iostream>

#include <TichuLib/Utils.h>
#include <TichuLib/Player.h>
#include <TichuLib/Game.h>

#define PORT 5000

class Server {
public:
	Server(int port) {
		if (listener.listen(port) != sf::Socket::Done) {
			std::cout << "Could NOT bind to port. Abort." << std::endl;
			return;
		}

		for (int i = 0; i < 4; i++) {
			players.push_back(std::make_pair<sf::TcpSocket*, Tichu::Player*>(new sf::TcpSocket(), nullptr));
			if (listener.accept(*players[i].first) != sf::Socket::Done) {
				std::cout << "Error on player " << i + 1 << std::endl;
				return;
			}
			sf::Packet packet;
			if (players[i].first->receive(packet) != sf::Socket::Done) {
				std::cout << "Error getting player username" << std::endl;
			}
			std::string username;
			packet >> username;
			players[i].second = new Tichu::Player(username);
			std::cout << "Player " << username << " connected." << std::endl;
		}
		// send names to all
		sf::Packet packet;
		packet << (int)Tichu::PACKET_TYPES::USERS;
		for (int i = 0; i < 4; i++) {
			packet << players[i].second->getName();
		}
		sendAll(packet);
	}

	void sendAll(sf::Packet packet) {
		for (int i = 0; i < 4; i++) {
			players[i].first->send(packet);
		}
	}

	void sendTo(sf::Packet packet, int i) {
		players[i].first->send(packet);
	}

	void mainloop() {
		sf::sleep(sf::seconds(1));
	}

	~Server() {
		for (int i = 0; i < players.size(); i++) {
			delete players[i].first;
			delete players[i].second;
		}
	}

	void setGame(Tichu::Game* game) {
		this->game = game;

		// set usernames
		for (int i = 0; i < 4; i++) {
			this->game->addPlayer(players[i].second);
		}
	}

	void startGame() {
		this->game->distributeDeckOfCards();

		// send cards to each
		for (int i = 0; i < 4; i++) {
			sf::Packet packet;
			packet << (int)Tichu::PACKET_TYPES::CARDS;

			packet << 14; // 14 cards are following

			std::vector<Tichu::Card> cards = players[i].second->getCards();
			for (int j = 0; j < cards.size(); j++) {
				packet << cards[j].getCardShortage().first << cards[j].getCardShortage().second;
			}

			sendTo(packet, i);
		}
		std::cout << "Distributed Deck of cards" << std::endl;
	}



private:
	sf::TcpListener listener;
	std::vector<std::pair<sf::TcpSocket*, Tichu::Player*>> players;
	Tichu::Game* game;
};

int main(int argc, const char* argv[]) {
	Server server(PORT);

	Tichu::Game game;

	server.setGame(&game);
	server.startGame();



	while (true)
		server.mainloop();
	return 0;
}