PROJROOT = $(CURDIR)
OBJDIR = bin
SRCDIR = src
INCDIR = inc
LDDIR = ldscripts

# compilation flags for gdb

CFLAGS = -g
ASFLAGS = -g 

# object files

USROBJS = main.o sysTimer.o scheduler.o linkedList.o uart.o
OBJS = $(addprefix $(OBJDIR)/,$(STARTUP) $(HAL_OBJS) $(USROBJS))

HAL_OBJS = stm32f1xx_hal_gpio.o stm32f1xx_hal_rcc_ex.o stm32f1xx_hal_rcc.o \
           stm32f1xx_hal.o stm32f1xx_hal_cortex.o stm32f1xx_hal_msp.o \
           stm32f1xx_hal_uart.o

# name of executable

PRG = $(notdir $(PROJROOT))
ELF = $(OBJDIR)/$(PRG).elf
BIN = $(OBJDIR)/$(PRG).bin
MAP = $(OBJDIR)/$(PRG).map
SIZ = $(PRG).size                  

# Tool path
TOOLVERSION = 6_2-2016q4
TOOLNAME = gcc-arm-none-eabi-$(TOOLVERSION)
TOOLROOT = /usr/local/$(TOOLNAME)
TOOLBINROOT = $(TOOLROOT)/bin

# Library path

SYSDIR = system

# Tools

CC = $(TOOLBINROOT)/arm-none-eabi-gcc
LD = $(TOOLBINROOT)/arm-none-eabi-gcc
OC = $(TOOLBINROOT)/arm-none-eabi-objcopy
AR = $(TOOLBINROOT)/arm-none-eabi-ar
AS = $(TOOLBINROOT)/arm-none-eabi-as
SZ = $(TOOLBINROOT)/arm-none-eabi-size
GDB = $(TOOLBINROOT)/arm-none-eabi-gdb

# Code Paths

DEVICE = $(SYSDIR)/cmsis
DRIVER = $(SYSDIR)/stm32f1xx

# Search path for standard files

vpath %.c $(SRCDIR)
vpath %.h $(INCDIR)

# Search path for driver library

vpath %.c $(DEVICE)/$(SRCDIR)
vpath %.s $(DEVICE)/$(SRCDIR)
vpath %.h $(DEVICE)/$(INCDIR)

vpath %.c $(DRIVER)/$(SRCDIR)
vpath %.h $(DRIVER)/$(INCDIR)

# Search path for additional libraries

#~ vpath %.c $(TEMPLATEROOT)/Library/ff9/src
#~ vpath %.c $(TEMPLATEROOT)/Library/ff9/src/option
#~ vpath %.c $(TEMPLATEROOT)/Library

#  Processor specific

PTYPE = STM32F103xB 
LDSCRIPT = $(LDDIR)/STM32F103RB.ld
STARTUP = startup_stm32f103xb.o system_stm32f1xx.o 

# Compilation Flags

FULLASSERT = -DUSE_FULL_ASSERT 

LDFLAGS += --specs=nosys.specs -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -Wl,-Map=$(MAP)
LDFLAGS += -Xlinker -gc-sections
CFLAGS += -mcpu=cortex-m3 -mthumb -Wall -std=gnu11
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -I$(INCDIR) -I$(DEVICE)/$(INCDIR) -I$(DRIVER)/$(INCDIR)
CFLAGS += -D$(PTYPE) -DUSE_HAL_DRIVER $(FULLASSERT)
#~ CFLAGS += -I$(TEMPLATEROOT)/Library/ff9/src -I$(TEMPLATEROOT)/Library

# Build executable 

all : $(ELF) secondary-outputs

$(ELF) : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(BIN) : $(ELF)
	$(OC) -O binary $(ELF) $(BIN)

size : $(ELF)
	$(SZ) --format=berkeley $(ELF)

# compile and generate dependency info

$(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $(@D)/$*.d

$(OBJDIR)/%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) $(ELF) $(BIN) $(OBJDIR)/startup_stm32f* $(MAP) $(CLEANOTHER)

debug: $(ELF)
	$(GDB) -iex "target extended-remote :4242" -ex "load" $(ELF)

secondary-outputs: $(BIN) size 

# pull in dependencies

-include $(OBJS:.o=.d)




