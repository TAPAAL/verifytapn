#ifndef VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_
#define VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_

#include <vector>
#include "TimeInterval.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		class TimedTransition;
		class TimedPlace;

		class TimedInputArc
		{
		public: // typedefs
			typedef std::vector< TimedInputArc* > Vector;
            typedef std::vector< const TimedInputArc* > CVector;
		public:
			TimedInputArc(const TimedPlace* place, const TimedTransition* transition) : interval(), place(place), transition(transition) { };
			TimedInputArc(const TimedPlace* place, const TimedTransition* transition, const TimeInterval& interval) : interval(interval), place(place), transition(transition) { };
			virtual ~TimedInputArc() { /* empty */}

		public: // modifiers
			inline const TimedPlace& InputPlace() const { return *place; }
			inline const TimedTransition& OutputTransition() const { return *transition; }
			inline const TimeInterval& Interval() const { return interval; }

		public: // Inspectors
			void Print(std::ostream& out) const;
		private:
			const TimeInterval interval;
			const TimedPlace* place;
			const TimedTransition* transition;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedInputArc& arc)
		{
			arc.Print(out);
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_ */
