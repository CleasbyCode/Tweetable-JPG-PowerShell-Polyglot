# jpws

A simple command-line tool to embed a PowerShell script within a JPG image to create a tweetable JPG-PowerShell polyglot image.

Share your PowerShell embedded image on Twitter and Reddit (Desktop/Browser only for Reddit).

![Demo Image_a](https://github.com/CleasbyCode/xif/blob/main/demo_image/bottle.jpg)   
***[{Original image source}](https://comfyanonymous.github.io/ComfyUI_examples/sdxl/)***

*(If you want to embed more data into an image for Twitter (up to 5MB) then please try **[pdvzip](https://github.com/CleasbyCode/pdvzip)**,  
or if you want to save AI image prompts (***Midjourney***) into a JPG image file (viewable as a basic Web page), then please try **[imgprmt](https://github.com/CleasbyCode/imgprmt)).***

**xif** works on **Linux** and **Windows**.  

Your embedded file is stored within the ***ICC Profile*** of the ***JPG*** image file.  

![Demo Image_b](https://github.com/CleasbyCode/xif/blob/main/demo_image/icc_dem.png)  

[***Video Demo***](https://youtu.be/SIMZe5Ix5Y8)

Compile and run the program under **Windows** or **Linux**  

## Usage (Linux Example)

```bash

$ g++ xif.cpp -s -o xif
$
$ ./xif

Usage:  xif  <jpg_image>  <data_file>
        xif  --info

$ ./xif  image.jpg  sdxl_workflow_json.rar

 Created output file: "xif_img.jpg 9256 Bytes"
 You can now tweet this image.

```
## Extracting Your Embedded File(s)

To get access to and extract your data file for a ***ZIP*** or ***RAR*** embedded file under **Linux Desktop**, just rename 
the ***.jpg*** file extension to ***.zip***, then click the file icon to open & extract the ***ZIP/RAR*** contents.

For just a ***RAR*** file under **Linux**, you don't need to rename the file extension. From a **Linux** terminal enter the following command:

```bash
 $ unrar e xif_img.jpg
```

For an embedded ***ZIP*** file under **Windows**, rename the ***.jpg*** file extension to ***.zip***. From a console type the following command:

```bash
 PS C:\Demo> Expand-Archive  .\xif_img.zip .
```
For a ***RAR*** file under **Windows**, you will need to use a program such as **WinRar** to extract your file from the image.

My other programs you may find useful:-  

* [pdvzip: CLI tool to embed a ZIP file within a tweetable and "executable" PNG-ZIP polyglot image.](https://github.com/CleasbyCode/pdvzip)
* [jdvrif: CLI tool to encrypt & embed any file type within a JPG image.](https://github.com/CleasbyCode/jdvrif)
* [imgprmt: CLI tool to embed an image prompt (e.g. "Midjourney") within a tweetable JPG-HTML polyglot image.](https://github.com/CleasbyCode/imgprmt)
* [pdvrdt: CLI tool to encrypt, compress & embed any file type within a PNG image.](https://github.com/CleasbyCode/pdvrdt)
* [pdvps: PowerShell / C++ CLI tool to encrypt & embed any file type within a tweetable & "executable" PNG image](https://github.com/CleasbyCode/pdvps)

##

