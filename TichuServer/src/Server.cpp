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
			players[i].first->setBlocking(false); // set to not block
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

	void setNextPlayer() {
		for (int i = 0; i < 4; i++) {
			if (players[i].second == game->getCurrentPlayer()) {
				game->setCurrentPlayer(players[(i + 1) % 4].second);
				sf::Packet pack;
				pack << (int)Tichu::PACKET_TYPES::YOUR_TURN;
				sendTo(pack, (i + 1) % 4);
				std::cout << "Next Player: " << players[(i + 1) % 4].second->getName() << std::endl;
				return;
			}
		}
	}

	void mainloop() {
		for (int i = 0; i < 4; i++) {
			sf::Packet packet;
			players[i].first->receive(packet);
			if (packet.getDataSize() != 0) {

				int j;
				packet >> j;
				std::cout << "Received packet from " << players[i].second->getName() << " with type " << j << std::endl;

				Tichu::PACKET_TYPES pack_type = Tichu::PACKET_TYPES(j);
				switch (pack_type)
				{
				case Tichu::PACKET_TYPES::PLAY_CARDS: {
					std::vector<int> data;
					int curr;
					while (!packet.endOfPacket()) {
						packet >> curr;
						data.push_back(curr);
					}
					Tichu::PlayTypes plType = Tichu::PlayTypes(data[0]);
					Tichu::PlayedBase* cardsPlayed;
					switch (plType)
					{
					case Tichu::PlayTypes::SINGLE_CARD:
						cardsPlayed = new Tichu::PlayedSingle(data);
						break;
					case Tichu::PlayTypes::PAIR:
						cardsPlayed = new Tichu::PlayedPair(data);
						break;
					case Tichu::PlayTypes::FOLLOWING_PAIRS:
						cardsPlayed = new Tichu::PlayedFollowingPairs(data);
						break;
					case Tichu::PlayTypes::TRIPLE:
						cardsPlayed = new Tichu::PlayedTriple(data);
						break;
					case Tichu::PlayTypes::FULLHOUSE:
						cardsPlayed = new Tichu::PlayedFullhouse(data);
						break;
					case Tichu::PlayTypes::STRAIGHT:
						cardsPlayed = new Tichu::PlayedStraight(data);
						break;
					case Tichu::PlayTypes::BOMB:
						cardsPlayed = new Tichu::PlayedBomb(data);
						break;
					case Tichu::PlayTypes::PLAYTYPES_UNINITIALIZED:
						break;
					default:
						break;
					}

					if (game->playCards(players[i].second, cardsPlayed)) {
						std::cout << players[i].second->getName() << " played cards" << std::endl;

						// remove cards from player
						for (auto& c : cardsPlayed->getCards()) {
							players[i].second->removeCard(c);
						}

						sf::Packet packConf;
						packConf << (int)Tichu::PACKET_TYPES::PLAY_VALID;
						players[i].first->send(packConf);

						setNextPlayer();

						// send to all the current middle cards
						sf::Packet pack;
						pack << (int)Tichu::PACKET_TYPES::MIDDLE_CARDS;
						std::cout << "Top Cards: " << game->topCards()->getCards().size() << std::endl;
						pack << (int)game->topCards()->getCards().size();
						for (auto& c : game->topCards()->getCards()) {
							pack << c.getCardShortage().first << c.getCardShortage().second;
						}
						sendAll(pack);

						// send pack with amount of cards left
						sf::Packet p2;
						p2 << (int)Tichu::PACKET_TYPES::REMAINING_CARDS;
						for (int i = 0; i < players.size(); i++) {
							p2 << players[i].second->getRemainingCards();
							std::cout << "Remaining Cards " << players[i].second->getName() << ": " << players[i].second->getRemainingCards() << std::endl;
						}
						sendAll(p2);
					}
					break;
				}
				case Tichu::PACKET_TYPES::PLAY_PASS: {
					setNextPlayer();
					break;
				}
				default:
					break;
				}
			}
		}
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

		sf::Packet p2;
		p2 << (int)Tichu::PACKET_TYPES::REMAINING_CARDS;
		for (int i = 0; i < players.size(); i++) {
			p2 << players[i].second->getRemainingCards();
			std::cout << "Remaining Cards " << players[i].second->getName() << ": " << players[i].second->getRemainingCards() << std::endl;
		}
		sendAll(p2);

		// send current user
		for (int i = 0; i < 4; i++) {
			if (players[i].second == game->getCurrentPlayer()) {
				sf::Packet pack;
				pack << (int)Tichu::PACKET_TYPES::YOUR_TURN;
				sendTo(pack, i);
				std::cout << "Current Player: " << players[i].second->getName() << std::endl;
				break;
			}
		}
	}



private:
	sf::TcpListener listener;
	std::vector<std::pair<sf::TcpSocket*, Tichu::Player*>> players;
	Tichu::Game* game = nullptr;
};

int main(int argc, const char* argv[]) {
	Server server(PORT);

	Tichu::Game game;

	server.setGame(&game);
	server.startGame();

	while (true) {
		server.mainloop();
	}

	return 0;
}