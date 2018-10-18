# Leon 3 with matrix multiplication
## Introduction
The Leon 3 soft core processor is developed by ESA and is maintained in the [GRLIB](https://www.gaisler.com/index.php/downloads/leongrlib) library from the Cobham Gaisler AB. The GRLIB library contains a flexible makefile implementation to rapidly create custom ASICs by selecting modules within the library. Some of these components are IP blocks and are therefore only free-of-charge whenever it is not used commercially. 

The goal of this project is to implement a custom 13x13 matrix mulitplication block in the Leon 3. The module should increase the performance of matrix multiplications. For now, there are a few possible implementations:
* Add the module to the AMBA AHP or ABP bus. Whilst the first is faster, the is easier to implement;
* Create a UART interface to the module (considered very slow);

In theory, such a module could do the whole matrix multiplication in one clockcyle. This would, however, require a lot of area and power due to all the adders and multipliers that are required. It might therefore be a good idea to this in a more distributed way, where all the resources are shared. The downside would be that there are more clockcyles needed to calculate the resulting matrix.

## Applications
As this project is carried out as an assignment on the University of Twente, we could only use freeware to synthesize and simulate the design. All builds have been tested on the following system:

```
Distributor ID:	Ubuntu
Description:	Ubuntu 18.04.1 LTS
Release:	18.04
Codename:	bionic
```

The following applications are required:
* QuartusII Lite (Web Edition) (18.1.0) to synthesize and place&route;
* ModelSIM(-Altera) (10.5b) to simulate the design;
* Bare Code Compiler to compile the software;
* GNU Make (4.1) to run the Makefiles;

## Installation
1. Download the quartus installer [here](http://download.altera.com/akdlm/software/acdsinst/18.1std/625/ib_tar/Quartus-lite-18.1.0.625-linux.tar). 
2. Extract with ```tar xfv Quartus-lite-18.1.0.625-linux.tar```
3. Run the setup with ```sh setup.sh```
4. Click through the installer, install the Cyclone IV and Cyclone V packages and make sure to install ModelSIM-Altera (the free version of ModelSIM).
5. Add everything to your path

6. Install 32 bit libraries ```sudo apt install gcc-multilib```
7. Install some more libraries
```
sudo apt-get install gcc-multilib g++-multilib \
lib32z1 lib32stdc++6 lib32gcc1 \
expat:i386 fontconfig:i386 libfreetype6:i386 libexpat1:i386 libc6:i386 libgtk-3-0:i386 \
libcanberra0:i386 libpng12-0:i386 libice6:i386 libsm6:i386 libncurses5:i386 zlib1g:i386 \
libx11-6:i386 libxau6:i386 libxdmcp6:i386 libxext6:i386 libxft2:i386 libxrender1:i386 \
libxt6:i386 libxtst6:i386
```
8. Download the freetype library [here](http://download.savannah.gnu.org/releases/freetype/freetype-2.4.12.tar.bz2)
9. Unpack, build and copy to the appropriate directory.
10. 
```
cd freetype-2.4.12
./configure --build=i686-pc-linux-gnu "CFLAGS=-m32" "CXXFLAGS=-m32" "LDFLAGS=-m32"
make -j8
mkdir modelsim_ase/lib32
sudo cp freetype-2.4.12/objs/.libs/libfreetype.so* modelsim_ase/lib32
```
7. Edit your binary vsim launch file

change mode=${MTI_VCO_MODE:-""} to mode=${MTI_VCO_MODE:-"32"}
add export LD_LIBRARY_PATH=${dir}/lib32 below dir=`dirname "$arg0"`
change vco="linux_rh60" to vco="linux"
11. Restart your terminal by closing it and opening it again
12. You should now be able to launch quartus by typing ```quartus``` and ModelSIM can be launched with ```vsim```

## Adding Shortcuts to your Launcher


https://gist.github.com/PrieureDeSion/e2c0945cc78006b00d4206846bdb7657
https://github.com/PrieureDeSion/Randoms/blob/master/VHDL/Getting%20Started.pdf

## Simulation
Follow the instructions section 3 in grlib.pdf.

## Potential Errors
If the following error occurs:
```
** Error: (vcom-11) Could not find altera_mf.altsyncram.
```
do in the source directory:
```
make distclean
```
###### Reference
http://permalink.gmane.org/gmane.comp.hardware.opencores.leon-sparc/19561
