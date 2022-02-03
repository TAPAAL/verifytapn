#ifndef VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_
#define VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_

#include <iostream>
#include "TimedPlace.hpp"
#include "TimedTransition.hpp"
#include "TimedInputArc.hpp"
#include "TransportArc.hpp"
#include "InhibitorArc.hpp"
#include "OutputArc.hpp"
#include "google/sparse_hash_map"
#include "boost/functional/hash.hpp"
#include "Pairing.hpp"

namespace VerifyTAPN {

	namespace TAPN {

		class TimedArcPetriNet
		{
		public: // typedefs
			typedef google::sparse_hash_map<TimedTransition, VerifyTAPN::Pairing, boost::hash<TAPN::TimedTransition> > HashMap;
		public:// construction
			TimedArcPetriNet(const TimedPlace::Vector& places,
				const TimedTransition::Vector& transitions,
				const TimedInputArc::Vector& inputArcs,
				const OutputArc::Vector& outputArcs,
				const TransportArc::Vector& transportArcs,
				const InhibitorArc::Vector& inhibitorArcs)
				: places(places), transitions(transitions), inputArcs(inputArcs), outputArcs(outputArcs), transportArcs(transportArcs), inhibitorArcs(inhibitorArcs), maxConstant(0) { };
			virtual ~TimedArcPetriNet() { /* empty */ }

		public: // inspectors
			void Print(std::ostream& out) const;
			inline int GetPlaceIndex(const TimedPlace& p) const { return p.GetIndex(); };
			int GetPlaceIndex(const std::string& placeName) const;
			const TimedPlace& GetPlace(const int placeIndex) const { return *places[placeIndex]; }
			const TimedTransition::Vector& GetTransitions() const { return transitions; }
			const TimedInputArc::Vector& GetInputArcs() const { return inputArcs; }
			const TransportArc::Vector& GetTransportArcs() const { return transportArcs; }
			const InhibitorArc::Vector& GetInhibitorArcs() const { return inhibitorArcs; }
			const TimedPlace::Vector& GetPlaces() const { return places; };
			const int GetNumberOfConsumingArcs() const { return inputArcs.size() + transportArcs.size(); }
			const OutputArc::Vector& GetOutputArcs() const { return outputArcs; }
			const int GetNumberOfOutputArcs() const { return outputArcs.size(); }
			const int GetNumberOfTransitions() const { return transitions.size(); }
			int NumberOfPlaces() const { return places.size(); };
			const Pairing& GetPairing(const TimedTransition& t) const { return pairings.find(t)->second; }
			inline int MaxConstant() const { return maxConstant; };
			inline const bool IsPlaceUntimed(int index) const { return places[index]->IsUntimed(); }
		public: // modifiers
			void Initialize(bool useUntimedPlaces);


		private: // modifiers
			void MakeTAPNConservative();
			void GeneratePairings();
			void UpdateMaxConstant(const TimeInterval& interval);
			void UpdateMaxConstant(const TimeInvariant& invariant);
			void MarkUntimedPlaces();
			void FindMaxConstants();

		private: // data
			const TimedPlace::Vector places;
			TimedTransition::Vector transitions;
			const TimedInputArc::Vector inputArcs;
			const OutputArc::Vector outputArcs;
			const TransportArc::Vector transportArcs;
			const InhibitorArc::Vector inhibitorArcs;
			mutable HashMap pairings;
			int maxConstant;
		};

		inline std::ostream& operator<<(std::ostream& out, const VerifyTAPN::TAPN::TimedArcPetriNet& tapn)
		{
			tapn.Print( out );
			return out;
		}

		inline std::size_t hash_value(const TimedTransition& transition)
		{
			boost::hash<std::string> hasher;
			return hasher(transition.GetName());
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_ */
