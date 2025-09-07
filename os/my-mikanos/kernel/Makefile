TARGET = kernel.elf
OBJS = main.o

CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -std=c++20
LDFLAGS += --entry KernelMain -z norelro --image-base 0x100000 --static

CXX = clang++
LD = ld.lld

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)

$(TARGET): $(OBJS) Makefile
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@