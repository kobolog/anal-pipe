#ifndef __DISTRIBUTION_OF_HPP_INCLUDED__
#define __DISTRIBUTION_OF_HPP_INCLUDED__

#include <iomanip>

#include "analysis_concept.hpp"
#include "sorted_heap.hpp"

namespace linkedin {

struct distribution_of: public analysis_concept {
    distribution_of(const std::string& key_):
        key(key_),
        total(0)
    { }

    void operator()(const parsed_line& parsed) {
        parsed_line::const_iterator it(parsed.find(key));

        if(it != parsed.end()) {
            aggregate[it->second]++;
            total++;
        }
    }

    void dump(std::ostream& stream) const {
        stream << "Distribution of '" << key << "':" << std::endl;

        sorted_heap heap;

        std::copy(
            aggregate.begin(),
            aggregate.end(),
            pusher(heap)
        );

        while(!heap.empty()) {
            stream << "\t" << heap.top().first << ": "
                   << std::fixed << std::setprecision(5) 
                   << heap.top().second * 100.0f / total << "%" << std::endl;
            
            heap.pop();
        }
    }
    
    const std::string key;
    uint64_t total;

    typedef std::map<
        std::string,
        uint64_t
    > container_type;

    container_type aggregate;
};

}

#endif
