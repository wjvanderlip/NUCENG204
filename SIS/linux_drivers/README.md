# NOTE
Tested on Ubuntu 10.04, 12.04, 14.04, and Mint 18 Cinnamon. Installation in 14.04 (or any other
distro that relies on libusb 1.0 or greater) requires the installation of 
libusb-0.X-compat. This is handled automatically in the install script.

# Install instructions

## Step 1: set up your system
Run:
`$ sudo ./sis\_dependencies.sh`

This will install all of the libraries your system needs to compile and run
the SIS drivers with all features (also installs some libs for building and
visualizing data).

To be really safe, reboot the system now (this step probably isnt necessary)

## Step 2: Try the installation

As a first attempt, you can try the automated installation. Run:

`$ sudo ./install\_sis.sh`

A couple notes:
 - This installs the drivers to the $HOME/SIS folder. You can change that in
   the script by altering the SISDIR variable
 - This script has only been tested more extensively in 12.04. It should work
   in 14.04 and **may** work in 10.04 (never tried).

If the install script fails and you cant figure out why, you can clean up 
(delete the $HOME/SIS directory) and follow the manual installation 
instructions

## Step 2a: If the install script failed, follow the manual instructions in 
`manual\_install\_instructions.txt`

## Step 3: Test the SIS drivers

If you managed to get through step 2 (or 2a) without any errors then the SIS
drivers should be successfully installed. To test this, you can use a tcl
program that ships with the SIS drivers.

The procedure is detailed in manual\_install\_instructions.txt in section Va
