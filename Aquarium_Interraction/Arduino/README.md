# Arduino

This folder contains all testing code for the Interactive Aquarium Testing

## Things to know
![Project Mechanisme](../Doc/project_legend.png "Title")

## Mechanism
To produce a movement along one single axis, motors need to be coupled

If both motors are turning in the same direction, it produces a vertical motion. If both motors are set to 'HIGH', the move in an anti-clockwise position, producing a movement toward the up. If both motors are set to LOW, they turn in a clockwise direction, producing a downward movement.

If the two motors turn in different direction, it produces an horizontal motion. If motor 1 is set to HIGH, and have an anti-clockwise movememnt, and motor 2 is set to LOW, having a clockwise movement, it produces a movement to the right.  If motor 2 is set to LOW, and have an clockwise movememnt, and motor 2 is set to HIGH, having an anti-clockwise movement, it produces a movement to the left. 

## Programms

Sorry, not all programs are well commented ... 

**0_clean_copy** : the last version of last year's code
**1_test_button_calibration** : a program that only test that the buttons are working
**2_single_motor_horizontal** : make just an horizontal displacement
**3_single_motor_vertical** : make just a veritcal displacement
**4_test_calibration** : test programme for the calibration of motors, the motors goes all the way to one stop and then counts the number of step up to the other stop
**5_Motor_and_Input_String** : Add the possibility to receive string and decode them via the serial port
**6_Add_Interractions** : Add the possibility to interract physically with the aquarium
**7_Add_Interractions_Flush** : Add the possibility to interract physically with the aquarium and ass a flush at the end of each interraction to smooth the interraction and limit the delay between interactions


## Configurations

The Arduino used for this project is an Arduino Giga R1 Wifi. To use it, you need to install the "Arduino Mbed OS Giga Board" in the Board Manager of Arduino IDE. 
This program relies on 2 libraries : SimpleTimer.h and ezButton.h
Both can be downloaded in the library manager of Arduino IDE