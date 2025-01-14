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

This is achieved by writing the following five byte string "\n<#\r\n" (0x0D, 0x3C, 0x23, 0x0D, 0x0A), containing the two byte open-comment block characters, within the JFIF header segment ***FFE0***.  

These bytes within the image are conveniently preserved by X/Twitter.  

***Important: When saving images from X/Twitter, always click the image in the post to FULLY EXPAND it before saving. This ensures you get the original size image, with all the embedded data.***

The PowerShell script is stored at the end of the color profile data of the JPG image, which is also preserved by X/Twitter. We use the first PowerShell open-comment block within the JFIF header to ignore the ICC profile segment header ***FFE2*** along with the color profile data.  

We then have a close-comment block at the end of the color profile data, followed by our PowerShell script, which now gets interpreted. At the end of the PowerShell script (*still within the color profile segment*) we use another open-comment block so that PowerShell ignores the remaining contents of the image file.  

Finally, for comment-block compliance, we need a close-comment block as near to the end of the image file as possible. 

Of course, things are never as straightforward as we would like them to be. The title for this section should probably of been "***How It Sometimes Works***".  

## Image Compatibilty, Issues and Limitations.

Always use a Progressive encoded JPG for your cover image. Progressive encoded JPG images are identifed by the segment marker "FFC2" (*Start of Frame 2/Progressive DCT*).  X/Twitter uses this method for encoding JPG images posted on its platform.  There are plenty of images on X/Twitter, which you could use for the cover image.

This program (*using the libjpeg-turbo library*) will re-encode your image if the format is not Progressive.

A Progressive encoded image posted on X/Twitter, *within file and dimension size limits*, will not be re-encoded. What you post will be the same as what you download, which is useful for our requirements.  

If X/Twitter re-encoded an image each time it was posted, ***jpws*** would never work.   **Note: X/Twitter will re-encode sections of the image if bytes of the image data are modified. More on that later*. 

The cover image must not contain any occurrence of the PowerShell close-comment block string "#>" (0x23, 0x3E), apart from the ones inserted by the program, as this will break the PowerShell script.  

Unfortunatly, with the comment-block string length being only two bytes, the probablilty that this character sequence will appear somewhere within the image data is quite high. The larger the image, the greater the probablity of multiple hits. I would not even consider trying images larger than 400KB. 

It's possible to edit out close-comment block strings within the image data by slightly decreasing the image dimensions using an editor such as GIMP. Repeat, if necessary.  

I may try to automate this process in later versions of jpws.

For the final close-comment block, we overwrite the last thirteen bytes of image data with a default string "0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x23, 0x3E, 0x0D, 0x23, 0x9e, 0xFF, 0xD9".  

To have any chance of getting this to work, we have no choice but to modifying bytes within a section of the image file that is compressed & encoded. This triggers X/Twitter to re-encode some (or all) of these bytes.  

The first six bytes of the above string can help with the encoding and are also expendable, so it does not matter if they are changed or removed, but the following four bytes "0x23, 0x3E, 0x0D, 0x23" are crucial and need to be preserved by X/Twitter for the PowerShell script to work after tweeting the image. For some images, these four bytes are retained by X/Twitter, but are removed or changed for others, making the image incompatible for this program.  

We can only find out which images work after tweeting them. Again, slightly decreasing image dimensions may help with compatiblity/encoding.

This repo contains a number of ready to use compatible images, should you find it difficult to find your own compatible cover image.  

A compatible image is an JPG that does not contain any occurrence of the close-comment block string (#>) and preserves the four crucial bytes (0x23, 0x3E, 0x0D, 0x23) near the end of the file, after the JPG-PowerShell polyglot image has been tweeted.

When saving an image from X/Twitter, make sure to first click the image in the post to fully expand it, then save it, so that you get the full, original image dimension size. 

X/Twitter allows a maximum size of 10KB for the color profile data, the segment in which we are storing the PowerShell script. With the overhead of the profile data, you will have about 9.5KB available for your PowerShell script.

PowerShell scripts that require the user to provide command-line arguments are currently not supported when embedded within the JPG image. This may change once I figure out how to do this.

## Executing The Embedded PowerShell Script

The easiest way, imo, to download the image from X/Twitter and run the embedded PowerShell script, is to use the ***wget*** command for *Linux and the ***iwr*** command for Windows.
**Make sure PowerShell is installed on your Linux PC.*  

You will first need to get the image link address from X/Twitter, once you have posted the embedded image.

Click the image in the post to fully expand it, then right-click on the image and select "*Copy image address*" from the menu.

For ***wget***, use the following command (enclose the image link address within quotation marks):
```console
 wget "https://pbs.twimg.com/media/Gg2JRntX0AAB1A0?format=jpg&name=large";pwsh Gg2JR*
```
The ***wget*** command downloads the image from X/Twitter, then the *pwsh* command followed by the name of the downloaded image runs the embedded PowerShell script.

For ***iwr***, use the following command:
```console
iwr -o fibo.ps1 "https://pbs.twimg.com/media/Gg2JRntX0AAB1A0?format=jpg&name=large";.\fibo.ps1
```
The ***iwr*** command will download the image from X/Twitter, renames the image file, which includes the PowerShell extension, then executes the script in the image.

https://github.com/user-attachments/assets/97e472d3-b425-4838-bd1c-11e4ce7c5d8e

Alternatively, you can just manually download the image from X/Twitter (remember to click on the image within the post to fully expand it before saving).

To run the script embedded with the image using Linux, just enter the following command within a terminal. There is no need to change the file extension with Linux. 

```console
$ pwsh jpws_85681.jpg
```
For Windows, after downloading the image from X/Twitter, you will need to rename the .jpg extension to .ps1. Also, depending on the Windows/PowerShell execution policy,
you will probably need to unblock the file before you can run the embedded script.

```console
G:\demo> ren jpws_85681.jpg jpws_85681.ps1
G:\demo> Unblock-File jpws_85681.ps1 (Only required for downloaded images or images taken from another machine).
G:\demo> .\jpws_85681.ps1
```
https://github.com/user-attachments/assets/c5c242e9-b073-47c8-9027-ed88b7f0b5c3

##

