// CLifeEngine.h
//
// Copyright (c) 2005 Symbian Softwares Ltd.  All rights reserved.
//

#if !defined(__CLIFEENGINE_H__)
#define __CLIFEENGINE_H__

#include <e32base.h>

// Cell array dimensions
const TInt DIM_X_ARRAY = 16;
const TInt DIM_Y_ARRAY = 8;

// Cell array type
typedef TBool TCellArray[DIM_X_ARRAY][DIM_Y_ARRAY];

// A "Game of Life" engine
// After John Conway, Scientific American 223 (October 1970): 120-123
class CLifeEngine: public CBase
	{
public:
	// Initalise engine with random seed
	CLifeEngine(const TInt64& aSeed);
	// Moves forward one generation
	void AddGeneration();
	// Gets cell array
	const TCellArray& GetCellArray() const
		{return iCellArray;}
	// Resets all cells to random state
	void Reset();

private:
	// Gets number of neighbors for cell x,y
	TInt NumNeighbors(TInt x, TInt y);

private:
	// Random num generator seed
	TInt64 iSeed;
	// Cell array
	TCellArray iCellArray;
	// Temporary working cell array
	TCellArray iTempCellArray;
	};


#endif