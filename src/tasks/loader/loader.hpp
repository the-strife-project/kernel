#ifndef LOADER_HPP
#define LOADER_HPP

#include "parsers/parser.hpp"
#include <mem/paging/paging.hpp>

namespace Loader {
	struct LoaderInfo {
		Paging paging;
		uint64_t base, stack;

		inline LoaderInfo() {}
		inline LoaderInfo(Paging paging, uint64_t base, uint64_t stack)
			: paging(paging), base(base), stack(stack)
		{}
	};

	void startELFParser();
	LoaderInfo load(const PrivList<Parser::Mapping>& mappings);
};

#endif
