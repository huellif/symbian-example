// FileBrowseDocument.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef __FILEBROWSEDOCUMENT_H__
#define __FILEBROWSEDOCUMENT_H__

#include <QikApplication.h>
#include <QikDocument.h>


class CRFsEngine;

class CFileBrowseDocument : public CQikDocument
	{
public:
	static CFileBrowseDocument* NewL(CQikApplication& aAppUi);
	~CFileBrowseDocument();
    CEikAppUi* CreateAppUiL();
    CRFsEngine& RFsEngine();
    
private:
	CFileBrowseDocument(CQikApplication& aApp);
	void ConstructL();
private:
	CRFsEngine* iEngine;
	};


#endif // __FILEBROWSEDOCUMENT_H__

