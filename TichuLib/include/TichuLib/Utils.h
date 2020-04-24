#pragma once

#include <iostream>

#define throwError(what) std::cout << "Error: " << what << std::endl

namespace Tichu {

	/* This enum stores all types of packages we send over the air */
	enum class PACKET_TYPES {
		USERS = 0, // std::string * 4 with usernames
		CARDS = 1, // i followed by i * (CARD_COLOR CARD_HEIGHT)
		REMAINING_CARDS = 2, // 4 * int
		PLAY_CARDS = 3, // type followed by shortage vector (see PlayTypes)
		MIDDLE_CARDS = 4,
		PLAY_VALID = 5,
		YOUR_TURN = 6,
		PLAY_PASS = 7,
		WRONG_TURN = 8,
		FAIL = -1
	};
}