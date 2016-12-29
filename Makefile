# compilation flags for gdb

CFLAGS  = -O1 -g
ASFLAGS = -g 

# object files

OBJS=  $(STARTUP) main.o
OBJS+= stm32f10x_gpio.o stm32f10x_rcc.o

# name of executable

PRG=$(notdir $(CURDIR))
ELF=$(PRG).elf
SIZ=$(PRG).size                  

# Tool path
TOOLVERSION=6_2-2016q4
TOOLNAME=gcc-arm-none-eabi-$(TOOLVERSION)
TOOLROOT=/usr/local/$(TOOLNAME)
TOOLBINPATH=$(TOOLROOT)/bin

# Library path

LIBROOT=$(CURDIR)/system

# Tools

CC=$(TOOLBINPATH)/arm-none-eabi-gcc
LD=$(TOOLBINPATH)/arm-none-eabi-gcc
AR=$(TOOLBINPATH)/arm-none-eabi-ar
AS=$(TOOLBINPATH)/arm-none-eabi-as
SZ=$(TOOLBINPATH)/arm-none-eabi-size

# Code Paths

DEVICE=$(LIBROOT)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CORE=$(LIBROOT)/Libraries/CMSIS/CM3/CoreSupport
PERIPH=$(LIBROOT)/Libraries/STM32F10x_StdPeriph_Driver

# Search path for standard files

vpath %.c $(TEMPLATEROOT)

# Search path for perpheral library

vpath %.c $(CORE)
vpath %.c $(PERIPH)/src
vpath %.c $(DEVICE)
vpath %.s $(DEVICE)/startup/gcc_ride7

# Search path for Library

#~ vpath %.c $(TEMPLATEROOT)/Library/ff9/src
#~ vpath %.c $(TEMPLATEROOT)/Library/ff9/src/option
#~ vpath %.c $(TEMPLATEROOT)/Library

#  Processor specific

PTYPE = STM32F103xB 
LDSCRIPT = $(TEMPLATEROOT)/STM32F103RB.ld
STARTUP = startup_stm32f10x_md.o system_stm32f10x.o 

# Compilation Flags

FULLASSERT = -DUSE_FULL_ASSERT 

LDFLAGS+= --specs=nosys.specs -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -Wl,-Map=$(PRG).map 
LDFLAGS+= -Xlinker -gc-sections
CFLAGS+= -mcpu=cortex-m3 -mthumb -Wall -std=gnu11
CFLAGS+= -I$(TEMPLATEROOT) -I$(DEVICE) -I$(CORE) -I$(PERIPH)/inc -I.
CFLAGS+= -D$(PTYPE) -DUSE_HAL_DRIVER $(FULLASSERT)
CFLAGS+= -I$(TEMPLATEROOT)/Library/ff9/src -I$(TEMPLATEROOT)/Library

# Build executable 

all : $(ELF) secondary-outputs

$(ELF) : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(SIZ) : $(ELF)
	$(SZ) --format=berkeley $(ELF)

# compile and generate dependency info

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $*.d

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) $(ELF) startup_stm32f* $(CLEANOTHER)

debug: $(ELF)
	arm-none-eabi-gdb -iex "target extended-remote :4242" $(ELF)

secondary-outputs: $(SIZ)

# pull in dependencies

-include $(OBJS:.o=.d)




