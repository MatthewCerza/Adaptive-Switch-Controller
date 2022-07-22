# Adaptive-Switch-Controller
Created by our "ACME" group for ECEN 1400 at CU Boulder.
This was based on VinDuv's incredible [Arduino Uno R3 Switch controller emulator project](https://github.com/VinDuv/switch-arduino-controller), which provided the basis for the communication between the Arduino and Nintendo Switch. If you want to compile this project, make sure you have the dependencies installed from there.
<br>
The adaptive switch controller project is aimed at helping people with Tourette's syndrome or other similar unwanted movement disorders enjoy playing video games on the Nintendo Switch. An Arduino UNO is used to emulate a USB Nintendo Switch controller and takes both digital and analog inputs that are processed and then passed through to the Switch. 
<br>
Inputs are filtered (configured by swsh.c) in order to ignore unintended inputs. The ideal setup includes pressure sensitive face buttons. If the user presses a button that is outside of a "normal" pressure range, the input is ignored and not passed through to the switch. Furthermore, the acceptable input range can be adjusted as the user plays (disabled by default).
<br>
![](ACME_controller.jpeg)
The front of our controller. All of the arcade-style buttons are pressure sensitive and configured to ignore unintentional inputs.
![](ACME_controller_wiring.jpeg)
The inside wiring of the controller. Running analogue sensors to the face buttons without a PCB made wire management a challenge.
<br>
# Flashing code to an Arduino Uno
Warning: If you flash the USB IC you will not be able to flash regular code to the Arduino. If you wish to flash code to the main chip (ATmega 328p) or otherwise restore the Arduino to it's factory configuration, see the bottom of this section.

1. Compile swsh.c using the make command in the main project directory
2. Flash the main ATmega 328p controller using the following command. Make sure you are in the main project directory. Replace the /dev/cu.usbmodem address with wherever your Uno is located on your system. If you do not know, you can find out through the Arduino IDE under "devices".
<code>avrdude -F -V -c arduino -p ATMEGA328P -P /dev/cu.usbmodem14101 -b 115200 -U flash:w:swsh.hex</code>
3. After flashing the main chip, unplug and replug the Arduino. Now it's time to flash the USB controller. First we need to put the USB IC (ATmega 16u2) into DFU mode by shorting the RST and GND pins on the IC header. Where these are  can vary by UNO model. Next, run the following commands:
Erase the USB IC: <code>sudo dfu-programmer atmega16u2 erase</code>
(Make sure you're in the correct directory) Flash the USB IC: <code>sudo dfu-programmer atmega16u2 flash usb-iface.hex</code>
Reset the USB IC: <code>sudo dfu-programmer atmega16u2 reset</code>

If you ever need to reflash the main chip, or otherwise want to restore the factory configuration of the Arduino, repeat step 3 of the above guide, but flash the original Arduino firmware back onto the USB controller. Those can be found [here](https://github.com/arduino/ArduinoCore-avr/tree/master/firmwares/atmegaxxu2).

For more information on flashing, see VinDuv's original instructions on their page.
