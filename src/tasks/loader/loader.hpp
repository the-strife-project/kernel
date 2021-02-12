#ifndef LOADER_HPP
#define LOADER_HPP

#include "parsers/parser.hpp"
#include <mem/paging/paging.hpp>

namespace Loader {
	void startELFParser();
	Paging load(const PrivList<Parser::Mapping>& mappings);
};

#endif
