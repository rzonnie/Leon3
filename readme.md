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
Codename:	bionic beaver
```

The following applications are required:
* QuartusII Lite (Web Edition) (18.1.0) to synthesize and place&route;
* ModelSIM(-Altera) (10.5b) to simulate the design;
* Bare Code Compiler to compile the software;
* GNU Make (4.1) to run the Makefiles;

## Installation
First we deal with the installation of Quartus and ModelSIM, afterwards, one should download the appropriate grlib library and set the paths correctly.
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
11. Open ```.bashrc``` to add the binaries to your path:
```
nano ~/.bashrc
```
12. Paste the following at the bottom, where you replace the ALTERAPATH to your quartus installation directory. Restart the terminal when you have saved the file.
```
# Quartus
export ALTERAPATH="/path/to/quartus/"
export ALTERAOCLSDKROOT="${ALTERAPATH}/hld"
export QUARTUS_ROOTDIR=${ALTERAPATH}/quartus
export QUARTUS_ROOTDIR_OVERRIDE="$QUARTUS_ROOTDIR"
export PATH=$PATH:${ALTERAPATH}/quartus/bin
export PATH=$PATH:${ALTERAPATH}/nios2eds/bin
export PATH=$PATH:${QSYS_ROOTDIR}
export QUARTUS_64BIT=1
export QSYS_ROOTDIR="QUARTUS_ROOTDIR/sopc_builder/bin"

# ModelSIM
export PATH=$PATH:${ALTERAPATH}/modelsim_ase/bin
# BCC
export PATH=/opt/sparc-elf/bin:$PATH
```
13. Re-open your terminal and edit your binary vsim launch file (if you are unsure where this is located type ```which vsim```)

change ```mode=${MTI_VCO_MODE:-""}``` to ```mode=${MTI_VCO_MODE:-"32"}```
add ```export LD_LIBRARY_PATH=${dir}/lib32``` below ```dir=`dirname "$arg0"` ```
change ```vco="linux_rh60"``` to ```vco="linux"```

14. You should now be able to launch quartus by typing ```quartus``` and ModelSIM can be launched with ```vsim```

For simulations it is very handy to add the grlib library to your path as well:
1. Download grlib [here](https://www.gaisler.com/products/grlib/grlib-gpl-2018.3-b4226.tar.gz)
2. ```nano ~/.bashrc```
3. Paste the following at the bottom of that file, replace the path to your installation of the grlib library.
```
export GRLIB=/path/to/grlib
```
# Installing Bare C compiler (BCC) for soft code compilation
Download the zipped file from [gaisler](https://www.gaisler.com/anonftp/bcc/bin/linux/).
copy the file to /opt/
execute ```sudo tar xjf sparc-elf-GCCVERSION-RELEASE_VERSION.tar.gz```

## Adding Shortcuts to your Launcher
1. Create two new files in ```/usr/share/applications/```
2. ```touch /usr/share/applications/modelsim.desktop /usr/share/applications/quartus.desktop```
3. Replace in the snippet below the paths for Exec to the ```bin/``` directory of modelsim, then paste it in modelsim.desktop:
```
[Desktop Entry]
Type=Application
Version=10.5b
Name=ModelSIM Altera (Free Edition 32-bit)
Comment=ModelSIM Altera 10.5b (32-bit)
Icon=/home/remi/Pictures/intel.png
Exec=/data/Programs/Quartus/modelsim_ase/bin/vsim
Terminal=false
Path=/data/Programs/Quartus
```
4. Replace in the snippet below the paths for Exec to the ```bin/``` directory of quartus, then paste it in quartus.desktop
```
[Desktop Entry]
Type=Application
Version=0.9.4
Name=Quartus II 13.0sp1 (64-bit) Web Edition
Comment=Quartus II 13.0sp1 (64-bit)
Icon=/data/Programs/Quartus/quartus/adm/quartusii.png
Exec=/data/Programs/Quartus/quartus/bin/quartus --64bit
Terminal=false
Path=/data/Programs/Quartus
```
5. When opening the launcher, one can now look for both applications and launch them.
6. (optional) You can add an icon to the ModelSIM launcher, simply download an image to a location and specify Icon.

## Simulation
Follow the instructions section 3 in grlib.pdf. It basically boils down to the following (assuming you have correctly cloned the git repository):
1. First ensure the Altera libraries have been correctly merged with grlib: ```make install-altera```
2. Instantiate a new ModelSIM project: ```make vsim```
3. Then you can either launch the project with ```make vsim-launch``` or you can directly run a simulation with ```make vsim-run```

## Synthesizing the design
It is assumed you have correctly cloned the repository and you have changed your directory to the root of the repository.
1. Ensure the Altera libraries have been correctly merged with grlib: ```make install-altera```
2. Instantiate a new Quartus Prime project (.qpf): ```make quartus```
3. Open Quartus by hand or type ```make quartus-launch```

## Committing files
As we don't want to make a mess out of this repository, please run ```make distclean``` before running ```git add -a```. Sorry for the inconvenience, we are looking for another solution.

## Potential Errors
If the following error occurs:
```
** Error: (vcom-11) Could not find altera_mf.altsyncram.
```
do in the source directory:
```
make distclean
```

If the font in the modelsim text editor is too small to read:
    Close Modelsim
    Open ~/.modelsim (use "nano ~/.modelsim" in terminal)
    Find: PrefDefault = ... textFontV2 {Verdana 12} (the name of the font may differ)
    Change 12 to -12, so it will looks like this: textFontV2 {Verdana -12}
    Save ~/.modelsim (Ctrl+O and then Enter)
    Reopen modelsim

###### References
http://permalink.gmane.org/gmane.comp.hardware.opencores.leon-sparc/19561
https://gist.github.com/PrieureDeSion/e2c0945cc78006b00d4206846bdb7657
https://github.com/PrieureDeSion/Randoms/blob/master/VHDL/Getting%20Started.pdf
