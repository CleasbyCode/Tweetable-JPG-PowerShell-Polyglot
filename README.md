# jpws

Embed a ***PowerShell*** script within a ***JPG*** image, creating a tweetable ***JPG-PowerShell*** polyglot file.  

![Demo Image](https://github.com/CleasbyCode/jpws/blob/main/demo_image/jpws_73210.jpg)  
***{Image Credit: [@awesome_visuals](https://x.com/awesome_visuals)} GameOfLife.ps1 embedded within this tweetable JPG image.***  
  
## Usage (***Linux***)

```console

user1@linuxbox:~/Downloads/src$ sudo apt-get install libturbojpeg-dev
user1@linuxbox:~/Downloads/src$ g++ main.cpp -O2 -lturbojpeg -s -o jpws
user1@linuxbox:~/Downloads/src$ sudo cp jpws /usr/bin

user1@linuxbox:~/Desktop$ jpws

Usage: jpws [-alt] <cover_image> <powershell_script>  
       jpws --info

user1@linuxbox:~/Desktop$ jpws cover_image38.jpg GameOfLife.ps1

Saved "PowerShell-embedded" JPG image: jpws_10247.jpg (121098 bytes).

Complete!
```
https://github.com/user-attachments/assets/2e49ea69-7e33-4b43-bcba-cb0a9678a4f7

## How It Works

***PowerShell comment blocks!***

An open-comment block is required as near to the start of the image file as possible.  

This is achieved by writing the string "*\n<#\r\n*" (*0x0D, ***0x3C, 0x23***, 0x0D, 0x0A*), containing the two byte open-comment block characters, within the ***JFIF*** header segment ***FFE0***.  

These five bytes are conveniently preserved by ***X/Twitter***.  

![JFIF Image](https://github.com/CleasbyCode/jpws/blob/main/demo_image/jfif_block.png) 

***When downloading images from X/Twitter, always click the image in the post to FULLY EXPAND it before saving. This ensures you get the original size image with all the embedded data.***

The ***PowerShell*** script is stored at the end of the colour profile data of the ***JPG*** image, which is also preserved. We use the first ***PowerShell*** open-comment block within the ***JFIF*** header to ignore the ***ICC*** profile segment header ***FFE2*** along with the colour profile data.  

We then have a close-comment block at the end of the colour profile data, followed by the ***PowerShell*** script, which now gets interpreted. At the end of the script (*still within the colour profile segment*) we use another open-comment block so that ***PowerShell*** ignores the remaining contents of the image file.  

Finally, for comment-block compliance, we need a close-comment block as near to the end of the image file as possible. 

Of course, things are never as straightforward as we would like them to be. The title for this section should probably of been "***How It Sometimes Works***".  

## Image Compatibility, Issues and Limitations.

This program (*using the libjpeg-turbo library*) will first re-encode your image as a ***progressive*** encoded ***JPG***.  

These images are identified by the segment marker "***FFC2***" (*Start of Frame 2/Progressive DCT*).   ***X/Twitter*** uses this method for encoding ***JPG*** images posted on its platform, if required.

A progressive encoded image posted on ***X/Twitter***, (*within file and dimension size limits*), will *not be re-encoded.  

What you post will be the same as what you download, which is useful for our requirements.  

If ***X/Twitter*** re-encoded an image each time it was posted, ***jpws*** would never work.   **Note: ***X/Twitter*** will re-encode/repair sections of the image if bytes of the compressed image data are corrupted/modified. More on that later*. 

The cover image ***must not*** contain any occurrence of the ***PowerShell*** close-comment block string "***#>***" (*0x23, 0x3E*), apart from the ones inserted by this program, as this will break the ***PowerShell*** script.  

Unfortunately, with the close-comment block string length being only two bytes, the probability that this character sequence will appear somewhere within the cover image is obviously quite high. The larger the image, the greater the probability of multiple comment-block character sequences. ***jpws*** has a maximum size limit of ***1MB*** for your cover image. 

It is often possible to remove these comment-block character sequences by using the following procedure.  

After initially re-encoding the cover image as a progressive JPG with a maximum quality setting, ***jpws*** will search the image file for the two-byte comment-block sequence. If found, the image is re-encoded (*using libjpeg-turbo & stb_image*) with the width & height dimension size being reduced by one pixel, along with the quality value being decremented.  

The modified image is searched again for the two-byte sequence and the process is repeated, incrementing the dimension size and quality reduction values with each cycle, until no comment-block sequences are found. There is a default of thirty decrease attempts before ***jpws*** gives up and requests you either try a different image or manually reduce the current image's dimensions (scale) using an editor, such as GIMP. 

For the final close-comment block, we overwrite the last eleven bytes of image data with a default string (*0x00, 0x00, 0x20, 0x20, 0x00, 0x00, ***0x23, 0x3E, 0x0D, 0x23***, 0x9e*).  

To have any chance of getting this to work, we have no choice but to overwrite bytes (using the above string) within a section of the image that is compressed/encoded. While this technically corrupts the image, most show no visible signs of distortion (miniscule for those that do) and no program refuses to display the image. Twitter allows it to be posted without complaint.  

The program ***ImageMagick*** does inform us about this corruption when we use the ***identify -verbose*** option. "*Corrupt JPEG data: premature end of data segment*".

The change of bytes triggers ***X/Twitter*** to re-encode/repair this small section of data (see images below).  ***ImageMagick*** no longer reports corruption when we check one of these downloaded images.

The first six bytes of the string can help with the encoding and are expendable, as it does not matter if they are changed or removed, but the following four bytes (***0x23, 0x3E, 0x0D, 0x23***) are crucial and need to be preserved by ***X/Twitter*** for the ***PowerShell*** script to work after tweeting the image.  

For many images, these four bytes (even the whole string) are preserved by ***X/Twitter***, but occasionally they are partially or completely removed for some images, which will cause the embedded ***PowerShell*** script to fail.

We can only find out which images initially work after tweeting them.  It is recommended that you test tweet the output image to make sure the crucial bytes are preserved before intentionally sharing the image with others. 

If an image fails to correctly preserve the crucial four bytes, you can retry ***jpws*** using the ***-alt*** option:  

```console
user1@linuxbox:~/Desktop$ jpws -alt cover_image23.jpg fibo.ps1
```

With this option selected, ***jpws*** will use a slightly different eleven byte string, that often works for images that have failed with the default string. Manually reducing the image dimensions (scale) using GIMP is another option to get this working. 

The first image below shows the default eleven byte close-comment block string from a ***JPG-PowerShell*** polyglot image ***before*** it has been tweeted.  

![BEFORE Image](https://github.com/CleasbyCode/jpws/blob/main/demo_image/before_tweet.png) 

The second image shows the same section of the file, this time ***after*** being tweeted. You can see that the string with the four crucial bytes has been preserved. You can also see (within the orange border) some extra bytes that have been generated by the ***X/Twitter*** re-encoding/repair process.  

![BEFORE Image](https://github.com/CleasbyCode/jpws/blob/main/demo_image/after_tweet.png) 

This repo contains a number of ready to use compatible images, should you want to save time in finding a working cover image. 

A compatible image is a ***JPG*** that does not contain any occurrence of the close-comment block string (***#>***) and preserves the four crucial bytes (***0x23, 0x3E, 0x0D, 0x23***) near the end of the file, after the ***JPG-PowerShell*** polyglot image has been tweeted.

As mentioned earlier, when downloading an image from ***X/Twitter***, make sure to first click the image in the post to fully expand it, then save it, so that you get the full original size image. 

***X/Twitter*** has a maximum size limit of ***10KB*** for the colour profile data segment (***0xFF 0xE2 0x28 0x00***), in which we are storing the ***PowerShell*** script. With the overhead of the profile data, you will have about ***9.5KB*** available for your ***PowerShell*** script.

***PowerShell*** scripts that use a top "script-level" ***param(...) block*** will ***not work*** when embedded within an image. The param(...) block enforces strict parsing at the start of the script. The only things allowed before the param(...) block are comments or blank lines (and sometimes a #requires statement). But having certain binary bytes before param(...) will break parsing. A param(...) block inside a function, rather than at the top of the script, should work fine. 

## Executing The Embedded PowerShell Script

The easiest way to download the image from ***X/Twitter*** and run the embedded ***PowerShell*** script, is to use the ***wget*** command for *Linux and the ***iwr*** command for Windows.
**Make sure ***PowerShell*** is installed on your Linux PC.*  

You will first need to get the image link address from ***X/Twitter***, after you have posted the embedded image.

Click the image in the post to fully expand it, then right-click on the image and select "*Copy image address*" from the menu.

For ***wget***, use the following command (enclose the image link address within quotation marks):
```console
wget "https://pbs.twimg.com/media/GhZTR8BXgAACc9Q?format=jpg&name=medium";pwsh GhZ*
```
The ***wget*** command downloads the image from ***X/Twitter***, then the *pwsh* command followed by the name of the downloaded image runs the embedded ***PowerShell*** script.

For ***iwr***, use the following command:
```console
iwr -o Game.ps1 "https://pbs.twimg.com/media/GhZTR8BXgAACc9Q?format=jpg&name=medium";.\Game.ps1
```
The ***iwr*** command will download the image from ***X/Twitter***, renames the image file, which includes the ***PowerShell*** extension, then executes the script in the image.

Alternatively, you can just manually download the image from ***X/Twitter*** (remember to click on the image within the post to fully expand it before saving).

To run the script embedded within the image using Linux, just enter the following command within a terminal. There is no need to change the file extension with Linux. 

```console
$ pwsh jpws_85681.jpg
```
For Windows, after downloading the image from ***X/Twitter***, you will need to rename the .jpg file extension to .ps1, also, depending on the Windows/PowerShell execution policy,
you will probably need to unblock the file before you can run the embedded script. You can see why the ***iwr*** option is the most convenient. 

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

