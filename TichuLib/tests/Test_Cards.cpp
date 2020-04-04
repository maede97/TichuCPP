#include <TichuLib/Card.h>
#include <cassert>

int main(int argc, const char* argv[]) {
	// those are the same, but different constructors
	Tichu::Card c1(Tichu::CARD_HEIGHTS::EIGHT, Tichu::CARD_COLORS::RED);
	Tichu::Card c2(Tichu::CARD_COLORS::RED, Tichu::CARD_HEIGHTS::EIGHT);
	// and a third one, different to others
	Tichu::Card c3(Tichu::CARD_COLORS::GREEN, Tichu::CARD_HEIGHTS::SEVEN);

	assert(c1 == c2);
	assert(c1 != c3);
	assert(c2 != c3);

	return 0;
}