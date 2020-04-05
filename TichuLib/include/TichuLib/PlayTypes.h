#pragma once

#include "Card.h"
#include <vector>

namespace Tichu {

	enum class PlayTypes {
		SINGLE_CARD = 1,
		PAIR = 2,
		FOLLOWING_PAIRS = 3,
		TRIPLE = 4,
		FULLHOUSE = 5,
		STRAIGHT = 6,
		BOMB = 7,
		PLAYTYPES_UNINITIALIZED = -1
	};

	class PlayedBase {
	public:
		PlayedBase();
		PlayedBase(PlayTypes type_);
		PlayTypes getType() const;
		virtual std::vector<Card> getCards() const = 0;
	protected:
		PlayTypes type = PlayTypes::PLAYTYPES_UNINITIALIZED;
	};

	class PlayedSingle : public PlayedBase {
	public:
		PlayedSingle();
		PlayedSingle(const Card& card);
		bool operator<(const PlayedSingle& other) const;
		bool operator>(const PlayedSingle& other) const;
		std::vector<Card> getCards() const override;
	private:
		Card card;
	};
	class PlayedPair : public PlayedBase {
	public:
		PlayedPair();
		PlayedPair(Card cards[2]);
		bool operator<(const PlayedPair& other) const;
		bool operator>(const PlayedPair& other) const;
		std::vector<Card> getCards() const override;
	private:
		Card cards[2];
	};
	class PlayedFollowingPairs : public PlayedBase {
	public:
		PlayedFollowingPairs();
		PlayedFollowingPairs(const std::vector<PlayedPair>& cards);
		bool operator<(const PlayedFollowingPairs& other) const;
		bool operator>(const PlayedFollowingPairs& other) const;
		std::vector<Card> getCards() const override;
	private:
		std::vector<PlayedPair> cards;
	};
	class PlayedTriple : public PlayedBase {
	public:
		PlayedTriple();
		PlayedTriple(Card cards[3]);
		bool operator<(const PlayedTriple& other) const;
		bool operator>(const PlayedTriple& other) const;
		std::vector<Card> getCards() const override;
	private:
		Card cards[3];
	};
	class PlayedFullhouse : public PlayedBase {
	public:
		PlayedFullhouse();
		PlayedFullhouse(const PlayedTriple& triple, const PlayedPair& pair);
		bool operator<(const PlayedFullhouse& other) const;
		bool operator>(const PlayedFullhouse& other) const;
		std::vector<Card> getCards() const override;
	private:
		PlayedTriple triple;
		PlayedPair pair;
	};
	class PlayedStraight : public PlayedBase {
	public:
		PlayedStraight();
		PlayedStraight(const std::vector<Card>& cards);
		bool operator<(const PlayedStraight& other) const;
		bool operator>(const PlayedStraight& other) const;
		std::vector<Card> getCards() const override;
	private:
		std::vector<Card> cards;
	};
	class PlayedBomb : public PlayedBase {
	public:
		enum BombType { Straight, Four, BombType_UNITINIZALIED };
		PlayedBomb();
		PlayedBomb(const PlayedStraight& straight);
		PlayedBomb(Card cards[4]);
		bool operator<(const PlayedBomb& other) const;
		bool operator>(const PlayedBomb& other) const;
		std::vector<Card> getCards() const override;
	private:
		BombType bType = BombType::BombType_UNITINIZALIED;
		// use one of those
		Card cards[4];
		PlayedStraight straight;
	};
};