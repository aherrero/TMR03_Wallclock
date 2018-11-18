# Import('env')
# env.Replace(FUSESCMD="avrdude $UPLOADERFLAGS -e -Ulock:w:0x3f:m -Uefuse:w:0xff:m -Uhfuse:w:0xd6:m -Ulfuse:w:0b11110111:m")


## Bootlaoder
## /opt/arduino-1.8.7/hardware/tools/avr/bin/avrdude -C/home/aherrero/.arduino15/packages/MiniCore/hardware/avr/2.0.1/avrdude.conf -v -patmega328p -cusbtiny -e -Ulock:w:0x3f:m -Uefuse:w:0xff:m -Uhfuse:w:0xd6:m -Ulfuse:w:0b11110111:m
## /opt/arduino-1.8.7/hardware/tools/avr/bin/avrdude -C/home/aherrero/.arduino15/packages/MiniCore/hardware/avr/2.0.1/avrdude.conf -v -patmega328p -cusbtiny -Uflash:w:/home/aherrero/.arduino15/packages/MiniCore/hardware/avr/2.0.1/bootloaders/optiboot_flash/atmega328p/optiboot_flash_atmega328p_UART0_115200_16000000L.hex:i -Ulock:w:0x0f:m
## Progrramming
## /opt/arduino-1.8.7/hardware/tools/avr/bin/avrdude -C/opt/arduino-1.8.7/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -cusbtiny -Uflash:w:/tmp/arduino_build_684084/powerDownWakeExternalInterrupt.ino.hex:i
