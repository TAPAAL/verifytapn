#ifndef TRANSPORTARC_HPP_
#define TRANSPORTARC_HPP_

#include <vector>
#include "TimeInterval.hpp"

namespace VerifyTAPN
{
	namespace TAPN
	{
		class TimedTransition;
		class TimedPlace;

		class TransportArc
		{
		public:
			typedef std::vector< TransportArc* > Vector;
            typedef std::vector< const TransportArc* > CVector;
		public:
			TransportArc(
					const TimedPlace* source,
					const TimedTransition* transition,
					const TimedPlace* destination,
					const TAPN::TimeInterval& interval
			) : interval(interval), source(source), transition(transition), destination(destination) {};

			virtual ~TransportArc() {};
		public:
			inline const TimedPlace& Source() const { return *source; }
			inline const TimedTransition& Transition() const { return *transition; }
			inline const TimedPlace& Destination() const { return *destination; }
			inline const TimeInterval& Interval() const { return interval; }

		public: // Inspectors
            void Print(std::ostream& out) const;
		private:
			const TAPN::TimeInterval interval;
			const TimedPlace* source;
			const TimedTransition* transition;
			const TimedPlace* destination;
		};

		inline std::ostream& operator<<(std::ostream& out, const TransportArc& arc)
		{
			arc.Print(out);
			return out;
		}
	}
}

#endif /* TRANSPORTARC_HPP_ */
