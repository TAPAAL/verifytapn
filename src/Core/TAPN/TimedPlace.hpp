
#ifndef VERIFYYAPN_TAPN_TIMEDPLACE_HPP_
#define VERIFYYAPN_TAPN_TIMEDPLACE_HPP_

#include <string>
#include <vector>
#include <iostream>
#include "TimeInvariant.hpp"
#include "TimedInputArc.hpp"
#include "OutputArc.hpp"

namespace VerifyTAPN{
	namespace TAPN{
		class TimedPlace {
		public: // static
			static const TimedPlace& Bottom() {
				static TimedPlace bottom;
				return bottom;
			}

			static const int BottomIndex() {
				return -1;
			}

			static const std::string BOTTOM_NAME;

		public: // typedefs
			typedef std::vector<TimedPlace* > Vector;
            typedef std::vector<const TimedPlace* > CVector;

		public: // construction / destruction
			TimedPlace(size_t index, const std::string& name, const std::string& id, const TimeInvariant timeInvariant, double x, double y)
			: index(index), name(name), id(id), timeInvariant(timeInvariant), isUntimed(false), maxConstant(0), hasInhibitorArcs(false), _x(x), _y(y) { };
			TimedPlace() : name(BOTTOM_NAME), timeInvariant(), index(BottomIndex()), isUntimed(false), maxConstant(0), hasInhibitorArcs(false) { };
			virtual ~TimedPlace() { /* empty */ };

		public: // modifiers
			inline void MarkPlaceAsUntimed() { isUntimed = true; }
			inline void SetMaxConstant(int max) { maxConstant = max; }
			inline void SetHasInhibitorArcs(bool inhibitorArcs) { hasInhibitorArcs = inhibitorArcs; }
		public: // inspection
			const std::string& GetName() const;
			const std::string& GetId() const;
			void Print(std::ostream& out) const;
			inline int GetIndex() const { return index; };
			inline const bool IsUntimed() const { return isUntimed; }
			inline const int GetMaxConstant() const { return maxConstant; }
			inline const TAPN::TimeInvariant& GetInvariant() const { return timeInvariant; };
            const std::pair<double,double> getPosition() const { return {_x, _y}; }
			inline bool HasInhibitorArcs() const { return hasInhibitorArcs; };
		private: // data
			int index;
			std::string	name;
			std::string id;
			TimeInvariant timeInvariant;
			bool isUntimed;
			int maxConstant;
			bool hasInhibitorArcs;
            double _x;
            double _y;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedPlace& place)
		{
			place.Print(out);
			return out;
		}

		// TAPAAL does not allow multiple places with the same name,
		// thus it is enough to use the name to determine equality.
		inline bool operator==(TimedPlace const& a, TimedPlace const& b)
		{
			return a.GetName() == b.GetName();
		}

		inline bool operator!=(TimedPlace const& a, TimedPlace const& b)
		{
			return !(a.GetName() == b.GetName());
		}
	}
}
#endif /* VERIFYYAPN_TAPN_TIMEDPLACE_HPP_ */
