# jpws

A simple CLI tool to embed a ***PowerShell*** script within a ***JPG*** image, creating a tweetable ***JPG-PowerShell*** polyglot file.  

![Demo Image](https://github.com/CleasbyCode/jpws/blob/main/demo_image/github_demo_pwsh_fibo.jpg) 
***{Image Credit: [@McC3Kat](https://x.com/McC3Kat)}***  
  
## Usage (***Linux***)

```console

user1@linuxbox:~/Downloads/jpws-main/src/jpws$ g++ main.cpp -O2 -s -o jpws
user1@linuxbox:~/Downloads/jpws-main/src/jpws$ sudo cp jpws /usr/bin

user1@linuxbox:~/Desktop$ jpws

Usage: jpws <cover_image> <powershell_script>  
       jpws --info

user1@linuxbox:~/Desktop$ jpws cover_image23.jpg fibo.ps1

Saved "PowerShell-embedded" JPG image: jpws_10247.jpg (321081 bytes).

Complete!
```
## How It Works

In a nutshell, ***PowerShell comment blocks***.  

An open-comment block is required as near to the start of the image file as possible. This is achieved by writing the open-comment block string within the JFIF header segment (***FFE0***), which is conveniently preserved by X/Twitter.  

The PowerShell script is stored at the end of the color profile data of the JPG image, which is also preserved by X/Twitter. We need the first PowerShell open-comment block within the JFIF header to ignore the ICC profile segment header (***FFE2***) and the color profile data.  

We then have a close-comment block at the end of the color profile data, followed by our PowerShell script, which now gets interpreted. At the end of the PowerShell script (*still within the color profile segment*) we need another open-comment block so that PowerShell ignores the remaining contents of the image file.  

Finally, we need a close-comment block as near to the end of the image file as possible. Unfortunately, things are never as straightforward as we would like them to be. The title for this section should probably of been "***How It Sometimes Works***".  This is mainly down to the short length of the comment-block string, image compatibility and X/Twitter image encoding.

## Image Compatibilty, Issues and Limitations.

To start with, always use images found on X/Twitter for your cover image. If you want to use an image not originally from that platform, post your image to X/Twitter, then save it. This ensures the JPG image will be encoded by the method used by X/Twitter. Once encoded, the whole image will be *preserved by X/Twitter when posted again on its platform, which is useful for our requirements. **X/Twitter will re-encode part of the image if bytes of the image data are modified. More on that later*. 

The cover image must not contain any occurance of the PowerShell close-comment block string (#>), apart from the ones inserted by the program, as this will cause the PowerShell script to fail. Unfortunatly, with the comment-block string length being only two bytes, the probablilty that this character sequence will appear somewhere within the image data is quite high. The larger the image, the greather the probablity of multiple hits. I would not even consider trying images larger than 400KB. 

It's possible to edit out close-comment block strings within the image data by decreasing the image dimensions using an editor such as GIMP. 

For the final close-comment block, we overwrite the last 13 bytes of image data with a default string "0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x23, 0x3E, 0x0D, 0x23, 0x9e, 0xFF, 0xD9".
Becuase we are modifying bytes within a section of image data that is compressed & encoded, this will trigger X/Twitter to re-encode some (or all) of these bytes.  

The first 6 bytes of the above string can help with the encoding and are also expendable, so it does not no matter if they are changed or removed, but following 4 bytes "0x23, 0x3E, 0x0D, 0x23" are cruical and need to be preserved by X/Twitter for the PowerShell script to work. For some images, these 4 bytes are retained by X/Twitter, but are removed or changed for others, making the image incompatible for this program.  

This repo (currently) contains 32 compatible images, should you find it difficult to find your own compatible image.  

A compatible image is an image that does not contain any occurance of the close-comment block string (#>) and preserves the 4 cruical bytes (0x23, 0x3E, 0x0D, 0x23) near the end of the file, after the PowerShell-embedded JPG image has been tweeted.

When saving an image from X/Twitter, make sure to first click the image in the post to fully expand it, then save it, so that you get the full, original image dimension size. 

X/Twitter allows a maximum size of 10KB for the color profile data, where we are storing the PowerShell script. With the overhead of the profile data, you will have about 9.5KB available for
your PowerShell script.

PowerShell scripts that require the user to provide command-line arguements are currently not supported when embedded within the JPG image. This may change once I figure out how to do this.

## Executing Your Embedded ***PowerShell*** Script From the Image

To run the script within ***Windows*** after successfully embedding your ***PowerShell*** script in a compatible image, you need to rename the *".jpg"* file extension to *".ps1"*. You can then start the script either from a ***PowerShell*** console, e.g. *PS C:\Users\Demo> .\\jpws_img.ps1* or start it from the ***Windows*** desktop by right-clicking the file icon and selecting *"Run with PowerShell"*.  

The best way to run the script after tweeting the image, is to first get the image address from ***Twitter***.  
You do this by clicking on the image within the tweet to fully expand it, then right-click on the image and select "***Copy image address***". From a ***PowerShell*** console type the following command with the pasted-in image address, which needs to be within quotation marks:

````bash
iwr -o fibo.ps1 "https://pbs.twimg.com/media/F6LKHaqXkAAbJ6n.jpg";.\fibo.ps1
````
The command will download the image from ***Twitter***, rename the image file to give it a ***PowerShell*** extension, then execute the script.

To start the script using ***Linux***, just enter the following command within a terminal. There is no need to change the file extension with ***Linux***. 

````bash
$ pwsh jpws_img.jpg
````
To download and run an image from ***Twitter*** using ***Linux***, get the address of the image from ***Twitter*** as described above for the Windows example.
Once you have the image address, type the following command in the terminal with the pasted-in image address, which again, needs to be within quotation marks:

```bash
 wget "https://pbs.twimg.com/media/F6LKHaqXkAAbJ6n.jpg";pwsh F6*
```
The *wget* command downloads the image from ***Twitter***, then the *pwsh* command followed by the name of the downloaded image runs the embedded ***PowerShell*** script.

My other programs you may find useful:-  

* [pdvzip: CLI tool to embed a ZIP file within a tweetable and "executable" PNG-ZIP polyglot image.](https://github.com/CleasbyCode/pdvzip)
* [jdvrif: CLI tool to encrypt & embed any file type within a JPG image.](https://github.com/CleasbyCode/jdvrif)
* [imgprmt: CLI tool to embed an image prompt (e.g. "Midjourney") within a tweetable JPG-HTML polyglot image.](https://github.com/CleasbyCode/imgprmt)
* [pdvrdt: CLI tool to encrypt, compress & embed any file type within a PNG image.](https://github.com/CleasbyCode/pdvrdt)
* [xif: CLI tool to embed small files (e.g. "workflow.json") within a tweetable JPG-ZIP polyglot image.](https://github.com/CleasbyCode/xif) 
* [pdvps: PowerShell / C++ CLI tool to encrypt & embed any file type within a tweetable & "executable" PNG image](https://github.com/CleasbyCode/pdvps)

##

