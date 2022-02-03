#include "SuccessorGenerator.hpp"
#include "../Core/TAPN/TimedInputArc.hpp"
#include "../Core/TAPN/Pairing.hpp"
#include "../Core/SymbolicMarking/SymbolicMarking.hpp"

#include <algorithm>
#include <set>

namespace VerifyTAPN {
	void SuccessorGenerator::GenerateDiscreteTransitionsSuccessors(const SymbolicMarking& marking, std::vector<Successor>& succ)
	{
		ClearAll();

		//this->Print(std::cout);

		const TAPN::TimedTransition::Vector& transitions = tapn.GetTransitions();

		CollectArcsAndAppropriateTokens(transitions, &marking);
		GenerateSuccessors(transitions, &marking, succ);
	}

    void SuccessorGenerator::UpdateArcInfo(const SymbolicMarking *marking, int currInputPlaceIndex, const TAPN::TimeInterval & ti, unsigned int & currInputArcIdx)
    {
        unsigned int nTokensFromCurrInputPlace = 0;
        for(unsigned int i = 0; i < marking->NumberOfTokens(); i++)
		{
			int placeIndex = marking->GetTokenPlacement(i);


			if(placeIndex == currInputPlaceIndex)
			{
				bool potentiallyUsable = marking->PotentiallySatisfies(i, ti);

				if(potentiallyUsable)
				{
					assert(currInputArcIdx <= nInputArcs);
					assert(nTokensFromCurrInputPlace <= static_cast<unsigned int>(options.GetKBound()));

					arcsArray[currInputArcIdx] = arcsArray[currInputArcIdx] + 1;
                    tokenIndices[toIndex(currInputArcIdx, nTokensFromCurrInputPlace)] = i;
					nTokensFromCurrInputPlace++;
				}
			}
		}
    }

	// Collects the number of tokens of potentially appropriate age for each input into arcsArray.
	// Note that arcs array should be sorted by transition. Further, if a token is potentially of
	// appropriate age we add the token index to the tokenIndices matrix for use when generating successors.
	void SuccessorGenerator::CollectArcsAndAppropriateTokens(const TAPN::TimedTransition::Vector& transitions, const SymbolicMarking* marking)
	{
		unsigned int currInputArcIdx = 0;

		for(auto& trans : transitions)
		{
			for(auto* ta : trans->GetTransportArcs())
			{
				const TAPN::TimeInterval& ti = ta->Interval();
				int currInputPlaceIndex = tapn.GetPlaceIndex(ta->Source());

				UpdateArcInfo(marking, currInputPlaceIndex, ti, currInputArcIdx);
				currInputArcIdx++;
			}

			for(auto* ia : trans->GetPreset())
			{
				const TAPN::TimeInterval& ti = ia->Interval();
				int currInputPlaceIndex = tapn.GetPlaceIndex(ia->InputPlace());

				UpdateArcInfo(marking, currInputPlaceIndex, ti, currInputArcIdx);
				currInputArcIdx++;
			}
		}
	}

	// Generate for each enabled transition a successor for each
	// permutation of tokens of appropriate age from the token matrix.
	void SuccessorGenerator::GenerateSuccessors(const TAPN::TimedTransition::Vector& transitions, const SymbolicMarking* marking, std::vector<Successor>& succ)
	{
		int currentTransitionIndex = 0;
		int realCurrentTransitionIndex = 0;
		for(TAPN::TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); ++iter)
		{
			unsigned int presetSize = (*iter)->GetPresetSize();
			if(IsTransitionEnabled(*(*iter),marking,currentTransitionIndex, presetSize))
			{
				// The indicesOfCurrentPermutation array stores the column indices
				// into the token matrix for the current permutation of input tokens.
				// I.e. it is used to select which tokens to consume when firing the transition.
				unsigned int indicesOfCurrentPermutation[presetSize];
				for(unsigned int i = 0; i < presetSize; ++i)
					indicesOfCurrentPermutation[i] = 0;

				bool done = false;
				transitionStatistics[realCurrentTransitionIndex]++;
				while(true)
				{

					GenerateSuccessorForCurrentPermutation(*(*iter), indicesOfCurrentPermutation, currentTransitionIndex, presetSize, marking, succ);

					// Generate next permutation of input tokens
					int j = presetSize - 1;
					if (j<0) { break; }

					while(true)
					{
						indicesOfCurrentPermutation[j] += 1;
						if(indicesOfCurrentPermutation[j] < arcsArray[currentTransitionIndex+j])
							break;
						indicesOfCurrentPermutation[j] = 0;
						j -= 1;
						if(j < 0)
						{
							done = true;
							break;
						}
					}
					if(done)
						break;
				}
			}

			realCurrentTransitionIndex++;
			currentTransitionIndex += presetSize; // jump to next start of next transition in arcsArray
		}
	}

	// After the input arcs and tokens have been collected
	// you can check the enabledness of a transition by specifying
	// the index of the first input arc of the transition
	// along with the size of the preset of the transition
	bool SuccessorGenerator::IsTransitionEnabled(const TAPN::TimedTransition& transition, const SymbolicMarking* marking, unsigned int currTransitionIndex, unsigned int presetSize) const
	{
		for(auto* ia : transition.GetInhibitorArcs())
		{
			int sourcePlaceIndex = ia->InputPlace().GetIndex();

			for(unsigned int i = 0; i < marking->NumberOfTokens(); i++)
			{
				if(marking->GetTokenPlacement(i) == sourcePlaceIndex)
					return false;
			}
		}

		for(unsigned int i = currTransitionIndex; i < currTransitionIndex + presetSize; ++i)
		{
			if(arcsArray[i] <= 0)
				return false;
		}
		return true;
	}

	// Generates a successor node for the current permutation of input tokens
	void SuccessorGenerator::GenerateSuccessorForCurrentPermutation(const TAPN::TimedTransition& transition, const unsigned int* currentPermutationindices, const unsigned int currentTransitionIndex, const unsigned int presetSize, const SymbolicMarking* marking, std::vector<Successor>& succ)
	{
		unsigned int kBound = options.GetKBound();
		bool trace = options.GetTrace() != NONE;

		const Pairing& pairing = tapn.GetPairing(transition);
		const TAPN::TimedInputArc::NakedPtrVector& preset = transition.GetPreset();
		std::set<int> tokensToRemove; // sets are sorted internally in ascending order. THIS MUST BE THE CASE OR THE CODE WONT WORK!
		SymbolicMarking* next = factory.Clone(*marking);

		for(unsigned int i = 0; i < transition.NumberOfTransportArcs(); ++i)
		{
			TAPN::TransportArc* ta = transition.GetTransportArcs()[i];
			const TAPN::TimeInterval& ti = ta->Interval();

			int tokenIndex = tokenIndices[toIndex(currentTransitionIndex+i, currentPermutationindices[i])];
			int outputPlaceIndex = tapn.GetPlaceIndex(ta->Destination());

			// constrain dbm with the guard of the input arc
			next->Constrain(tokenIndex, ti);
			next->MoveToken(tokenIndex, outputPlaceIndex);

			if(next->IsEmpty())
			{
				delete next;
				return;
			}

		}

		int offset = transition.NumberOfTransportArcs();
		// move all tokens that are currently in the net
		for(unsigned int i = 0; i < transition.NumberOfInputArcs(); ++i)
		{
			auto* inputArc = preset[i];
			int inputPlace = tapn.GetPlaceIndex(inputArc->InputPlace());
			const TAPN::TimeInterval& ti = inputArc->Interval();
			const std::list<int>& outputPlaces = pairing.GetOutputPlacesFor(inputPlace);

			// only BOTTOM is allowed to have more than 1 associated output place
			assert(outputPlaces.size() <= 1);

			for(auto outputPlaceIndex : outputPlaces)
			{
				// change placement
				int tokenIndex = tokenIndices[toIndex(currentTransitionIndex+offset+i, currentPermutationindices[offset+i])];

				// constrain dbm with the guard of the input arc
				next->Constrain(tokenIndex, ti);

				if(outputPlaceIndex == TAPN::TimedPlace::BottomIndex())
					tokensToRemove.insert(tokenIndex);
				else
					next->MoveToken(tokenIndex, outputPlaceIndex);

				if(next->IsEmpty())
				{
					delete next;
					return;
				}
			}
		}

		// reset clocks of moved tokens
		for (unsigned int i = transition.NumberOfTransportArcs(); i < presetSize; ++i) {
			int tokenIndex = tokenIndices[toIndex(currentTransitionIndex+i, currentPermutationindices[i])];

			next->Reset(tokenIndex);
		}

		unsigned int prevMaxTokens = maxUsedTokens; // We record this in case the invariants makes next an invalid marking

		// check if we need to add or remove tokens in the successor marking
		int diff = presetSize - transition.GetPostsetSize();
		if(diff > 0) // preset bigger than postset, i.e. more tokens consumed than produced
		{
			assert(tokensToRemove.size() == static_cast<unsigned int>(std::abs(diff)));

			// remove tokens in placement and dbm
			next->RemoveTokens(tokensToRemove);
		}
		else if(diff < 0) // postset bigger than preset, i.e. more tokens produced than consumed
		{
			const std::list<int>& outputPlaces = pairing.GetOutputPlacesFor(TAPN::TimedPlace::BottomIndex());
			unsigned int tokensInSuccessor = next->NumberOfTokens() + outputPlaces.size();
			maxUsedTokens = std::max(maxUsedTokens, tokensInSuccessor);
			// Perform under-approximation in case the net is not k-bounded.
			// I.e. only allow up to k tokens in a given marking.
			if(tokensInSuccessor > kBound) {
				delete next;
				return;
			}

			next->AddTokens(outputPlaces);
		}

		for(unsigned int i = 0; i < next->NumberOfTokens(); i++)
		{
			const TAPN::TimeInvariant& invariant = tapn.GetPlace(next->GetTokenPlacement(i)).GetInvariant();
			next->Constrain(i, invariant);

			if(next->IsEmpty())
			{
				maxUsedTokens = prevMaxTokens; // This wasn't really a valid successor, so we "roll back" any potential update to the max used tokens counter.
				delete next;
				return;
			}
		}


		//next->DBMIntern(); // TODO: Handle interning internally in the marking

		// Store trace information
		if(trace){
			TraceInfo* traceInfo = new TraceInfo(marking->UniqueId(), transition.GetIndex(), next->UniqueId());
			IndirectionTable mapping;
			MakeIdentity(mapping, options.GetKBound());

			// handle transport arcs
			for(unsigned int i = 0; i < transition.NumberOfTransportArcs(); i++)
			{
				int tokenIndex = tokenIndices[toIndex(currentTransitionIndex+i, currentPermutationindices[i])];
				TAPN::TransportArc* ia = transition.GetTransportArcs()[i];
				const TAPN::TimeInterval& ti = ia->Interval();
				int indexAfterFiring = tokenIndex;
				for(auto to_remove : tokensToRemove)
				{
					if(to_remove < tokenIndex) indexAfterFiring--;
					assert(to_remove != tokenIndex);
				}
				assert(tokenIndex < static_cast<int>(marking->NumberOfTokens())); assert(indexAfterFiring < static_cast<int>(next->NumberOfTokens()));

				int placement = next->GetTokenPlacement(indexAfterFiring);
				Participant participant(tokenIndex, ti, placement, TRANSPORT_ARC);
				traceInfo->AddParticipant(participant);
			}

			// handle normal arcs
			for(unsigned int i = 0; i < transition.NumberOfInputArcs(); ++i)
			{
				int tokenIndex = tokenIndices[toIndex(currentTransitionIndex+offset+i, currentPermutationindices[offset+i])];
				TAPN::TimedInputArc* ia = preset[i];
				const TAPN::TimeInterval& ti = ia->Interval();
				int indexAfterFiring = tokenIndex;
				for(std::set<int>::iterator iter = tokensToRemove.begin(); iter != tokensToRemove.end(); ++iter)
				{
					if(*iter < tokenIndex) indexAfterFiring--;
					else if(*iter == tokenIndex){
						indexAfterFiring = -1;
						UpdateTraceMapping(mapping, tokenIndex);
						break;
					}
				}

				assert(tokenIndex < static_cast<int>(marking->NumberOfTokens()));
				assert(indexAfterFiring < static_cast<int>(next->NumberOfTokens()));
				int placement =  indexAfterFiring == -1 ? -1 : next->GetTokenPlacement(indexAfterFiring);
				Participant participant(tokenIndex, ti, placement, NORMAL_ARC);
				traceInfo->AddParticipant(participant);
			}

			// Check if we added tokens from bottom
			if(diff < 0){
				TAPN::TimeInterval inf;
				for(int i = diff; i < 0; i++){
					int indexAfterFiring = (marking->NumberOfTokens() - diff + i);
					Participant participant(indexAfterFiring,inf, next->GetTokenPlacement(indexAfterFiring), NORMAL_ARC);
					traceInfo->AddParticipant(participant);
				}
			}

			std::vector<TraceInfo::Invariant> invariants;
			for(unsigned int i = 0; i < marking->NumberOfTokens(); i++)
			{
				unsigned int place_index = marking->GetTokenPlacement(i);
				const TAPN::TimeInvariant& inv = tapn.GetPlace(place_index).GetInvariant();

				if(inv != TAPN::TimeInvariant::LS_INF)
				{
					invariants.push_back(TraceInfo::Invariant(i, inv));
				}
			}

			traceInfo->SetInvariants(invariants);
			traceInfo->SetTransitionFiringMapping(mapping);
			succ.push_back(Successor(next, traceInfo));
		}else{
			succ.push_back(Successor(next));
		}
		tokensToRemove.clear();
	}

	void SuccessorGenerator::MakeIdentity(IndirectionTable& mapping, unsigned int size) const
	{
		for(unsigned int i = 0; i < size; ++i)
		{
			mapping.AddMapping(i,i);
		}
	}

    void SuccessorGenerator::UpdateTraceMapping(IndirectionTable& mapping, unsigned int tokenToRemove) const
    {
    	IndirectionTable table;
    	unsigned int mapped_toRemove = mapping.MapForward(tokenToRemove);
		for(unsigned int i = 0; i < mapping.Size(); i++)
		{
			unsigned int mapped = mapping.MapForward(i);

			if(i == tokenToRemove) table.AddMapping(i, mapping.Size()-1);
			else if(mapped > mapped_toRemove) table.AddMapping(i, mapped-1);
			else table.AddMapping(i, mapped);
		}
    	mapping.Swap(table);
    }

	void SuccessorGenerator::PrintTransitionStatistics(std::ostream& out) const {
		out << std::endl << "TRANSITION STATISTICS";
		for (int i=0;i<tapn.GetNumberOfTransitions();i++) {
			if ((i) % 6 == 0) {
				out << std::endl;
				out << "<" << tapn.GetTransitions()[i]->GetName() << ":" << transitionStatistics[i] << ">";
			}
			else {
				out << " <"  <<tapn.GetTransitions()[i]->GetName() << ":" << transitionStatistics[i] << ">";
			}
		}
		out << std::endl;
		out << std::endl;
	}
}
