# Configuring Cubieboard2


## GPIO activation

In order to start working with GPIO these needed to be activated first. This can be done via sysfs and also a programmable way. As for me I will you sysfs as I don't need to have high speed, handle interrupts or something more. Sysfs interface is fine for me.

I use the mainline kernel, at the moment it is 4.9.0 release.

Use the link https://linux-sunxi.org/GPIO to get understanding of how to make GPIO available in sysfs. The link https://linux-sunxi.org/A20/PIO explains naming of pins on A20 SoC.

In my case I need PB18 line, this is activated so way:

    echo 50 > /sys/class/gpio/export


## GPIO connections

Take a board keeping the ethernet socket at top left corner. GPIO wires should look at you. 


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


PB18 - a management line which controls an external transistor like 2N2222. This line gives only +3.3V but I use transistor for passing +5V to a needed me relay through it.

