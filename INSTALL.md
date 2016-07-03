# Install and Flashing Instructions
The flashing procedure consists of the "unlocking" of the board,as it is read/write protected originally, and flashing the actual firmware. A ST-LINK v2 is used, either clone, original, or Discovery/Nucleo board.

Connections to the programming port require 3 wires, ground, swclk and swdat (swio). While flashing, the quad is powered from its battery.

# Windows

See [RC Groups Thread First Post](http://www.rcgroups.com/forums/showthread.php?t=2634611) for now.

# Linux

For flashing on Linux, the [OpenOCD](http://openocd.org/) toolchain is used. The install instructions have been tested to work with OpenOCD 0.9.0 on Debian-based systems (Ubuntu 14.04).

## Install OpenOCD 0.9.0

On some OS versions, older versions of OpenOCD might be the default. On Debian-based systems you can check which version is the default by running
```
apt-cache policy openocd
```
* If the output indicates a version below 0.9.0, you can install it on Debian-based systems (such as Ubuntu 14.04) using these commands:
```
wget http://lug.mtu.edu/ubuntu/pool/universe/h/hidapi/libhidapi-hidraw0_0.8.0~rc1+git20140201.3a66d4e+dfsg-3_amd64.deb
sudo dpkg -i libhidapi-hidraw0_0.8.0~rc1+git20140201.3a66d4e+dfsg-3_amd64.deb
wget http://ubuntu.mirrors.tds.net/ubuntu/pool/universe/j/jimtcl/libjim0.76_0.76-1_amd64.deb
sudo dpkg -i libjim0.76_0.76-1_amd64.deb
wget http://ubuntu.mirrors.tds.net/ubuntu/pool/universe/o/openocd/openocd_0.9.0-1build1_amd64.deb
sudo dpkg -i openocd_0.9.0-1build1_amd64.deb
```
* If the output of the check indicates a version equal or higher to 0.9.0, just install openocd:
```
sudo apt-get install openocd
```

## Toolchain Install and Build

Run the following to install the necessary build tools:
```
apt-get install git build-essential gcc-arm-none-eabi libnewlib-arm-none-eabi
```
Clone the repository:
```
git clone https://github.com/silver13/CG023
cd CG023
```
Build the firmware:
```
cd gcc
make
```

## Flashing

Before being able to flash, the board needs to be unlocked. **This only has to be performed once for every flight controller board.** 
```
openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f0x.cfg -c init -c "reset halt" -c "stm32f0x unlock 0" -c "reset run" -c shutdown
```

The board needs a power cycle after unlocking.

Once the board is unlocked, the firmware can be flashed using
```
openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f0x.cfg -c init -c "reset halt" -c "flash write_image erase cg023 0x08000000" -c "verify_image cg023 0x08000000" -c "reset run" -c shutdown
```

#Size

The firmware compiled by gcc seems about 40% larger, presumably due to the libs used by gcc. This means it may be over 16K in some cases. Some options can be disabled to reduce size, mainly headless and the flip sequencer. Of couse, the acro only build is a lot smaller, and should always fit.
