#include <iostream>

#include "Core/TAPN/TAPNModelBuilder.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/Query/UpwardClosedVisitor.hpp"
#include "Core/Query/NormalizationVisitor.hpp"
#include "Core/Query/ToStringVisitor.hpp"
#include "Core/Query/BadPlaceVisitor.hpp"

#include "ReachabilityChecker/Search/SearchStrategy.hpp"
#include "ReachabilityChecker/Search/BFS.hpp"
#include "ReachabilityChecker/Search/DFS.hpp"
#include "ReachabilityChecker/Search/CoverMostSearch.hpp"
#include "ReachabilityChecker/Search/RandomSearch.hpp"

#include "Core/SymbolicMarking/UppaalDBMMarkingFactory.hpp"
#include "Core/SymbolicMarking/DiscreteInclusionMarkingFactory.hpp"

#include "ReachabilityChecker/Trace/trace_exception.hpp"

#include <unfoldtacpn.h>
#include <Colored/ColoredPetriNetBuilder.h>

#include <fstream>

using namespace std;
using namespace VerifyTAPN;
using namespace VerifyTAPN::TAPN;
using namespace boost;

std::shared_ptr<MarkingFactory> CreateFactory(const VerificationOptions& options, TAPN::TimedArcPetriNet* tapn)
{
	switch(options.GetFactory())
	{
	case OLD_FACTORY:
		return std::make_shared<UppaalDBMMarkingFactory>(tapn);
	default:// Note that the constructor of DiscreteInclusionMarkingFactory automatically disables discrete inclusion
		    // if DEFAULT is chosen
		return std::make_shared<DiscreteInclusionMarkingFactory>(tapn, options);
	};
}

std::shared_ptr<SearchStrategy> CreateSearchStrategy(TAPN::TimedArcPetriNet* tapn, SymbolicMarking* initialMarking, AST::Query* query, const VerificationOptions& options, MarkingFactory* factory)
{
	std::shared_ptr<SearchStrategy> strategy;

	switch(options.GetSearchType())
	{
	case DEPTHFIRST:
		strategy = std::make_shared<DFS>(*tapn, initialMarking, query, options, factory);
		break;
	case COVERMOST:
		if(options.GetFactory() == DISCRETE_INCLUSION)
			strategy = std::make_shared<CoverMostSearch>(*tapn, initialMarking, query, options, factory);
		else
			strategy = std::make_shared<BFS>(*tapn, initialMarking, query, options, factory);
		break;
	case RANDOM:
		strategy = std::make_shared<RandomSearch>(*tapn, initialMarking, query, options, factory);
		break;
	default:
		strategy = std::make_shared<BFS>(*tapn, initialMarking, query, options, factory);
		break;
	}
	strategy->Init();
	return strategy;
}

void FixIncSet(const TimedArcPetriNet& tapn, VerificationOptions& options){
	std::vector<std::string>& inc = options.GetIncPlaces();
	if(inc.size() == 1 && inc[0] == "*NONE*"){
		inc.clear();
	}else if(inc.size() == 1 && inc[0] == "*ALL*"){
		inc.clear();
		const TimedPlace::Vector& places = tapn.GetPlaces();
		for(TimedPlace::Vector::const_iterator it = places.begin(); it != places.end(); it++){
			inc.push_back((*it)->GetName());
		}
	}
}

void RemoveBadPlacesFromINC(const AST::Query& normalizedQuery, const TimedArcPetriNet& tapn, VerificationOptions& options){
	FixIncSet(tapn, options);

	AST::BadPlaceVisitor visitor;
	visitor.FindBadPlaces(normalizedQuery);

	std::vector<int>& badPlaces = visitor.GetBadPlaces();
	std::vector<std::string>& inc = options.GetIncPlaces();
	for(std::vector<int>::iterator it = badPlaces.begin(); it != badPlaces.end(); it++){
		const std::string& name = tapn.GetPlace(*it).GetName();
		inc.erase(std::remove(inc.begin(), inc.end(), name), inc.end());
	}
}

std::pair<std::vector<int>, std::unique_ptr<TAPN::TimedArcPetriNet>>
build_net(unfoldtacpn::ColoredPetriNetBuilder& builder) {
    TAPNModelBuilder modelBuilder;
    builder.unfold(modelBuilder);
    return {modelBuilder.initialMarking(), std::unique_ptr<TAPN::TimedArcPetriNet>
        {modelBuilder.make_tapn()}};
}

std::pair<std::vector<int>, std::unique_ptr<TAPN::TimedArcPetriNet>>
parse_net_file(unfoldtacpn::ColoredPetriNetBuilder& builder, const std::string& filename) {
    std::ifstream mf(filename);
    builder.parseNet(mf);
    mf.close();
    return build_net(builder);
}

int main(int argc, char* argv[])
{
	srand ( time(nullptr) );

	VerificationOptions options = VerificationOptions::parse(argc, argv);

    unfoldtacpn::ColoredPetriNetBuilder builder;
    auto [initialVector, tapn] = parse_net_file(builder, options.GetInputFile());

    if(tapn != nullptr)
    {
        if(!options.getOutputModelFile().empty())
        {
            std::fstream of(options.getOutputModelFile(), std::ios::out);
            tapn->toTAPNXML(of, initialVector);
            of.close();
        }
	} else {
		std::cout << "There was an error parsing the model file: " << options.GetInputFile() << std::endl;
		return 1;
	}

	tapn->Initialize(options.GetUntimedPlacesEnabled());

	std::unique_ptr<AST::Query> query;
	try{
        auto qfile = std::ifstream(options.QueryFile());
        if(!qfile)
        {
            std::cerr << "Could not open " << options.QueryFile() << std::endl;
            std::exit(-1);
        }
        auto qnums = options.getQueryNumbers();
        size_t quid = 0;
        std::vector<std::pair<unfoldtacpn::PQL::Condition_ptr, std::string>> ast_queries;
        if (qfile.peek() == '<') { // assumed XML
            if (qnums.empty()) {
                std::cerr << "Missing query-indexes for query-file (which is identified as XML-format), assuming only first query is to be verified" << std::endl;
                qnums.emplace(0);
            }
            quid = *qnums.begin();
            ast_queries = unfoldtacpn::parse_xml_queries(builder, qfile, qnums);
        } else {
            // not xml
            if (qnums.size() > 0) {
                std::cerr << "Queries not provided in XML-format, --xml-queries argument is ignored" << std::endl;
            }
            ast_queries = unfoldtacpn::parse_string_queries(builder, qfile);
        }
        if (ast_queries.empty()) {
            std::cerr << "There was an error parsing " << options.QueryFile() << std::endl;
            std::exit(-1);
        }

        if (!options.getOutputQueryFile().empty()) {
            std::fstream of(options.getOutputQueryFile(), std::ios::out);
            unfoldtacpn::PQL::to_xml(of, ast_queries);
        }
        query = AST::toAST(ast_queries[quid].first, *tapn);
	}catch(...){
		std::cout << "There was an error parsing the query file." << std::endl;
		return 1;
	}

	if(options.GetFactory() == DISCRETE_INCLUSION)
	{
		AST::NormalizationVisitor visitor;
		AST::Query* normalized = visitor.Normalize(*query);
		RemoveBadPlacesFromINC(*normalized, *tapn, options);
		delete normalized;
		/*AST::UpwardClosedVisitor visitor;
		bool upward_closed = visitor.IsUpwardClosed(*query);
		if(!upward_closed)
		{
			options.SetFactory(DEFAULT);
			std::cout << "The specified query is not upward closed and is therefore not supported by the discrete inclusion optimization." << std::endl;
			return 1;
		}*/
	}

	auto factory = CreateFactory(options, tapn.get());
    std::vector<int> initialPlacement;
    for(size_t i = 0; i < initialVector.size(); ++i) // convert into placement vector
        for(size_t n = 0; n < initialVector[i]; ++n)
            initialPlacement.emplace_back(i);
	std::shared_ptr<SymbolicMarking> initialMarking(factory->InitialMarking(initialPlacement));
	if(initialMarking->NumberOfTokens() > options.GetKBound())
	{

		std::cout << "The specified k-bound (" << options.GetKBound() << ") is less than the number of tokens in the initial marking (" << initialMarking->NumberOfTokens() << ")." << std::endl;
		return 1;
	}

	auto strategy = CreateSearchStrategy(tapn.get(), initialMarking.get(), query.get(), options, factory.get());

	std::cout << options << std::endl;
	bool result = strategy->Verify();
	std::cout << strategy->GetStats() << std::endl;
	strategy->PrintTransitionStatistics();
	std::cout << "Query is " << (result ? "satisfied" : "NOT satisfied") << "." << std::endl;
	std::cout << "Max number of tokens found in any reachable marking: ";
	if(strategy->MaxUsedTokens() == options.GetKBound() + 1)
		std::cout << ">" << options.GetKBound() << std::endl;
	else
		std::cout << strategy->MaxUsedTokens() << std::endl;

	try{
		strategy->PrintTraceIfAny(result);
	}catch(const trace_exception& e){
		std::cout << "There was an error generating a trace. This is a bug. Please report this on launchpad and attach your TAPN model and this error message: ";
		std::cout << e.what() << std::endl;
		return 1;
	}

	return 0;
}


