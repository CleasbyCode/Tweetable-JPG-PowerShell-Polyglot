//	JPG-PowerShell Polyglot for X/Twitter (jpws v1.0) Created by Nicholas Cleasby (@CleasbyCode) 12/12/2024
//
//	To compile program (Linux):
// 	$ g++ main.cpp -O2 -s -o jpws
//	$ sudo cp jpws /usr/bin

// 	Run it:
// 	$ jpws

#include "jpws.h"

int main(int argc, char** argv) {
    if (argc == 2 && std::string(argv[1]) == "--info") {
        displayInfo();
        return 0;
    }
    
    if (argc !=3) {
        std::cout << "\nUsage: jpws <cover_image> <powershell_script>\n\t\bjpws --info\n\n";
        return 1;
    }

    const std::string IMAGE_FILENAME = argv[1];
    std::string powershell_filename = argv[2];

    constexpr const char* REG_EXP = ("(\\.[a-zA-Z_0-9\\.\\\\\\s\\-\\/]+)?[a-zA-Z_0-9\\.\\\\\\s\\-\\/]+?(\\.[a-zA-Z0-9]+)?");
    const std::regex regex_pattern(REG_EXP);

    if (!std::regex_match(IMAGE_FILENAME, regex_pattern) || !std::regex_match(powershell_filename, regex_pattern)) {
        std::cerr << "\nInvalid Input Error: Characters not supported by this program found within filename arguments.\n\n";
        return 1;
    }

    const std::filesystem::path
        IMAGE_FILE_PATH(IMAGE_FILENAME),
        POWERSHELL_FILE_PATH(powershell_filename);

    const std::string
        IMAGE_EXTENSION = IMAGE_FILE_PATH.extension().string(),
        POWERSHELL_EXTENSION = POWERSHELL_FILE_PATH.extension().string();

    if (IMAGE_EXTENSION != ".jpg" && IMAGE_EXTENSION != ".jpeg" && IMAGE_EXTENSION != ".jfif") {
		std::cerr << "\nFile Type Error: Invalid file extension. Expecting only \".jpg, .jpeg or .jfif\" image extensions.\n\n";
        	return 1;
    }

    if (POWERSHELL_EXTENSION != ".ps1") {
	std::cerr << "\nFile Type Error: Invalid file extension. Expecting only \".ps1\" PowerShell extension.\n\n";
        return 1;
    }

    if (!std::filesystem::exists(IMAGE_FILENAME) || !std::filesystem::exists(powershell_filename) || !std::filesystem::is_regular_file(powershell_filename)) {
        std::cerr << (!std::filesystem::exists(IMAGE_FILENAME)
            ? "\nImage File Error: File not found."
            : "\nPowerShell File Error: File not found or not a regular file.")
            << " Check the filename and try again.\n\n";
        return 1;
    }

    jpws(IMAGE_FILENAME, powershell_filename);
}