# jpws

A simple CLI tool to embed a ***PowerShell*** script within a ***JPG*** image, creating a tweetable ***JPG-PowerShell*** polyglot file.  

![Demo Image](https://github.com/CleasbyCode/jpws/blob/main/demo_image/github_demo_pwsh_fibo.jpg) 
***{Image Credit: [@McC3Kat](https://x.com/McC3Kat)} X/Twitter compatible image with embedded PowerShell script (Fibonacci sequence)***  
  
## Usage (***Linux***)

```console

user1@linuxbox:~/Downloads/jpws-main/src/jpws$ g++ main.cpp -O2 -s -o jpws
user1@linuxbox:~/Downloads/jpws-main/src/jpws$ sudo cp jpws /usr/bin

user1@linuxbox:~/Desktop$ jpws

Usage: jpws [-alt] <cover_image> <powershell_script>  
       jpws --info

user1@linuxbox:~/Desktop$ jpws cover_image23.jpg fibo.ps1

Saved "PowerShell-embedded" JPG image: jpws_10247.jpg (321081 bytes).

Complete!
```

https://github.com/user-attachments/assets/b9e1888a-10c9-4933-aa7a-cae40219b87d

## How It Works

***PowerShell comment blocks!***

An open-comment block is required as near to the start of the image file as possible.  

This is achieved by writing the string "*\n<#\r\n*" (*0x0D, ***0x3C, 0x23***, 0x0D, 0x0A*), containing the two byte open-comment block characters, within the ***JFIF*** header segment ***FFE0***.  

These five bytes are conveniently preserved by ***X/Twitter***.  

![JFIF Image](https://github.com/CleasbyCode/jpws/blob/main/demo_image/jfif_block.png) 

***When downloading images from X/Twitter, always click the image in the post to FULLY EXPAND it before saving. This ensures you get the original size image with all the embedded data.***

The ***PowerShell*** script is stored at the end of the color profile data of the ***JPG*** image, which is also preserved. We use the first ***PowerShell*** open-comment block within the ***JFIF*** header to ignore the ***ICC*** profile segment header ***FFE2*** along with the color profile data.  

We then have a close-comment block at the end of the color profile data, followed by the ***PowerShell*** script, which now gets interpreted. At the end of the script (*still within the color profile segment*) we use another open-comment block so that ***PowerShell*** ignores the remaining contents of the image file.  

Finally, for comment-block compliance, we need a close-comment block as near to the end of the image file as possible. 

Of course, things are never as straightforward as we would like them to be. The title for this section should probably of been "***How It Sometimes Works***".  

## Image Compatibilty, Issues and Limitations.

Always use a ***progressive*** encoded ***JPG*** for your cover image. These images are identifed by the segment marker "***FFC2***" (*Start of Frame 2/Progressive DCT*).  ***X/Twitter*** uses this method for encoding ***JPG*** images posted on its platform.

This program (*using the libjpeg-turbo library*) will re-encode your image if the format is not progressive, although image quality may suffer. It's recommended that you source your cover image directly from ***X/Twitter*** or use the ones supplied within this GitHub repo.

A progressive encoded image posted on ***X/Twitter***, (*within file and dimension size limits*), will not be re-encoded. What you post will be the same as what you download, which is useful for our requirements.  

If ***X/Twitter*** re-encoded an image each time it was posted, ***jpws*** would never work.   **Note: ***X/Twitter*** will re-encode sections of the image if bytes of the compressed image data are modified. More on that later*. 

The cover image must not contain any occurrence of the ***PowerShell*** close-comment block string "***#>***" (*0x23, 0x3E*), apart from the ones inserted by the program, as this will break the ***PowerShell*** script.  

Unfortunatly, with the close-comment block string length being only two bytes, the probablilty that this character sequence will appear somewhere within the cover image is quite high. The larger the image, the greater the probablity of multiple comment-block character sequences. ***jpws*** has a maximum size limit of ***512KB*** for the cover image. 

If detected within the cover image, ***jpws*** (*using libjpeg-turbo & stb_image*) will attempt to eliminate the close-comment block strings by slightly decreasing image dimensions. This will cause the image to be re-encoded, potentially removing those unwanted character sequences.  

The image will be checked again for the presence of that string, repeating the procedure of decreasing image dimensions if required. There is a maximum of five decrease attempts before ***jpws*** gives up and requests you try a different image.

For the final close-comment block, we overwrite the last thirteen bytes of the image with a default string "*0x00, 0x00, 0x20, 0x20, 0x00, 0x00, ***0x23, 0x3E, 0x0D, 0x23***, 0x9e, 0xFF, 0xD9*".  

To have any chance of getting this to work, we have no choice but to overwrite bytes (using the above string) within a section of the image file that is compressed & encoded. This triggers ***X/Twitter*** to potentially re-encode these bytes.  

The first six bytes of the string can help with the encoding and are expendable, so it does not matter if they are changed or removed, but the following four bytes "***0x23, 0x3E, 0x0D, 0x23***" are crucial and need to be preserved by ***X/Twitter*** for the ***PowerShell*** script to work after tweeting the image. For some images, these four bytes are preserved by ***X/Twitter***, but are changed or removed for other image, which will causes the embedded ***PowerShell*** script to fail.

We can only find out which images work after tweeting them.  

If an image fails to preserve the crucial four bytes, you can retry ***jpws*** using the ***-alt*** option:  

```console
user1@linuxbox:~/Desktop$ jpws -alt cover_image23.jpg fibo.ps1
```

With this option selected, ***jpws*** will use a slightly different thirteen byte string, that usually works for images that have failed with the default string. Another potential fix is to manually decrease image dimensions using an editor such as GIMP.

The first image below shows the default thirteen byte close-comment block string from a ***JPG-PowerShell*** polyglot image ***before*** it has been tweeted.  

![BEFORE Image](https://github.com/CleasbyCode/jpws/blob/main/demo_image/before_tweet.png) 

This second image shows the same section of the file, this time ***after*** being tweeted. You can see that the string with the four crucial bytes has been preserved. You can also see (within the orange border) some extra bytes that have been generated by ***X/Twitter*** re-encoding.  

![BEFORE Image](https://github.com/CleasbyCode/jpws/blob/main/demo_image/after_tweet.png) 

This repo contains a number of ready to use compatible images, should you want to save time in finding a working cover image. 

A compatible image is a ***JPG*** that does not contain any occurrence of the close-comment block string (***#>***) and preserves the four crucial bytes (*0x23, 0x3E, 0x0D, 0x23*) near the end of the file, after the ***JPG-PowerShell*** polyglot image has been tweeted.

As mentioned earlier, when downloading an image from ***X/Twitter***, make sure to first click the image in the post to fully expand it, then save it, so that you get the full original size image. 

X/Twitter allows a maximum size of ***10KB*** for the color profile data, the segment in which we are storing the ***PowerShell*** script. With the overhead of the profile data, you will have about ***9.5KB*** available for your ***PowerShell*** script.

***PowerShell*** scripts that require the user to provide command-line arguments are currently not supported when embedded within the ***JPG*** image. This may change once I figure out how to do this.

## Executing The Embedded PowerShell Script

The easiest way, imo, to download the image from ***X/Twitter*** and run the embedded ***PowerShell*** script, is to use the ***wget*** command for *Linux and the ***iwr*** command for Windows.
**Make sure ***PowerShell*** is installed on your Linux PC.*  

You will first need to get the image link address from ***X/Twitter***, once you have posted the embedded image.

Click the image in the post to fully expand it, then right-click on the image and select "*Copy image address*" from the menu.

For ***wget***, use the following command (enclose the image link address within quotation marks):
```console
 wget "https://pbs.twimg.com/media/Gg2JRntX0AAB1A0?format=jpg&name=large";pwsh Gg2JR*
```
The ***wget*** command downloads the image from ***X/Twitter***, then the *pwsh* command followed by the name of the downloaded image runs the embedded ***PowerShell*** script.

For ***iwr***, use the following command:
```console
iwr -o fibo.ps1 "https://pbs.twimg.com/media/Gg2JRntX0AAB1A0?format=jpg&name=large";.\fibo.ps1
```
The ***iwr*** command will download the image from ***X/Twitter***, renames the image file, which includes the ***PowerShell*** extension, then executes the script in the image.

Alternatively, you can just manually download the image from ***X/Twitter*** (remember to click on the image within the post to fully expand it before saving).

To run the script embedded with the image using Linux, just enter the following command within a terminal. There is no need to change the file extension with Linux. 

```console
$ pwsh jpws_85681.jpg
```
For Windows, after downloading the image from ***X/Twitter***, you will need to rename the .jpg extension to .ps1. Also, depending on the Windows/PowerShell execution policy,
you will probably need to unblock the file before you can run the embedded script.

```console
G:\demo> ren jpws_85681.jpg jpws_85681.ps1
G:\demo> Unblock-File jpws_85681.ps1 (Only required for downloaded images or images taken from another machine).
G:\demo> .\jpws_85681.ps1
```
## Third-Party Libraries

This project makes use of the following third-party libraries:  

stb_image by Sean Barrett (“nothings”)  

libjpeg-turbo (see [***LICENSE***](https://github.com/libjpeg-turbo/libjpeg-turbo/blob/main/LICENSE.md) file)  
{This software is based in part on the work of the Independent JPEG Group.}
##

