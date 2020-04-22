#pragma once

#include "Player.h"

namespace Tichu {

	class Game {
	public:
		Game();
		
		void addPlayer(Player* player);

		void distributeDeckOfCards();

		bool playCards(Player* player, PlayedBase* cards);

		void setCurrentPlayer(Player* player);
		Player* getCurrentPlayer();

		const PlayedBase* topCards() const;

		bool isFinished();

		void clearTable();

	private:
		std::vector<Player*> players;
		OnTable* onTable = nullptr;
		Player* currentPlayer = nullptr;
	};
}