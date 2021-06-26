#ifndef ELF_HPP
#define ELF_HPP

#include "../parser.hpp"

extern size_t ELF_PID;
void startELFParser();


class ELF : public Parser {
private:
	uint64_t rawbegin, rawend;

public:
	inline ELF(uint64_t rawbegin, uint64_t rawend)
		: rawbegin(rawbegin), rawend(rawend)
	{}

	void parse() override;
};


#endif
