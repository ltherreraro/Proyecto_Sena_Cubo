LM32_CC=lm32-elf-g++
LM32_LD=lm32-elf-ld
LM32_OBJCOPY=lm32-elf-objcopy
LM32_OBJDUMP=lm32-elf-objdump



SREC2VRAM ?= ../../tools/srec2vram/srec2vram
VRAMFILE=image.ram

CFLAGS=-MMD -O2 -Wall -g -s -fomit-frame-pointer -mbarrel-shift-enabled -msign-extend-enabled  -mmultiply-enabled -mdivide-enabled 
LDFLAGS=-nostdlib -nodefaultlibs -Tlinker.ld 
SEGMENTS = -j .text -j .rodata -j .data

all: image.srec $(VRAMFILE) 

crt0ram.o: crt0ram.S
	$(LM32_CC) $(CFLAGS) -c crt0ram.S

main.o: main.c
	$(LM32_CC) $(CFLAGS) -c main.c

soc-hw.o: soc-hw.c
	$(LM32_CC) $(CFLAGS) -c soc-hw.c

pwm.o: pwm.cpp
	$(LM32_CC) $(CFLAGS) -c pwm.cpp

uart.o: uart.cpp
	$(LM32_CC) $(CFLAGS) -c uart.cpp

timer.o: timer.cpp
	$(LM32_CC) $(CFLAGS) -c timer.cpp
camera.o: camera.cpp
	$(LM32_CC) $(CFLAGS) -c camera.cpp

image: crt0ram.o main.o soc-hw.o pwm.o timer.o uart.o camera.o
	$(LM32_LD) $(LDFLAGS) -Map image.map -N -o image crt0ram.o main.o soc-hw.o pwm.o timer.o uart.o camera.o

image.lst: image
	$(LM32_OBJDUMP) -h -S $< > $@

image.srec: image.lst
	$(LM32_OBJCOPY) $(SEGMENTS) -O srec image image.srec

$(VRAMFILE): image.srec
	$(SREC2VRAM) image.srec 0x00000000 0x1000 > $(VRAMFILE)

clean:
	rm -f image image.lst image.bin image.srec image.map  *.o *.d *.ram

DEPS := $(wildcard *.d)
ifneq ($(DEPS),)
include $(DEPS)
endif

