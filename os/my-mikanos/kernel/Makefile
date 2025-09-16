TARGET = kernel.elf
OBJS = main.o pixel_writer.o font.o console.o newlib_support.o io.o pci.o mouse.o logger.o libcxx_support.o \
	usb/memory.o usb/device.o usb/xhci/device.o usb/xhci/devmgr.o usb/xhci/port.o \
	usb/xhci/registers.o usb/xhci/ring.o usb/xhci/trb.o usb/xhci/xhci.o \
	usb/classdriver/base.o usb/classdriver/hid.o usb/classdriver/keyboard.o usb/classdriver/mouse.o
MORE_OBJS = hankaku.o

CFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone
CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -std=c++17
LDFLAGS += --entry KernelMain -z norelro --image-base 0x200000 --static

#CUSTOM_CLANG_DIR = $(HOME)/clang+llvm-7.1.0-x86_64-linux-gnu-ubuntu-14.04/bin/

DEVENV_BASE = $(HOME)/osbook/devenv/x86_64-elf
EDK_BASE = $(HOME)/edk2

CFLAGS += -I$(DEVENV_BASE)/include/c++/v1 -I$(DEVENV_BASE)/include -I$(DEVENV_BASE)/include/freetype2 -I$(EDK_BASE)/MdePkg/Include -I$(EDK_BASE)/MdePkg/Include/X64
CXXFLAGS += -I$(DEVENV_BASE)/include/c++/v1 -I$(DEVENV_BASE)/include -I$(DEVENV_BASE)/include/freetype2 -I$(EDK_BASE)/MdePkg/Include -I$(EDK_BASE)/MdePkg/Include/X64
CXXFLAGS += -nostdlibinc -D__ELF__ -D_LDBL_EQ_DBL -D_GNU_SOURCE -D_POSIX_TIMERS -DEFIAPI='__attribute__((ms_abi))'
LDFLAGS += -L$(DEVENV_BASE)/lib

CXXFLAGS += -I.

C = $(CUSTOM_CLANG_DIR)clang
CXX = $(CUSTOM_CLANG_DIR)clang++
LD = $(CUSTOM_CLANG_DIR)ld.lld

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)

$(TARGET): $(OBJS) Makefile
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(MORE_OBJS) -lc -lc++

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c Makefile
	$(C) $(CFLAGS) -c $< -o $@

%.o: %.asm Makefile
	nasm -f elf64 -o $@ $<