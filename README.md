# jpws

A simple command-line tool to embed a ***PowerShell*** script within a ***JPG*** image to create a tweetable ***JPG-PowerShell*** polyglot file. Share your ***PowerShell*** embedded image on ***Twitter*** or ****Reddit*** (Desktop/Browser only for ***Reddit***).

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
## Image Selection for Your PowerShell Script

As a requirement for this program, due to image encoding issues, you need to first download images from Twitter to use with this program. There are a wide variety of images posted on ***Twitter***, epecially many cool AI generated images created by ***Midjourney***.  

When saving an image from ***Twitter***, make sure to click on the image first, so as to fully expand it, then save it, so that you get the full image
dimension size. You will probably have to download and try a few images before a compatible one is found. The incompatiblity issue is caused 
by the presence within the ***JPG*** image file of the *end-comment block string* ***"#>"***, which breaks the script when we execute it from the image.  

Within the ***Images*** folder of this repo, I have included a few compatible images for you to use. 

## Executing Your Embedded ***PowerShell*** Script From the Image

To run the script within ***Windows*** after successfully embedding your ***PowerShell*** script in a compatible image, you need to rename the *".jpg"* file extension to *".ps1"*. You can then start the script either from a ***PowerShell*** console, e.g. *PS C:\Users\Demo> .\\jpws_img.ps1* or start it from the ***Windows*** desktop by right-clicking the file icon and selecting *"Run with PowerShell"*.  

The best way to run the script after tweeting the image, is to first get the image address from ***Twitter***.  
You do this by clicking on the image within the tweet to fully expand it, then right-click on the image and select "***Copy image address***". From a ***PowerShell*** console type the following command with the pasted-in image address, which needs to be within quotation marks:

````bash
iwr "https://pbs.twimg.com/media/F6KdhMtX0AAhSQf?format=jpg&name=small" -OutFile fib.ps1;.\fib.ps1
````
The command will download the image from ***Twitter***, rename the image file to give it a ***PowerShell*** extension, then execute the script.

To start the script using ***Linux***, just enter the following command within a terminal. There is no need to change the file extension with ***Linux***. 

````bash
$ pwsh jpws_img.jpg
````


```bash
 PS C:\Demo> Expand-Archive  .\xif_img.zip
```
For a ***RAR*** file under **Windows**, you will need to use a program such as **WinRar** to extract your file from the image.

My other programs you may find useful:-  

* [pdvzip: CLI tool to embed a ZIP file within a tweetable and "executable" PNG-ZIP polyglot image.](https://github.com/CleasbyCode/pdvzip)
* [jdvrif: CLI tool to encrypt & embed any file type within a JPG image.](https://github.com/CleasbyCode/jdvrif)
* [imgprmt: CLI tool to embed an image prompt (e.g. "Midjourney") within a tweetable JPG-HTML polyglot image.](https://github.com/CleasbyCode/imgprmt)
* [pdvrdt: CLI tool to encrypt, compress & embed any file type within a PNG image.](https://github.com/CleasbyCode/pdvrdt)
* [pdvps: PowerShell / C++ CLI tool to encrypt & embed any file type within a tweetable & "executable" PNG image](https://github.com/CleasbyCode/pdvps)

##

