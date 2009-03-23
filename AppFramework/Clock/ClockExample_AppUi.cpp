// ClockExample_AppUi.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#include "ClockExample.h"

// Second phase constructor of the application UI class.
// It creates and owns a single view.
void CExampleAppUi::ConstructL()
	{
	BaseConstructL();
	iAppView = CExampleAppView::NewL(ClientRect());
	}

// The application UI class owns one view, and is responsible
// for destroying it. 
CExampleAppUi::~CExampleAppUi()
	{
	delete iAppView;
	}

// Called by the UI framework when a command has been issued.
void CExampleAppUi::HandleCommandL( TInt aCommand )
	{
	
	switch (aCommand)
		{
	// Digital clock commands	
	case ETestDo0:
	case ETestDo1:
	case ETestDo2:
	case ETestDo3:
	case ETestDo4:
	case ETestDo5:
	case ETestDo6:
	case ETestDo7:
	case ETestDo8:
	case ETestDo9:
	case ETestDoA:
	case ETestDoB:
	case ETestDoC:
	case ETestDoD:
	case ETestDoE:
	case ETestDoF:	
	iAppView->DoExperimentL( aCommand );
	break;	
	
	// Analogue	clock commands
	case ETestDo10:
	case ETestDo11:
	case ETestDo12:
	case ETestDo13:
	case ETestDo14:
	case ETestDo15:
	case ETestDo16:
	case ETestDo17:
	case ETestDo18:
	case ETestDo19:
	case ETestDo1A:
	case ETestDo1B:
	case ETestDo1C:
	case ETestDo1D:
	case ETestDo1E:
	case ETestDo1F:
	iAppView->DoExperimentL( aCommand );
	break;			 

	case EEikCmdExit: 
		Exit();
		break;
		}
		
	}

