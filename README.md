# Omnipresense

Omnipresense is an omni-directional modular robotics platform for
full telepresence VR immersion with Natural interaction control
using RGBD camera with (Nite 2.0) (Open Ni 2.2) and onboard phone
IMU.

# Feature Video
[![DEMO Video](http://img.youtube.com/vi/bha4GeMLiIQ/0.jpg)](http://www.youtube.com/watch?v=bha4GeMLiIQ)

# Composition
* Pyton server running on Raspberry Pi 2/Intel Galileo accepts the 
socket UDP stream from the RGBD sensor and IMU from the phone and 
maps the user movement to the onboard camera by controlling the 
stepper.
* Full body and hand tracking is performed using features extracted 
from NITE 2.0 middleware and sent as a UDP stream to python server.
* Custom Android App for Screen Splitting in Google Cardboard 
session.

# Base Platform
![Track 1](https://raw.githubusercontent.com/quinasura/Omnipresence/master/Images/base.jpg)

# Modular Stages
![Track 1](https://raw.githubusercontent.com/quinasura/Omnipresence/master/Images/modular.jpg)

# Build Instructions
Body and hand tracking compilation was done using Visual Studio 
and requires NITE 2.0 and OpenNI2 libraries.
