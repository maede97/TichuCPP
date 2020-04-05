#pragma once

#include "PlayTypes.h"
#include "OnTable.h"
namespace Tichu {

	class Player {
	public:
		Player();
		Player(std::string name);

		void setName(std::string name);

		void setTable(OnTable* table);

		void layCards(PlayedBase* cards);

		void addCard(Card card);
		void removeCard(Card card);

		std::string getName() const;
		int getRemainingCards() const;
			
		std::vector<Card> getCards() const;

	private:
		std::vector<Card> cards;
		std::string name;
		OnTable* table = nullptr;
	};

}