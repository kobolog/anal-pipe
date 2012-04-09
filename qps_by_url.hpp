#ifndef __QPS_BY_URL_INCLUDED__
#define __QPS_BY_URL_INCLUDED__

#include "analysis_concept.hpp"
#include "sorted_heap.hpp"

namespace linkedin {

struct qps_by_url: public analysis_concept {
    void operator()(const parsed_line& parsed) {
        parsed_line::const_iterator time(parsed.find("time"));
        parsed_line::const_iterator url(parsed.find("url"));

        // We keep initial timestamp and move the end timestamp further on every
        // iteration, so that we keep track of the time window of the logfile.

        if(time != parsed.end() && url != parsed.end()) {
            if(start.empty()) {
                start = time->second;
            } else {
                end = time->second;
            }

            aggregate[url->second]++;
        }
    }

    void dump(std::ostream& stream) const {
        stream << "QPS by URL:" << std::endl;

        sorted_heap heap;

        std::copy(
            aggregate.begin(),
            aggregate.end(),
            pusher(heap)
        );

        // There might be only one record in the logfile.

        if(start.empty() || end.empty()) {
            std::cerr << "error: insufficient time slice" << std::endl;
            return;
        }

        struct tm tm_start, tm_end;

        // Parse the time string into a tm struct.

        if(!::strptime(start.c_str(), "%d/%b/%Y:%H:%M:%S", &tm_start) ||
           !::strptime(end.c_str(), "%d/%b/%Y:%H:%M:%S", &tm_end))
        {
            std::cerr << "error: failed to parse timings" << std::endl;
            return;
        }
        
        // Convert separated tm struct values into a UNIX timestamp.

        time_t epoch_start = ::mktime(const_cast<struct tm *>(&tm_start)),
               epoch_end = ::mktime(const_cast<struct tm *>(&tm_end));

        double diff = ::difftime(epoch_end, epoch_start);

        // Sort the map and print the results.

        while(!heap.empty()) {
            stream << "\t" << heap.top().first << ": "
                   << heap.top().second / diff << std::endl;
        
            heap.pop();
        }
    }
    
    std::string start, end;

    typedef std::map<
        std::string,
        uint64_t
    > container_type;

    container_type aggregate;
};

}

#endif
