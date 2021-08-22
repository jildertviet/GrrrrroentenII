#!/bin/sh
sudo /usr/bin/python3 ./oscServer.py &
sudo /usr/bin/python3 ./streamOsc.py &
#sudo /home/pi/openFrameworks/apps/groenten/groentenServer/bin/groentenServer &
sudo ip link set wlan0 down
sudo ifconfig eth0 192.168.0.2/24
exit 0;
