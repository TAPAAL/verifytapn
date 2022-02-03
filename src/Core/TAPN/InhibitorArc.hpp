#ifndef INHIBITORARC_HPP_
#define INHIBITORARC_HPP_

#include "TimeInterval.hpp"

#include <vector>
#include <memory>

namespace VerifyTAPN {
	namespace TAPN {
		class TimedTransition;
		class TimedPlace;

		class InhibitorArc {
			public: // typedefs
				typedef std::vector< std::shared_ptr<InhibitorArc> > Vector;
				typedef std::vector< InhibitorArc* > NakedPtrVector;
			public:
				InhibitorArc(const std::shared_ptr<TimedPlace>& place, const std::shared_ptr<TimedTransition>& transition) : place(place), transition(transition) { };
				virtual ~InhibitorArc() { /* empty */ }

			public: // modifiers
				inline TimedPlace& InputPlace() { return *place; }
				inline TimedTransition& OutputTransition() { return *transition; }

			public: // Inspectors
				void Print(std::ostream& out) const;
			private:
				const std::shared_ptr<TimedPlace> place;
				const std::shared_ptr<TimedTransition> transition;
		};

		inline std::ostream& operator<<(std::ostream& out, const InhibitorArc& arc)
		{
			arc.Print(out);
			return out;
		}
	}

}

#endif /* INHIBITORARC_HPP_ */
