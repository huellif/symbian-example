// FileBrowseApplication.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include <EikStart.h>
#include "FileBrowseApplication.h"
#include "FileBrowseDocument.h"
#include "FileBrowseGlobals.h"


CApaDocument* CFileBrowseApplication::CreateDocumentL()
  {
  return CFileBrowseDocument::NewL(*this);
  }

TUid CFileBrowseApplication::AppDllUid() const
  {
  return KUidFileBrowseID;
  }

CApaApplication* NewApplication()
  {
  return new CFileBrowseApplication;
  }

GLDEF_C TInt E32Main()
  {
  return EikStart::RunApplication(NewApplication);
  }


