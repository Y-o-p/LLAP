#include "debug.h"

namespace LLAP {

	void log(const std::string& string, const TAG& tag) {
		auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		auto time_s = std::put_time(std::localtime(&time), "%T");

		std::string tag_s = "";
		switch (tag) {
		case MESSAGE:
			tag_s = "{MESSAGE}: ";
			break;
		case WARNING:
			tag_s = "{WARNING}: ";
			break;
		case ERROR:
			tag_s = "{ERROR}: ";
			std::cerr << "[" << time_s << "]" << tag_s << string << "\n";
			throw std::runtime_error(string);
			break;
		default:
			break;
		}

		std::cout << "[" << time_s << "]" << tag_s << string << "\n";
	}

}