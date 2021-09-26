# VFS5 Vox footswitch emulation  

Based on the reverse engineering from [Whitelamp](http://www.whitelamp.com/public/vox_vfs5.html) and the PWM implementation from [@santoxyz](https://github.com/santoxyz/vfs5)

This is an emulator for the VFS5 Vox footswitch for their own amps running on a Arduino nano.

### Features:
 - 5 Buttons (4 channels + 1 Bank selection)
 - 4 LEDs to indicate channel (1-4 solid, 5 - 8 slow blinking)
 - Powered directly by the amp.
 - Printable case


 ### Schematics:

                  +------------------------------------------------------------------+
                  |                                                                  |
                  |                                                                  |
      FROM AMP    |             +---------------------+                              |
     +--------+   |             |                     |                              |
     |        |   |             |                     |                              |
     |      T +-----------------+ VCC             OUT +---->1K2Ω+---+----->1k2Ω+-----+
     |        |   |             |                     |             |                |
     |      R | <-+    +--------+ GND   ARDUINO       |             |                |
     |        |        |        |        NANO         |             v                v
     |      S +--------+        |                     |           47nF             47nF
     |        |                 |                 GND +-----+       +                +
     +--------+                 |                     |     +-------+----------------+
                                |                     |
                                |---------------------|

- LEDs go on Pins: 7,8,9,10:  
		Pin ----- 330Ohm ----- LED ---- GND

 - Buttons go on Pin 0,3,4,6 and use Pullups:  
 		Pin ---- BTN ---- GND

### Case
 - Simple Case: if you design something more elaborate drop me a note.
 - Supports 5 Cherry Style keyboard switches (press-fit)
 - Supports 4 5mm LEds (press-fit)
 - Supports 5x7 PCB (press-fit)

 ![Case1](Case1.jpg | width=100)
 ![Case2](Case2.jpg | width=100)
#### Known Bugs
  - PCB holder a bit to small => the PCB needs to be trimmed by 1mm
  - Screw-holes to small to fit a nut => the case fits okayish without screws, otherwise use longer screws so the nut is on the top. 