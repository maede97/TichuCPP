#include "..\include\TichuLib\Player.h"

Tichu::Player::Player()
{
}

Tichu::Player::Player(std::string name)
{
	this->name = name;
}

void Tichu::Player::setTable(OnTable* table)
{
	this->table = table;
}

void Tichu::Player::layCards(PlayedBase* cards)
{
	table->playCards(cards);

	for (auto& c : cards->getCards()) {
		removeCard(c);
	}
}

void Tichu::Player::addCard(Card card)
{
	cards.push_back(card);
}

void Tichu::Player::removeCard(Card card)
{
	std::vector<Card>::iterator it = cards.end();
	for (int i = 0; i < cards.size(); i++) {
		if (card.isSame(cards[i])) {
			it = cards.begin() + i;
		}
	}

	if (it != cards.end()) {
		cards.erase(it);
	}
}

std::string Tichu::Player::getName() const
{
	return name;
}

int Tichu::Player::getRemainingCards() const
{
	return cards.size();
}
