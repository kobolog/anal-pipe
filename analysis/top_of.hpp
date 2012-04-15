#ifndef __TOP_OF_HPP_INCLUDED__
#define __TOP_OF_HPP_INCLUDED__

#include "analysis_concept.hpp"
#include "sorted_heap.hpp"

namespace linkedin {

struct top_of: public analysis_concept {
    top_of(const std::string& key_, size_t limit_):
        key(key_),
        limit(limit_)
    { }

    void operator()(const parsed_line& parsed) {
        parsed_line::const_iterator it(parsed.find(key));

        if(it != parsed.end()) {
            aggregate[it->second]++;
        }
    }

    void dump(std::ostream& stream) const {
        stream << "Top " << limit << " of '" << key << "':" << std::endl;
        
        // Sort the map and print the results up to the specified limit.
        
        kv_heap<std::string, uint64_t> heap;

        std::copy(
            aggregate.begin(),
            aggregate.end(),
            pusher(heap)
        );

        size_t n = limit;

        while(--n && !heap.empty()) {
            stream << "\t" << heap.top().first << ": " 
                   << heap.top().second << std::endl;
            
            heap.pop();
        }
    }
    
    const std::string key;
    const size_t limit;

    typedef std::map<
        std::string,
        uint64_t
    > container_type;

    container_type aggregate;
};

}

#endif
