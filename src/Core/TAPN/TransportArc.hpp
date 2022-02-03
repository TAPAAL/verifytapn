#ifndef TRANSPORTARC_HPP_
#define TRANSPORTARC_HPP_

#include "TimeInterval.hpp"

#include <vector>
#include <memory>

namespace VerifyTAPN
{
	namespace TAPN
	{
		class TimedTransition;
		class TimedPlace;

		class TransportArc
		{
		public:
			typedef std::vector< std::shared_ptr<TransportArc> > Vector;
			typedef std::vector< TransportArc* > NakedPtrVector;
		public:
			TransportArc(
					const std::shared_ptr<TimedPlace>& source,
					const std::shared_ptr<TimedTransition>& transition,
					const std::shared_ptr<TimedPlace>& destination,
					const TAPN::TimeInterval& interval
			) : interval(interval), source(source), transition(transition), destination(destination) {};

			virtual ~TransportArc() {};
		public:
			inline TimedPlace& Source() { return *source; }
			inline TimedTransition& Transition() { return *transition; }
			inline TimedPlace& Destination() { return *destination; }
			inline const TimeInterval& Interval() { return interval; }

		public: // Inspectors
				void Print(std::ostream& out) const;
		private:
			const TAPN::TimeInterval interval;
			const std::shared_ptr<TimedPlace> source;
			const std::shared_ptr<TimedTransition> transition;
			const std::shared_ptr<TimedPlace> destination;
		};

		inline std::ostream& operator<<(std::ostream& out, const TransportArc& arc)
		{
			arc.Print(out);
			return out;
		}
	}
}

#endif /* TRANSPORTARC_HPP_ */
