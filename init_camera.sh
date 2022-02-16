#!/bin/sh
echo "----Test i2c1"
i2cdetect -y 1
i2cset -y 1 0x70 0x00 0x04
echo "----Detecting the /dev/video0 device"
if [ -c "/dev/video0" ]; then
echo "video0 detected!"
