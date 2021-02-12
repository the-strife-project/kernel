#ifndef PARSER_HPP
#define PARSER_HPP

#include <common.hpp>
#include <klibc/klibc.hpp>

#define PARSER_FLAGS_W 0b01
#define PARSER_FLAGS_X 0b10

#define PARSER_FLAGS_WX 0b11

class Parser {
public:
	struct Mapping {
		uint64_t orig, dst, size;
		uint8_t flags = 0;
	};

	uint64_t entrypoint;
	PrivList<Mapping> mappings;

	virtual void parse() = 0;
};

#endif
