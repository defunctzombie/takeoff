#!/bin/sh

appname=$(basename $0 | sed s,\.sh$,,)
dirname=$(dirname $0)
if [ "${dirname:0:1}" != "/" ]; then
    dirname=$PWD/$dirname
fi
 
export DYLD_LIBRARY_PATH=$dirname/../Resources/lib
export PATH=$dirname/../Resources/bin:$PATH
$appname $*