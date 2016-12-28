# Configuring Cubieboard2


## GPIO activation

In order to start working with GPIO these needed to be activated first. This can be done via sysfs and also a programmable way. As for me I will you sysfs as I don't need to have high speed, handle interrupts or something more. Sysfs interface is fine for me.

I use the mainline kernel, at the moment it is 4.9.0 release.

Use the link https://linux-sunxi.org/GPIO to get understanding of how to make GPIO available in sysfs. The link https://linux-sunxi.org/A20/PIO explains naming of pins on A20 SoC.

In my case I need PB18 line, this is activated so way:

    echo 50 > /sys/class/gpio/export

### How to get pin a number suitable for linux mainline kernel

In order to map a pin name and a pin interface via sysfs. Pins should be recognized properly.

Most of GPIO ports have similar names starting with "P" letter within one more letter and a pin order number, correct names are PB18, PH9, PE16, PD9, etc.

For example I will use pin named PB18.

Use the formula for getting a mainline kernel suitable number for using it in /sys/class/gpio/export,unexport.

    1. Take the port name and determine its number in alphabetical order
        A - 1   E - 5
        B - 2   F - 6
        C - 3   G - 7
        D - 4   H - 8
        Out port number is 8 (H)
    2. Extract out of the got number 1: 8 - 1
    3. Multiple the result on 32: (8 - 1)*32
    4. Add to the result pin order number: (8 - 1)*32 + 18 (PB18) = 242
 
The total number is used for activation (deactivation) appropriate GPIO port line:

    echo 242 > /sys/class/gpio/export
    and then /sys/class/gpio/gpio242 becomes available for further using
    echo 242 > /sys/class/gpio/unexport     # makes GPIO unavailable inside OS

If you don't want to calculate the pin numbers then use the link https://linux-sunxi.org/A20/PIO and take ready numbers.

There is also another way to find a correct base, discovered in a kernel source header.

Observe this file https://raw.githubusercontent.com/torvalds/linux/master/drivers/pinctrl/sunxi/pinctrl-sunxi.h and find a section with definitions as listed below. 

    #define PA_BASE 0
    #define PB_BASE 32
    #define PC_BASE 64
    #define PD_BASE 96
    #define PE_BASE 128
    #define PF_BASE 160
    #define PG_BASE 192
    #define PH_BASE 224
    #define PI_BASE 256
    #define PL_BASE 352
    #define PM_BASE 384
    #define PN_BASE 416

But according to a line found in the header "Allwinner A1X SoCs pinctrl driver" ensure you have such SoC or even A20 should have same pins layout. Unfortunately I didn't find yet how to map these definitions to a board I use, it is cubieboard2.

## GPIO connections

Take a board keeping the ethernet socket at top left corner. GPIO wires should look at you. This is all about Cubieboard2.


                          =========
           +-----+        |       |
           | o +-|-- +5V  | Ether |
           | o o |        |  net  |
    PB18 --|-+ o |        =========
           | o o |
           | o o |
           | o o |
           | o o |
           | o o |
           | o o |
           | o +-|-- GND
           | o o |
           | o o |
           | ... |


PB18 - a management line which controls an external transistor like 2N2222. This line gives only +3.3V but I use transistor for passing +5V to a needed me relay through it. In short, transistor is a key which opens a relay.

## System requirements

In order to start using the GPIO pins they to be configured properly. Because of I use Gentoo Linux, I placed such bash script in file /etc/local.d/50-enable-gpio.start && chmod +x on this script.

    #!/bin/bash
    
    out_pins="50 239"
    pins_path=/sys/class/gpio
    group=remad
    
    for pin in $out_pins; do
        echo $pin > $pins_path/export
        echo out > $pins_path/gpio$pin/direction
    
        chgrp $group $pins_path/gpio$pin/value
        chmod g+w $pins_path/gpio$pin/value
    done

