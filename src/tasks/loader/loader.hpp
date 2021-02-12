#ifndef LOADER_HPP
#define LOADER_HPP

#include "parsers/parser.hpp"

namespace Loader {
	void startELFParser();
	void load(const PrivList<Parser::Mapping>&);
};

#endif
