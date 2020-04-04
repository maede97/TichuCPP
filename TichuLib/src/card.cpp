#include "..\include\TichuLib\Card.h"

Tichu::Card::Card()
{
}

Tichu::Card::Card(std::string fileName_) : imageFileName(fileName_)
{
}

Tichu::Card::Card(CARD_HEIGHTS height_, CARD_COLORS color_) : height(height_), color(color_)
{
}

Tichu::Card::Card(CARD_COLORS color_, CARD_HEIGHTS height_) : height(height_), color(color_)
{

}

bool Tichu::Card::operator==(const Card& other)
{
	return (height == other.height);
}

bool Tichu::Card::operator!=(const Card& other)
{
	return !operator==(other);
}

bool Tichu::Card::operator>(const Card& other)
{
	// if we have no special card
	if ((int)height > 0 && (int)height < 15) {
		// no special card from other
		if ((int)other.height > 0 && (int)other.height < 15) {
			return (int)height > (int)other.height;
		}
		// special card from other
		else {
			// if dragon or phoenix
			if (other.height == CARD_HEIGHTS::DRAGON || other.height == CARD_HEIGHTS::PHOENIX) {
				return false;
			}
			else {
				// dog, should never happen
				return false;
			}
		}
	}
	// special card from us
	else {
		// no special card from other
		if ((int)other.height > 0 && (int)other.height < 15) {
			if (height == CARD_HEIGHTS::DRAGON || height == CARD_HEIGHTS::PHOENIX) {
				return true;
			}
			else {
				// dog, incomparable
				return false;
			}
		}
		// special card from other (dog, phoenix or dragon)
		else {
			if (other.height == CARD_HEIGHTS::DRAGON) {
				return false;
			}
			else if (other.height == CARD_HEIGHTS::PHOENIX) {
				return height == CARD_HEIGHTS::DRAGON;
			}
			else {
				// dog, incomparable
				return false;
			}
		}
	}
	return false;
}

bool Tichu::Card::operator<(const Card& other)
{
	return false;
}
