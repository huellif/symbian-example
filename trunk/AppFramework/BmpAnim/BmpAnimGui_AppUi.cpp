// BmpAnimGui_AppUi.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include "BmpAnimGui.h"

// Second phase constructor of the application UI class.
// It creates and owns a single view.
void CExampleAppUi::ConstructL()
	{
	// Complete the UI framework's construction of the App UI.
	BaseConstructL();
   	// Create the appview.
	iAppView = CExampleAppView::NewL( ClientRect() );
	}

// The application UI class owns one view, and is responsible
// for destroying it
CExampleAppUi::~CExampleAppUi()
	{
	delete iAppView;
	}

// Called by the UI framework when a command has been issued.
void CExampleAppUi::HandleCommandL( TInt aCommand )
	{
	
	switch (aCommand)
		{
	case EGuiBaseDo0:
		iAppView->iBitmapAnimPlayer.SetPlayModeL( CBitmapAnimClientData::EPlay );
		iAppView->iBitmapAnimPlayer.StartL();		
		break;				 
	
	case EGuiBaseDo1:
	 	iAppView->iBitmapAnimPlayer.StopL();
		break;
	
	case EGuiBaseDo2:
		iAppView->iBitmapAnimPlayer.SetPlayModeL( CBitmapAnimClientData::EBounce );
		iAppView->iBitmapAnimPlayer.StartL();
		break;
		
	case EGuiBaseDo3:
		iAppView->iBitmapAnimPlayer.SetPlayModeL( CBitmapAnimClientData::ECycle );
		iAppView->iBitmapAnimPlayer.StartL();
		break;

	case EEikCmdExit: 
		Exit();
		break;
		}
	
	}

