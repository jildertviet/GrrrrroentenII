# GrrrrroentenII
Software for Raspberry Pi based camera system for Grrrrroenten II  
Made and tested with of_v0.10.0_osx_release  
ofxDatGui is customized and not included, so OSX projects won't build.  
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
- In a scene the: (0,1) pan/tilt positions, (2) zoom and (3) active camera are saved.
- Arrow keys for camera movement  
- Hold shift to decrease step size for pan/tilt movement.  
### groentenPreview  
:computer: Shows an overview of all the streams, run at MacBook.  
### groentenServer  
:tv: Runs at the TV-Raspberry. Receives messages to restart groentenDisplay with selected camera.  

Had to do the restart thing, since switching between stream introduced increasing/permanent lag in the stream.

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
MacBook is 192.168.0.10 (or anything besides, 2, 3, 4, 5, 6 and 11).  

## PROBLEMEN

Wat als de videoserver niet reageert:  
1.) groentenGui-app herstarten  
2.) Videoserver-RBP rebooten:  
    Open Terminal  
    Typ: ssh pi@192.168.0.11  
    password: bloemkool (je ziet niks als je typt, dat klopt)  
    Dan, als je ingelogd bent, typ:  
    sudo reboot -h now  
    Op het scherm (tv) zie je nu dingen gebeuren  

Wat als een camera niet werkt?  
Staat er stroom op? Check rode ledje op Raspberry Pi.   
Is ie te pingen: (Terminal -> ping 192.168.0.2. Als je iets een tijd in ms krijgt: is ie te pingen en staat dus aan).  
    192.168.0.2 is Raspberry met sticker 1,  
    192.168.0.3 is Raspberry met sticker 2,  
    etc  
Als ie niet te bereiken is:   
Hard-reset: stroom er af en in. Via micro-USB aan zijkant, of stekker volgen en in stopcontact in/uit.  
Bij het opstarten hoor je de motortjes kort.  

Als ie dan weer niet te bereiken is qua IP: SD kaart kapot?  
