#pragma once

#include "Card.h"

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

	class CardsPlayed {
	public:
		CardsPlayed();
		CardsPlayed(PlayTypes type_);
	private:
		PlayTypes type = PlayTypes::PLAYTYPES_UNINITIALIZED;
	};
} // namespace Tichu