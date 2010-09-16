// CDirectDisplayLife.cpp
//
// Copyright (c) 2005 Symbian Softwares Ltd.  All rights reserved.
//

#include <coemain.h>

#include "CDirectDisplayLife.h"

// Dimension of each cell
const TInt CDirectDisplayLife::KBlockSize = 20;

// Delay between generations (microseconds)
// Actual minimum is 1/10s on WINS
const TInt CDirectDisplayLife::KGenerationInterval = 1000 * 1000;

// X & Y origins of upper left of cell display
const TInt CDirectDisplayLife::iXOrigin = 50;
const TInt CDirectDisplayLife::iYOrigin = 30;

CDirectDisplayLife::CDirectDisplayLife(RWsSession& aClient, RWindow& aWindow,
        CLifeEngine& aLifeEngine) :
    CTimer(CActive::EPriorityStandard), iClient(aClient), iWindow(aWindow),
            iLifeEngine(aLifeEngine)
    {
    }

CDirectDisplayLife::~CDirectDisplayLife()
    {
    Cancel();
    delete iDirectScreenAccess;
    }

void CDirectDisplayLife::ConstructL()
    {
    CTimer::ConstructL();
    // Create the DSA object
    iDirectScreenAccess = CDirectScreenAccess::NewL(iClient, // WS session
            *(CCoeEnv::Static()->ScreenDevice()), // CWsScreenDevice
            iWindow, // RWindowBase
            *this // MDirectScreenAccess
            );

    CActiveScheduler::Add(this);
    }

// Start game display
void CDirectDisplayLife::StartL()
    {
    // Initialise DSA
    iDirectScreenAccess->StartL();
    // Get graphics context for it
    iGc = iDirectScreenAccess->Gc();
    iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    // Get region that DSA can draw in
    iRegion = iDirectScreenAccess->DrawingRegion();
    // Set the display to clip to this region
    iGc->SetClippingRegion(iRegion);

    After(TTimeIntervalMicroSeconds32(KGenerationInterval));
    }

// Implement MDirectScreenAccess
void CDirectDisplayLife::Restart(RDirectScreenAccess::TTerminationReasons /*aReason*/)
    {
    // Restart display
    // Note that this will result in the clipping region being updated
    // so that menus, overlaying dialogs, etc. will not be drawn over
    StartL();
    }

void CDirectDisplayLife::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
    {
    // Cancel timer and display
    Cancel();
    }

// Draw cells using DSA
void CDirectDisplayLife::RunL()
    {
    // Update engine
    iLifeEngine.AddGeneration();
    const TCellArray& iCells = iLifeEngine.GetCellArray();

    // Force screen update: this required for WINS, but may
    // not be for all hardware
    iDirectScreenAccess->ScreenDevice()->Update();

    // Loop through cells drawing each
    TRect drawBlock(iXOrigin, iYOrigin, iXOrigin + KBlockSize, iYOrigin
            + KBlockSize);
    for (int col = 0; col < COLS_NUM; col++)
        {
        for (int row = 0; row < ROWS_NUM; row++)
            {
            if (iCells[row][col])
                {
                iGc->SetBrushColor(KRgbBlue);
                }
            else
                {
                iGc->SetBrushColor(KRgbYellow);
                }
            iGc->DrawRect(drawBlock);
            drawBlock.Move(KBlockSize, 0);
            }
        drawBlock.iTl.iX = iXOrigin;
        drawBlock.iBr.iX = iXOrigin + KBlockSize;
        drawBlock.Move(0, KBlockSize);
        }
    iClient.Flush();
    // Renew request
    After(TTimeIntervalMicroSeconds32(KGenerationInterval));
    }

void CDirectDisplayLife::DoCancel()
    {
    // Cancel timer
    CTimer::DoCancel();
    // Cancel DSA
    iDirectScreenAccess->Cancel();
    }

