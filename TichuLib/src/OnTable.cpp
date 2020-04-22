#include "..\include\TichuLib\OnTable.h"
#include "..\include\TichuLib\Utils.h"
#include <iostream>

Tichu::OnTable::OnTable()
{
}

bool Tichu::OnTable::playCards(PlayedBase* played)
{
	if (playedCards.size() == 0) {
		currentType = played->getType();
		playedCards.push_back(played);
		return true;
	}

	if (played->getType() != currentType) {
		if (played->getType() == PlayTypes::BOMB) {
			currentType = PlayTypes::BOMB;
			playedCards.push_back(played);
		}
		else {
			throwError("OnTable::playCards wrong type");
			return false;
		}
	}
	else {
		// cast to type, then call operator< on them
		bool playable = false;
		switch (played->getType())
		{
		case PlayTypes::SINGLE_CARD:
			playable = (*dynamic_cast<PlayedSingle*>(getTopCards()) < *dynamic_cast<PlayedSingle*>(played));
			break;
		case PlayTypes::PAIR:
			playable = (*dynamic_cast<PlayedPair*>(getTopCards()) < *dynamic_cast<PlayedPair*>(played));
			break;
		case PlayTypes::TRIPLE:
			playable = (*dynamic_cast<PlayedTriple*>(getTopCards()) < *dynamic_cast<PlayedTriple*>(played));
			break;
		case PlayTypes::FOLLOWING_PAIRS:
			playable = (*dynamic_cast<PlayedFollowingPairs*>(getTopCards()) < *dynamic_cast<PlayedFollowingPairs*>(played));
			break;
		case PlayTypes::FULLHOUSE:
			playable = (*dynamic_cast<PlayedFullhouse*>(getTopCards()) < *dynamic_cast<PlayedFullhouse*>(played));
			break;
		case PlayTypes::STRAIGHT:
			playable = (*dynamic_cast<PlayedStraight*>(getTopCards()) < *dynamic_cast<PlayedStraight*>(played));
			break;
		case PlayTypes::BOMB:
			playable = (*dynamic_cast<PlayedBomb*>(getTopCards()) < *dynamic_cast<PlayedBomb*>(played));
			break;
		default:
			break;
		}

		if (playable) {
			playedCards.push_back(played);
			return true;
		}
		else {
			// not bigger, don't place them on table
			return false;
		}
	}
	return false;
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

void Tichu::OnTable::clearTable()
{
	for (auto& i : playedCards) {
		delete i;
	}
	playedCards.clear();
	currentType = PlayTypes::PLAYTYPES_UNINITIALIZED;
}
