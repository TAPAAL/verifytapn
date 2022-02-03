#include "DBMMarking.hpp"
#include <iostream>
#include "dbm/print.h"

namespace VerifyTAPN
{
	std::shared_ptr<TAPN::TimedArcPetriNet> DBMMarking::tapn;

	// Add a token in each output place of placesOfTokensToAdd
	// and add placesOfTokensToAdd.size() clocks to the DBM.
	// The DBM library requires arrays of bitvectors indicating
	// which tokens are in the original dbm (bitSrc) and which are in the resulting DBM (bitDst).
	void DBMMarking::AddTokens(const std::list<int>& placeIndices)
	{
		unsigned int tokens = NumberOfTokens();
		unsigned int nAdditionalTokens = placeIndices.size();
		unsigned int oldDimension = dbm.getDimension();
		unsigned int newDimension = oldDimension + nAdditionalTokens;

		unsigned int bitArraySize = (newDimension % 32 == 0 ? newDimension/32 : newDimension/32+1);

		unsigned int bitSrc[bitArraySize];
		unsigned int bitDst[bitArraySize];
		unsigned int table[newDimension];

		// setup the bitvectors
		for(unsigned int i = 0; i < bitArraySize; ++i)
		{
			if(oldDimension >= i*32 && oldDimension < (i+1)*32)
				bitSrc[i] = 0 | ((1 << oldDimension%32)-1);
			else if(oldDimension >= i*32 && oldDimension >= (i+1)*32)
				bitSrc[i] = ~(bitSrc[i] & 0);
			else
				bitSrc[i] = 0;

			bitDst[i] = ~(bitDst[i] & 0);
		}

		if(newDimension%32 != 0)
		{
			bitDst[bitArraySize-1] ^= ~((1 << newDimension % 32)-1);
		}

		dbm.resize(bitSrc, bitDst, bitArraySize, table);

		unsigned int i = 0;
		unsigned int newTokenIndex = tokens;
		for(std::list<int>::const_iterator iter = placeIndices.begin(); iter != placeIndices.end(); ++iter)
		{
			dbm(oldDimension+i) = 0; // reset new clocks to zero
			mapping.SetMapping(newTokenIndex, oldDimension+i);
			dp.AddTokenInPlace(*iter);
			i++;newTokenIndex++;
		}

		assert(IsConsistent());
	}

	// Remove each token in tokensToRemove from the placement vector and from the DBM.
	// These tokens represent tokens that are moving to BOTTOM.
	// The DBM library requires arrays of bitvectors indicating which tokens are in
	// the original dbm (bitSrc) and which are in the resulting DBM (bitDst).
	void DBMMarking::RemoveTokens(const std::set<int>& tokenIndices)
	{
		std::vector<int> dbmTokensToRemove;
		for(std::set<int>::const_iterator it = tokenIndices.begin(); it != tokenIndices.end(); it++)
		{
			dbmTokensToRemove.push_back(mapping.GetMapping(*it));
		}

		unsigned int oldDimension = dbm.getDimension();

		assert(oldDimension-dbmTokensToRemove.size() > 0); // should at least be the zero clock left in the DBM

		unsigned int bitArraySize = (oldDimension % 32 == 0 ? oldDimension/32 : oldDimension/32+1);

		unsigned int bitSrc[bitArraySize];
		unsigned int bitDstMask[bitArraySize];
		unsigned int bitDst[bitArraySize];
		unsigned int table[oldDimension];

		// setup the bitvectors
		for(unsigned int i = 0; i < bitArraySize; ++i)
		{
			bitSrc[i] = ~(bitSrc[i] & 0);
			bitDst[i] = ~(bitDst[i] & 0);
			bitDstMask[i] = 0;
		}

		if(oldDimension%32 != 0)
		{
			bitSrc[bitArraySize-1] ^= ~((1 << oldDimension % 32)-1);
			bitDst[bitArraySize-1] ^= ~((1 << oldDimension % 32)-1);
		}


		for(unsigned int i = 0; i < dbmTokensToRemove.size(); ++i)
		{
			bitDstMask[dbmTokensToRemove[i]/32] |= (1 << dbmTokensToRemove[i] % 32);
		}

		for(unsigned int i = 0; i < bitArraySize; ++i)
		{
			bitDst[i] ^= bitDstMask[i];
		}

		for(unsigned int i = 0; i < oldDimension; ++i)
		{
			table[i] = std::numeric_limits<unsigned int>().max();
		}

		dbm.resize(bitSrc,bitDst,bitArraySize, table);

		// fix token mapping according to new DBM:
		for(unsigned int i = 0; i < oldDimension; ++i)
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

		// remove tokens from mapping and placement
		for(std::set<int>::const_reverse_iterator it = tokenIndices.rbegin(); it != tokenIndices.rend(); it++)
		{
			mapping.RemoveToken(*it);
			dp.RemoveToken(*it);
		}

		assert(IsConsistent());
	}

	void DBMMarking::InitMapping()
	{
		for(unsigned int i = 0; i < dp.size(); i++)
		{
			mapping.SetMapping(i, i+1);
		}
	}

	relation DBMMarking::ConvertToRelation(relation_t relation) const
	{
		switch(relation)
		{
		case base_SUPERSET: return SUPERSET;
		case base_SUBSET: return SUBSET;
		case base_EQUAL: return EQUAL;
		default: return DIFFERENT;
		}
	}

	bool DBMMarking::IsUpperPositionGreaterThanPivot(int upper, int pivotIndex) const
	{
		int placeUpper = dp.GetTokenPlacement(upper);
		int pivot = dp.GetTokenPlacement(pivotIndex);
		unsigned int mapUpper = mapping.GetMapping(upper);
		unsigned int mapPivot = mapping.GetMapping(pivotIndex);
		if(mapPivot > dp.size()+1){
			std::cout << "*";
		}
		return DiscreteMarking::IsUpperPositionGreaterThanPivot(upper, pivotIndex)
				|| (placeUpper == pivot && dbm(0,mapUpper) >  dbm(0,mapPivot))
				|| (placeUpper == pivot && dbm(0,mapUpper) == dbm(0,mapPivot) && dbm(mapUpper,0) > dbm(mapPivot,0))
				|| (placeUpper == pivot && dbm(0,mapUpper) == dbm(0,mapPivot) && dbm(mapUpper,0) == dbm(mapPivot,0) && (mapPivot > mapUpper ? dbm(mapPivot,mapUpper) > dbm(mapUpper,mapPivot) : dbm(mapUpper,mapPivot) > dbm(mapPivot,mapUpper)));
	}

	void DBMMarking::Swap(int i, int j)
	{
		DiscreteMarking::Swap(i,j);
		dbm.swapClocks(mapping.GetMapping(i), mapping.GetMapping(j));
	}

	void DBMMarking::Print(std::ostream& out) const
	{
		out << "Placement: ";
		for(unsigned int i = 0; i < NumberOfTokens(); i++)
		{
			out << GetTokenPlacement(i) << ", ";
		}
		out << std::endl;
		out << "Mapping (token:clock): ";
		for(unsigned int i = 0; i < NumberOfTokens(); i++)
		{
			out << i << ":" << GetClockIndex(i) << ", ";
		}
		out << std::endl;
		out << "DBM:" << std::endl;
		//out << dbm;
		out << "FIXME" << std::endl;
	};
}
