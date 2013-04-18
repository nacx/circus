#!/bin/sh

# Copyright (c) 2011 Ignasi Barrera
# This file is released under the MIT License, see LICENSE file.

LIB_NAME=circus
LIB_VERSION=1.2.0

GIT_REVISION=`git show-ref --head -s | head -n 1`
BUILD_PLATFORM=`uname -srm`
BUILD_DATE=`date +"%Y-%m-%d %H:%M"`

LFILE=../LICENSE
VFILE=lib/version.c

license() {
    echo "/*" >$1
    cat $LFILE | nl -b a -s ' * ' | cut -c7- >>$1
    echo " */\n" >>$1
}

license $VFILE
echo '#include "version.h"\n' >>$VFILE
echo "const char* lib_name = \"$LIB_NAME\";" >>$VFILE 
echo "const char* lib_version = \"$LIB_VERSION\";" >>$VFILE 
echo "const char* git_revision = \"$GIT_REVISION\";" >>$VFILE 
echo "const char* build_platform = \"$BUILD_PLATFORM\";" >>$VFILE 
echo "const char* build_date = \"$BUILD_DATE\";" >>$VFILE 
echo >>$VFILE
