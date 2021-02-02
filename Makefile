SRCPATH=src
OBJPATH=obj
RESULT=kernel.elf

CXX=amd64-elf-g++
INCLUDES=-Isrc
CXXFLAGS=$(INCLUDES) -std=c++11 -ffreestanding -O3 -Wall -Wextra -Werror -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-stack-protector -fpic -fpie -fomit-frame-pointer -mno-red-zone -mno-80387 -mno-mmx -mno-3dnow -mno-sse

LINKER=$(CXX)
LINKER_FLAGS=-T linker.ld -nostdlib -pie -z max-page-size=0x1000

CXX_OBJS = $(shell cd src && find . -type f -iname '*.cpp' | sed 's/\.cpp/\.o/g' | xargs -I {} echo "$(OBJPATH)/"{})

.PHONY: all
all: $(RESULT)

$(RESULT): $(CXX_OBJS)
	$(LINKER) `find $(OBJPATH) -type f -iname '*.o'` -o $@ $(LINKER_FLAGS)
	strip $(RESULT)

-include $(CXX_OBJS:.o=.o.d)

$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(OBJPATH)
	$(CXX) -c -o $@ $< $(CXXFLAGS)
	$(CXX) -MM $< -o $@.d.tmp $(CXXFLAGS)
	@sed -e 's|.*:|$@:|' < $@.d.tmp > $@.d
	@rm -f $@.d.tmp

$(OBJPATH):
	cd src && find . -type d -exec mkdir -p ../$(OBJPATH)/{} \;

clean:
	rm -rf $(RESULT) $(OBJPATH)/
