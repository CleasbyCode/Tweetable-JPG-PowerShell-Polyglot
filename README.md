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

In a nutshell, ***PowerShell comment blocks***. An open-comment block is required as near to the start of the image file as possible. This is achieved by writing the open-comment block string within the JFIF header segment (***FFE0***), which is conveniently preserved by X/Twitter. The PowerShell script is stored at the end of the color profile data of the JPG image, which is also preserved by X/Twitter. We need the first PowerShell open-comment block within the JFIF header to ignore the ICC profile segment header (***FFE2***) and the color profile data. We then have a close-comment block at the end of the color profile data, followed by our PowerShell script, which now gets interpreted. At the end of the PowerShell script (*still within the color profile segment*) we need another open-comment block so that PowerShell ignores the remaining contents of the image file. Finally, we need a close-comment block as near to the end of the image file as possible. Unfortunately, things are never as straightforward as we would like them to be. The title for this section should probably of been "***How It Sometimes Works***".  This is mainly down to the short length of the comment-block string, image compatibility and X/Twitter image encoding.

## Image Compatibilty, Issues and Limitations.

To start with, always use images found on X/Twitter for your cover image. If you want to use an image not originally from that platform, post your image to X/Twitter, then save it. This ensures the JPG image will be encoded by the method used by X/Twitter. Once encoded, the whole image will be *preserved by X/Twitter when posted again on its platform, which is useful for our requirements. **X/Twitter will re-encode part of the image if bytes of the image data are modified. More on that later*. 


When saving an image from ***Twitter***, make sure to click on the image first, so as to fully expand it, then save it, so that you get the full image
dimension size. You will probably have to download and try a few images before a compatible one is found. The incompatiblity issue is caused 
by the presence within the ***JPG*** image file of the *end-comment block string* ***"#>"***, which breaks the script when we execute it from the image.  

Within the ***Images*** folder of this repo, I have included a few compatible images for you to use. 

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

