// CLifeEngine.cpp
//
// Copyright (c) 2005 Symbian Softwares Ltd.  All rights reserved.
//

#include <e32math.h>

#include "CLifeEngine.h"

CLifeEngine::CLifeEngine(const TInt64& aSeed) :
	iSeed(aSeed)
	{
	Reset();
	}

// Sets random initial cell states
void CLifeEngine::Reset()
	{
	for (TInt y = 0; y < DIM_Y_ARRAY; y++)
		{
		for (TInt x = 0; x < DIM_X_ARRAY; x++)
			{
			iCellArray[x][y] = (Math::Rand(iSeed) > KMaxTInt / 2) ? ETrue : EFalse;
			}
		}
	}

// Loops through cell array, storing a version altered according to the 
// game of life, in a temporary array
void CLifeEngine::AddGeneration()
	{
	TInt numNeighbours;

	for (TInt y = 0; y < DIM_Y_ARRAY; y++)
		{
		for (TInt x = 0; x < DIM_X_ARRAY; x++)
			{
			numNeighbours = NumNeighbors(x, y);

			if (iCellArray[x][y])
			// Filled cell
				{
				if ((numNeighbours == 2) || (numNeighbours == 3))
					iTempCellArray[x][y] = ETrue;
				else
					iTempCellArray[x][y] = EFalse;
				}
			else
			// Empty cell
				{
				if (numNeighbours == 3)
					iTempCellArray[x][y] = ETrue;
				else
					iTempCellArray[x][y] = EFalse;
				}
			}
		}

	for (TInt y2 = 0; y2 < DIM_Y_ARRAY; y2++)
		{
		for (TInt x2 = 0; x2 < DIM_X_ARRAY; x2++)
			{
			iCellArray[x2][y2] = iTempCellArray[x2][y2];
			}
		}
	}

// Gets the number of adjacent cells to the specified cell
TInt CLifeEngine::NumNeighbors(TInt x, TInt y)
	{
	TInt numNeighbors = 0;
	TInt i = 0;

	// Get neighbors to the left
	if ((x - 1) >= 0)
		{
		for (i = y - 1; i <= y + 1; i++)
			{
			if ((i >= 0) && (i < DIM_Y_ARRAY))
				{
				if (iCellArray[x - 1][i])
					{
					numNeighbors++;
					}
				}
			}
		}
	
	// Get neighbors to the right
	if (x + 1 < DIM_X_ARRAY)
		{
		for (i = y - 1; i <= y + 1; i++)
			{
			if ((i >= 0) && (i < DIM_Y_ARRAY))
				{
				if (iCellArray[x + 1][i])
					{
					numNeighbors++;
					}
				}
			}
		}
		
	// Get neighbors straight above
	if ((y - 1) >= 0)
		{
		if (iCellArray[x][y - 1])
			{
			numNeighbors++;
			}
		}
	
	// Get neighbors straight below
	if ((y + 1) < DIM_Y_ARRAY)
		{	
		if (iCellArray[x][y + 1])
			{
			numNeighbors++;
			}
		}
	
	return numNeighbors;
	}
