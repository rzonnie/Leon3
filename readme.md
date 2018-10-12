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
Release:	    18.04
Codename:	    bionic
```

The following applications are required:
* QuartusII Lite (Web Edition) (18.1.0) to synthesize and place&route;
* ModelSIM(-Altera) (10.5b) to simulate the design;
* Bare Code Compiler to compile the software;
* GNU Make (4.1) to run the Makefiles;

## Installation

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
