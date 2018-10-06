#!/bin/bash
# To mount from Raspberry Pi using sshfs

echo 'sshfs mounting!'
sshfs -o allow_other pi@192.168.1.125:/home/pi/servopi /home/scar1et/sshfs-path




##* How to configure sshfs *##
# replace <username> with your user name
# sudo apt-get install sshfs
# sudo apt-get install fuse
# groupadd fuse
# usermod -aG fuse <username>
# sudo vim /etc/fuse.conf
# uncomment the "user_allow_other" and save
# mkdir /home/<username>/sshfs-path/
# sshfs -o allow_other pi@192.168.1.109:/home/pi/MotorPi /home/<username>/sshfs-path

##* How to configure sshfs *##
#fusermount -u /home/<username>/sshfs-path
