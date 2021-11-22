#include "TimedTransition.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void TimedTransition::Print(std::ostream& out) const
		{
			out << GetName() << "(" << index << ")";
		}

		void TimedTransition::AddToPreset(const std::shared_ptr<TimedInputArc>& arc)
		{
			if(arc)
			{
				preset.push_back(arc.get());
			}
		}

		void TimedTransition::AddTransportArcGoingThrough(const std::shared_ptr<TransportArc>& arc)
		{
			if(arc)
			{
				transportArcs.push_back(arc.get());
			}
		}

		void TimedTransition::AddIncomingInhibitorArc(const std::shared_ptr<InhibitorArc>& arc)
		{
			if(arc)
			{
				inhibitorArcs.push_back(arc.get());
			}
		}

		void TimedTransition::AddToPostset(const std::shared_ptr<OutputArc>& arc)
		{
			if(arc)
			{
				postset.push_back(arc.get());
			}
		}
	}
}




