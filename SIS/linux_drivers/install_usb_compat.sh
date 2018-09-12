#! /bin/bash

# Install libusb-0.1.4 compatible library on system that has libusb-1.0
# installed, but requires libusb-0.1.4 functionality

if [[ $EUID -ne 0 ]]; then
  echo "This script must be run as root" 1>&2
  exit 1
fi
# Get users in order to run this script as multiple users                       
USER_LIST=`users`                                                               
stringarray=($USER_LIST)                                                        
NON_ROOT_USER=${stringarray[0]}  
# Env for running as a different user                                           
RUNAS="sudo -u $NON_ROOT_USER"

USBCOMPAT_URL=http://sourceforge.net/projects/libusb/files/libusb-compat-0.1/libusb-compat-0.1.5/libusb-compat-0.1.5.tar.bz2/download
DL_NAME=libusb-compat.tar.bz2
INSTALL_DIR=$HOME/SIS
INSTALL_NAME=libusb-compat-0.1.5
CURDIR=`pwd`

# Run as user, not root
$RUNAS bash<<___
  # Get the source
  wget -O $DL_NAME $USBCOMPAT_URL
  tar --use-compress-prog=bzip2 -xf $DL_NAME -C $INSTALL_DIR
  rm $DL_NAME
  # Build source
  cd $INSTALL_DIR/$INSTALL_NAME
  ./configure
  make
___
# Install libusb-compat
cd $INSTALL_DIR/$INSTALL_NAME
make install
# Configure environment
echo "# For libusb-0.1.5 backwards-compatibility" >> $HOME/.bashrc
echo "export LD_LIBRARY_PATH=/usr/local/lib:\$LD_LIBRARY_PATH" >> $HOME/.bashrc
# Get back to install folder
cd $CURDIR
