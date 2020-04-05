#pragma once

#include "PlayTypes.h"

namespace Tichu {
	class OnTable {

	public:
		OnTable();

		void playCards(PlayedBase* played);
		PlayedBase* getTopCards();

	private:
		std::vector<PlayedBase*> playedCards;
		PlayTypes currentType = PlayTypes::PLAYTYPES_UNINITIALIZED;
	};

}