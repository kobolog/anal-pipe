#ifndef __ANALYSIS_CONCEPT_HPP_INCLUDED__
#define __ANALYSIS_CONCEPT_HPP_INCLUDED__

#include <ostream>

#include "common.hpp"

// Base class to derive from for analysis objects.

namespace linkedin {

typedef std::map<std::string, std::string> parsed_line;

struct analysis_concept {
    virtual ~analysis_concept() = 0;

    virtual void operator()(const parsed_line& parsed) = 0;
    virtual void dump(std::ostream& stream) const = 0;
};

analysis_concept::~analysis_concept() { }

}

#endif
