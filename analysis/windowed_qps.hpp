#ifndef __WINDOWED_QPS_HPP_INCLUDED__
#define __WINDOWED_QPS_HPP_INCLUDED__

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/conversion.hpp>

#include "analysis_concept.hpp"
#include "sorted_heap.hpp"

namespace linkedin {

namespace pt = boost::posix_time;

struct windowed_qps: public analysis_concept {
    windowed_qps(time_t window_size):
        window(pt::ptime(), pt::from_time_t(window_size))
    { }

    void operator()(const parsed_line& parsed) {
        parsed_line::const_iterator it(parsed.find("time"));

        if(it != parsed.end()) {
            struct tm time;

            memset(&time, 0, sizeof(time));

            // Parse the line timestamp.

            if(!::strptime(it->second.c_str(), "%d/%b/%Y:%H:%M:%S", &time)) {
                std::cerr << "error: failed to parse timings" << std::endl;
                return;
            }

            pt::ptime stamp = pt::ptime_from_tm(time);

            if(window.contains(stamp)) {
            }
        }
    }

    void dump(std::ostream& stream) const {
    }

    pt::ptime current;
    pt::time_period window;

    typedef std::map<
        pt::ptime,
        uint64_t
    > container_type;

    container_type aggregate;
};

}

#endif
