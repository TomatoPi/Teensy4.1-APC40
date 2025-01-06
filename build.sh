#!/usr/bin/bash

set -e                          # exit on failure

SKETCHNAME="Teensy4.1-APC40"
SKETCHDIR=$SKETCHNAME
SRCDIR="mycelium"

test $SKETCHNAME
test $SKETCHDIR
test $SRCDIR

cd ~/Arduino/$SKETCHNAME

echo "Runnig from $(pwd)"

bash unit_tests/run_tests.sh

echo "Copy libraries to sketch folder"

rm -rf $SKETCHDIR/src
mkdir -p $SKETCHDIR/src/utils

cp -p -r $SRCDIR/src/utils/*/*.hpp $SKETCHDIR/src/utils
cp -p -r $SRCDIR/src/utils/*/*.hxx $SKETCHDIR/src/utils

echo "Copy sources to sketch folder"

cp -p -r $SRCDIR/src/hw/*/*.hpp $SKETCHDIR/src
cp -p -r $SRCDIR/src/hw/*/*.hxx $SKETCHDIR/src
cp -p -r $SRCDIR/src/hw/*/*.cpp $SKETCHDIR/src

echo "Copy sketch file"

cp -p $SRCDIR/$SKETCHNAME.ino $SKETCHDIR/


echo "Generate platform defines"


BUILD_ID=$(echo $RANDOM | md5sum | head -c 8)
BUILD_DATE=$(date --rfc-3339=s)
BUILD_SHORT_DATE=$(echo $BUILD_DATE | head -c 10)
echo "date='$BUILD_DATE' version='$BUILD_ID'"

DEFINESFILE="$SKETCHNAME/defines.h"

echo "
/* auto generated file */

#define BUILD_ID            0x$BUILD_ID
#define BUILD_STR           \"$BUILD_ID\"
#define BUILD_DATE          \"$BUILD_DATE\"
#define BUILD_SHORT_DATE    \"$BUILD_SHORT_DATE\"
#define LOG_FILE            \"logs/$BUILD_SHORT_DATE-$BUILD_ID.log\"
" > $DEFINESFILE

echo "Compiling sketch"

# arduino-cli compile -b teensy:avr:teensy41 --verbose --warnings all --verify -e --build-path ~/build.Arduino/$SKETCHNAME.build $SKETCHNAME

echo "Register successfull build"
BUILDS_LIST=build-versions.log
touch $BUILDS_LIST
echo "$BUILD_DATE: build-id='$BUILD_ID' git-head='$(git rev-parse --short HEAD)' dirty-files:" >> $BUILDS_LIST
git diff --name-only | grep -e '^mycelium/' | sed $'s/^/\t/' >> $BUILDS_LIST

echo "Searching board ..."

BOARD=$(arduino-cli board list | grep '^usb\S*' -o)

echo " ... found on port: $BOARD"
echo "Uploading sketch"

arduino-cli upload -b teensy:avr:teensy41 -p $BOARD --input-dir ~/Arduino/$SKETCHNAME/$SKETCHNAME/build/teensy.avr.teensy41 -v -t

echo "Done"
