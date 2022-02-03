#ifndef DISCRETEINCLUSIONMARKINGFACTORY_HPP_
#define DISCRETEINCLUSIONMARKINGFACTORY_HPP_

#include "UppaalDBMMarkingFactory.hpp"
#include "DiscretePartInclusionMarking.hpp"

#include "dbm/print.h"
#include <iostream>

#include "../VerificationOptions.hpp"

namespace VerifyTAPN {

class DiscreteInclusionMarkingFactory : public UppaalDBMMarkingFactory {
public:
	DiscreteInclusionMarkingFactory(const std::shared_ptr<TAPN::TimedArcPetriNet>& tapn, const VerificationOptions& options)
		: UppaalDBMMarkingFactory(tapn), tapn(tapn), inc_places(tapn->NumberOfPlaces(), false), empty_inc(options.GetFactory() == DEFAULT) { MarkPlacesForInclusion(options.GetIncPlaces()); };
	virtual ~DiscreteInclusionMarkingFactory() {};

	virtual StoredMarking* Convert(SymbolicMarking* marking) const // TODO: who should clean up marking?
	{
		DBMMarking* dbmMarking = static_cast<DBMMarking*>(marking);
		std::vector<int> eq;
		std::vector<int> inc(tapn->NumberOfPlaces(), 0);
		TokenMapping mapping;

		for(unsigned int i = 0; i < marking->NumberOfTokens(); i++)
		{
			int place = marking->GetTokenPlacement(i);
			if(BelongsToINC(i, *dbmMarking))
				inc[place]++;
			else
			{
				int newIndex = eq.size();
				eq.push_back(place);
				mapping.SetMapping(newIndex, marking->GetClockIndex(i));
			}
		}
		dbm::dbm_t dbm = projectToEQPart(eq, mapping, dbmMarking->GetDBM());

		return new DiscretePartInclusionMarking(dbmMarking->id, eq, inc, mapping, dbm);
	};

	virtual SymbolicMarking* Convert(StoredMarking* marking) const
	{
		DiscretePartInclusionMarking* dpiMarking = static_cast<DiscretePartInclusionMarking*>(marking);

		TokenMapping mapping;
		std::vector<int> dpVec;

		unsigned int nextIncIndex = dpiMarking->eq.size()+1;

		unsigned int i = 0, place_j = 0;
		while(i < dpiMarking->eq.size() || place_j < dpiMarking->inc.size())
		{
			unsigned int place_i = i < dpiMarking->eq.size() ? dpiMarking->eq[i] : std::numeric_limits<int>::max();

			if(i < dpiMarking->eq.size() && place_i <= place_j)
			{
				int newIndex = dpVec.size();
				dpVec.push_back(place_i);
				mapping.SetMapping(newIndex, dpiMarking->GetClockIndex(i));
				i++;
				continue;
			}
			else if(place_j < place_i && dpiMarking->inc[place_j] > 0)
			{
				for(int t = 0; t < dpiMarking->inc[place_j]; t++)
				{
					int newIndex = dpVec.size();
					dpVec.push_back(place_j);
					mapping.SetMapping(newIndex, nextIncIndex);
					assert(mapping.GetMapping(newIndex) == nextIncIndex);
					nextIncIndex++;
				}
			}
			place_j++;
		}

		for(unsigned int i = 0; i < dpVec.size(); i++)
		{
			assert(mapping.GetMapping(i) <= dpiMarking->size()+1);
		}


		DiscretePart dp(dpVec);
		dbm::dbm_t dbm = ProjectToAllClocks2(dp,mapping, dpiMarking->dbm); // improved speed
		//dbm::dbm_t dbm = ProjectToAllClocks(dp, mapping, dpiMarking->dbm); // old correct for comparison

		TokenMapping identity_mapping;
		for(unsigned int i = 0; i < dp.size(); i++)
		{
			identity_mapping.SetMapping(i,i+1);
		}

		assert(dp.size()+1 == dbm.getDimension());
		DBMMarking* result =  new DBMMarking(dp, identity_mapping, dbm);
		result->id = dpiMarking->id;
		return result;
	};

	virtual void Release(SymbolicMarking* marking)
	{
		if(marking != 0) delete marking;
	};

	virtual void Release(StoredMarking* marking)
	{
		if(marking != 0) delete marking;
	};

private:
	bool BelongsToINC(int token, const DBMMarking& marking) const
	{
		if(empty_inc) return false;

		int placeIndex = marking.GetTokenPlacement(token);
		if(!inc_places[placeIndex]) return false;

		const TimedPlace& place = tapn->GetPlace(placeIndex);

		assert(placeIndex != TAPN::TimedPlace::BottomIndex());
		if(place.GetInvariant() != TAPN::TimeInvariant::LS_INF) return false;
		if(place.HasInhibitorArcs()) return false;
		if(place.IsUntimed()) return true;

		raw_t lowerBound = marking.GetLowerBound(marking.GetClockIndex(token));
		if(lowerBound == dbm_bound2raw(-place.GetMaxConstant(), dbm_STRICT)) return true; // TODO: check that == is correct
		return false;
	};

	dbm::dbm_t projectToEQPart(const std::vector<int>& eq, TokenMapping& mapping, const dbm::dbm_t& dbm) const
	{
		unsigned int dim = dbm.getDimension();
		if(eq.size()+1 == dim) return dbm::dbm_t(dbm);

		unsigned int bitArraySize = (dim % 32 == 0 ? dim/32 : dim/32+1);
		unsigned int bitSrc[bitArraySize];
		unsigned int bitDst[bitArraySize];
		unsigned int table[dim];

		for(unsigned int i = 0; i < bitArraySize; i++)
		{
			if(i == 0){
				bitSrc[i] = 1;
				bitDst[i] = 1;
			}else{
				bitSrc[i] = 0;
				bitDst[i] = 0;
			}
		}

		for(unsigned int i = 0; i < dim; i++)
		{
			table[i] = std::numeric_limits<unsigned int>::max();
			bitSrc[i/32] |= (1 << (i % 32));
		}

		for(unsigned int i = 0; i < eq.size(); i++)
		{
			unsigned int arrayIndex = mapping.GetMapping(i)/32;
			unsigned int offset = mapping.GetMapping(i) % 32;
			bitDst[arrayIndex] |= (1 << offset);
		}

		dbm::dbm_t copy(dbm);
		copy.resize(bitSrc, bitDst, bitArraySize, table);
		assert(dbm.getDimension() == dim);
		assert(eq.size()+1 == copy.getDimension());

		for(unsigned int i = 0; i < dim; ++i)
		{
			if(table[i] != std::numeric_limits<unsigned int>().max())
			{
				for(unsigned int j = 0; j < mapping.size(); ++j)
				{
					if(mapping.GetMapping(j) == i)
						mapping.SetMapping(j, table[i]);
				}
			}
		}

		return copy;
	};

	/*
	 * Old version for comparison. This correctly projects DBM to all clocks
	 * and subsequently reorders clocks such that token i has clock i+1
	 */
	dbm::dbm_t ProjectToAllClocks(const DiscretePart& dp, TokenMapping& mapping, const dbm::dbm_t& dbm) const
	{
		//std::cout << "eq-dbm: \n" << dbm << std::endl;
		unsigned int dim = dbm.getDimension();
		unsigned int totalClocks = dp.size()+1;
		if(dim == totalClocks) return dbm::dbm_t(dbm);

		unsigned int bitArraySize = (totalClocks % 32 == 0 ? totalClocks/32 : totalClocks/32+1);
		unsigned int bitSrc[bitArraySize];
		unsigned int bitDst[bitArraySize];
		unsigned int table[totalClocks];

		for(unsigned int i = 0; i < bitArraySize; ++i)
		{
			if(dim >= i*32 && dim < (i+1)*32)
				bitSrc[i] = 0 | ((1 << dim%32)-1);
			else if(dim >= i*32 && dim >= (i+1)*32)
				bitSrc[i] = ~(bitSrc[i] & 0);
			else
				bitSrc[i] = 0;

			bitDst[i] = ~(bitDst[i] & 0);
		}

		if(totalClocks % 32 != 0)
		{
			bitDst[bitArraySize-1] ^= ~((1 << totalClocks % 32)-1);
		}


		dbm::dbm_t copy(dbm);
		copy.resize(bitSrc, bitDst, bitArraySize, table);

		assert(dbm.getDimension() == dim);
		for(unsigned int i = 0; i < dp.size(); i++)
		{
			if(mapping.GetMapping(i) >= dim)
			{
				const TimedPlace& place = tapn->GetPlace(dp.GetTokenPlacement(i));
				if(!place.IsUntimed()) copy.constrain(0, mapping.GetMapping(i), dbm_bound2raw(-place.GetMaxConstant(), dbm_STRICT));
			}
		}

		unsigned int map[copy.getDimension()];
		map[0] = 0;
		for(unsigned int i = 1; i < copy.getDimension(); i++)
		{
			map[i] = mapping.GetMapping(i-1);
		}
		copy.changeClocks(map, copy.getDimension());

		//std::cout << "computed-dbm: \n" << copy << std::endl;
		return copy;
	};

	/*
	 * Improved version of above, that should be a little faster.
	 */
	dbm::dbm_t ProjectToAllClocks2(const DiscretePart& dp, const TokenMapping& mapping, const dbm::dbm_t& dbm) const
	{
		unsigned int dim = dbm.getDimension();
		unsigned int totalClocks = dp.size()+1;
		unsigned int map[totalClocks];
		map[0] = 0;

		bool fromInc[totalClocks];
		fromInc[0] = false;
		for(unsigned int i = 1; i < totalClocks; i++)
		{
			unsigned int target = mapping.GetMapping(i-1);
			if(target >= dim)
			{
				map[i] = ~0;
				fromInc[i] = true;
			}
			else
			{
				map[i] = target;
				fromInc[i] = false;
			}
		}

		dbm::dbm_t copy(dbm);
		copy.changeClocks(map, totalClocks);

		for(unsigned int i = 0; i < dp.size(); i++)
		{
			if(fromInc[i+1])
			{
				const TimedPlace& place = tapn->GetPlace(dp.GetTokenPlacement(i));
				if(!place.IsUntimed()) copy.constrain(0, i+1, dbm_bound2raw(-place.GetMaxConstant(), dbm_STRICT));
			}
		}

		return copy;
	};

	void MarkPlacesForInclusion(const std::vector<std::string>& places)
	{
		if((places.size() == 1 && places[0] == "*NONE*") || places.size() == 0)
		{
			return;
		}
		else if(places.size() == 1 && places[0] == "*ALL*")
		{
			for(unsigned int i = 0; i < inc_places.size(); i++)
			{
				inc_places[i] = true;
			}
		}
		else
		{
			for(std::vector<std::string>::const_iterator it = places.begin(); it != places.end(); it++)
			{
				int index = tapn->GetPlaceIndex(*it);
				if(index != TimedPlace::BottomIndex())
				{
					inc_places[index] = true;
				}
			}
		}
	};
private:
	std::shared_ptr<TAPN::TimedArcPetriNet> tapn;
	std::vector<bool> inc_places;
	bool empty_inc;
};

}

#endif /* DISCRETEINCLUSIONMARKINGFACTORY_HPP_ */
