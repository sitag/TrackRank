#include "Cmn.h"

// split string by delimiter
std::vector<std::string> Cmn::split(const std::string &target, char delimiter) {
	std::vector<std::string> words;
	Cmn::split(target, delimiter, words);
	return words;
}


