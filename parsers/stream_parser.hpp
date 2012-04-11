#ifndef __STREAM_PARSER_HPP_INCLUDED__
#define __STREAM_PARSER_HPP_INCLUDED__

#include "analysis_concept.hpp"

// A bit faster FSM-like stream parser.

namespace linkedin {

struct stream_parser {
    void operator()(const std::string& line, parsed_line& parsed) {
        std::string consume;
        std::istringstream stream(line);

        if(!std::getline(stream, parsed["ip"], ' ')) {
            std::cerr << "error: failed to parse ip" << std::endl;
        } else if(!std::getline(stream, parsed["ident"], ' ')) {
            std::cerr << "error: failed to parse ident" << std::endl;
        } else if(!std::getline(stream, parsed["user"], ' ')) {
            std::cerr << "error: failed to parse user" << std::endl;
        } else if(!std::getline(stream, consume, '[')) {
            std::cerr << "error: failed to parse time" << std::endl;
        } else if(!std::getline(stream, parsed["time"], ']')) {
            std::cerr << "error: failed to parse time" << std::endl;
        } else if(!std::getline(stream, consume, '"')) {
            std::cerr << "error: failed to parse request line" << std::endl;
        } else if(!std::getline(stream, parsed["method"], ' ')) {
            std::cerr << "error: failed to parse request line" << std::endl;
        } else if(!std::getline(stream, parsed["url"], ' ')) {
            std::cerr << "error: failed to parse request line" << std::endl;
        } else if(!std::getline(stream, parsed["protocol"], '"')) {
            std::cerr << "error: failed to parse request line" << std::endl;
        } else if(!std::getline(stream, consume, ' ')) {
            std::cerr << "error: failed to parse code" << std::endl;
        } else if(!std::getline(stream, parsed["code"], ' ')) {
            std::cerr << "error: failed to parse code" << std::endl;
        } else if(!std::getline(stream, parsed["size"], ' ')) {
            std::cerr << "error: failed to parse size" << std::endl;
        } else if(!std::getline(stream, consume, '"')) {
            std::cerr << "error: failed to parse referrer" << std::endl;
        } else if(!std::getline(stream, parsed["referrer"], '"')) {
            std::cerr << "error: failed to parse referrer" << std::endl;
        } else if(!std::getline(stream, consume, '"')) {
            std::cerr << "error: failed to parse useragent" << std::endl;
        } else if(!std::getline(stream, parsed["useragent"], '"')) {
            std::cerr << "error: failed to parse useragent" << std::endl;
        }
    }
};

}

#endif
