#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace LLAP {

	typedef enum TAG {
		MESSAGE,
		WARNING,
		ERROR,
	} TAG;

	void log(const std::string& string, const TAG& tag = MESSAGE);

}