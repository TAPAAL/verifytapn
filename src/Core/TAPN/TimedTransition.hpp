#ifndef VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_
#define VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_

#include "TimedInputArc.hpp"
#include "TransportArc.hpp"
#include "InhibitorArc.hpp"
#include "OutputArc.hpp"

#include <string>
#include <vector>
#include <memory>


namespace VerifyTAPN {

class SymMarking;

	namespace TAPN {
		class TimedArcPetriNet;

		class TimedTransition
		{
		public: // typedefs
			typedef std::vector< std::shared_ptr<TimedTransition> > Vector;
		public:
			TimedTransition(const std::string& name, const std::string& id) : name(name), id(id), preset(), postset(), transportArcs(), index(-1) { };
			TimedTransition() : name("*EMPTY*"), id("-1"), preset(), postset(), transportArcs(), index(-1) { };
			virtual ~TimedTransition() { /* empty */ }

		public: // modifiers
			void AddToPreset(const std::shared_ptr<TimedInputArc>& arc);
			void AddToPostset(const std::shared_ptr<OutputArc>& arc);
			void AddTransportArcGoingThrough(const std::shared_ptr<TransportArc>& arc);
			void AddIncomingInhibitorArc(const std::shared_ptr<InhibitorArc>& arc);

			inline void SetIndex(int i) { index = i; };
		public: // inspectors
			inline const std::string& GetName() const { return name; };
			inline const std::string& GetId() const { return id; };
			void Print(std::ostream&) const;
			inline const TimedInputArc::NakedPtrVector& GetPreset() const { return preset; }
			inline const TransportArc::NakedPtrVector& GetTransportArcs() const { return transportArcs; }
			inline const InhibitorArc::NakedPtrVector& GetInhibitorArcs() const { return inhibitorArcs; }
			inline const unsigned int GetPresetSize() const { return NumberOfInputArcs() + NumberOfTransportArcs(); }
			inline const OutputArc::NakedPtrVector& GetPostset() const { return postset; }
			inline const unsigned int GetPostsetSize() const { return postset.size() + transportArcs.size(); }
			inline unsigned int NumberOfInputArcs() const { return preset.size(); };
			inline unsigned int NumberOfTransportArcs() const { return transportArcs.size(); };
		//	bool isEnabledBy(const TimedArcPetriNet& tapn, const VerifyTAPN::SymMarking& marking) const;
			inline const bool isConservative() const { return preset.size() == postset.size(); }
			inline unsigned int GetIndex() const { return index; };

		private: // data
			std::string name;
			std::string id;
			TimedInputArc::NakedPtrVector preset;
			OutputArc::NakedPtrVector postset;
			TransportArc::NakedPtrVector transportArcs;
			InhibitorArc::NakedPtrVector inhibitorArcs;
			unsigned int index;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedTransition& transition)
		{
			transition.Print(out);
			return out;
		}

		// TAPAAL does not allow multiple places with the same name,
		// thus it is enough to use the name to determine equality.
		inline bool operator==(TimedTransition const& a, TimedTransition const& b)
		{
			return a.GetName() == b.GetName();
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_ */
