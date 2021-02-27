SRCPATH=src
OBJPATH=obj
RESULT=kernel.elf

CXX=amd64-elf-g++
INCLUDES=-Isrc
CXXFLAGS=$(INCLUDES) -std=c++11 -ffreestanding -O3 -Wall -Wextra -Werror -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-stack-protector -fpic -fpie -fomit-frame-pointer -mno-red-zone -mno-80387 -mno-mmx -mno-3dnow -mno-sse

ASM=nasm
ASMFLAGS=-f elf64

LINKER=$(CXX)
LINKER_FLAGS=-T linker.ld -nostdlib -pie -z max-page-size=0x1000

CXX_OBJS := $(shell cd src && find . -type f -iname '*.cpp' | sed 's/\.cpp/\.o/g' | xargs -I {} echo "$(OBJPATH)/"{})
ASM_OBJS := $(shell cd src && find . -type f -iname '*.asm' | sed 's/\.asm/\.o/g' | xargs -I {} echo "$(OBJPATH)/"{})
ALL_OBJS := $(shell echo "$(CXX_OBJS) $(ASM_OBJS)" | xargs -n1 | sort | xargs)

.PHONY: all clean
all: $(RESULT)

$(RESULT): $(ALL_OBJS)
	@echo "Linking..."
	@$(LINKER) `find $(OBJPATH) -type f -iname '*.o'` -o $@ $(LINKER_FLAGS)
	@echo "Stripping..."
	@strip $(RESULT)

-include $(CXX_OBJS:.o=.o.d)

$(ALL_OBJS): | $(OBJPATH)

$(CXX_OBJS): $(OBJPATH)/%.o: $(SRCPATH)/%.cpp
	@echo "===> $<"
	@$(CXX) -c -o $@ $< $(CXXFLAGS)
	@$(CXX) -MM $< -o $@.d.tmp $(CXXFLAGS)
	@sed -e 's|.*:|$@:|' < $@.d.tmp > $@.d
	@rm -f $@.d.tmp

$(ASM_OBJS): $(OBJPATH)/%.o: $(SRCPATH)/%.asm
	@echo "===> $<"
	@$(ASM) $< -o $@ $(ASMFLAGS)

$(OBJPATH):
	@echo "Creating obj/ hierarchy..."
	@cd src && find . -type d -exec mkdir -p ../$(OBJPATH)/{} \;

clean:
	rm -rf $(RESULT) $(OBJPATH)/
