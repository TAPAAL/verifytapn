#include "VerificationOptions.hpp"
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace VerifyTAPN {

    std::string enumToString(Trace trace) {
        switch (trace) {
            case SOME:
                return "some";
            default:
                return "no";
        }
    }

    std::string SearchTypeEnumToString(SearchType s) {
        switch (s) {
            case COVERMOST:
                return "Maximum Cover Search";
            case RANDOM:
                return "Random Search";
            case DEPTHFIRST:
                return "Depth-First Search";
            default:
                return "Breadth-First Search";
        }
    }

    std::string FactoryEnumToString(Factory factory) {
        switch (factory) {
            case OLD_FACTORY:
                return "old DBM";
            case DISCRETE_INCLUSION:
                return "discrete inclusion";
            default:
                return "default";
        }
    }

    std::ostream& operator<<(std::ostream& out, const VerificationOptions& options) {
        out << "Using " << SearchTypeEnumToString(options.GetSearchType()) << std::endl;
        out << "k-bound is: " << options.GetKBound() << std::endl;
        out << "Generating " << enumToString(options.GetTrace()) << " trace\n";
        out << "Symmetry Reduction is " << (options.GetSymmetryEnabled() ? "ON" : "OFF") << std::endl;
        out << "Untimed place optimization is " << (options.GetUntimedPlacesEnabled() ? "ON" : "OFF") << std::endl;
        out << "Using " << (options.GetGlobalMaxConstantsEnabled() ? "global maximum constant" : "local maximum constants") << " for extrapolation" << std::endl;
        out << "Using " << FactoryEnumToString(options.GetFactory()) << " marking factory" << std::endl;
        if (options.GetFactory() == DISCRETE_INCLUSION) {
            out << "Considering the places ";
            if (options.GetIncPlaces().size() == 0) {
                out << "*NONE*";
            } else {
                bool first = true;
                for (std::vector<std::string>::const_iterator it = options.GetIncPlaces().begin(); it != options.GetIncPlaces().end(); it++) {
                    if (!first) out << ", ";
                    out << *it;
                    first = false;
                }
            }
            out << " for discrete inclusion." << std::endl;
        }
        out << "Model file is: " << options.GetInputFile() << std::endl;
        out << "Query file is: " << options.QueryFile() << std::endl;
        return out;
    }

    void initialize(po::options_description& args, VerificationOptions& options) {

        args.add_options()
            ("help,h", "Displays this help message.")
            ("version,v", "Displays version information.")
            ("k-bound,k", po::value<uint32_t>(), "Max tokens to use during exploration.")
            ("xml-queries,x", po::value<std::string>(), "Parse XML query file and verify queries of a given comma-seperated list")
            ("search-strategy,s", po::value<std::string>(),
            "Specify the desired search strategy\n"
            " BFS\n"
            " DFS\n"
            " RDFS\n"
            " MAX-COVER")
            ("trace,t", po::value<uint32_t>(),
            "Specify the desired trace option.\n"
            " 0: none (default)\n"
            " 1: some")
            ("global-max-constants", "Use global maximum constant for extrapolation (as opposed to local constants).")
            ("disable-untimed-opt", "Disables the untimed place optimization.")
            ("disable-symmetry", "Disables symmetry reduction.")
            ("inclusion-check", po::value<uint32_t>(), "Specify the desired marking inclusion check.\n"
            " 0: Default\n"
            " 1: Discrete-inclusion\n"
            " 2: Old factory")
            ("inclusion-places", po::value<std::string>(), "Specify a list of places to consider for discrete inclusion. No spaces after the commas!\n"
            "Special values: *ALL*, *NONE*")
            ("write-unfolded-net", po::value<std::string>(), "Outputs the model to the given file before after unfolding")
            ("write-unfolded-queries", po::value<std::string>(), "Outputs the queries to the given file after unfolding");
    }

    uint64_t parseInt(const std::string& str) {
        if (std::any_of(str.begin(), str.end(), [](char a) {
                return !std::isdigit(a); })) {
        std::cout << "Not a number: " << str << std::endl;
    }
        std::stringstream ss(str);
        uint64_t a;
        ss >> a;

        return a;
    }

    std::set<size_t> parseNumbers(const std::string& str) {
        size_t last = 0;
        size_t i = 0;
        std::set<size_t> res;
        for (; i < str.size(); ++i) {
            if (str[i] != ',') continue;
            std::string tmp = str.substr(last, i - last);
            auto n = parseInt(tmp);
            if (n <= 0) {
                std::cerr << "Query-indexes are 1-index, got a 0";
                std::exit(-1);
            }
            res.emplace(n - 1); // 0-indexed in parser
            last = i + 1; // skip comma
        }
        if (last < i) {
            std::string tmp = str.substr(last, i - last);
            auto n = parseInt(tmp);
            if (n <= 0) {

                std::cerr << "Query-indexes are 1-index, got a 0";
                std::exit(-1);
            }
            res.emplace(n - 1); // 0-indexed in parser
        }
        return res;
    }

    SearchType toSearchType(const std::string& str) {
        if (str == "BFS")
            return BREADTHFIRST;
        if (str == "DFS")
            return DEPTHFIRST;
        if (str == "RANDOM" || str == "RDFS")
            return RANDOM;
        if (str == "MAX-COVER")
            return COVERMOST;
        std::cout << "ERROR: Unknown search strategy '" << str << "' specified." << std::endl;
        std::exit(1);
    }

    VerificationOptions VerificationOptions::parse(int argc, char *argv[]) {
        VerificationOptions opts;
        try {
            po::options_description args(VERIFYTAPN_NAME " [options] model.pnml (query.xml)");
            initialize(args, opts);
            po::variables_map vm;

            auto parsed = po::command_line_parser(argc, argv).options(args).allow_unregistered().run();
            po::store(parsed, vm);
            po::notify(vm);

            if (vm.count("help")) {
                std::cout << args << std::endl;
                std::exit(0);
            }

            if (vm.count("version")) {
                std::cout << "VerifyTAPN " << VERIFYTAPN_VERSION << std::endl;
                std::cout << "Licensed under GPLv2." << std::endl;
                std::exit(0);
            }

            if (vm.count("k-bound"))
                opts.k_bound = vm["k-bound"].as<uint32_t>();

            if (vm.count("xml-queries"))
                opts.query_numbers = parseNumbers(vm["xml-queries"].as<std::string>());

            if (vm.count("search-strategy"))
                opts.searchType = toSearchType(vm["search-strategy"].as<std::string>());


            if (vm.count("trace"))
                opts.trace = vm["trace"].as<uint32_t>() == 0 ? NONE : SOME;

            if (vm.count("global-max-constants"))
                opts.useGlobalMaxConstants = true;

            if (vm.count("disable-untimed-opt"))
                opts.useUntimedPlaces = false;

            if (vm.count("disable-symmetry"))
                opts.symmetry = false;

            if (vm.count("inclusion-places")) {
                size_t last = 0;
                std::string str = vm["inclusion-places"].as<std::string>();
                size_t i = 0;
                for (; i < str.size(); ++i) {
                    if (str[i] == ',') {
                        if (last != i)
                            opts.inc_places.emplace_back(str.substr(i, (i - last)));
                        last = i + 1;
                    }
                }
                if (last != i)
                    opts.inc_places.emplace_back(str.substr(i, (i - last)));
            } else {
                opts.inc_places.emplace_back("*ALL*");
            }

            if (vm.count("inclusion-check"))
            {
                auto val = vm["inclusion-check"].as<uint32_t>();
                switch(val)
                {
                    case 0:
                        opts.factory = DEFAULT;
                        break;
                    case 1:
                        opts.factory = DISCRETE_INCLUSION;
                        break;
                    case 2:
                        opts.factory = OLD_FACTORY;
                        break;
                    default:
                        std::cerr << "ERROR: Unsupported value for --inclusion-check (" << val << ")" << std::endl;
                        std::exit(-1);
                }
            }

            if (vm.count("write-unfolded-net"))
                opts.out_model = vm["write-unfolded-net"].as<std::string>();

            if (vm.count("write-unfolded-queries"))
                opts.out_queries = vm["write-unfolded-queries"].as<std::string>();

            std::vector<std::string> files = po::collect_unrecognized(parsed.options, po::include_positional);
            // remove everything that is just a space
            files.erase(std::remove_if(files.begin(), files.end(),
                [](auto& s) {
                    return std::all_of(s.begin(), s.end(),
                        [](auto c) {
                            return std::isspace(c);
                        });
                }
            ), files.end());

            if (files.size() != 2) {
                std::cerr << "Expected exactly 2 trailing files (a model and a query), got [";
                for (size_t i = 0; i < files.size(); ++i) {
                    if (i != 0) std::cerr << ", ";
                    std::cerr << files[i];
                }
                std::cerr << "]" << std::endl;
                std::exit(-1);
            }
            opts.inputFile = files[0];
            opts.queryFile = files[1];
        } catch (const std::exception& ex) {
            std::cerr << "ERROR: Could not parse options: " << ex.what() << std::endl;
            std::exit(-1);
        }

        return opts;
    }
}
