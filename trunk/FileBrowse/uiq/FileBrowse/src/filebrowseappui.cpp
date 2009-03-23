// FileBrowseAppUi.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include "FileBrowseAppUi.h"
#include "FileBrowseDocument.h"
#include "FileBrowseBaseView.h"

void CFileBrowseAppUi::ConstructL()
  {
  BaseConstructL();
  
  // Get the model from the document class
  CFileBrowseDocument* document = static_cast<CFileBrowseDocument*>(iDocument);
  CRFsEngine& engine(document->RFsEngine());
	
  CFileBrowseBaseView* baseView = CFileBrowseBaseView::NewLC(*this,engine);
  AddViewL(*baseView);
  CleanupStack::Pop(baseView);
  }

CFileBrowseAppUi::~CFileBrowseAppUi()
  {}
