# E-Cake

*E-Cake is a perfect gift as engineers. It greets the birthday person, plays the birthday song, and lights up an RGB LED candle. Like any other candle and birthday celebration, you can make a wish then blow out the candle via the AM2320/TMP36 sensors. The 1st button acts as a system switch while the 2nd acts as a reset for the birthday song and candle, and the potentiometer acts as a volume knob. With this project, I was able to apply Microcontroller Unit (MCU) integration concepts like the pull-up resistor configuration, events, interrupts, transitions, flags, debouncing, and Finite State Machines (FSM).

*Try it here (Tinkercad)- https://www.tinkercad.com/things/f8ZcvaADujy-digital-birthday-cake?sharecode=ICyaT2BlDS7_xqUmgh013fXfsvGTh08yXwxuQOfO9i8

*Code at E-Cake/E-Cake.ino

*Comment AM2320 or TMP36 codes accordingly to switch between the 2 sensors

*AM2320 is used for physical circuit while TMP36 is used for Tinkercad simulation

*Comment out '#include<Wire.h>' for Tinkercad

*Documentations at Doc folder

## Materials

*Electronic candle w/ AM2320/TMP36 and Diffused Common Cathode RGB, HBD Song w/ 4216 buzzer, Text w/ 1602 HD44780 I2C, powered by Arduino UNO R3

*Used 10kΩ pull-up resistors for the push buttons, 220Ω for LED, B5K potentiometer as volume knob

## Acknowledgements
1. Robson Couto, David A. Mellis modified by Arturo Guadalupi
2. Clgproject- https://youtu.be/-eqirn-2yKA
3. LiquidCrystal_I2C library
4. Adafruit_AM2320 and Adafruit_Sensor libraries
