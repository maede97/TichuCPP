#include "..\include\TichuLib\OnTable.h"
#include "..\include\TichuLib\Utils.h"

Tichu::OnTable::OnTable()
{
}

void Tichu::OnTable::playCards(PlayedBase* played)
{
	if (playedCards.size() == 0) {
		currentType = played->getType();
	}

	if (played->getType() != currentType) {
		throwError("OnTable::playCards wrong type");
	}
	else {
		playedCards.push_back(played);
	}
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
