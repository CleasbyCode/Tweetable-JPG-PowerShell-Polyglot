#include <cstdlib>  // Required for system()
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

int main() {
    const char* command_one = "wget -S -O /var/www/hire-caravan.com/html/cam_update/webcam5.txt -q https://www.overcombebeachcam.co.uk/ 2>/dev/null";
    const char* command_two = "wget -S -O /var/www/hire-caravan.com/html/cam_update/webcam6.txt -q https://www.greenhillbeachcam.co.uk/ 2>/dev/null";	
    constexpr uint_fast8_t STRING_SIG[] { 0x3F, 0x61, 0x75, 0x74, 0x6F };

    std::string webfile = "/var/www/hire-caravan.com/html/webcams.php";
    // search the vector for the ?autoplay string

    // Execute the wget command
    int result_one = system(command_one);
    int result_two = system(command_two);

    // open the file
    std::ifstream webcam5_ifs("/var/www/hire-caravan.com/html/cam_update/webcam5.txt", std::ios::binary);
    std::ifstream webcam6_ifs("/var/www/hire-caravan.com/html/cam_update/webcam6.txt", std::ios::binary);
	
    // read in and store contents of file into vector.
    std::vector<uint8_t>cam5_Vec((std::istreambuf_iterator<char>(webcam5_ifs)), std::istreambuf_iterator<char>());
    std::vector<uint8_t>cam6_Vec((std::istreambuf_iterator<char>(webcam6_ifs)), std::istreambuf_iterator<char>());

    uint_fast32_t first_index = static_cast<uint_fast32_t>(std::search(cam5_Vec.begin(), cam5_Vec.end(), std::begin(STRING_SIG), std::end(STRING_SIG)) - cam5_Vec.begin());

    uint_fast32_t second_index = static_cast<uint_fast32_t>(std::search(cam6_Vec.begin(), cam6_Vec.end(), std::begin(STRING_SIG), std::end(STRING_SIG)) - cam6_Vec.begin());

    first_index -= 11;
    second_index -= 11;

    std::string link_five;
    std::string link_six;

    // Create the link strings.
    for (int i = 0; i < 11; i++){
      link_five += cam5_Vec[first_index+i];
      link_six += cam6_Vec[second_index+i];
    }

    // open website webcam.php file
    std::fstream file(webfile, std::ios::in | std::ios::out | std::ios::binary);

    // Move the write pointer to the correct index locations.
    std::streampos first_index_position = 0xB7C;
    std::streampos second_index_position = 0xC8B;
    file.seekp(first_index_position);

    // Write the Webcam5 link string to webcams.php
    file.write(link_five.c_str(), link_five.size());

    file.seekp(second_index_position);
	
    // Write the Webcam6 link string to webcams.php
    file.write(link_six.c_str(), link_six.size());
	
    // Close the file
    file.close();

    return 0;
}
