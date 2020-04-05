#include "..\include\TichuLib\PlayTypes.h"
#include "..\include\TichuLib\Utils.h"

Tichu::PlayedBase::PlayedBase()
{
}

Tichu::PlayedBase::PlayedBase(PlayTypes type_) : type(type_)
{
}

Tichu::PlayTypes Tichu::PlayedBase::getType() const
{
	return type;
}

Tichu::PlayedSingle::PlayedSingle() : PlayedBase(PlayTypes::SINGLE_CARD)
{
}

Tichu::PlayedSingle::PlayedSingle(const Card& card) : PlayedBase(PlayTypes::SINGLE_CARD)
{
	this->card = card;
}

bool Tichu::PlayedSingle::operator<(const PlayedSingle& other) const
{
	return this->card < other.card;
}

bool Tichu::PlayedSingle::operator>(const PlayedSingle& other) const
{
	return this->card > other.card;
}

std::vector<Tichu::Card> Tichu::PlayedSingle::getCards() const
{
	std::vector<Card> c(1);
	c[0] = card;
	return c;
}

Tichu::PlayedPair::PlayedPair() : PlayedBase(PlayTypes::PAIR)
{
}

Tichu::PlayedPair::PlayedPair(Card cards[2]) : PlayedBase(PlayTypes::PAIR)
{
	if (cards[0] != cards[1]) {
		type = PlayTypes::PLAYTYPES_UNINITIALIZED;
		throwError("PlayedPair Constructor, cards don't match");
		return;
	}
	this->cards[0] = cards[0];
	this->cards[1] = cards[1];
}

bool Tichu::PlayedPair::operator<(const PlayedPair& other) const
{
	return false;
}

bool Tichu::PlayedPair::operator>(const PlayedPair& other) const
{
	return other < *this;
}

std::vector<Tichu::Card> Tichu::PlayedPair::getCards() const
{
	std::vector<Card> c(2);
	c[0] = cards[0];
	c[1] = cards[1];
	return c;
}

Tichu::PlayedFollowingPairs::PlayedFollowingPairs() : PlayedBase()
{
}

Tichu::PlayedFollowingPairs::PlayedFollowingPairs(const std::vector<PlayedPair>& cards) : PlayedBase(PlayTypes::FOLLOWING_PAIRS)
{
	Tichu::CARD_HEIGHTS initHeight = Tichu::CARD_HEIGHTS::HEIGHT_UNINITIALIZED;
	for (int i = 0; i < cards.size; i++) {
		if (i == 0) {
			initHeight = cards[i].getCards()[0].getHeight();

			if ((int)initHeight < 2 || (int)initHeight > 14) {
				throwError("PlayedFollowingPairs constr. initial card has wrong height");
				type = PlayTypes::PLAYTYPES_UNINITIALIZED;
				return;
			}
		}
		else {
			// compare to previous
			int initIntheight = (int)initHeight;
			int height =(int)cards[i].getCards()[0].getHeight();
			if (height > 1 && height < 15) {
				if (height != initIntheight + 1) {
					throwError("PlayedFollowingPairs constr. wrong heights");
					type = PlayTypes::PLAYTYPES_UNINITIALIZED;
					return;
				}
			}
			initHeight = cards[i].getCards()[0].getHeight();
		}
	}
	// all good
	this->cards = cards;
}

bool Tichu::PlayedFollowingPairs::operator<(const PlayedFollowingPairs& other) const
{
	return cards[0] < other.cards[0];
}

bool Tichu::PlayedFollowingPairs::operator>(const PlayedFollowingPairs& other) const
{
	return other < *this;
}

std::vector<Tichu::Card> Tichu::PlayedFollowingPairs::getCards() const
{
	std::vector<Tichu::Card> c;
	c.reserve(cards.size() * 2);
	for (auto& p : cards) {
		c.push_back(p.getCards()[0]);
		c.push_back(p.getCards()[1]);
	}
	return c;
}

Tichu::PlayedTriple::PlayedTriple() : PlayedBase()
{
}

Tichu::PlayedTriple::PlayedTriple(Card cards[3]) : PlayedBase(PlayTypes::TRIPLE)
{
	if (cards[0].getHeight() != cards[1].getHeight() || cards[0].getHeight() != cards[2].getHeight() || cards[1].getHeight() != cards[2].getHeight()) {
		type = PlayTypes::PLAYTYPES_UNINITIALIZED;
		throwError("PlayedTriple const. wrong heights");
		return;
	}
	this->cards[0] = cards[0];
	this->cards[1] = cards[1];
	this->cards[2] = cards[2];
}

bool Tichu::PlayedTriple::operator<(const PlayedTriple& other) const
{
	return cards[0] < other.cards[0];
}

bool Tichu::PlayedTriple::operator>(const PlayedTriple& other) const
{
	return other < *this;
}

std::vector<Tichu::Card> Tichu::PlayedTriple::getCards() const
{
	std::vector<Card> c(3);
	c[0] = cards[0];
	c[1] = cards[1];
	c[2] = cards[2];
	return c;
}

Tichu::PlayedFullhouse::PlayedFullhouse() : PlayedBase()
{
}

Tichu::PlayedFullhouse::PlayedFullhouse(const PlayedTriple& triple, const PlayedPair& pair) : PlayedBase(PlayTypes::FULLHOUSE)
{
	this->triple = triple;
	this->pair = pair;
}

bool Tichu::PlayedFullhouse::operator<(const PlayedFullhouse& other) const
{
	return this->triple < other.triple;
}

bool Tichu::PlayedFullhouse::operator>(const PlayedFullhouse& other) const
{
	return other < *this;
}

std::vector<Tichu::Card> Tichu::PlayedFullhouse::getCards() const
{
	std::vector<Card> c(5);
	c[0] = pair.getCards()[0];
	c[1] = pair.getCards()[1];
	c[2] = triple.getCards()[0];
	c[3] = triple.getCards()[1];
	c[4] = triple.getCards()[2];
	return c;
}

Tichu::PlayedStraight::PlayedStraight() : PlayedBase()
{
}

Tichu::PlayedStraight::PlayedStraight(const std::vector<Card>& cards) : PlayedBase(PlayTypes::STRAIGHT)
{
	if (cards.size() < 5) {
		type = PlayTypes::PLAYTYPES_UNINITIALIZED;
		throwError("PlayedStraight const. wrong length");
		return;
	}
	int h = (int)cards[0].getHeight();
	for (int i = 1; i < cards.size(); i++) {

		if ((int)cards[i].getHeight() != h + 1) {
			type = PlayTypes::PLAYTYPES_UNINITIALIZED;
			throwError("PlayedStraight const. wrong height following");
			return;
		}
		h = (int)cards[i].getHeight();
	}
	this->cards = cards;
}

bool Tichu::PlayedStraight::operator<(const PlayedStraight& other) const
{
	if (cards.size() != other.cards.size()) {
		return false;
		// throw error...
	}
	return cards[0] < other.cards[0];
}

bool Tichu::PlayedStraight::operator>(const PlayedStraight& other) const
{
	return other < * this;
}

std::vector<Tichu::Card> Tichu::PlayedStraight::getCards() const
{
	return cards;
}

Tichu::PlayedBomb::PlayedBomb() : PlayedBase()
{
}

Tichu::PlayedBomb::PlayedBomb(const PlayedStraight& straight) : PlayedBase(PlayTypes::BOMB)
{
	bType = BombType::Straight;
	this->straight = straight;
}

Tichu::PlayedBomb::PlayedBomb(Card cards[4])
{
	bType = BombType::Four;
	if (cards[0].getHeight() == cards[1].getHeight() && cards[0].getHeight() == cards[2].getHeight() && cards[0].getHeight() == cards[3].getHeight()) {
		type = PlayTypes::PLAYTYPES_UNINITIALIZED;
		throwError("PlayedStraight const. wrong height following");
		return;
	}
	else {
		for (int i = 0; i < 4; i++) {
			this->cards[i] = cards[i];
		}
	}
}

bool Tichu::PlayedBomb::operator<(const PlayedBomb& other) const
{
	if (bType == BombType::Four) {
		if (other.bType == BombType::Straight) {
			return false; // straight is always better than four
		}
		else {
			return cards[0] < other.cards[0];
		}
	}
	else {
		if (other.bType == BombType::Straight) {
			return this->straight < other.straight;
		}
		else {
			return true;
		}
	}
}

bool Tichu::PlayedBomb::operator>(const PlayedBomb& other) const
{
	return other < *this;
}

std::vector<Tichu::Card> Tichu::PlayedBomb::getCards() const
{
	if (bType == BombType::Four) {
		std::vector<Card> c(4);
		c[0] = cards[0];
		c[1] = cards[1];
		c[2] = cards[2];
		c[3] = cards[3];
	}
	else {
		return straight.getCards();
	}
}
