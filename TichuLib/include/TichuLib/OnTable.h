#pragma once

#include "PlayTypes.h"

namespace Tichu {
	class OnTable {

	public:
		OnTable();

		/* true if correct, false if wrong*/
		bool playCards(PlayedBase* played);
		PlayedBase* getTopCards();

		void clearTable();

	private:
		std::vector<PlayedBase*> playedCards;
		PlayTypes currentType = PlayTypes::PLAYTYPES_UNINITIALIZED;
	};

}