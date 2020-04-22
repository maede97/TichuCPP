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

Tichu::PlayedSingle::PlayedSingle(std::vector<int> packetData) : PlayedBase(PlayTypes::SINGLE_CARD)
{
	if (packetData[0] != (int)PlayTypes::SINGLE_CARD) {
		return;
	}
	if (packetData.size() != 3) {
		return;
	}
	card = Card(std::make_pair(packetData[1], packetData[2]));
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

std::vector<int> Tichu::PlayedSingle::getCardPacketData() const
{
	std::vector<int> ret(3);
	ret[0] = (int)PlayTypes::SINGLE_CARD;
	ret[1] = card.getCardShortage().first;
	ret[2] = card.getCardShortage().second;
	return ret;
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

Tichu::PlayedPair::PlayedPair(std::vector<int> packetData) : PlayedBase(PlayTypes::PAIR)
{
	if (packetData[0] != (int)PlayTypes::PAIR) {
		return;
	}
	if (packetData.size() != 5) {
		return;
	}
	cards[0] = Card(std::make_pair(packetData[1], packetData[2]));
	cards[1] = Card(std::make_pair(packetData[3], packetData[4]));
}

bool Tichu::PlayedPair::operator<(const PlayedPair& other) const
{
	return cards[0] < other.cards[0];
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

std::vector<int> Tichu::PlayedPair::getCardPacketData() const
{
	std::vector<int> ret(5);
	ret[0] = (int)PlayTypes::PAIR;
	ret[1] = cards[0].getCardShortage().first;
	ret[2] = cards[0].getCardShortage().second;
	ret[3] = cards[1].getCardShortage().first;
	ret[4] = cards[1].getCardShortage().second;
	return ret;
}

Tichu::PlayedFollowingPairs::PlayedFollowingPairs() : PlayedBase()
{
}

Tichu::PlayedFollowingPairs::PlayedFollowingPairs(const std::vector<PlayedPair>& cards) : PlayedBase(PlayTypes::FOLLOWING_PAIRS)
{
	Tichu::CARD_HEIGHTS initHeight = Tichu::CARD_HEIGHTS::HEIGHT_UNINITIALIZED;
	for (int i = 0; i < cards.size(); i++) {
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
			int height = (int)cards[i].getCards()[0].getHeight();
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

Tichu::PlayedFollowingPairs::PlayedFollowingPairs(std::vector<int> packetData) : PlayedBase(PlayTypes::FOLLOWING_PAIRS)
{
	// FOLLOWING_PAIRS LENGTH c1c c1h c1.2c c1.2h c2c c2h c2.2c c2.2h etc
	if (packetData[0] != (int)PlayTypes::FOLLOWING_PAIRS) {
		return;
	}
	if (packetData.size() != packetData[1] * 4 + 2) {
		return;
	}
	int length = packetData[1];
	cards.reserve(length);
	Card cards[2];
	for (int i = 0; i < length; i++) {
		cards[0] = Card(std::make_pair(packetData[2 + i * 4], packetData[3 + i * 4]));
		cards[1] = Card(std::make_pair(packetData[4 + i * 4], packetData[5 + i * 4]));
		this->cards.push_back(PlayedPair(cards));
	}
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

std::vector<int> Tichu::PlayedFollowingPairs::getCardPacketData() const
{
	//FOLLOWING_PAIRS LENGTH c1c c1h c1.2c c1.2h c2c c2h c2.2c c2.2h etc
	std::vector<int> ret(2 + cards.size() * 4);
	ret[0] = (int)PlayTypes::FOLLOWING_PAIRS;
	ret[1] = cards.size();
	int index = 0;
	for (int i = 0; i < cards.size(); i++) {
		ret[index++] = cards[i].getCards()[0].getCardShortage().first;
		ret[index++] = cards[i].getCards()[0].getCardShortage().second;
		ret[index++] = cards[i].getCards()[1].getCardShortage().first;
		ret[index++] = cards[i].getCards()[1].getCardShortage().second;
	}
	return ret;
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

Tichu::PlayedTriple::PlayedTriple(std::vector<int> packetData) : PlayedBase(PlayTypes::TRIPLE)
{
	if (packetData[0] != (int)PlayTypes::TRIPLE) {
		return;
	}
	if (packetData.size() != 7) {
		return;
	}
	cards[0] = Card(std::make_pair(packetData[1], packetData[2]));
	cards[1] = Card(std::make_pair(packetData[3], packetData[4]));
	cards[2] = Card(std::make_pair(packetData[5], packetData[6]));
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

std::vector<int> Tichu::PlayedTriple::getCardPacketData() const
{
	std::vector<int> ret(7);
	ret[0] = (int)PlayTypes::PAIR;
	ret[1] = cards[0].getCardShortage().first;
	ret[2] = cards[0].getCardShortage().second;
	ret[3] = cards[1].getCardShortage().first;
	ret[4] = cards[1].getCardShortage().second;
	ret[5] = cards[2].getCardShortage().first;
	ret[6] = cards[2].getCardShortage().second;
	return ret;
}

Tichu::PlayedFullhouse::PlayedFullhouse() : PlayedBase()
{
}

Tichu::PlayedFullhouse::PlayedFullhouse(const PlayedTriple& triple, const PlayedPair& pair) : PlayedBase(PlayTypes::FULLHOUSE)
{
	this->triple = triple;
	this->pair = pair;
}

Tichu::PlayedFullhouse::PlayedFullhouse(std::vector<int> packetData) : PlayedBase(PlayTypes::FULLHOUSE)
{
	// triple pair
	if (packetData[0] != (int)PlayTypes::FULLHOUSE) {
		return;
	}
	if (packetData.size() != 7) {
		return;
	}
	Card cards[3];
	cards[0] = Card(std::make_pair(packetData[1], packetData[2]));
	cards[1] = Card(std::make_pair(packetData[3], packetData[4]));
	cards[2] = Card(std::make_pair(packetData[5], packetData[6]));
	triple = PlayedTriple(cards);
	Card cards2[2];
	cards2[0] = Card(std::make_pair(packetData[7], packetData[8]));
	cards2[1] = Card(std::make_pair(packetData[9], packetData[10]));
	pair = PlayedPair(cards2);
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

std::vector<int> Tichu::PlayedFullhouse::getCardPacketData() const
{
	// triple pair
	std::vector<int> ret(11);
	ret[0] = (int)PlayTypes::FULLHOUSE;
	ret[1] = triple.getCards()[0].getCardShortage().first;
	ret[2] = triple.getCards()[0].getCardShortage().second;
	ret[3] = triple.getCards()[1].getCardShortage().first;
	ret[4] = triple.getCards()[1].getCardShortage().second;
	ret[5] = triple.getCards()[2].getCardShortage().first;
	ret[6] = triple.getCards()[2].getCardShortage().second;
	ret[7] = pair.getCards()[0].getCardShortage().first;
	ret[8] = pair.getCards()[0].getCardShortage().second;
	ret[9] = pair.getCards()[1].getCardShortage().first;
	ret[10] = pair.getCards()[1].getCardShortage().second;
	return ret;
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

Tichu::PlayedStraight::PlayedStraight(std::vector<int> packetData) : PlayedBase(PlayTypes::STRAIGHT)
{
	if (packetData[0] != (int)PlayTypes::STRAIGHT) {
		return;
	}
	if (packetData.size() != packetData[1] * 2 + 2) {
		return;
	}
	int size = packetData[1];
	cards.reserve(size);
	for (int i = 0; i < size; i++) {
		cards.push_back(Card(std::make_pair(packetData[i * 2 + 2], packetData[i * 2 + 2])));
	}
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
	return other < *this;
}

std::vector<Tichu::Card> Tichu::PlayedStraight::getCards() const
{
	return cards;
}

std::vector<int> Tichu::PlayedStraight::getCardPacketData() const
{
	std::vector<int> ret(2 + cards.size() * 2);
	ret[0] = (int)PlayTypes::STRAIGHT;
	ret[1] = cards.size();
	int index = 2;
	for (auto& c : cards) {
		ret[index++] = c.getCardShortage().first;
		ret[index++] = c.getCardShortage().second;
	}
	return ret;
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

Tichu::PlayedBomb::PlayedBomb(std::vector<int> packetData) : PlayedBase(PlayTypes::BOMB)
{
	if (packetData[0] != (int)PlayTypes::BOMB) {
		return;
	}
	BombType type = BombType(packetData[1]);
	if (type == BombType::Straight) {
		std::vector<int> ret;
		ret.push_back((int)PlayTypes::STRAIGHT);
		for (int i = 2; i < packetData.size(); i++) {
			ret.push_back(packetData[i]);
		}
		straight = PlayedStraight(ret);
	}
	else {
		// four of a kind
		cards[0] = Card(std::make_pair(packetData[2], packetData[3]));
		cards[1] = Card(std::make_pair(packetData[4], packetData[5]));
		cards[2] = Card(std::make_pair(packetData[6], packetData[7]));
		cards[2] = Card(std::make_pair(packetData[8], packetData[9]));
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

std::vector<int> Tichu::PlayedBomb::getCardPacketData() const
{
	if (bType == BombType::Straight) {
		std::vector<int> ret;
		ret.push_back((int)PlayTypes::BOMB);
		ret.push_back((int)BombType::Straight);
		std::vector<int> d = straight.getCardPacketData();
		for (int i = 0; i < d.size(); i++) {
			ret.push_back(d[i]);
		}
		return ret;
	}
	else {
		std::vector<int> ret(10);
		ret.push_back((int)PlayTypes::BOMB);
		ret.push_back((int)BombType::Four);
		ret[2] = cards[0].getCardShortage().first;
		ret[3] = cards[0].getCardShortage().second;
		ret[4] = cards[1].getCardShortage().first;
		ret[5] = cards[1].getCardShortage().second;
		ret[6] = cards[2].getCardShortage().first;
		ret[7] = cards[2].getCardShortage().second;
		ret[8] = cards[3].getCardShortage().first;
		ret[9] = cards[3].getCardShortage().second;
		return ret;
	}
}
