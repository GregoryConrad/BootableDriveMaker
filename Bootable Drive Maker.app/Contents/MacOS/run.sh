#!/bin/sh

#################
# Set Variables #
#################
devID=`cat devID.txt`
copyFile=`cat path.txt`
fileExt=${copyFile##*.}

#############
# Copy File #
#############
echo "Copying..." > currStep.txt
echo "Copying File To Needed Location..." >> cmdOut.txt 2>&1
if [ "$fileExt" == "iso" ]
then
	cp "$copyFile" "file.iso"
else
	cp "$copyFile" "file.img.dmg"
fi
if [ "$?" != "0" ]
then
	exitValue=$?
	echo "ERROR: Copy Failed. File does not exist?" > currStep.txt
	echo "ERROR: Copy Failed. File does not exist?" >> cmdOut.txt 2>&1
	exit $?
fi

##################
# Convert To IMG #
##################
if [ "$fileExt" == "iso" ]
then
echo "Converting..." > currStep.txt
echo "Converting The ISO To IMG..." >> cmdOut.txt 2>&1
hdiutil convert -format UDRW -o "file.img.dmg" "file.iso" >> cmdOut.txt 2>&1
if [ "$?" != "0" ]
then
	exitValue=$?
	echo "ERROR: Conversion Failed. Invalid ISO?" > currStep.txt
	echo "ERROR: Conversion Failed. Invalid ISO?" >> cmdOut.txt 2>&1
	exit $?
fi
fi

################
# Unmount Disk #
################
echo "Unmounting..." > currStep.txt
echo "Unmounting The Disk..." >> cmdOut.txt 2>&1
diskutil unmountDisk "$devID" >> cmdOut.txt 2>&1
if [ "$?" != "0" ]
then
	exitValue=$?
	echo "ERROR: Unmount Failed. Device Present?" > currStep.txt
	echo "ERROR: Unmount Failed. Device Present?" >> cmdOut.txt 2>&1
	exit $?
fi


################
# Create Drive #
################
echo "Creating Drive..." > currStep.txt
echo "Creating The Bootable Drive (This May Take A Long Time)..." >> cmdOut.txt 2>&1
sudo dd if="file.img.dmg" of="$devID" bs=4m >> cmdOut.txt 2>&1
if [ "$?" == "0" ]
then
	echo "Success!" > currStep.txt
	exit 0
else
	exitValue=$?
	echo "ERROR: DD Failed. Device Removed?" > currStep.txt
	echo "ERROR: DD Failed. Device Removed?" >> cmdOut.txt 2>&1
	exit $?
fi