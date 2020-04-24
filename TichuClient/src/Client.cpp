#include <string>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <TGUI/TGUI.hpp>

#include <TichuLib/Utils.h>
#include <TichuLib/Player.h>
#include <TichuLib/Game.h>

// hide client window
#define _WIN32_WINNT 0x0500
#include <windows.h>

#define DATA_FOLDER "C:/Users/Matthias/Documents/Github/TichuCPP/data"


enum class GAMESTATE {
	LOGIN,
	PLAYING
};

enum class ACTION {
	SEND_CARDS,
	AWAIT_SEND_CARDS_CONFIRMATION,
	PASS,
	PLAY_BOMB,
	MY_TURN,
	NOTHING
};

struct PlayerData {
	std::string name;
	int cards = 14;
	tgui::Label::Ptr playerLabel;
};

class SelectableCard {
public:
	bool isClicked(int mouseX, int mouseY) {
		if (selected) {
			if (rectSelected.getGlobalBounds().contains(mouseX, mouseY)) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			if (rectNormal.getGlobalBounds().contains(mouseX, mouseY)) {
				return true;
			}
			else {
				return false;
			}
		}
	}

	void toggle() {
		selected = !selected;
		if (selected) {
			image.setPosition(rectSelected.getPosition());
		}
		else {
			image.setPosition(rectNormal.getPosition());
		}
	}

	void draw(sf::RenderWindow& window) {
		if (selected) {
			window.draw(rectSelected);
		}
		else {
			window.draw(rectNormal);
		}
		window.draw(image);
	}
public:
	sf::RectangleShape rectNormal;
	sf::RectangleShape rectSelected;
	bool selected = false;
	Tichu::Card card;
	sf::Sprite image;
};

struct MiddleCard {
	sf::RectangleShape rect;
	Tichu::Card card;
	void draw(sf::RenderWindow& window) {
		window.draw(rect);
		window.draw(image);
	}

	sf::Sprite image;
};

class Client {
public:
	Client() {
		playerData.resize(4);
		font.loadFromFile(DATA_FOLDER"/orange juice 2.0.ttf");

		// load all images
		for (auto& c : allCards) {
			cardImages.push_back(new sf::Texture());
			cardImages.back()->loadFromFile(DATA_FOLDER"/images/"+c.getFileName());
		}

	}

	bool connect(std::string ip, int port, std::string username) {
		sf::Socket::Status status = socket.connect(ip, port);
		if (status != sf::Socket::Done) {
			MessageBoxA(NULL, "Could not connect to server.", "Error", MB_OK | MB_ICONWARNING);
			return false;
		}
		std::cout << "Connected to server" << std::endl;
		sf::Packet packet;

		packet << username; // store username in this packet
		if (socket.send(packet)) {
			std::cout << "Could not send username, exit" << std::endl;
			return false;
		}

		socket.setBlocking(false); // set to not be blocking!
		return true;
	}
	~Client() {

	}

	void setPlayer(Tichu::Player* player) {
		this->player = player;
	}

	/* should be called every frame/loop */
	void mainloop(sf::RenderWindow& window, ACTION& action) {
		sf::Packet packet;
		socket.receive(packet);
		if (packet.getDataSize() != 0) {

			// process packet
			int i;
			packet >> i;
			Tichu::PACKET_TYPES pack_type = Tichu::PACKET_TYPES(i);

			switch (pack_type)
			{
			case Tichu::PACKET_TYPES::USERS: {
				// read all user names
				std::cout << "Connected Users:" << std::endl;
				std::vector<std::string> usernames;
				for (int i = 0; i < 4; i++) {
					std::string user;
					packet >> user;
					usernames.push_back(user);
					if (user == player->getName()) {
						my_index = i;
					}
					std::cout << user << std::endl;
				}
				// set up usernames in struct
				for (int i = my_index; i < my_index + 4; i++) {
					playerData[i - my_index].name = usernames[i % 4];
				}
				break;
			}

			case Tichu::PACKET_TYPES::MIDDLE_CARDS: {
				std::cout << "Recv. middle cards ";
				middleCards.clear();
				int size;
				int c, h;
				packet >> size;
				std::cout << size << std::endl;
				middleCards.reserve(size);
				for (int i = 0; i < size; i++) {
					packet >> c >> h;
					middleCards.push_back(MiddleCard());
					middleCards.back().card = Tichu::Card(Tichu::CARD_COLORS(c), Tichu::CARD_HEIGHTS(h));

					middleCards.back().rect = sf::RectangleShape(sf::Vector2f(20, 30));
					middleCards.back().rect.setPosition(window.getSize().x / 3 + i * 20, window.getSize().y / 2);

					middleCards.back().image.setTexture(*cardImages[middleCards.back().card.indexInOrderedDeck()], true);
					middleCards.back().image.setPosition(window.getSize().x / 3 + i * 20, window.getSize().y / 2);
				}
				std::cout << size << std::endl;
				break;
			}
			case Tichu::PACKET_TYPES::CARDS: {
				int amount;
				packet >> amount;
				int c, h;

				int cW = window.getSize().x / 17;
				int cH = cW * 1.4;
				for (int i = 0; i < amount; i++) {
					packet >> c >> h;
					player->addCard(Tichu::Card(std::pair<int, int>(c, h)));
					selectableCards.push_back(SelectableCard());
					selectableCards.back().card = Tichu::Card(std::pair<int, int>(c, h));

					selectableCards.back().rectNormal = sf::RectangleShape(sf::Vector2f(cW, cH));
					selectableCards.back().rectNormal.setPosition(cW + i * cW + cW / 16 * i, window.getSize().y - cH);

					selectableCards.back().rectSelected = sf::RectangleShape(sf::Vector2f(cW, cH));
					selectableCards.back().rectSelected.setPosition(cW + i * cW + cW / 16 * i, window.getSize().y - cH * 1.5);

					selectableCards.back().image.setTexture(*cardImages[selectableCards.back().card.indexInOrderedDeck()], true);
					selectableCards.back().image.setPosition(cW + i * cW + cW / 16 * i, window.getSize().y - cH);
				}

				std::cout << "I have those cards:" << std::endl;
				for (auto& ca : player->getCards()) {
					std::cout << " - " << ca.getCardShortage().first << "/" << ca.getCardShortage().second << std::endl;
				}

				break;
			}
			case Tichu::PACKET_TYPES::PLAY_VALID: {
				if (action == ACTION::AWAIT_SEND_CARDS_CONFIRMATION) {
					// my cards are valid, remove them from my deck
					for (auto& c : playedCards->getCards()) {
						player->removeCard(c);
						for (auto it = selectableCards.begin(); it != selectableCards.end(); it++) {
							if (it->card.isSame(c)) {
								selectableCards.erase(it);
								break;
							}
						}
					}
				}
				break;
			}
			case Tichu::PACKET_TYPES::YOUR_TURN: {
				action = ACTION::MY_TURN;
				std::cout << "IT IS MY TURN." << std::endl;
				//MessageBoxA(NULL, "It is your turn. Please play your cards or pass.", "Information", MB_OK | MB_ICONINFORMATION);
				break;
			}
			case Tichu::PACKET_TYPES::REMAINING_CARDS: {
				for (int i = 0; i < 4; i++) {
					packet >> playerData[(i - my_index + 4) % 4].cards;
				}
				std::cout << "I have " << playerData[0].cards << " cards left." << std::endl;
				for (int i = 1; i < 4; i++) {
					playerData[i].playerLabel->setText(playerData[i].name + ": " + std::to_string(playerData[i].cards) + " cards");
					std::cout << "Updating label to " << playerData[i].playerLabel->getText().toAnsiString() << std::endl;
				}
			}
			default:
				break;
			}
		}

		switch (action)
		{
		case ACTION::SEND_CARDS: {
			// send a single card...
			playedCards = getPlayedCards();
			if (playedCards == nullptr) {
				action = ACTION::NOTHING;
				break;
			}
			std::cout << "Sending cards with type " << (int)playedCards->getType() << std::endl;
			sf::Packet pack;
			pack << (int)Tichu::PACKET_TYPES::PLAY_CARDS;
			if (playedCards)  delete playedCards;
			for (auto& i : playedCards->getCardPacketData()) {
				pack << i;
			}
			socket.send(pack);
			action = ACTION::AWAIT_SEND_CARDS_CONFIRMATION;
			std::cout << "Awaiting confirmation" << std::endl;
			break;
		}
		case ACTION::PASS: {
			sf::Packet pack;
			pack << (int)Tichu::PACKET_TYPES::PLAY_PASS;
			socket.send(pack);
			action = ACTION::NOTHING;
		}
		default:
			break;
		}
	}

	Tichu::PlayedBase* getPlayedCards() {
		std::vector<Tichu::Card> cards;
		for (auto& c : selectableCards) {
			if (c.selected) {
				cards.push_back(c.card);
			}
		}
		if (cards.size() == 0) {
			return nullptr;
		}

		// try to detect what the user want's to play...
		if (cards.size() == 1) {
			return new Tichu::PlayedSingle(cards[0]);
		}
		else if (cards.size() == 2) {
			Tichu::Card cards2[2];
			cards2[0] = cards[0];
			cards2[1] = cards[1];
			if (cards2[0] != cards2[1]) return nullptr;
			return new Tichu::PlayedPair(cards2);
		}
		else if (cards.size() == 3) {
			Tichu::Card cards3[3];
			cards3[0] = cards[0];
			cards3[1] = cards[1];
			cards3[2] = cards[2];
			if (cards3[0] != cards3[1] || cards3[0] != cards3[2]) return nullptr;
			return new Tichu::PlayedTriple(cards3);
		}
		else if (cards.size() == 5) {
			// fullhouse or straight
			// find pair and triple
			std::cout << "sorting..." << std::endl;
			std::sort(cards.begin(), cards.end());

			if (cards[0] == cards[1]) {
				if (cards[0] == cards[2] && cards[3] == cards[4]) {
					// fullhouse with triple, pair
					Tichu::Card card2[2];
					card2[0] = cards[3];
					card2[1] = cards[4];
					Tichu::Card card3[3];
					card3[0] = cards[0];
					card3[1] = cards[1];
					card3[2] = cards[2];
					return new Tichu::PlayedFullhouse(Tichu::PlayedTriple(card3), Tichu::PlayedPair(card2));
				}
				else if (cards[0] != cards[2] && cards[2] == cards[3] && cards[2] == cards[4]) {
					// fullhouse with pair, triple
					Tichu::Card card2[2];
					card2[0] = cards[0];
					card2[1] = cards[1];
					Tichu::Card card3[3];
					card3[0] = cards[2];
					card3[1] = cards[3];
					card3[2] = cards[4];
					return new Tichu::PlayedFullhouse(Tichu::PlayedTriple(card3), Tichu::PlayedPair(card2));
				}
			}
			else {
				// possibly a straight
				int height = (int)cards[0].getHeight();
				for (int i = 1; i < 5; i++) {
					if (height + 1 != (int)cards[i].getHeight()) {
						return nullptr;
					}
					else {
						height++;
					}
				}
				// we have a straight
				return new Tichu::PlayedStraight(cards);
			}
			return nullptr;
		}
		else {
			// multiple pairs or straight with more
			std::sort(cards.begin(), cards.end());

			if (cards[0] == cards[1]) {
				// should be multiple pairs
				int pairs = cards.size() / 2;
				int height = (int)cards[0].getHeight() - 1;
				std::vector<Tichu::PlayedPair> pairs_holder;
				for (int i = 0; i < pairs; i++) {
					if (cards[i * 2] != cards[i * 2 + 1]) {
						return nullptr;
					}
					if (height + 1 != (int)cards[2 * i].getHeight()) {
						return nullptr;
					}
					else {
						height++;
					}
					Tichu::Card cp[2];
					cp[0] = cards[2 * i];
					cp[1] = cards[2 * i + 1];
					pairs_holder.push_back(Tichu::PlayedPair(cp));
				}
				return new Tichu::PlayedFollowingPairs(pairs_holder);
			}
			else {
				// straight
				int height = (int)cards[0].getHeight();
				for (int i = 1; i < 5; i++) {
					if (height + 1 != (int)cards[i].getHeight()) {
						return nullptr;
					}
					else {
						height++;
					}
				}
				// we have a straight
				return new Tichu::PlayedStraight(cards);
			}
		}
		return nullptr; // not implemented yet
	}

	std::vector<SelectableCard>& getSelectableCards() {
		return selectableCards;
	}

	std::vector<MiddleCard>& getMiddleCards() {
		return middleCards;
	}

	std::vector<PlayerData>& getPlayerData() {
		return playerData;
	}

private:
	sf::TcpSocket socket;
	std::string username;
	Tichu::Player* player = nullptr;
	Tichu::PlayedBase* playedCards = nullptr;
	std::vector<PlayerData> playerData;
	std::vector<SelectableCard> selectableCards;
	std::vector<MiddleCard> middleCards;


	std::vector<Tichu::Card> allCards = Tichu::Game::getDeckOfCards();
	std::vector<sf::Texture*> cardImages;

	int my_index = 0;
	sf::Font font;
};

void login(tgui::EditBox::Ptr ip, tgui::EditBox::Ptr port, tgui::EditBox::Ptr username, Client* client, Tichu::Player* player, GAMESTATE* state, sf::RenderWindow* window) {
	try {
		player->setName(username->getText().toAnsiString());
		if (!client->connect(ip->getText().toAnsiString(), std::stoi(port->getText().toAnsiString()), player->getName())) {
			return; // there was an error
		}
		client->setPlayer(player);

		*state = GAMESTATE::PLAYING;
		window->setTitle("Tichu - " + username->getText().toAnsiString());
		std::cout << "Username: " << username->getText().toAnsiString() << std::endl;
	}
	catch (std::invalid_argument e) {
		std::cout << "Wrong argument given. Check port.";
	}
}

int main()
{
	// hide console window
	//HWND hWnd = GetConsoleWindow();
	//ShowWindow(hWnd, SW_HIDE);

	// Save our current state
	GAMESTATE state = GAMESTATE::LOGIN;
	ACTION currentAction = ACTION::NOTHING;

	Client* client = new Client();
	Tichu::Player* player = new Tichu::Player();

	sf::RenderWindow window(sf::VideoMode(800, 600), "Tichu!");

	tgui::Gui gui(window);
	auto editBoxIP = tgui::EditBox::create();
	editBoxIP->setSize({ "66.67%", "12.5%" });
	editBoxIP->setPosition({ "16.67%", "14%" });
	editBoxIP->setDefaultText("IP Address of Server");
	editBoxIP->setText("127.0.0.1");
	editBoxIP->setCaretPosition(0);
	gui.add(editBoxIP);

	auto editBoxPort = tgui::EditBox::create();
	editBoxPort->setSize({ "66.67%", "12.5%" });
	editBoxPort->setPosition({ "16.67%", "32%" });
	editBoxPort->setDefaultText("Port of Server");
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

	button->connect("pressed", login, editBoxIP, editBoxPort, editBoxUsername, client, player, &state, &window);

	tgui::Gui gameGui(window);
	auto& pData = client->getPlayerData();
	pData[1].playerLabel = tgui::Label::create("Player 2");
	pData[1].playerLabel->setSize({ "10%","10%" });
	pData[1].playerLabel->setPosition({ "90%", "50%" });
	pData[2].playerLabel = tgui::Label::create("Player 3");
	pData[2].playerLabel->setSize({ "10%","10%" });
	pData[2].playerLabel->setPosition({ "50%", "10%" });
	pData[3].playerLabel = tgui::Label::create("Player 4");
	pData[3].playerLabel->setSize({ "10%","10%" });
	pData[3].playerLabel->setPosition({ "10%", "50%" });
	gameGui.add(pData[1].playerLabel);
	gameGui.add(pData[2].playerLabel);
	gameGui.add(pData[3].playerLabel);

	auto& bPlay = tgui::Button::create("Play cards");
	bPlay->setSize({ "10%","10%" });
	bPlay->setPosition({ "50%", "75%" });
	gameGui.add(bPlay);

	bPlay->connect("pressed", [&]() {currentAction = ACTION::SEND_CARDS; });

	auto& bPass = tgui::Button::create("Pass");
	bPass->setSize({ "10%","10%" });
	bPass->setPosition({ "25%", "75%" });
	gameGui.add(bPass);

	bPass->connect("pressed", [&]() {currentAction = ACTION::PASS; });

	auto& bBomb = tgui::Button::create("Bomb!");
	bBomb->setSize({ "10%","10%" });
	bBomb->setPosition({ "75%", "75%" });
	gameGui.add(bBomb);

	bBomb->connect("pressed", [&]() {currentAction = ACTION::PLAY_BOMB; });

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
				if (state == GAMESTATE::LOGIN) {
					gui.setView(window.getView());
				}
				else {
					gameGui.setView(window.getView());
				}
			}
			else if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					for (auto& s : client->getSelectableCards()) {
						if (s.isClicked(event.mouseButton.x, event.mouseButton.y)) {
							s.toggle();
						}
					}

				}
			}

			if (state == GAMESTATE::LOGIN)
				gui.handleEvent(event);
			else {
				gameGui.handleEvent(event);
			}
		}

		window.clear(sf::Color(100, 255, 100)); // use some kind of green

		if (state == GAMESTATE::LOGIN) {
			gui.draw();
		}
		else {
			gameGui.draw();
			client->mainloop(window, currentAction);

			// draw card counters
			for (auto& s : client->getSelectableCards()) {
				s.draw(window);
			}
			for (auto& m : client->getMiddleCards()) {
				m.draw(window);
			}

		}

		window.display();
	}

	delete client;
	delete player;

	return 0;
}