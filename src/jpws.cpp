int jpws(const std::string& IMAGE_FILENAME, std::string& powershell_filename) {

	constexpr int MAX_COMBINED_FILE_SIZE 	= 5 * 1024 * 1024;  	// 5MB. (Image + PowerShell file)
	constexpr int MAX_POWERSHELL_FILE_SIZE 	= 10 * 1024;		// 10KB. 
	constexpr int MIN_IMAGE_FILE_SIZE	= 134;			// 134Bytes.
	constexpr int MIN_POWERSHELL_FILE_SIZE	= 10; 			// 10Bytes.

	const size_t 
		IMAGE_FILE_SIZE 	= std::filesystem::file_size(IMAGE_FILENAME),
		POWERSHELL_FILE_SIZE 	= std::filesystem::file_size(powershell_filename),
		COMBINED_FILE_SIZE 	= POWERSHELL_FILE_SIZE + IMAGE_FILE_SIZE;

	if (COMBINED_FILE_SIZE > MAX_COMBINED_FILE_SIZE) {
   	 	std::cerr << "File Size Error: Combined file sizes will exceed the maximum allowed: 5MB.\n";
    		return 1;
	}

	if (MIN_IMAGE_FILE_SIZE > IMAGE_FILE_SIZE) {
    		std::cerr << "File Size Error: Image file size is smaller than the minimum allowed: 134Bytes.\n";
    		return 1;
	}

	if (MIN_POWERSHELL_FILE_SIZE > POWERSHELL_FILE_SIZE) {
    		std::cerr << "File Size Error: PowerShell file size is smaller than the minimum allowed: 10Bytes.\n";
    		return 1;
	}

	if (POWERSHELL_FILE_SIZE > MAX_POWERSHELL_FILE_SIZE) {
    		std::cerr << "File Size Error: PowerShell file size exceeds the maximum allowed: 10KB.\n";
    		return 1;
	}

	std::ifstream
		image_file_ifs(IMAGE_FILENAME, std::ios::binary),
		powershell_file_ifs(powershell_filename, std::ios::binary);

	if (!image_file_ifs || !powershell_file_ifs) {
		std::cerr << "\nRead File Error: Unable to read " << (!image_file_ifs 
			? "image file" 
			: "PowerShell file") << ".\n\n";
		return 1;
	}

	std::vector<uint8_t> Image_Vec;
	Image_Vec.resize(IMAGE_FILE_SIZE); 
	
	image_file_ifs.read(reinterpret_cast<char*>(Image_Vec.data()), IMAGE_FILE_SIZE);
	image_file_ifs.close();

	std::vector<uint8_t> PowerShell_Vec;
	PowerShell_Vec.resize(POWERSHELL_FILE_SIZE); 
	
	powershell_file_ifs.read(reinterpret_cast<char*>(PowerShell_Vec.data()), POWERSHELL_FILE_SIZE);
	powershell_file_ifs.close();

	constexpr uint8_t
		SOI_SIG[]	{ 0xFF, 0xD8 },
		EOI_SIG[] 	{ 0xFF, 0xD9 },
		SOFP_SIG[]	{ 0xFF, 0xC2 },
		BOM_SIG[]	{ 0xEF, 0xBB, 0xBF };

	if (!std::equal(std::begin(SOI_SIG), std::end(SOI_SIG), std::begin(Image_Vec)) || !std::equal(std::begin(EOI_SIG), std::end(EOI_SIG), std::end(Image_Vec) - 2)) {
        	std::cerr << "\nImage File Error: This is not a valid JPG image.\n\n";
		return 1;
	}
	
	const uint32_t SOFP_POS = searchFunc(Image_Vec, 0, 0, SOFP_SIG);

	if (SOFP_POS == Image_Vec.size()) {
		std::cerr << "\nImage File Error: Cover image does not appear to be a X/Twitter encoded JPG image. Please use images from X/Twitter.\n"
			  << "If you want to use this image, post it to X/Twitter, save it, then retry it with this program.\n\n";
		std::exit(EXIT_FAILURE);
	}

	eraseSegments(Image_Vec);

	if (std::equal(std::begin(BOM_SIG), std::end(BOM_SIG), PowerShell_Vec.begin())) {
        	PowerShell_Vec.erase(PowerShell_Vec.begin(), PowerShell_Vec.begin() + 3);
        }

	// Search JPG cover image for the PowerShell end-comment block string: "#>".
	// Any occurrence of this string within the image data will break the PowerShell script, as it closes the comment block too soon.
	// We need the last end-comment block string to only occur as near to the end of the image file as possible. The program will add this string itself.
	// An image with more than one occurrence of this end-comment block string is not supported.

	constexpr uint8_t COMMENT_BLOCK_SIG[] {0x23, 0x3E};
	constexpr int COMMENT_BLOCK_SIZE = 2;

    	int endCommentBlockCount = 0;
 
   	auto searchStart = Image_Vec.begin();

    	while (true) {
        	auto it = std::search(
            		searchStart,
            		Image_Vec.end(),
            		std::begin(COMMENT_BLOCK_SIG),
            		std::end(COMMENT_BLOCK_SIG)
        	);

        	if (it == Image_Vec.end()) {
            		break;
        	}

        	const auto foundIndex = std::distance(Image_Vec.begin(), it);
        
        	++endCommentBlockCount;

        	if (endCommentBlockCount < 2) {
        		Image_Vec[foundIndex] = 0x1F;
            		searchStart = it + COMMENT_BLOCK_SIZE;
        	} else {
			std::cerr << "\nImage File Error: This image is not supported as it contains too many end-comment block strings: \"#>\". "
                      		  << "End-comment block strings within the image data will cause the PowerShell script to fail."
				  << "A single occurance of a end-comment block is supported and can be altered, usually without causing any problems."
				  << "Please try another image.\n\n";
			return 1;
        	}
    	}

	if (endCommentBlockCount) {
		std::cout << "\nWarning: For compatibility requirements, a single byte change was made to the image.\n"
			" Please check output image for signs of image distortion.\n";
	}

	constexpr int POWERSHELL_INSERT_INDEX = 6;

	// Read-in and store the PowerShell script into vector "Profile_Vec" at the relevant index location within the main profile.
	Profile_Vec.insert(Profile_Vec.end() - POWERSHELL_INSERT_INDEX, PowerShell_Vec.begin(), PowerShell_Vec.end());

	int
		bits = 16,						
		segment_size_field_index = 0x16;		
		
	const size_t SEGMENT_SIZE = Profile_Vec.size() - segment_size_field_index;
	
	while (bits) {
		Profile_Vec[segment_size_field_index++] = (SEGMENT_SIZE >> (bits -= 8)) & 0xff;
	}

	Image_Vec.insert(Image_Vec.begin(), Profile_Vec.begin(), Profile_Vec.end());
	
	// End-comment block, overwrites last 13 bytes of cover image file. Required for PowerShell script to run from image.
	constexpr uint8_t EndCommentBlock[] { 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x23, 0x3E, 0x0D, 0x23, 0x9e, 0xFF, 0xD9 };

	std::copy(std::rbegin(EndCommentBlock), std::rend(EndCommentBlock), Image_Vec.rbegin());

	if (!writeFile(Image_Vec)) {
		return 1;
	}

	std::cout << "\nComplete!\n\n";
	return 0;
}

