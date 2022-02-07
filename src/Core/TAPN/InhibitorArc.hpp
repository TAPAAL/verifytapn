#ifndef INHIBITORARC_HPP_
#define INHIBITORARC_HPP_

#include <vector>
#include "TimeInterval.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		class TimedTransition;
		class TimedPlace;

		class InhibitorArc {
			public: // typedefs
				typedef std::vector<InhibitorArc*> Vector;
                typedef std::vector<const InhibitorArc*> CVector;
			public:
				InhibitorArc(const TimedPlace* place, const TimedTransition* transition) : place(place), transition(transition) { };
				virtual ~InhibitorArc() { /* empty */ }

			public: // modifiers
				inline const TimedPlace& InputPlace() const { return *place; }
				inline const TimedTransition& OutputTransition() const { return *transition; }

			public: // Inspectors
				void Print(std::ostream& out) const;
			private:
				const TimedPlace* place;
				const TimedTransition* transition;
		};

		inline std::ostream& operator<<(std::ostream& out, const InhibitorArc& arc)
		{
			arc.Print(out);
			return out;
		}
	}

}

#endif /* INHIBITORARC_HPP_ */
