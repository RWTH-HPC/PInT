#pragma once

#include <iostream>

#ifdef PRINT_DEBUG
	#define DEBUG_MESSAGE(Message) std::cout << Message << std::endl;
#else
	#define DEBUG_MESSAGE(Message)
#endif
