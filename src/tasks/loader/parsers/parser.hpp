#ifndef PARSER_HPP
#define PARSER_HPP

#include <common.hpp>
#include <klibc/klibc.hpp>

#define PARSER_FLAGS_R 0b001
#define PARSER_FLAGS_W 0b010
#define PARSER_FLAGS_X 0b100

#define PARSER_FLAGS_RWX 0b111

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
