// FileBrowseApplication.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef __FILEBROWSEAPPLICATION_H__
#define __FILEBROWSEAPPLICATION_H__

#include <QikApplication.h>

class CFileBrowseApplication : public CQikApplication
	{
public:
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	CApaApplication* NewApplication();
	};

#endif // __FILEBROWSEAPPLICATION_H__

