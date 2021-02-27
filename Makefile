PROJNAME := Kernel
RESULT := kernel.elf

OPTS := PROJNAME="$(PROJNAME)" RESULT="$(RESULT)" asm=yes LINKER_FILE=linker.ld

all: kernel.elf

%: force
	@$(MAKE) -f ../helper/Makefile $@ $(OPTS) --no-print-directory
force: ;
