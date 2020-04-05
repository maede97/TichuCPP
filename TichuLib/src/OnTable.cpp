#include "..\include\TichuLib\OnTable.h"
#include "..\include\TichuLib\Utils.h"

Tichu::OnTable::OnTable()
{
}

bool Tichu::OnTable::playCards(PlayedBase* played)
{
	if (playedCards.size() == 0) {
		currentType = played->getType();
	}

	if (played->getType() != currentType) {
		if (played->getType() == PlayTypes::BOMB) {
			currentType = PlayTypes::BOMB;
		}
		else {
			throwError("OnTable::playCards wrong type");
			return false;
		}
	}
	else {
		playedCards.push_back(played);
	}

	return true;
}

Tichu::PlayedBase* Tichu::OnTable::getTopCards()
{
	if (playedCards.size() == 0) {
		return nullptr;
	}
	else {
		return playedCards.back();
	}
}
