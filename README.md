# VoiceChanger_TIDSP
This project uses TI DSP board(BOOST5545ULP) to realize the real-time voice changing system based on the principle of fractional frequency conversion
## Preconditon
+ Please make sure you've went through **[C5545 DSP BoosterPack Software User’s Guide](https://www.ti.com.cn/cn/lit/pdf/sprui92)**,and you can build or debug demos **[C5545 DSP BoosterPack Software Installer 01.01.00.00](https://www.ti.com.cn/cn/lit/zip/sprcae7)**.  
![linkedfolder](/Images/link_folders.png) ![linkfolders](Images/link.jpg)  
+ Download this project and import it to ccs.Then create two new subfolders(`audio_common` and `common`) and link them to counterparts in one of the demos.
## How to Run?
+ Just like running other projects in ccs.
  + *Build*  
  + *Launch Target Configuration*(Create a new C5545 Target configuration file if you don't have one)  
  + *Connect Target*(Make sure that the board and PC are normally connected through the debug port and powered on) 
  + *Load Programs*  
  + *Resume*.
  Or you could just simply hit the *Debug* button,you should 
