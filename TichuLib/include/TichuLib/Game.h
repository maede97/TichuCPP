#pragma once

#include "Player.h"


namespace Tichu {

	class Game {
	public:
		Game();
		
		void addPlayer(Player* player);

		void distributeDeckOfCards();
	private:
		std::vector<Player*> players;
	};
}