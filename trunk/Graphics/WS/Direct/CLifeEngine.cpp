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
    for (TInt col = 0; col < COLS_NUM; col++)
        {
        for (TInt row = 0; row < ROWS_NUM; row++)
            {
            iCellArray[row][col]
                    = (Math::Rand(iSeed) > KMaxTInt / 2) ? ETrue : EFalse;
            }
        }
    }

// Loops through cell array, storing a version altered according to the 
// game of life, in a temporary array
void CLifeEngine::AddGeneration()
    {
    TInt numNeighbours;

    for (TInt col = 0; col < COLS_NUM; col++)
        {
        for (TInt row = 0; row < ROWS_NUM; row++)
            {
            numNeighbours = NumNeighbors(row, col);

            if (iCellArray[row][col])
            // Filled cell
                {
                if ((numNeighbours == 2) || (numNeighbours == 3))
                    {
                    iTempCellArray[row][col] = ETrue;
                    }
                else
                    {
                    iTempCellArray[row][col] = EFalse;
                    }
                }
            else
            // Empty cell
                {
                if (numNeighbours == 3)
                    {
                    iTempCellArray[row][col] = ETrue;
                    }
                else
                    {
                    iTempCellArray[row][col] = EFalse;
                    }
                }
            }
        }

    for (TInt col1 = 0; col1 < COLS_NUM; col1++)
        {
        for (TInt row1 = 0; row1 < ROWS_NUM; row1++)
            {
            iCellArray[row1][col1] = iTempCellArray[row1][col1];
            }
        }
    }

// Gets the number of adjacent cells to the specified cell
TInt CLifeEngine::NumNeighbors(TInt row, TInt col)
    {
    TInt numNeighbors = 0;
    TInt i = 0;

    // Get neighbors to the left
    if ((row - 1) >= 0)
        {
        for (i = col - 1; i <= col + 1; i++)
            {
            if ((i >= 0) && (i < COLS_NUM))
                {
                if (iCellArray[row - 1][i])
                    {
                    numNeighbors++;
                    }
                }
            }
        }

    // Get neighbors to the right
    if (row + 1 < ROWS_NUM)
        {
        for (i = col - 1; i <= col + 1; i++)
            {
            if ((i >= 0) && (i < COLS_NUM))
                {
                if (iCellArray[row + 1][i])
                    {
                    numNeighbors++;
                    }
                }
            }
        }

    // Get neighbors straight above
    if ((col - 1) >= 0)
        {
        if (iCellArray[row][col - 1])
            {
            numNeighbors++;
            }
        }

    // Get neighbors straight below
    if ((col + 1) < COLS_NUM)
        {
        if (iCellArray[row][col + 1])
            {
            numNeighbors++;
            }
        }

    return numNeighbors;
    }
