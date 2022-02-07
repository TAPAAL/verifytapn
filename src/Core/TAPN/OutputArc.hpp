#ifndef VERIFYTAPN_TAPN_OUTPUTARC_HPP_
#define VERIFYTAPN_TAPN_OUTPUTARC_HPP_

#include <vector>
#include <ostream>

namespace VerifyTAPN {
	namespace TAPN {
		class TimedPlace;
		class TimedTransition;

		class OutputArc
		{
		public: // typedefs
			typedef std::vector<OutputArc* > Vector;
            typedef std::vector<const OutputArc* > CVector;
		public:
			OutputArc(const TimedTransition* transition, const TimedPlace* place)
				: transition(transition), place(place) { };
			virtual ~OutputArc() { /* empty */ }

		public: // modifiers
			const TimedPlace& OutputPlace() const;
			const  TimedTransition& InputTransition() const;

		public: // inspectors
			void Print(std::ostream& out) const;
		private:
			const TimedTransition* transition;
			const TimedPlace* place;
		};

		inline std::ostream& operator<<(std::ostream& out, const OutputArc& arc)
		{
			arc.Print(out);
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_OUTPUTARC_HPP_ */
