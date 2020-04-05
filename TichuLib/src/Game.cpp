#include "..\include\TichuLib\Game.h"
#include <algorithm>
#include <random>

Tichu::Game::Game()
{
	players.reserve(4);
}

void Tichu::Game::addPlayer(Player* player)
{
	if (players.size() < 4) {
		players.push_back(player);
	}
}

void Tichu::Game::distributeDeckOfCards()
{
	// give each player 14 cards
	if (players.size() != 4) return;

	std::vector<Card> allCards(14 * 4);
	// create all cards
	int index = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 2; j < 15; j++) {
			allCards[index++] = Card(CARD_COLORS(i), CARD_HEIGHTS(j));
		}
	}
	allCards[index++] = Card(CARD_COLORS::SPECIAL, CARD_HEIGHTS::DOG);
	allCards[index++] = Card(CARD_COLORS::SPECIAL, CARD_HEIGHTS::DRAGON);
	allCards[index++] = Card(CARD_COLORS::SPECIAL, CARD_HEIGHTS::PHOENIX);
	allCards[index++] = Card(CARD_COLORS::SPECIAL, CARD_HEIGHTS::MAH_JONG);

	// shuffle deck
	std::shuffle(allCards.begin(), allCards.end(), std::mt19937(23));

	index = 0;
	for (int i = 0; i < 14; i++) {
		for (int pl = 0; pl < 4; pl++) {
			players[pl]->addCard(allCards[index++]);
		}
	}
}
