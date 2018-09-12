#! /bin/bash

if [[ $EUID -ne 0 ]]; then
  echo "This script must be run as root" 1>&2
  exit 1
fi

apt-get install g++ gcc cmake libxext-dev libapr1 libaprutil1 libexpat1-dev libfontconfig1-dev libfreetype6-dev libsvn1 libxft-dev libxpm-dev libxrender-dev subversion x11proto-render-dev zlib1g-dev libdrm-dev libice-dev libsm-dev libxt-dev mesa-common-dev python-dev tk-dev tcl  tcl8.6 tcl8.6-dev libglu1-mesa libqt4-opengl-dev libcppunit-dev
#changed tcl8.5 to 8.6 at the end of this linear
