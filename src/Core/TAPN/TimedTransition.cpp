#include "TimedTransition.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void TimedTransition::Print(std::ostream& out) const
		{
			out << GetName() << "(" << index << ")";
		}

		void TimedTransition::AddToPreset(const TimedInputArc* arc)
		{
			if(arc)
			{
				preset.push_back(arc);
			}
		}

		void TimedTransition::AddTransportArcGoingThrough(const TransportArc* arc)
		{
			if(arc)
			{
				transportArcs.push_back(arc);
			}
		}

		void TimedTransition::AddIncomingInhibitorArc(const InhibitorArc* arc)
		{
			if(arc)
			{
				inhibitorArcs.push_back(arc);
			}
		}

		void TimedTransition::AddToPostset(const OutputArc* arc)
		{
			if(arc)
			{
				postset.push_back(arc);
			}
		}
	}
}




