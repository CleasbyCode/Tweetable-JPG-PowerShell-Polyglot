#pragma once

#include <algorithm>
#include <filesystem>
#include <random>
#include <cstdint>
#include <fstream>
#include <regex>
#include <iostream>
#include <string>
#include <vector>
#include <iterator>

#include "profileVec.cpp"
#include "searchFunc.cpp"
#include "eraseSegments.cpp"
#include "writeFile.cpp"
#include "information.cpp"
#include "jpws.cpp"

template <uint8_t N>
uint32_t searchFunc(std::vector<uint8_t>&, uint32_t, const uint8_t, const uint8_t (&)[N]);

bool writeFile(std::vector<uint8_t>&);

void
	eraseSegments(std::vector<uint8_t>&),
	displayInfo();

int jpws(const std::string&, std::string&);