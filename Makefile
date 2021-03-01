export PROJNAME := Kernel
export RESULT := kernel.elf
export asm := yes
export LINKER_FILE := linker.ld
export nostdlib := true
export nostdlibh := true

all: $(RESULT)

%: force
	@$(MAKE) -f ../helper/Makefile $@ --no-print-directory
force: ;
