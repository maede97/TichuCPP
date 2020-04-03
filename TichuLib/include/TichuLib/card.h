#pragma once

#include <string>

namespace Tichu {

	enum CARD_HEIGHTS {
		TWO = 2,
		THREE = 3,
		FOUR = 4,
		FIVE = 5,
		SIX = 6,
		SEVEN = 7,
		EIGHT = 8,
		NINE = 9,
		TEN = 10,
		JACK = 11,
		QUEEN = 12,
		KING = 13,
		ASS = 14,
		DOG = 15,
		PHOENIX = 16,
		DRAGON = 17,
		MAH_JONG = 18
	};

	class Card {
	public:
		Card();

		/*
			Create card from image file name
		*/
		Card(std::string fileName);

	private:
		std::string imageFileName;
		CARD_HEIGHTS height;

	};

}; // namespace Tichu