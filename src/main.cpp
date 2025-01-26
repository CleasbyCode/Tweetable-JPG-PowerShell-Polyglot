//	JPG-PowerShell Polyglot for X/Twitter (jpws v1.1) Created by Nicholas Cleasby (@CleasbyCode) 12/12/2024
//
//	To compile program (Linux):
// 	$ sudo apt-get install libturbojpeg-dev
// 	$ g++ main.cpp -O2 -lturbojpeg -s -o jpws
//	$ sudo cp jpws /usr/bin

// 	Run it:
// 	$ jpws

enum class ArgOption {
	Default,
	Alt
};

#include "jpws.h"

int main(int argc, char** argv) {

    if (argc == 2 && std::string(argv[1]) == "--info") {
        displayInfo();
        return 0;
    }
    
    if (argc < 3 || argc > 4) {
        std::cout << "\nUsage: jpws [-alt] <cover_image> <powershell_script>\n\t\bjpws --info\n\n";
        return 1;
    }

    ArgOption lastBlockString = ArgOption::Default;
    uint8_t argIndex = 1;

    if (argc == 4) {
         if (std::string(argv[1]) != "-alt") {
         std::cerr << "\nInput Error: Invalid arguments. Expecting \"-alt\" as the only optional argument.\n\n";
         return 1;
    	 }

     	 lastBlockString = ArgOption::Alt;
     	 argIndex = 2;
    }

    const std::string 
	IMAGE_FILENAME = argv[argIndex],
    	POWERSHELL_FILENAME = argv[++argIndex];

    constexpr const char* REG_EXP = ("(\\.[a-zA-Z_0-9\\.\\\\\\s\\-\\/]+)?[a-zA-Z_0-9\\.\\\\\\s\\-\\/]+?(\\.[a-zA-Z0-9]+)?");
    const std::regex regex_pattern(REG_EXP);

    if (!std::regex_match(IMAGE_FILENAME, regex_pattern) || !std::regex_match(POWERSHELL_FILENAME, regex_pattern)) {
        std::cerr << "\nInvalid Input Error: Characters not supported by this program found within filename arguments.\n\n";
        return 1;
    }

    const std::filesystem::path
        IMAGE_FILE_PATH(IMAGE_FILENAME),
        POWERSHELL_FILE_PATH(POWERSHELL_FILENAME);

    const std::string IMAGE_EXTENSION = IMAGE_FILE_PATH.extension().string();

    const std::set<std::string> VALID_IMAGE_EXTENSIONS = {".jpg", ".jpeg", ".jfif"};
    const std::string POWERSHELL_EXTENSION = ".ps1";

    if (VALID_IMAGE_EXTENSIONS.find(IMAGE_EXTENSION) == VALID_IMAGE_EXTENSIONS.end())  {
		std::cerr << "\nFile Type Error: Invalid file extension. Expecting only \".jpg, .jpeg or .jfif\" image extensions.\n\n";
        	return 1;
    }

    if (POWERSHELL_FILE_PATH.extension() != POWERSHELL_EXTENSION) {
	std::cerr << "\nFile Type Error: Invalid file extension. Expecting only \".ps1\" PowerShell extension.\n\n";
        return 1;
    }

    if (!std::filesystem::exists(IMAGE_FILENAME) || !std::filesystem::exists(POWERSHELL_FILENAME) || !std::filesystem::is_regular_file(POWERSHELL_FILENAME)) {
        std::cerr << (!std::filesystem::exists(IMAGE_FILENAME)
            ? "\nImage File Error: File not found."
            : "\nPowerShell File Error: File not found or not a regular file.")
            << " Check the filename and try again.\n\n";
        return 1;
    }

    jpws(IMAGE_FILENAME, POWERSHELL_FILENAME, lastBlockString);
}