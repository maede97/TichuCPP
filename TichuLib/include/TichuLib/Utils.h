#pragma once

#include <iostream>

#define throwError(what) std::cout << "Error: " << what << std::endl

namespace Tichu {

	/* This enum stores all types of packages we send over the air */
	enum class PACKET_TYPES {
		USERS = 0, // std::string * 4 with usernames
		CARDS = 1, // i followed by i * (CARD_COLOR CARD_HEIGHT)
		FAIL = -1
	};
}