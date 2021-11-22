#ifndef SUCCESSORGENERATOR_HPP_
#define SUCCESSORGENERATOR_HPP_

#include "../Core/TAPN/TimedArcPetriNet.hpp"
#include "Successor.hpp"
#include "../Core/VerificationOptions.hpp"
#include "../Core/SymbolicMarking/MarkingFactory.hpp"

#include <vector>
#include <boost/smart_ptr.hpp>

namespace VerifyTAPN {
	class SymbolicMarking;

	class SuccessorGenerator {
	public:
	    SuccessorGenerator(const TAPN::TimedArcPetriNet & tapn, const MarkingFactory & factory, const VerificationOptions & options, unsigned int tokensInInitialMarking)
	    :tapn(tapn), factory(factory), arcsArray(tapn.GetNumberOfConsumingArcs()),
                nInputArcs(tapn.GetNumberOfConsumingArcs()),
                transitionStatistics(tapn.GetNumberOfTransitions()),
                numberOfTransitions(tapn.GetNumberOfTransitions()),
                options(options),
                tokenIndices(nInputArcs * options.GetKBound()),
                maxUsedTokens(tokensInInitialMarking)
	    {
	        ClearTransitionsArray();
	    }

	    ;
	    virtual ~SuccessorGenerator()
	    {}

	    ;
	public:
	    void GenerateDiscreteTransitionsSuccessors(const SymbolicMarking & marking, std::vector<VerifyTAPN::Successor> & succ);
	    void PrintTransitionStatistics(std::ostream & out) const;
	    inline void ClearAll()
	    {
	        ClearArcsArray();
	        ClearTokenIndices();
	    }

	    inline void ClearArcsArray()
	    {
            std::fill(arcsArray.begin(), arcsArray.end(), 0);
	    }

	    inline void ClearTransitionsArray() {
            std::fill(transitionStatistics.begin(), transitionStatistics.end(), 0);
	    }

	    inline void ClearTokenIndices()
	    {
            std::fill(tokenIndices.begin(), tokenIndices.end(), 0);
	    }

	    unsigned int MaxUsedTokens() const
	    {
	    	return maxUsedTokens;
	    }
	private:
	    void CollectArcsAndAppropriateTokens(const TAPN::TimedTransition::Vector & transitions, const SymbolicMarking *marking);
	    void GenerateSuccessors(const TAPN::TimedTransition::Vector & transitions, const SymbolicMarking *marking, std::vector<Successor> & succ);
	    void GenerateSuccessorForCurrentPermutation(const TAPN::TimedTransition & currTransition, const unsigned int *indices, const unsigned int currTransitionIndex, const unsigned int presetSize, const SymbolicMarking *marking, std::vector<Successor> & succ);
	private:
	    bool IsTransitionEnabled(const TAPN::TimedTransition& transition, const SymbolicMarking* marking, unsigned int currTransitionIndex, unsigned int presetSize) const;
	    void UpdateArcInfo(const SymbolicMarking *marking, int currInputPlaceIndex, const TAPN::TimeInterval & ti, unsigned int & currInputArcIdx);

	    void MakeIdentity(IndirectionTable& mapping, unsigned int size) const;
	    void UpdateTraceMapping(IndirectionTable& mapping, unsigned int tokenToRemove) const;
        uint64_t toIndex(int currInputArcIdx, int nTokensFromCurrInputPlace) {
            return currInputArcIdx*options.GetKBound() + nTokensFromCurrInputPlace;
        }
        int getIndicy(int currInputArcIdx, uint32_t nTokensFromCurrInputPlace) {
            return tokenIndices[toIndex(currInputArcIdx, nTokensFromCurrInputPlace)];
        }
	private:
	    const TAPN::TimedArcPetriNet& tapn;
		const MarkingFactory& factory;
		std::vector<uint32_t> arcsArray;
		unsigned int nInputArcs;
		std::vector<uint32_t> transitionStatistics;
		const int numberOfTransitions;
		const VerificationOptions& options;
		std::vector<int> tokenIndices;
		unsigned int maxUsedTokens;
	};

}
#endif /* SUCCESSORGENERATOR_HPP_ */
