// This software is based in part on the work of the Independent JPEG Group.
// https://github.com/libjpeg-turbo/libjpeg-turbo

// stb_image by Sean Barrett (“nothings”).
// https://github.com/nothings/stb

// Apart from a few tweaks, function originally generated by ChatGPT and updated by Grok to support stb_image_resize2.h 

void resizeImage(std::vector<uint8_t>& Image_Vec, uint8_t quality_val, uint16_t decrease_dims_val, bool shouldDecreaseVals)
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

    if (width < decrease_dims_val || height < decrease_dims_val) {
        tjDestroy(decompressor);
        throw std::runtime_error("Image is too small to decrease by 1 pixel.");
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

    if (shouldDecreaseVals) {
        newWidth  = width  - decrease_dims_val;
        newHeight = height - decrease_dims_val;
    } else {
        newWidth  = width;
        newHeight = height;
    }

    std::cout << "\r" << std::string(44, ' ') << "\r"; 
    std::cout << "Quality: " << (int)quality_val << "% | Width: " << newWidth << " | Height: " << newHeight << std::flush; 

    std::vector<uint8_t> resized(newWidth * newHeight * channels);

    // Use stbir_resize_uint8_srgb from stb_image_resize2.h
    if (!stbir_resize_uint8_srgb(
        decodedData.data(),
        width, height,
        0, // input stride in bytes (0 = tightly packed)
        resized.data(),
        newWidth, newHeight,
        0, // output stride in bytes (0 = tightly packed)
        static_cast<stbir_pixel_layout>(channels) // Use enum for pixel layout
    )) {
        throw std::runtime_error("stbir_resize_uint8_srgb failed.");
    }

    tjhandle compressor = tjInitCompress();
    if (!compressor) {
        throw std::runtime_error("tjInitCompress() failed.");
    }

    unsigned char* jpegBuf  = nullptr;
    unsigned long  jpegSize = 0;

    int flags = TJFLAG_PROGRESSIVE;  

    if (tjCompress2(
            compressor,
            resized.data(),
            newWidth,
            0,         
            newHeight,
            TJPF_RGB,
            &jpegBuf,
            &jpegSize,
            jpegSubsamp,  // Use the detected subsampling
            // TJSAMP_444,
            quality_val,     
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
