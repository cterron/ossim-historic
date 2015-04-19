#!/bin/bash

set -x
BUILD_DIR=/tmp

if [ "`id -u`" != "0" ]; then
    echo "You need to be root.."
    exit
fi

if [ "$1" != "" ]; then
    CHECKOUT="cvs -d:ext:$1@cvs.sf.net:/cvsroot/os-sim co os-sim"
else
    CHECKOUT="cvs -d:pserver:anonymous@cvs.sf.net:/cvsroot/os-sim co os-sim"
fi


cd $BUILD_DIR
rm -rf os-sim
`$CHECKOUT`
cd os-sim/
dpkg-buildpackage -uc -b
cd -

