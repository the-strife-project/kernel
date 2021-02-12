#ifndef USU_HPP
#define USU_HPP

#include "../parser.hpp"
#include <mem/PMM/PMM.hpp>
#include <klibc/klibc.hpp>

#define USU_MAGIC 0x5553557F

class USU : public Parser {
private:
	struct Header {
		uint32_t magic;
		uint64_t entrypoint;
	} __attribute__((packed));

	uint64_t rawbegin, rawend;

public:
	inline USU(uint64_t rawbegin, uint64_t rawend)
		: rawbegin(rawbegin), rawend(rawend)
	{}

	void parse() override;
};

#endif
