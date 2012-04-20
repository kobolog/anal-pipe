#ifndef __ERRORS_BY_URL_HPP_INCLUDED__
#define __ERRORS_BY_URL_HPP_INCLUDED__

#include <boost/lexical_cast.hpp>

#include "analysis_concept.hpp"
#include "sorted_heap.hpp"

namespace linkedin {

struct errors_by_url: public analysis_concept {
    void operator()(const parsed_line& parsed) {
        parsed_line::const_iterator code(parsed.find("code"));
        parsed_line::const_iterator url(parsed.find("url"));
        
        if(code != parsed.end() && url != parsed.end()) {
            int n = boost::lexical_cast<int>(code->second);
        
            if(n >= 400 && n < 600) {
                aggregate[n][url->second]++;
            }
        }
    }

    void dump(std::ostream& stream) const {
        stream << "Errors by URL: " << std::endl;

        // Sort the maps and print the results.
        
        for(container_type::const_iterator it = aggregate.begin();
            it != aggregate.end();
            ++it)
        {
            stream << "\tCode " << it->first << ":" << std::endl;

            kv_heap heap;

            std::copy(
                it->second.begin(),
                it->second.end(),
                pusher(heap)
            );

            while(!heap.empty()) {
                stream << "\t\t" << heap.top().second << ": " 
                       << heap.top().first << std::endl;
                
                heap.pop();
            }
        }
    }
    
    typedef std::map<
        int,
        std::map<
            std::string,
            uint64_t
        >
    > container_type;

    container_type aggregate;
};

}

#endif
