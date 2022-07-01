export PROJNAME := Kernel
export RESULT := kernel.elf
export asm := yes
export LINKER_FILE := linker.ld
export nostdlib := true

.PHONY: all

all: $(RESULT)
	@

%: force
	@$(MAKE) -f $(STRIFE_HELPER)/Makefile $@ --no-print-directory
force: ;
