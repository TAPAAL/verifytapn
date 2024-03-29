#ifndef VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_
#define VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_

#include <string>
#include <vector>
#include "TimedInputArc.hpp"
#include "TransportArc.hpp"
#include "InhibitorArc.hpp"
#include "OutputArc.hpp"

namespace VerifyTAPN {

    class SymMarking;

	namespace TAPN {
		class TimedArcPetriNet;

		class TimedTransition
		{
		public: // typedefs
			typedef std::vector< TimedTransition* > Vector;
            typedef std::vector< const TimedTransition* > CVector;
		public:
			TimedTransition(size_t index, const std::string& name, const std::string& id, double x, double y)
            : name(name), id(id), preset(), postset(), transportArcs(), index(index), _x(x), _y(y) { };
			TimedTransition() : name("*EMPTY*"), id("-1"), preset(), postset(), transportArcs(), index(-1) { };
			virtual ~TimedTransition() { /* empty */ }

		public: // modifiers
			void AddToPreset(const TimedInputArc* arc);
			void AddToPostset(const OutputArc* arc);
			void AddTransportArcGoingThrough(const TransportArc* arc);
			void AddIncomingInhibitorArc(const InhibitorArc* arc);

		public: // inspectors
			inline const std::string& GetName() const { return name; };
			inline const std::string& GetId() const { return id; };
			void Print(std::ostream&) const;
			inline const TimedInputArc::CVector& GetPreset() const { return preset; }
			inline const TransportArc::CVector& GetTransportArcs() const { return transportArcs; }
			inline const InhibitorArc::CVector& GetInhibitorArcs() const { return inhibitorArcs; }
			inline const unsigned int GetPresetSize() const { return NumberOfInputArcs() + NumberOfTransportArcs(); }
			inline const OutputArc::CVector& GetPostset() const { return postset; }
			inline const unsigned int GetPostsetSize() const { return postset.size() + transportArcs.size(); }
			inline unsigned int NumberOfInputArcs() const { return preset.size(); };
			inline unsigned int NumberOfTransportArcs() const { return transportArcs.size(); };
		//	bool isEnabledBy(const TimedArcPetriNet& tapn, const VerifyTAPN::SymMarking& marking) const;
			inline const bool isConservative() const { return preset.size() == postset.size(); }
			inline unsigned int GetIndex() const { return index; };
            std::pair<double,double> getPosition() const { return {_x, _y}; }

		private: // data
			std::string name;
			std::string id;
			TimedInputArc::CVector preset;
			OutputArc::CVector postset;
			TransportArc::CVector transportArcs;
			InhibitorArc::CVector inhibitorArcs;
			unsigned int index;
            double _x;
            double _y;
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
