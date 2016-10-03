#!/bin/sh

copyFile=`cat path.txt`
devID=`cat devID.txt`

echo "Working..." > currStep.txt
echo "Copying File To Needed Location..." > cmdOut.txt
if ! cp "$copyFile" "file.iso"
then
	echo "ERROR: Copy Failed. File does not exist?" > currStep.txt
	exit 1
fi

echo "Converting..." > currStep.txt
if ! hdiutil convert -format UDRW -o "file.img.dmg" "file.iso" &> cmdOut.txt
then
	echo "ERROR: Conversion Failed. Invalid ISO?" > currStep.txt
	exit 1
fi

echo "Unmounting..." > currStep.txt
if ! diskutil unmountDisk "$devID" &> cmdOut.txt
then
	echo "ERROR: Unmount Failed. Device Present?" > currStep.txt
	exit 1
fi

echo "Creating Drive (This May Take A Long Time)..." > currStep.txt
if sudo dd if="file.img.dmg" of="$devID" bs=4m &> cmdOut.txt
then
	echo "Success!" > currStep.txt
	exit 0
else
	echo "ERROR: DD Failed. Device Removed?" > currStep.txt
	exit 1
fi