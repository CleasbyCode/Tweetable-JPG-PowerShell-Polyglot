void displayInfo() {
	std::cout << R"(

JPG-PowerShell Polyglot for X/Twitter (jpws v1.0) Created by Nicholas Cleasby (@CleasbyCode) 12/12/2024 

CLI tool for embedding a PowerShell script within a tweetable JPG image file.

Compile & run jpws (Linux):
		
user1@linuxbox:~/Downloads/Tweetable-JPG-PowerShell-Polyglot-main/src$ sudo apt-get install libturbojpeg-dev
user1@linuxbox:~/Downloads/Tweetable-JPG-PowerShell-Polyglot-main/src$ g++ main.cpp -O2 -lturbojpeg -s -o jpws
user1@linuxbox:~/Downloads/Tweetable-JPG-PowerShell-Polyglot-main/src$ sudo cp jpws /usr/bin

$ jpws
		
Usage: jpws [-alt] <cover_image> <powershell_file>
       jpws --info
		
Share your "PowerShell-embedded" JPG image on X/Twitter.

Max PowerShell script size is 10KB. 
Max cover image size is 512KB.

https://github.com/CleasbyCode/Tweetable-JPG-PowerShell-Polyglot

)";
}
