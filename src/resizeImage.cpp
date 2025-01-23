// This software is based in part on the work of the Independent JPEG Group.
// https://github.com/libjpeg-turbo/libjpeg-turbo

// stb_image by Sean Barrett (“nothings”).
// https://github.com/nothings/stb


void resizeImage(std::vector<uint8_t>& Image_Vec, uint8_t quality, uint8_t dec_val, bool decrease)
{
    tjhandle decompressor = tjInitDecompress();
    if (!decompressor) {
        throw std::runtime_error("tjInitDecompress() failed.");
    }

    int width = 0, height = 0;
    int jpegSubsamp = 0, jpegColorspace = 0;

    if (tjDecompressHeader3(
            decompressor,
            Image_Vec.data(),
            static_cast<unsigned long>(Image_Vec.size()),
            &width,
            &height,
            &jpegSubsamp,
            &jpegColorspace) != 0)
    {
        tjDestroy(decompressor);
        throw std::runtime_error(std::string("tjDecompressHeader3: ") + tjGetErrorStr());
    }

    if (width < dec_val || height < dec_val) {
        tjDestroy(decompressor);
        throw std::runtime_error("Image is too small to shrink by 1 pixel.");
    }

    const int channels = 3;
    std::vector<uint8_t> decodedData(width * height * channels);

    if (tjDecompress2(
            decompressor,
            Image_Vec.data(),
            static_cast<unsigned long>(Image_Vec.size()),
            decodedData.data(),
            width,
            0,               
            height,
            TJPF_RGB,
            0                
        ) != 0)
    {
        tjDestroy(decompressor);
        throw std::runtime_error(std::string("tjDecompress2: ") + tjGetErrorStr());
    }

    tjDestroy(decompressor);
  
    int newWidth = 0;
    int newHeight = 0;

    if (decrease) {
        newWidth  = width  - dec_val;
        newHeight = height - dec_val;
    } else {
        newWidth  = width;
        newHeight = height;
    }

    std::cout << "\r" << std::string(44, ' ') << "\r"; // Clear the line completely
    std::cout << "Quality: " << (int)quality << "% | Width: " << newWidth << " | Height: " << newHeight << std::flush; 

    std::vector<uint8_t> resized(newWidth * newHeight * channels);

    int success = stbir_resize_uint8(
        decodedData.data(),
        width, height,
        width * channels,
        resized.data(),
        newWidth, newHeight,
        newWidth * channels,
        channels
    );
    if (!success) {
        throw std::runtime_error("stbir_resize_uint8 failed (possibly out of memory).");
    }

    tjhandle compressor = tjInitCompress();
    if (!compressor) {
        throw std::runtime_error("tjInitCompress() failed.");
    }

    unsigned char* jpegBuf  = nullptr;
    unsigned long  jpegSize = 0;

    int flags = TJFLAG_PROGRESSIVE;  

    // Use the detected jpegSubsamp directly
    if (tjCompress2(
            compressor,
            resized.data(),
            newWidth,
            0,         
            newHeight,
            TJPF_RGB,
            &jpegBuf,
            &jpegSize,
            jpegSubsamp,  // Use the detected subsampling here
            quality,     
            flags
        ) != 0)
    {
        tjDestroy(compressor);
        throw std::runtime_error(std::string("tjCompress2: ") + tjGetErrorStr());
    }

    tjDestroy(compressor);

    std::vector<uint8_t> outJpeg(jpegBuf, jpegBuf + jpegSize);
    tjFree(jpegBuf);

    Image_Vec.swap(outJpeg);
}
