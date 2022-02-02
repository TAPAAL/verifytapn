#ifndef SUCCESSORGENERATOR_HPP_
#define SUCCESSORGENERATOR_HPP_

#include "../Core/TAPN/TimedArcPetriNet.hpp"
#include "Successor.hpp"
#include "../Core/VerificationOptions.hpp"
#include "../Core/SymbolicMarking/MarkingFactory.hpp"

#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace VerifyTAPN {
	class SymbolicMarking;

	class SuccessorGenerator {
	public:
	    SuccessorGenerator(const TAPN::TimedArcPetriNet & tapn, const MarkingFactory & factory, const VerificationOptions & options, unsigned int tokensInInitialMarking)
	    :tapn(tapn), factory(factory), arcsArray(), nInputArcs(tapn.GetNumberOfConsumingArcs()), transitionStatistics(), numberOfTransitions(tapn.GetNumberOfTransitions()), options(options), tokenIndices(), maxUsedTokens(tokensInInitialMarking)
	    {
	        arcsArray = new unsigned [nInputArcs];
	        transitionStatistics = new unsigned [numberOfTransitions];
	        tokenIndices = new boost::numeric::ublas::matrix<int>(nInputArcs, options.GetKBound());
	        ClearTransitionsArray();
	    }

	    ;
	    virtual ~SuccessorGenerator()
	    {
	        delete [] arcsArray;
	        delete tokenIndices;
		delete [] transitionStatistics;
	    }

	    ;
	public:
	    void GenerateDiscreteTransitionsSuccessors(const SymbolicMarking & marking, std::vector<VerifyTAPN::Successor> & succ);
	public:
	    void Print(std::ostream & out) const;
	    void PrintTransitionStatistics(std::ostream & out) const;
	public:
	    inline void ClearAll()
	    {
	        ClearArcsArray();
	        ClearTokenIndices();
	    }

	    inline void ClearArcsArray()
	    {
	        memset(arcsArray, 0, nInputArcs * sizeof (arcsArray[0]));
	    }

	    inline void ClearTransitionsArray() {
	    	memset(transitionStatistics, 0, numberOfTransitions * sizeof (transitionStatistics[0]));
	    }

	    inline void ClearTokenIndices()
	    {
	        tokenIndices->clear();
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
	private:
	    const TAPN::TimedArcPetriNet& tapn;
		const MarkingFactory& factory;
		unsigned int* arcsArray;
		unsigned int nInputArcs;
		unsigned int* transitionStatistics;
		const int numberOfTransitions;
		const VerificationOptions& options;
		boost::numeric::ublas::matrix<int>* tokenIndices;
		unsigned int maxUsedTokens;
	};

	inline std::ostream& operator<<(std::ostream& out, const VerifyTAPN::SuccessorGenerator& succGen)
	{
		succGen.Print( out );
		return out;
	}
}
#endif /* SUCCESSORGENERATOR_HPP_ */
