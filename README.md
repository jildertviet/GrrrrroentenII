# GrrrrroentenII
Software for Raspberry Pi based camera system for Grrrrroenten II  
Made and tested with of_v0.10.0_osx_release  
ofxDatGui is customized  
groentenGui also builds on of_v0.11.0
Turn WiFi off, since it may mess with the IP's  

![GrrrrroentenGui](https://github.com/jildertviet/GrrrrroentenII/blob/main/groentenGui/bin/data/grrrrroentenGui.png)

## Apps  
### groentenApp  
:x: Seems deprecated? Probably used before switchting to python IP stream.
### groentenBlack  
:tv: Very simple app to guarantee a black view on the TV  
### groentenDisplay  
:tv: (re-)Runs on the Raspberry Pi connected to the TV. Options: select cam (as argument @ startup), play movie.
### groentenGui  
:computer: Control app to run on MacBook. Select the camera's and control the movements.  
- To save a cuePoint (for an individual camera) or scene (for all cameras): hold shift and press the button. 
### groentenPreview  
:computer: Shows an overview of all the streams, run at MacBook.  
### groentenServer  
:tv: Runs at the TV-Raspberry. Receives messages to restart groentenDisplay with selected camera.  

Had to do the restart thing, since switching between stream introduced lag.

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
