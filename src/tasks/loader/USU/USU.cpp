#include "USU.hpp"
#include <panic/panic.hpp>
#include <klibc/klibc.hpp>

USU::USU(uint64_t rawbegin, uint64_t rawend) {
	Header* module = (Header*)(rawbegin);
	if(module->magic != USU_MAGIC)
		panic(Panic::ELF_PARSER_NOT_USU);

	entrypoint = module->entrypoint;
	size = rawend  - (rawbegin + sizeof(Header));

	printf("TODO");
	hlt(); while(true);
}
