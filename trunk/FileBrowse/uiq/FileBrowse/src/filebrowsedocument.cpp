// FileBrowseDocument.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include <QikApplication.h>

#include "FileBrowseAppUi.h"
#include "FileBrowseDocument.h"
#include "RFsEngine.h"

CFileBrowseDocument::CFileBrowseDocument (CQikApplication& aApp) : CQikDocument(aApp)
	{}

CFileBrowseDocument* CFileBrowseDocument::NewL(CQikApplication& aApp)
	{
	CFileBrowseDocument* self = new (ELeave) CFileBrowseDocument(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CFileBrowseDocument::ConstructL()
	{	
	iEngine = CRFsEngine::NewL();
	}

CFileBrowseDocument::~CFileBrowseDocument()
	{
	delete iEngine;
	}

CEikAppUi* CFileBrowseDocument::CreateAppUiL()
	{
	return new(ELeave) CFileBrowseAppUi;
	}

CRFsEngine& CFileBrowseDocument::RFsEngine()
	{
	return *iEngine;
	}
