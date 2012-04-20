#include <boost/assign/std/vector.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>

#include "line_iterator.hpp"

#include "analysis/distribution_of.hpp"
#include "analysis/errors_by_url.hpp"
#include "analysis/top_of.hpp"
#include "analysis/qps_by_url.hpp"
#include "analysis/windowed_qps.hpp"

#include "parsers/regex_parser.hpp"
#include "parsers/stream_parser.hpp"

using namespace linkedin;

// Analyzer class template
// -----------------------
// Takes in a parser for a specified log format.
// Various analysises (is that a word?) can be attached to it to form
// a pipeline.

template<class Parser>
struct analyzer {
    struct invoker {
        invoker(const parsed_line& parsed_):
            parsed(parsed_)
        { }

        template<class T>
        void operator()(T& analysis) {
            (*analysis)(parsed);
        }

        const parsed_line& parsed;
    };

    struct dumper {
        dumper(std::ostream& stream_):
            stream(stream_)
        { }

        template<class T>
        void operator()(const T& analysis) {
            analysis->dump(stream);
        }

        std::ostream& stream;
    };

public:
    analyzer(const Parser& parser):
        m_parser(parser)
    { }

    analyzer& operator += (const boost::shared_ptr<analysis_concept>& analysis) {
        m_chain.push_back(analysis);
        return *this;
    }

    template<class InputIterator>
    void run(InputIterator begin, InputIterator end) {
        parsed_line parsed;
        
        while(begin != end) {
            m_parser(*begin, parsed);

            if(!parsed.empty()) {
                std::for_each(
                    m_chain.begin(),
                    m_chain.end(),
                    invoker(parsed)
                );
            }

            ++begin;
        }
    }

    void dump(std::ostream& stream) const {
        std::for_each(
            m_chain.begin(),
            m_chain.end(),
            dumper(stream)
        );
    }

private:
    Parser m_parser;
    std::vector< boost::shared_ptr<analysis_concept> > m_chain;
};

// Helper function to create analyzers with template arguments deduced.

template<class Parser>
analyzer<Parser> make_analyzer(const Parser& parser) {
    return analyzer<Parser>(parser);
}

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(int argc, char * argv[]) {
    po::options_description hidden_options, options, combined;
    po::positional_options_description positional;
    po::variables_map vm;

    // Prepare the command line parsers

    hidden_options.add_options()
        ("logfile", po::value<std::string>());

    positional.add("logfile", -1);

    options.add_options()
        ("distribution-of", po::value< std::vector<std::string> >(), "Show distribution of a specified field")
        ("top-of", po::value< std::vector<std::string> >(), "Show top ten entities of a specified field")
        ("windowed-qps", po::value<long>(), "Show QPS divided by time periods, in seconds")
        ("errors-by-url", "Show URLs grouped by errors")
        ("qps-by-url", "Show QPS grouped by URL")
        ("help,h", "Show this message");

    combined.add(hidden_options).add(options);

    // Parse the command line
    
    try {
        po::store(
            po::command_line_parser(argc, argv).
            options(combined).
            positional(positional).
            run(),
        vm);
        po::notify(vm);
    } catch(const po::unknown_option& e) {
        std::cerr << "error: " << e.what() << std::endl;
        std::cout << "Usage: " << argv[0] << " <file-path> <options>" << std::endl;
        std::cout << options << std::endl;
        return EXIT_FAILURE;
    } catch(const po::ambiguous_option& e) {
        std::cerr << "error: " << e.what() << std::endl;
        std::cout << "Usage: " << argv[0] << " <file-path> <options>" << std::endl;
        std::cout << options << std::endl;
        return EXIT_FAILURE;
    }

    if(vm.count("help")) {
        std::cout << "Usage: " << argv[0] << " <file-path> <options>" << std::endl;
        std::cout << options << std::endl;
        return EXIT_SUCCESS;
    }

    // Logfile should be specified at all times.

    if(!vm.count("logfile")) {
        std::cerr << "error: no file specified" << std::endl;
        return EXIT_FAILURE;
    }

    fs::path filepath(vm["logfile"].as<std::string>());

    // Ensure that the filename specified is an actual file.

    if(!fs::exists(filepath) && !fs::is_regular(filepath)) {
        std::cerr << "error: '" << filepath.string() << "' doesn't seem to be a file." << std::endl;
        return EXIT_FAILURE;
    }

    boost::shared_ptr<fs::ifstream> input = boost::make_shared<fs::ifstream>(filepath);
    
    // We might not have the required priveleges to read it.

    if(!input->good()) {
        std::cerr << "error: unable to open '" << filepath.string() << "'" << std::endl;
        return EXIT_FAILURE;
    }

    /* Regex parser
     * ------------
     *  Validating and reusable, but slow.

    using namespace boost::assign;

    std::vector<std::string> keys;

    // This is a hack to workaround the boost::regex design shortcoming, as it
    // doesn't return the named match group names in the match results.

    keys += "ip", "ident", "user", "time", "method", "url", "protocol",
            "code", "size", "referrer", "useragent";

    regex_parser parser("(?<ip>[0-9.:a-f]+) (?<ident>[^ ]+) (?<user>[^ ]+) \\[(?<time>[^ ]+) [+-][0-9]{4}\\] \"(?<method>[A-Z]+) " \
                        "(?<url>.+?) (?<protocol>.+?)\" (?<code>[0-9]+) (?<size>[-0-9]+) \"(?<referrer>.+)\" \"(?<useragent>.+)\"$",
                        keys);
    */

    stream_parser parser;

    auto analyzer = make_analyzer(parser);

    // Attach the requested analysis objects to the analyzer.

    if(vm.count("distribution-of")) {
        std::vector<std::string> fields = vm["distribution-of"].as< std::vector<std::string> >();

        for(std::vector<std::string>::const_iterator it = fields.begin(); it != fields.end(); ++it) {
            analyzer += boost::make_shared<distribution_of>(*it);
        }
    }

    if(vm.count("top-of")) {
        std::vector<std::string> fields = vm["top-of"].as< std::vector<std::string> >();

        for(std::vector<std::string>::const_iterator it = fields.begin(); it != fields.end(); ++it) {
            analyzer += boost::make_shared<top_of>(*it, 10);
        }
    }

    if(vm.count("errors-by-url")) {
        analyzer += boost::make_shared<errors_by_url>();
    }

    if(vm.count("qps-by-url")) {
        analyzer += boost::make_shared<qps_by_url>();
    }

    if(vm.count("windowed-qps")) {
        analyzer += boost::make_shared<windowed_qps>(vm["windowed-qps"].as<long>());
    }

    line_iterator begin(input), end;
    
    // Parse and show the results.

    analyzer.run(begin, end);
    analyzer.dump(std::cout); 

    return EXIT_SUCCESS;
}
