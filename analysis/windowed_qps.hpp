#ifndef __WINDOWED_QPS_HPP_INCLUDED__
#define __WINDOWED_QPS_HPP_INCLUDED__

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/conversion.hpp>

#include <boost/lambda/bind.hpp>

#include "analysis_concept.hpp"
#include "sorted_heap.hpp"

namespace linkedin {

namespace pt = boost::posix_time;

struct windowed_qps: public analysis_concept {
    typedef std::map<
        pt::ptime,
        uint64_t
    > window_type;
    
    typedef struct {
        uint64_t min, max;
        float mean, median;
    } result_type;

    windowed_qps(long seconds):
        step(pt::seconds(seconds)),
        current(pt::ptime(), pt::ptime())
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

            if(current.is_null()) {
                // First iteration, init.
                current = pt::time_period(stamp, step);
            }
            
            while(!current.contains(stamp) && stamp > current.last()) {
                // Move the time window forward, if required.
                aggregate[to_simple_string(current)] = process(window);
                current.shift(step);
                window.clear();
            }

            window[stamp]++;
        }
    }

    template<class T>
    struct push_to {
        push_to(T& target_):
            target(target_)
        { }

        template<class U>
        void operator()(const U& x) {
            target(x.second);
        }

        T& target;
    };

    template<class T>
    static push_to<T> make_pusher(T& target) {
        return push_to<T>(target);
    }

    result_type process(window_type& window) {
        result_type result;
        
        if(window.empty()) {
            return result;
        }
        
        boost::accumulators::accumulator_set<
            uint64_t,
            boost::accumulators::features<
                boost::accumulators::tag::min,
                boost::accumulators::tag::max,
                boost::accumulators::tag::mean,
                boost::accumulators::tag::median
            >
        > acc;

        std::for_each(
            window.begin(),
            window.end(),
            make_pusher(acc)
        );

        result.min = boost::accumulators::min(acc);
        result.max = boost::accumulators::max(acc);
        result.mean = boost::accumulators::mean(acc);
        result.median = boost::accumulators::median(acc);

        return result;
    }

    void dump(std::ostream& stream) const {
        stream << "QPS by " << step.total_seconds() << "-second periods:" << std::endl;
   
        for(container_type::const_iterator it = aggregate.begin();
            it != aggregate.end();
            ++it)
        { 
            stream << "\t" << it->first << ":" << std::endl
                   << "\t\tMin: " << it->second.min << std::endl
                   << "\t\tMax: " << it->second.max << std::endl
                   << "\t\tMean: " << it->second.mean << std::endl
                   << "\t\tMedian: " << it->second.median << std::endl;
        }
    }

    const pt::time_duration step;
    
    pt::time_period current;
    window_type window;

    typedef std::map<
        std::string,
        result_type
    > container_type;

    container_type aggregate;
};

}

#endif
