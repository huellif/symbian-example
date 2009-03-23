// Anim_AppUi.cpp
//
//  Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include "AnimExample.h"

// Second phase constructor of the application UI class.
// It creates and owns a single view.
void CAnimationAppUi::ConstructL()
    {
    
	// Complete the UI framework's construction of the App UI.
	BaseConstructL();
	// Create the appview.
	iAppView = CAnimationAppView::NewL(ClientRect());
	}

// The application UI class owns one view, and is responsible for destroying it
CAnimationAppUi::~CAnimationAppUi()
	{
	delete iAppView;
	}

// Called by the UI framework when a command has been issued.
void CAnimationAppUi::HandleCommandL(TInt aCommand)
	{
	
	switch (aCommand)
		{
	case EAnimToDo0:
		
		iAppView->ResetSpriteAnimAndMover();
		iAppView->ProduceSpriteAnimL();
		break;				 
	
	case EAnimToDo1:
	
		iAppView->ResetBasicAnimAndMover();
		iAppView->ProduceBasicAnimL();
		break;
	
	case EAnimToDo2:

		iAppView->DoSpriteAnimOperationL( EAnimPause );
		break;
		
	case EAnimToDo3:	

		iAppView->DoSpriteAnimOperationL( EAnimResume );
		break;
		
		case EAnimToDo4:
		iAppView->DoSpriteAnimOperationL( EAnimStop );
				
		break;
		
	case EAnimToDo5:

		iAppView->MoveAnimsL();
		break;				 
	
	case EAnimToDo6:

		iAppView->DoBasicAnimOperationL( EAnimPause );
		break;

	case EAnimToDo7:

		iAppView->DoBasicAnimOperationL( EAnimResume );
		break;
		
	case EAnimToDo8:
	
		iAppView->DoBasicAnimOperationL( EAnimStop );
		break;
			
	case EAnimToDo9:	
	case EEikCmdExit:
	
		Exit();
		break;
		}
		
	}

	


