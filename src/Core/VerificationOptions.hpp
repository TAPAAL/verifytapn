#ifndef VERIFICATIONOPTIONS_HPP_
#define VERIFICATIONOPTIONS_HPP_

#include <string>
#include <iosfwd>
#include <vector>
#include <set>

namespace VerifyTAPN {
	enum Trace { NONE, SOME };
	enum SearchType { BREADTHFIRST, DEPTHFIRST, RANDOM, COVERMOST };
	enum Factory { DEFAULT, DISCRETE_INCLUSION, OLD_FACTORY };

	class VerificationOptions {
    private:
        VerificationOptions() {}
    public:
        VerificationOptions(
            const std::string& inputFile,
            const std::string& queryFile,
            SearchType searchType,
            unsigned int k_bound,
            bool symmetry,
            Trace trace,
            bool xml_trace,
            bool useUntimedPlaces,
            bool useGlobalMaxConstants,
            Factory factory,
            const std::vector<std::string>& inc_places,
                const std::string& out_model,
                const std::string& out_queries,
                const std::set<size_t>& q_nums
        ) :	inputFile(inputFile),
            queryFile(queryFile),
            searchType(searchType),
            k_bound(k_bound),
            symmetry(symmetry),
            trace(trace),
            useUntimedPlaces(useUntimedPlaces),
            useGlobalMaxConstants(useGlobalMaxConstants),
            factory(factory),
            inc_places(inc_places),
            out_model(out_model),
            out_queries(out_queries),
            query_numbers(q_nums)
        { };

		public: // inspectors
			const std::string GetInputFile() const { return inputFile; }
			const std::string QueryFile() const {return queryFile; }
			inline const unsigned int GetKBound() const { return k_bound; }
			inline const Trace GetTrace() const { return trace; };
			inline const bool GetSymmetryEnabled() const { return symmetry; }
			inline const bool GetUntimedPlacesEnabled() const { return useUntimedPlaces; }
			inline const bool GetGlobalMaxConstantsEnabled() const { return useGlobalMaxConstants; }
			inline const SearchType GetSearchType() const { return searchType; }
			inline Factory GetFactory() const { return factory; };
			inline void SetFactory(Factory f) { factory = f; };
			inline const std::vector<std::string>& GetIncPlaces() const { return inc_places; };
			inline std::vector<std::string>& GetIncPlaces(){ return inc_places; };
            const std::string& getOutputQueryFile() const {
                    return out_queries;
            }

            const std::set<size_t>& getQueryNumbers() const {
                return query_numbers;
            }

            const std::string& getOutputModelFile() const {
                    return out_model;
            }

            static VerificationOptions parse(int argc, char *argv[]);

    private:
			std::string inputFile;
			std::string queryFile;
			SearchType searchType = COVERMOST;
			unsigned int k_bound = 0;
			bool symmetry = true;
			Trace trace = NONE;
			bool useUntimedPlaces = true;
			bool useGlobalMaxConstants = false;
			Factory factory = DEFAULT;
			std::vector<std::string> inc_places;
            std::string out_model;
            std::string out_queries;
            std::set<size_t> query_numbers;
	};

	std::ostream& operator<<(std::ostream& out, const VerificationOptions& options);
}

#endif /* VERIFICATIONOPTIONS_HPP_ */
