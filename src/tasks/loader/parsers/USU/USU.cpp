#include "USU.hpp"
#include <panic/panic.hpp>

void USU::parse() {
	Header* module = (Header*)(rawbegin);
	if(module->magic != USU_MAGIC)
		panic(Panic::ELF_PARSER_NOT_USU);

	entrypoint = module->entrypoint;

	size_t begin = rawbegin + sizeof(Header);
	size_t size = rawend  - begin;

	Mapping mapping;
	mapping.orig = rawbegin + sizeof(Header);
	mapping.dst = 0;
	mapping.size = size;
	mapping.flags = PARSER_FLAGS_RWX;

	mappings.push_back(mapping);
}
