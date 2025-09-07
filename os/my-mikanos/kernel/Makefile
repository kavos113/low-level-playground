TARGET = kernel.elf
OBJS = main.o

CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -std=c++17
LDFLAGS += --entry KernelMain -z norelro --image-base 0x100000 --static

CUSTOM_CLANG_DIR = $(HOME)/clang+llvm-7.1.0-x86_64-linux-gnu-ubuntu-14.04/bin

CXX = $(CUSTOM_CLANG_DIR)/clang++
LD = $(CUSTOM_CLANG_DIR)/ld.lld

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)

$(TARGET): $(OBJS) Makefile
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@