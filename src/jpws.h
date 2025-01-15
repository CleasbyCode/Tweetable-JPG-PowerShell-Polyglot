#pragma once

// stb_image by Sean Barrett (“nothings”).
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"
// https://github.com/nothings/stb

// This software is based in part on the work of the Independent JPEG Group.
#include "libjpeg-turbo/turbojpeg.h"
// https://github.com/libjpeg-turbo/libjpeg-turbo

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
#include "resizeImage.cpp"
#include "writeFile.cpp"
#include "information.cpp"
#include "jpws.cpp"

template <uint8_t N>
uint32_t searchFunc(std::vector<uint8_t>&, uint32_t, const uint8_t, const uint8_t (&)[N]);

bool writeFile(std::vector<uint8_t>&);

void
	eraseSegments(std::vector<uint8_t>&),
	resizeImage(std::vector<uint8_t>&, uint8_t, bool),
	displayInfo();

int jpws(const std::string&, std::string&, bool);
