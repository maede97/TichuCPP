#pragma once

#include <string>

namespace Tichu {

	enum class CARD_HEIGHTS {
		MAH_JONG = 1,
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
		HEIGHT_UNINITIALIZED = -1
	};

	enum class CARD_COLORS {
		GREEN = 1,
		BLUE = 2,
		BLACK = 3,
		RED = 4,
		SPECIAL = 5,
		COLOR_UNINITIALIZED = -1
	};

	class Card {
	public:
		Card();

		/*
			Create card from image file name
		*/
		Card(std::string fileName_);

		Card(CARD_HEIGHTS height_, CARD_COLORS color_);
		Card(CARD_COLORS color_, CARD_HEIGHTS height_);
		
		void setPhoenixHeight(CARD_HEIGHTS height);

		// These operands only work with the height of the card
		bool operator==(const Card& other) const;
		bool operator!=(const Card& other) const;
		bool operator>(const Card& other) const;
		bool operator<(const Card& other) const;

		CARD_HEIGHTS getHeight() const;

		bool isSame(const Card& other) const;

	private:
		std::string imageFileName;
		CARD_HEIGHTS height = CARD_HEIGHTS::HEIGHT_UNINITIALIZED;
		CARD_COLORS color = CARD_COLORS::COLOR_UNINITIALIZED;
		CARD_HEIGHTS phoenixHeight = CARD_HEIGHTS::HEIGHT_UNINITIALIZED;
	};

} // namespace Tichu