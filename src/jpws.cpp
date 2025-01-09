int jpws(const std::string& IMAGE_FILENAME, std::string& powershell_filename) {

	constexpr uint32_t MAX_COMBINED_FILE_SIZE 	= 5 * 1024 * 1024;  	// 5MB. (Image + PowerShell file)
	constexpr uint16_t MAX_POWERSHELL_FILE_SIZE 	= 10 * 1024;		// 10KB. 

	constexpr uint8_t 
		MIN_IMAGE_FILE_SIZE		= 134,	// 134Bytes.
		MIN_POWERSHELL_FILE_SIZE	= 10; 	// 10Bytes.

	const size_t 
		IMAGE_FILE_SIZE 	= std::filesystem::file_size(IMAGE_FILENAME),
		POWERSHELL_FILE_SIZE 	= std::filesystem::file_size(powershell_filename),
		COMBINED_FILE_SIZE 	= POWERSHELL_FILE_SIZE + IMAGE_FILE_SIZE;

	if (COMBINED_FILE_SIZE > MAX_COMBINED_FILE_SIZE) {
   	 	std::cerr << "\nFile Size Error: Combined file sizes will exceed the maximum allowed: 5MB.\n\n";
    		return 1;
	}

	if (MIN_IMAGE_FILE_SIZE > IMAGE_FILE_SIZE) {
    		std::cerr << "\nFile Size Error: Image file size is smaller than the minimum allowed: 134 Bytes.\n\n";
    		return 1;
	}

	if (MIN_POWERSHELL_FILE_SIZE > POWERSHELL_FILE_SIZE) {
    		std::cerr << "\nFile Size Error: PowerShell file size is smaller than the minimum allowed: 10 Bytes.\n\n";
    		return 1;
	}

	if (POWERSHELL_FILE_SIZE > MAX_POWERSHELL_FILE_SIZE) {
    		std::cerr << "\nFile Size Error: PowerShell file size exceeds the maximum allowed: 10KB.\n\n";
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
		std::cerr << "\nImage File Error: Cover image does not appear to be an X/Twitter encoded JPG image.\n"
			  << "\t\bIf you want to use this image, post it to X/Twitter first, save it and retry.\n\n";
		return 1;
	}

	eraseSegments(Image_Vec);

	if (std::equal(std::begin(BOM_SIG), std::end(BOM_SIG), PowerShell_Vec.begin())) {
        	PowerShell_Vec.erase(PowerShell_Vec.begin(), PowerShell_Vec.begin() + 3);
        }

	constexpr uint8_t CLOSE_COMMENT_BLOCK_SIG[] {0x23, 0x3E};
 
	const uint32_t CLOSE_COMMENT_BLOCK_POS = searchFunc(Image_Vec, 0, 0, CLOSE_COMMENT_BLOCK_SIG);
    
      	if (CLOSE_COMMENT_BLOCK_POS != Image_Vec.size()) {
        	std::cerr << "\nImage File Error:\n\nThis image cannot be used as it contains the two byte close-comment block character sequence: \"#>\". "
                      	  << "\nPresence of the close-comment block within the image data will break the PowerShell script."
			  << "\n\nIt's possible to remove close-comment blocks within the image by slightly decreasing\nthe image dimensions using an editor such as GIMP. Repeat, if necessary.\n\n";
		return 1;	
	}

	constexpr int POWERSHELL_INSERT_INDEX = 6;

	Profile_Vec.insert(Profile_Vec.end() - POWERSHELL_INSERT_INDEX, PowerShell_Vec.begin(), PowerShell_Vec.end());

	uint8_t
		bits = 16,						
		segment_size_field_index = 0x16;		
		
	const size_t SEGMENT_SIZE = Profile_Vec.size() - segment_size_field_index;
	
	if (SEGMENT_SIZE > MAX_POWERSHELL_FILE_SIZE) {
		std::cerr <<"\nSegment Size Error: The profile segment (FFE2) exceeds the maximum size of 10KB.\n\n";
		return 1;
	}

	while (bits) {
		Profile_Vec[segment_size_field_index++] = (SEGMENT_SIZE >> (bits -= 8)) & 0xff;
	}

	Image_Vec.insert(Image_Vec.begin(), Profile_Vec.begin(), Profile_Vec.end());
	
	constexpr uint8_t CloseCommentBlockString[] { 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x23, 0x3E, 0x0D, 0x23, 0x9e, 0xFF, 0xD9  };

	std::copy(std::rbegin(CloseCommentBlockString), std::rend(CloseCommentBlockString), Image_Vec.rbegin());

	if (!writeFile(Image_Vec)) {
		return 1;
	}

	std::cout << "\nComplete!\n\n";
	return 0;
}
