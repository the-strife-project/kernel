#ifndef USU_HPP
#define USU_HPP

#include <mem/PMM/PMM.hpp>

#define USU_MAGIC 0x5553557F

class USU {
private:
	struct Header {
		uint32_t magic;
		uint64_t entrypoint;
	} __attribute__((packed));

	uint64_t begin;
	uint64_t size;
	uint64_t entrypoint;

public:
	USU(uint64_t rawbegin, uint64_t rawend);
};

#endif
