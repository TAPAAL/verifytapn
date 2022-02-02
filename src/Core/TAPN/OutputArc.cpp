#include "OutputArc.hpp"
#include "TimedPlace.hpp"
#include "TimedTransition.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void OutputArc::Print(std::ostream& out) const
		{
			out << "From " << transition->GetName() << " to " << place->GetName();
		}

		const TimedPlace& OutputArc::OutputPlace() const
		{
			return *place;
		}

		const TimedTransition& OutputArc::InputTransition() const
		{
			return *transition;
		}
	}
}
