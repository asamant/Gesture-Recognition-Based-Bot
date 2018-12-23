# Gesture-Recognition-Based-Bot
Archived code for a laboratory-oriented project back from my undergrad days (Sem 1, 2015-16)

## A short summary
The project was intended to demonstrate gesture recognition using a camera's input feed to perform robotic movement. The implementation was carried out using:
* a laptop webcam for the input video feed,
* OpenCV C++ libraries for some base functionality related to image processing (the final algorithm making use of the OpenCV methods can be found in the final report in the repo),
* an Arduino board (UNO) for driving the robot's motor's signals,
* L293D ICs and some associated circuitry for driving the motors, and
* other miscellaneous components (Visual Studio, Arduino IDE)

## Algorithms

Though there are lots of ways to recognize gestures (both intelligent ways and "dumb" ways), we settled on a skin colour-based hand identification method for our project, as it seemed fairly robust as compared with some other approaches we tried (and it's particularly interesting to know how reasonably accurate certain algorithms can be when it comes to detecting skin tones).

Two algorithms were implemented to drive the movement of the robot:
1. A finger-count approach (1 = forward, 2 = left, 3 = right, and so on) in which the number of fingers is counted by the algorithm from the camera data and is used to drive the motors in the desired direction(s).
2. A finger-direction approach - based on the direction in which a finger is pointed, the motors are driven to make the robot move in the corresponding direction.

## Requirements
* An IDE for compiling and running C++ code (we used Visual Studio)
* Arduino IDE
* An input camera (a laptop webcam works)
* An Arduino board
* L293D ICs
* Jumper cables, breadboard, serial cables, etc.
* Li-Po battery for driving the motors
* Motors for moving the robot's chassis
