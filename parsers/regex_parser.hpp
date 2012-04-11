#ifndef __REGEX_PARSER_HPP_INCLUDED__
#define __REGEX_PARSER_HPP_INCLUDED__

#include <boost/regex.hpp>

#include "analysis_concept.hpp"

// Simple and slow regex parser based on boost::regex.

namespace linkedin {

struct regex_parser {
    struct match_transformer {
        match_transformer(parsed_line& map, const boost::smatch& results):
            m_map(map),
            m_results(results)
        { }

        void operator()(const std::string& key) {
            m_map[key] = m_results[key];
        }

        parsed_line& m_map;
        const boost::smatch& m_results;
    };

public:
    regex_parser(const std::string& regex, const std::vector<std::string>& keys):
        m_regex(regex),
        m_keys(keys)
    { }

    void operator()(const std::string& line, parsed_line& parsed) {
        boost::smatch match_results;

        if(boost::regex_match(line, match_results, m_regex)) {
            std::for_each(
                m_keys.begin(),
                m_keys.end(),
                match_transformer(parsed, match_results)
            );
        } else {
            std::cerr << "error: unable to parse line: " << line << std::endl;
        }
    }

private:
    boost::regex m_regex;
    std::vector<std::string> m_keys;
};

}

#endif
