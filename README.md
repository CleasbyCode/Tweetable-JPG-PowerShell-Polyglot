# jpws

A simple command-line tool to embed a ***PowerShell*** script within a ***JPG*** image to create a tweetable ***JPG-PowerShell*** polyglot image. Share your ***PowerShell*** embedded image on ***Twitter*** or ****Reddit*** (Desktop/Browser only for ***Reddit***).

![Demo Image_a](https://github.com/CleasbyCode/jpws/blob/main/demo_image/fib.jpg)  
***{Image Credit: [@no_dualitat](https://twitter.com/no_dualitat/status/1701678793172082849)}***  
***Image embedded with PowerShell script. Simple Fibonacci number sequence program.***  

Compile and run the program under ***Windows*** or ***Linux***  

## Usage (***Linux*** Example)

```bash

$ g++ jpws.cpp -s -o jpws
$
$ ./jpws

Usage:  jpws  <jpg_image>  <script_file>
        jpws  --info

$ ./jpws  spiral.jpg  fibonacci.ps1

 Created output file: "jpws_img.jpg 59332 Bytes"
 You can now tweet this image.

```
## Images Selection for Your PowerShell Script

As a requirement for this program, due to image encoding issues, you need to download images from Twitter to use with this program when
embedding your ***PowerShell*** script. There are a wide variety of images posted on ***Twitter***, epecially cool AI generated images created by ***Midjourney***.  

When saving an image from Twitter, make sure to click on the image first, so as to fully expand it, then save it, so that you get the full image
dimension size. You will probably have to download & try a few images before a compatible one is found for ***jpws***. The incompatiblity issue is caused 
by the preseance within the image file of the ***PowerShell*** *end-comment block* string "#>", which will break the script when we execute it from the image.

## Executing Your Embedded ***PowerShell*** Script From the Image File

After sucesscully embedding your ***PowerShell*** script within a compatible image, to run the script from Windows, you need to rename the *".jpg"*  
file extenstion to *".ps1"*. You can then run the script either from a PowerShell console, e.g. *PS C:\Users\Demo> \.jpws_img.ps1* or run it from
the Desktop by right-clicking the file icon and select *Run with PowerShell*.


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

