// CMediaClientDocument.cpp
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
//

#include "CMediaClientDocument.h"
#include "CMediaClientAppUi.h"
#include "CMediaClientEngine.h"

#include <eikenv.h>
#include <eikproc.h>

// The document's only job is to own the engine

CMediaClientDocument* CMediaClientDocument::NewL(CEikApplication& aApp)
	{
	CMediaClientDocument* self=new (ELeave) CMediaClientDocument(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMediaClientDocument::CMediaClientDocument(CEikApplication& aApp)
: CEikDocument(aApp)
    {
    }

void CMediaClientDocument::ConstructL()
	{
	iEngine = new (ELeave) CMediaClientEngine(CEikonEnv::Static()->FsSession());
	}

CMediaClientDocument::~CMediaClientDocument()
    {
	delete iEngine;
    }

CMediaClientEngine& CMediaClientDocument::Engine()
	{
	return *iEngine;
	}

//
// CreateAppUiL
//
// Called by the framework to get our AppUi object
CEikAppUi* CMediaClientDocument::CreateAppUiL() 
    {
    return new (ELeave) CMediaClientAppUi;
    }


