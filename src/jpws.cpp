int jpws(const std::string& IMAGE_FILENAME, std::string& powershell_filename, bool isAltOption) {

	constexpr uint32_t MAX_IMAGE_FILE_SIZE		= 512 * 1024;	// 512KB.
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
                  		: "greater than the maximum allowed: 512KB")
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

	std::vector<uint8_t> PowerShell_Vec;
	PowerShell_Vec.resize(POWERSHELL_FILE_SIZE); 
	
	powershell_file_ifs.read(reinterpret_cast<char*>(PowerShell_Vec.data()), POWERSHELL_FILE_SIZE);
	powershell_file_ifs.close();

	constexpr uint8_t
		JFIF_SIG[]	{ 0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00},
		SOI_SIG[]	{ 0xFF, 0xD8 }, // FFD8 is a marker that indicates the start of a JPEG file's data stream. It's also known as the "Start of Image" (SOI) marker.
		EOI_SIG[] 	{ 0xFF, 0xD9 }, // FFD9 is the marker for end of image.
		SOFP_SIG[]	{ 0xFF, 0xC2 }, // FFC2 is a marker in JPEG files that stands for "Start of Frame 2" and indicates Progressive DCT.
		BOM_SIG[]	{ 0xEF, 0xBB, 0xBF }, // Byte-order mark, we need to remove this if found within the PowerShell script file.
		CLOSE_COMMENT_BLOCK_SIG[] {0x23, 0x3E},
		JIFF_SIG_LENGTH = 20;

	if (!std::equal(std::begin(SOI_SIG), std::end(SOI_SIG), std::begin(Image_Vec)) || !std::equal(std::begin(EOI_SIG), std::end(EOI_SIG), std::end(Image_Vec) - 2)) {
        	std::cerr << "\nImage File Error: This is not a valid JPG image.\n\n";
		return 1;
	}

	eraseSegments(Image_Vec);

	Image_Vec.insert(Image_Vec.begin(), std::begin(JFIF_SIG), std::end(JFIF_SIG));

	const uint32_t SOFP_POS = searchFunc(Image_Vec, 0, 0, SOFP_SIG); 

	uint8_t quality = 100;

	bool 
		decrease = false,
		isImageModified = false;

	if (SOFP_POS == Image_Vec.size()) {
		std::cerr << "\nImage File Encoding:\n\nImage does not appear to be a Progressive encoded JPG image.\n"
			  << "For X/Twitter compatibility, your cover image will be re-encoded.\n\n"
			  << "To avoid re-encoding, source your JPG images from X/Twitter.\n"
			  << "Please check output image to make sure image quality is acceptable.\n";

			  resizeImage(Image_Vec, quality, decrease);
	}

	if (std::equal(std::begin(BOM_SIG), std::end(BOM_SIG), PowerShell_Vec.begin())) {
        	PowerShell_Vec.erase(PowerShell_Vec.begin(), PowerShell_Vec.begin() + 3);
        }

	uint16_t sequence_count = 0;
	uint32_t 
		close_comment_block_pos = 0,
		sequence_index = 0;

	while(true) {
		close_comment_block_pos = searchFunc(Image_Vec, close_comment_block_pos, 1, CLOSE_COMMENT_BLOCK_SIG);
    
      		if (close_comment_block_pos == Image_Vec.size()) {
        		break;
		}

		++sequence_count;
		sequence_index = close_comment_block_pos;
	}

	if (sequence_count == 1) {
		std::cout << "\nJust one close-comment block character sequence was found within the cover image.\n"
			  << "\nInstead of reducing image dimensions, which we do automatically for multiple occurrences"
			  << "\nof the comment-block sequence, we can attempt to directly modify a single byte."
			  << "\n\nThis quite often works without any noticeable difference to the output cover image." 
			  << "\nIf the output image does appear corrupt/distorted, retry jpws and select a different byte option."
			  << "\n\nReplace character 0x23 with:\n"
	 		  << "\n Option_1: 0x22"
 			  << "\n Option_2: 0x2A"
 			  << "\n Option_3: 0x1F"
			  << "\n Option_4: 0x24"
			  << "\n Option_5: Reduce the image dimensions"
			  << "\n\nChoose your option (1-5) then press Enter: ";

		bool isChoiceInvalid = true;
		int choice = 0;

		while(isChoiceInvalid) {
			std::cin >> choice;
			if (choice >= 1 && choice <= 5) {
				isChoiceInvalid = false;
			}
			switch (choice) {
        			case 1:
            				Image_Vec[sequence_index] = 0x22;
           				break;
        			case 2:
            				Image_Vec[sequence_index] = 0x2A;
            				break;
        			case 3:
            				Image_Vec[sequence_index] = 0x1F;
            				break;
        			case 4:
            				Image_Vec[sequence_index] = 0x24;
            				break;
        			case 5:
            				++sequence_count;
            				break;
        			default:
            				std::cout << "\nInvalid choice!";
					std::cout << "\nChoose your option (1-5) then press Enter: ";
            			break;
    			}
		}
	}

	close_comment_block_pos = 0;

	if (sequence_count > 1) {
		uint8_t decrease_attempts = 5;

		isImageModified = true;
		decrease = true;

		while(close_comment_block_pos != Image_Vec.size()) {
			
			resizeImage(Image_Vec, quality, decrease);
			--decrease_attempts;
			--quality;

			close_comment_block_pos = searchFunc(Image_Vec, 0, 0, CLOSE_COMMENT_BLOCK_SIG);

			if (!decrease_attempts){
		  	std::cerr << "\nImage Compatibility Error:\n\nProcedure failed to remove close-comment block sequences from cover image.\n"
			   	 << "Try another image or use an editor such as GIMP to reduce (scale) image dimensions.\n\n";
	          	return 1;
			}
		}
	}

	std::vector<uint8_t> 
		CloseCommentBlockString,
		DefaultEndStringVec 	{ 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x23, 0x3E, 0x0D, 0x23, 0x9e, 0xFF, 0xD9 },
		AltEndStringVec		{ 0x9e, 0x23, 0x3e, 0x0d, 0x23, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0xFF, 0xD9 };

	if (isAltOption) {
		CloseCommentBlockString = AltEndStringVec;
	} else {
		CloseCommentBlockString = DefaultEndStringVec;
	}
	
	std::copy(CloseCommentBlockString.rbegin(), CloseCommentBlockString.rend(), Image_Vec.rbegin());

	constexpr uint8_t POWERSHELL_INSERT_INDEX = 6;

	Profile_Vec.insert(Profile_Vec.end() - POWERSHELL_INSERT_INDEX, PowerShell_Vec.begin(), PowerShell_Vec.end());

	uint8_t
		bits = 16,	
		jfif_open_comment_block_index = 0x0D,					
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
	
	constexpr uint8_t JFIF_OPEN_COMMENT_BLOCK[] {0x0D, 0x3C, 0x23, 0x0D, 0x0A};

	std::copy(std::begin(JFIF_OPEN_COMMENT_BLOCK), std::end(JFIF_OPEN_COMMENT_BLOCK), Image_Vec.begin() + jfif_open_comment_block_index);

	if (!writeFile(Image_Vec)) {
		return 1;
	}

	if (isImageModified) {
		std::cout << "\nBy reducing image dimensions, close-comment block sequences\nwere succesfully removed from this image.\n"
			  << "\nPlease check to make sure quality of cover image is acceptable.\n";
	}
	std::cout << "\nComplete!\n\n";
	return 0;
}
