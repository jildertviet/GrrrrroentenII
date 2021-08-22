# GrrrrroentenII
Software for Raspberry Pi based camera system for Grrrrroenten II  
Made and tested with of_v0.10.0_osx_release  

![GrrrrroentenGui](https://github.com/jildertviet/GrrrrroentenII/blob/main/groentenGui/bin/data/grrrrroentenGui.png | width=100)

## Apps  
### groentenApp  
Seems deprecated? Probably used before switchting to python IP stream.
### groentenBlack  
Very simple app to guarantee a black view on the TV  
### groentenDisplay  
Runs on the Raspberry Pi connected to the TV. Options: select cam, play movie.
### groentenGui  
Control app to run on MacBook. Select the camera's and control the movements.
### groentenPreview  
Shows an overview of all the streams, run at MacBook.  
### groentenServer  
...

## Hardware  
6x Raspberry Pi Model 3B+ 1GB  
5x 5MP V1.3 camera  
5x Pan-Tilt HAT  

Switch (TL-SG1016D) + CAT5 cables  
MacBook Pro  

### Camera units  
Each camera has a groentenSetup.sh file which runs at boot.
```
#!/bin/sh
sudo /usr/bin/python3 ./oscServer.py &
sudo /usr/bin/python3 ./streamOsc.py &
#sudo /home/pi/openFrameworks/apps/groenten/groentenServer/bin/groentenServer &
sudo ip link set wlan0 down
sudo ifconfig eth0 192.168.0.2/24
exit 0;
```  
streamOsc.py runs the videostream  
oscServer.py runs the pan/tilt HAT  

## IPs  
192.168.0.*  
Server is @ 192.168.0.11  
Camera's are in range [2-6]  
Netmask: 255.255.255.0  

xx
