#!/bin/bash

set -x
BUILD_DIR=/tmp

if [ "`id -u`" != "0" ]; then
    echo "You need to be root.."
    exit
fi

if [ "$1" != "" ]; then
    CHECKOUT="cvs -d:ext:$1@os-sim.cvs.sourceforge.net:/cvsroot/os-sim co"
else
    CHECKOUT="cvs -d:pserver:anonymous@os-sim.cvs.sourceforge.net:/cvsroot/os-sim co"
fi


cd $BUILD_DIR
rm -rf os-sim
`$CHECKOUT os-sim`
cd os-sim/
find . | grep CVS$ | xargs rm -rf
dpkg-buildpackage



cd $BUILD_DIR
rm -f ossim-agent* # remove old ossim-agent package

rm -rf agent
`$CHECKOUT agent`
cd agent/
find . | grep CVS$ | xargs rm -rf
dpkg-buildpackage
cd -

