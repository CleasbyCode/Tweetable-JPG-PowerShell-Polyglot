int jpws(const std::string& IMAGE_FILENAME, std::string& powershell_filename, bool isAltOption) {
	constexpr uint32_t MAX_IMAGE_FILE_SIZE		= 1024 * 1024;	// 1MB.
	constexpr uint16_t MAX_POWERSHELL_FILE_SIZE 	= 10 * 1024; 	// 10KB. 

	constexpr uint8_t 
		MIN_IMAGE_FILE_SIZE		= 134,	// 134Bytes.
		MIN_POWERSHELL_FILE_SIZE	= 10; 	// 10Bytes.

	const size_t 
		IMAGE_FILE_SIZE 	= std::filesystem::file_size(IMAGE_FILENAME),
		POWERSHELL_FILE_SIZE 	= std::filesystem::file_size(powershell_filename);

	if (IMAGE_FILE_SIZE < MIN_IMAGE_FILE_SIZE || IMAGE_FILE_SIZE > MAX_IMAGE_FILE_SIZE) {
    		std::cerr << "\nImage Size Error: The image file size is "
              		<< (IMAGE_FILE_SIZE < MIN_IMAGE_FILE_SIZE
                 	 	? "smaller than the minimum allowed: 134 Bytes"
                  		: "greater than the maximum allowed: 1MBKB")
              		<< ".\n\n";
    		return 1;
	}

	if (MIN_POWERSHELL_FILE_SIZE > POWERSHELL_FILE_SIZE || POWERSHELL_FILE_SIZE > MAX_POWERSHELL_FILE_SIZE) {
    		std::cerr << "\nFile Size Error: PowerShell file size is "
			<< (MIN_POWERSHELL_FILE_SIZE > POWERSHELL_FILE_SIZE 
				? "smaller than the minimum allowed: 10 Bytes."
				: "greater than the maximum allowed: 10KB.")
			<< ".\n\n";
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

	bool encodeImage = Image_Vec[0x0D] != 0x19;

	std::vector<uint8_t> PowerShell_Vec;
	PowerShell_Vec.resize(POWERSHELL_FILE_SIZE); 
	
	powershell_file_ifs.read(reinterpret_cast<char*>(PowerShell_Vec.data()), POWERSHELL_FILE_SIZE);
	powershell_file_ifs.close();

	constexpr uint8_t
		JFIF_SIG[]	{ 0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00},
		SOI_SIG[]	{ 0xFF, 0xD8 }, 
		EOI_SIG[] 	{ 0xFF, 0xD9 }, 
		BOM_SIG[]	{ 0xEF, 0xBB, 0xBF }, 
		COMMENT_BLOCK_SIG[] {0x23, 0x3E}, 
		JIFF_SIG_LENGTH = 20;

	if (!std::equal(std::begin(SOI_SIG), std::end(SOI_SIG), std::begin(Image_Vec)) || !std::equal(std::begin(EOI_SIG), std::end(EOI_SIG), std::end(Image_Vec) - 2)) {
        	std::cerr << "\nImage File Error: This is not a valid JPG image.\n\n";
		return 1;
	}

	eraseSegments(Image_Vec);
	
	Image_Vec.insert(Image_Vec.begin(), std::begin(JFIF_SIG), std::end(JFIF_SIG));

	std::vector<uint8_t>Image_Vec_Copy;
	Image_Vec_Copy = Image_Vec;

	if (std::equal(std::begin(BOM_SIG), std::end(BOM_SIG), PowerShell_Vec.begin())) {
        	PowerShell_Vec.erase(PowerShell_Vec.begin(), PowerShell_Vec.begin() + 3);
        }

	bool 
		decrease = false,
		modified = false;

	if (encodeImage) {	// Skip this section for our repo compatible images.

		uint8_t 
			quality = 100,
			decrease_attempts = 30,
			dec_val = 0;
	
		resizeImage(Image_Vec, quality, dec_val, decrease);
		decrease = true;

		uint32_t comment_block_pos = searchFunc(Image_Vec, 0, 0, COMMENT_BLOCK_SIG);

		while(comment_block_pos != Image_Vec.size()) {
			Image_Vec.clear();
			Image_Vec = Image_Vec_Copy; // Use the fresh copy.
			--decrease_attempts;
			++dec_val;
			quality -= (decrease_attempts >= 15) ? 0 : 1;
			resizeImage(Image_Vec, quality, dec_val, decrease);
			comment_block_pos = searchFunc(Image_Vec, 0, 0, COMMENT_BLOCK_SIG);
			if (!decrease_attempts){
		  	std::cerr << "\nImage Compatibility Error:\n\nProcedure failed to remove comment block sequences from the cover image.\n"
			   	 << "Try another image or use an editor such as GIMP to manually reduce (scale) image dimensions.\n\n";
	          	return 1;
			}
		}
	}

	modified = decrease;

	std::vector<uint8_t> 
		CommentBlockString,
		DefaultEndStringVec 	{ 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x23, 0x3E, 0x0D, 0x23, 0x9e },
		AltEndStringVec		{ 0x9e, 0x23, 0x3e, 0x0d, 0x23, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00 }; // If the default string fails to be preserved, try this one (-alt option).

	if (isAltOption) {
		CommentBlockString = AltEndStringVec;
	} else {
		CommentBlockString = DefaultEndStringVec;
	}
	
	std::copy(CommentBlockString.rbegin(), CommentBlockString.rend(), Image_Vec.rbegin() + 2);

	constexpr uint8_t POWERSHELL_INSERT_INDEX = 6;

	Profile_Vec.insert(Profile_Vec.end() - POWERSHELL_INSERT_INDEX, PowerShell_Vec.begin(), PowerShell_Vec.end());

	uint8_t
		bits = 16,	
		jfif_comment_block_index = 0x0D,					
		segment_size_field_index = 0x16;		
		
	const size_t SEGMENT_SIZE = (Profile_Vec.size() + JIFF_SIG_LENGTH) - segment_size_field_index;
	
	if (SEGMENT_SIZE > MAX_POWERSHELL_FILE_SIZE) {
		std::cerr <<"\nSegment Size Error: The profile segment (FFE2) exceeds the maximum size limit of 10KB.\n\n";
		return 1;
	}

	Image_Vec.insert(Image_Vec.begin() + JIFF_SIG_LENGTH , Profile_Vec.begin(), Profile_Vec.end());

	while (bits) {
		Image_Vec[segment_size_field_index++] = (SEGMENT_SIZE >> (bits -= 8)) & 0xff;
	}
	
	constexpr uint8_t JFIF_COMMENT_BLOCK[] {0x0D, 0x3C, 0x23, 0x0D, 0x0A};

	std::copy(std::begin(JFIF_COMMENT_BLOCK), std::end(JFIF_COMMENT_BLOCK), Image_Vec.begin() + jfif_comment_block_index);

	if (!writeFile(Image_Vec)) {
		return 1;
	}

	if (modified) {
		std::cout << "\nBy reducing image dimensions and quality, close-comment block sequences\nwere succesfully removed from this image.\n"
			  << "\nPlease check to make sure size & quality of cover image is acceptable.\n";
	}
	std::cout << "\nComplete!\n\n";
	return 0;
}
