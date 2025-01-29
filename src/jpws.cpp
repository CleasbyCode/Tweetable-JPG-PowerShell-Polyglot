int jpws(const std::string& IMAGE_FILENAME, const std::string& POWERSHELL_FILENAME, ArgOption lastBlockString) {
	constexpr uint32_t MAX_IMAGE_FILE_SIZE		= 2048 * 1024;	// 2MB.
	constexpr uint16_t MAX_POWERSHELL_FILE_SIZE 	= 10 * 1024; 	// 10KB. 

	constexpr uint8_t 
		MIN_IMAGE_FILE_SIZE		= 134,	// 134Bytes.
		MIN_POWERSHELL_FILE_SIZE	= 10; 	// 10Bytes.

	const size_t 
		IMAGE_FILE_SIZE 	= std::filesystem::file_size(IMAGE_FILENAME),
		POWERSHELL_FILE_SIZE 	= std::filesystem::file_size(POWERSHELL_FILENAME);

	bool shouldEncodeImage = false;

	if (IMAGE_FILE_SIZE < MIN_IMAGE_FILE_SIZE || IMAGE_FILE_SIZE > MAX_IMAGE_FILE_SIZE) {
    		std::cerr << "\nImage Size Error: The image file size is "
              		<< (IMAGE_FILE_SIZE < MIN_IMAGE_FILE_SIZE
                 	 	? "smaller than the minimum allowed: 134 Bytes"
                  		: "greater than the maximum allowed: 2MB")
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
		powershell_file_ifs(POWERSHELL_FILENAME, std::ios::binary);

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

	constexpr uint8_t COMPATIBLE_IMAGE_VAL = 0x19;

	if (Image_Vec[0x0D] != COMPATIBLE_IMAGE_VAL) {
		shouldEncodeImage = true;
	}
	
	eraseSegments(Image_Vec);
	
	Image_Vec.insert(Image_Vec.begin(), std::begin(JFIF_SIG), std::end(JFIF_SIG));

	std::vector<uint8_t>Image_Vec_Copy;
	Image_Vec_Copy = Image_Vec;

	if (std::equal(std::begin(BOM_SIG), std::end(BOM_SIG), PowerShell_Vec.begin())) {
        	PowerShell_Vec.erase(PowerShell_Vec.begin(), PowerShell_Vec.begin() + 3);
        }

	bool 
		shouldDecreaseVals = false,
		isImageModified = false;

	std::cout << '\n';

	if  (shouldEncodeImage) {	

		uint8_t quality_val = 100;
			
		uint16_t 
			decrease_attempts = 300,
			decrease_dims_val = 0;

		std::cout << "Checking cover image for comment-block close sequences \"#>\" (0x23, 0x3E).\n\n"
			  << "Image quality & dimensions will be reduced in an attempt to remove these sequences.\n\n";

		resizeImage(Image_Vec, quality_val, decrease_dims_val, shouldDecreaseVals);
		
		isImageModified = true;

		uint32_t comment_block_pos = searchFunc(Image_Vec, 0, 0, COMMENT_BLOCK_SIG);

		shouldDecreaseVals = true;

		while(comment_block_pos != Image_Vec.size()) {
			Image_Vec.clear();
			Image_Vec = Image_Vec_Copy; // Use the fresh copy.

			--decrease_attempts;
			++decrease_dims_val;
			quality_val -= (decrease_attempts % 15 == 0) ? 2 : 0;
			resizeImage(Image_Vec, quality_val, decrease_dims_val, shouldDecreaseVals);

			comment_block_pos = searchFunc(Image_Vec, 0, 0, COMMENT_BLOCK_SIG);

			if (!decrease_attempts) {
		  		std::cerr << "\n\nImage Compatibility Error:\n\nProcedure failed to remove close-comment block sequences from cover image.\n"
			   		 << "Try another image or use an editor such as GIMP to manually reduce (scale) image dimensions.\n\n";
	          		return 1;
			}
		}
	}

	std::vector<uint8_t>().swap(Image_Vec_Copy);

	std::cout << '\n';

	std::vector<uint8_t> 
		CommentBlockString,
		DefaultEndStringVec 	{ 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x23, 0x3E, 0x0D, 0x23, 0x9e },
		AltEndStringVec		{ 0x9e, 0x23, 0x3e, 0x0d, 0x23, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00 }; // If the default string fails to be preserved, try this one (-alt option).

	if (lastBlockString == ArgOption::Alt) {
		CommentBlockString = AltEndStringVec;
	} else {
		CommentBlockString = DefaultEndStringVec;
	}
	
	std::copy(CommentBlockString.rbegin(), CommentBlockString.rend(), Image_Vec.rbegin() + 2);

	constexpr uint8_t POWERSHELL_INSERT_INDEX = 6;

	Profile_Vec.insert(Profile_Vec.end() - POWERSHELL_INSERT_INDEX, PowerShell_Vec.begin(), PowerShell_Vec.end());

	std::vector<uint8_t>().swap(PowerShell_Vec);

	uint8_t
		bits = 16,	
		jfif_comment_block_index = 0x0C,					
		segment_size_field_index = 0x16,
		profile_size_field_index = 0x26;		
		
	const size_t 
		SEGMENT_SIZE = (Profile_Vec.size() + JIFF_SIG_LENGTH) - segment_size_field_index,
		PROFILE_SIZE = SEGMENT_SIZE - bits;
	
	if (SEGMENT_SIZE > MAX_POWERSHELL_FILE_SIZE) {
		std::cerr <<"\nSegment Size Error: The profile segment (FFE2) exceeds the maximum size limit of 10KB.\n\n";
		return 1;
	}

	Image_Vec.insert(Image_Vec.begin() + JIFF_SIG_LENGTH , Profile_Vec.begin(), Profile_Vec.end());

	// Update the two byte segment size for the color profile (FFE2xx).
	while (bits) {
		Image_Vec[segment_size_field_index++] = (SEGMENT_SIZE >> (bits -= 8)) & 0xFF;
	}

	bits = 32;
	
	// Update the four byte size field of the color profile. 
	while (bits) {
		Image_Vec[profile_size_field_index++] = (PROFILE_SIZE >> (bits -= 8)) & 0xFF;
	}
	
	constexpr uint8_t JFIF_COMMENT_BLOCK[] {0x58, 0x54, 0x57, 0x0A, 0x3C, 0x23};

	std::copy(std::begin(JFIF_COMMENT_BLOCK), std::end(JFIF_COMMENT_BLOCK), Image_Vec.begin() + jfif_comment_block_index);

	if (!writeFile(Image_Vec)) {
		return 1;
	}

	if (isImageModified) {
		std::cout << "\nComment-block close sequences succesfully removed from image.\n"
			  << "\nPlease check to make sure size & quality of cover image is acceptable.\n";
	}
	std::cout << "\nComplete!\n\n";
	return 0;
}