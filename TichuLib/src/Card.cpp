#include "..\include\TichuLib\Card.h"

Tichu::Card::Card()
{
}

Tichu::Card::Card(CARD_HEIGHTS height_, CARD_COLORS color_) : height(height_), color(color_)
{
}

Tichu::Card::Card(CARD_COLORS color_, CARD_HEIGHTS height_) : height(height_), color(color_)
{

}

Tichu::Card::Card(std::pair<int, int> shortage) : color(CARD_COLORS(shortage.first)), height(CARD_HEIGHTS(shortage.second))
{
}

void Tichu::Card::setPhoenixHeight(CARD_HEIGHTS height)
{
	if (height == CARD_HEIGHTS::PHOENIX) {
		phoenixHeight = height;
	}
}

bool Tichu::Card::operator==(const Card& other) const
{
	return (height == other.height);
}

bool Tichu::Card::operator!=(const Card& other) const
{
	return !operator==(other);
}

bool Tichu::Card::operator>(const Card& other) const
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

bool Tichu::Card::operator<(const Card& other) const
{
	return other > *this;
}

Tichu::CARD_HEIGHTS Tichu::Card::getHeight() const
{
	if (height == CARD_HEIGHTS::PHOENIX) {
		return phoenixHeight;
	}
	return height;
}

bool Tichu::Card::isSame(const Card& other) const
{
	return height == other.height && color == other.color;
}

std::pair<int, int> Tichu::Card::getCardShortage() const
{
	return std::make_pair<int, int>((int)color, (int)height);
}
std::string Tichu::Card::getFileName() const
{
	std::string result = "sc";
	switch (color)
	{
	case Tichu::CARD_COLORS::GREEN:
		result += "2-";
		break;
	case Tichu::CARD_COLORS::BLUE:
		result += "1-";
		break;
	case Tichu::CARD_COLORS::BLACK:
		result += "0-";
		break;
	case Tichu::CARD_COLORS::RED:
		result += "3-";
		break;
	case Tichu::CARD_COLORS::SPECIAL:
		// do this later
		break;
	case Tichu::CARD_COLORS::COLOR_UNINITIALIZED:
		break;
	default:
		break;
	}

	switch (height)
	{
	case Tichu::CARD_HEIGHTS::MAH_JONG:
		result += "4-1";
		break;
	case Tichu::CARD_HEIGHTS::TWO:
	case Tichu::CARD_HEIGHTS::THREE:
	case Tichu::CARD_HEIGHTS::FOUR:
	case Tichu::CARD_HEIGHTS::FIVE:
	case Tichu::CARD_HEIGHTS::SIX:
	case Tichu::CARD_HEIGHTS::SEVEN:
	case Tichu::CARD_HEIGHTS::EIGHT:
	case Tichu::CARD_HEIGHTS::NINE:
	case Tichu::CARD_HEIGHTS::TEN:
	case Tichu::CARD_HEIGHTS::JACK:
	case Tichu::CARD_HEIGHTS::QUEEN:
	case Tichu::CARD_HEIGHTS::KING:
	case Tichu::CARD_HEIGHTS::ASS:
		result += std::to_string((int)height);
		break;
	case Tichu::CARD_HEIGHTS::DOG:
		result += "5-0";
		break;
	case Tichu::CARD_HEIGHTS::PHOENIX:
		result += "7-0";
		break;
	case Tichu::CARD_HEIGHTS::DRAGON:
		result += "6-15";
		break;
	case Tichu::CARD_HEIGHTS::HEIGHT_UNINITIALIZED:
		break;
	default:
		break;
	}

	result += ".png";

	return result;
}

int Tichu::Card::indexInOrderedDeck() const
{	switch (height)
	{
	case Tichu::CARD_HEIGHTS::MAH_JONG:
		return 55;
		break;
	case Tichu::CARD_HEIGHTS::TWO:
	case Tichu::CARD_HEIGHTS::THREE:
	case Tichu::CARD_HEIGHTS::FOUR:
	case Tichu::CARD_HEIGHTS::FIVE:
	case Tichu::CARD_HEIGHTS::SIX:
	case Tichu::CARD_HEIGHTS::SEVEN:
	case Tichu::CARD_HEIGHTS::EIGHT:
	case Tichu::CARD_HEIGHTS::NINE:
	case Tichu::CARD_HEIGHTS::TEN:
	case Tichu::CARD_HEIGHTS::JACK:
	case Tichu::CARD_HEIGHTS::QUEEN:
	case Tichu::CARD_HEIGHTS::KING:
	case Tichu::CARD_HEIGHTS::ASS:
		return 13*((int)color-1) + (int)height-2;
		break;
	case Tichu::CARD_HEIGHTS::DOG:
		return 53;
		break;
	case Tichu::CARD_HEIGHTS::PHOENIX:
		return  54;
		break;
	case Tichu::CARD_HEIGHTS::DRAGON:
		return 52;
		break;
	case Tichu::CARD_HEIGHTS::HEIGHT_UNINITIALIZED:
		break;
	default:
		break;
	}
	return -1;
}
