#!/bin/bash
# to remove the sshfs path and upload something to raspberry pi

mountpath="/home/scar1et/sshfs-path/MyClient"
sudo rm -r $mountpath
sudo cp -r /home/lihux/Coding/CPP1X/MotorPi/MyClient /home/lihux/sshfs-path
