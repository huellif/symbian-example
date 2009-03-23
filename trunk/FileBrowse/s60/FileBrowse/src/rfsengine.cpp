// FileBrowseEngine.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include <f32file.h>
#include "RFsEngine.h"

CRFsEngine* CRFsEngine::NewL()
	{
	CRFsEngine* me = new (ELeave) CRFsEngine();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);	
	return (me);		
	}
	
	
CRFsEngine::~CRFsEngine() 
	{
	delete iFileList;
	delete iDirList;
	iFs.Close();
	}


TInt CRFsEngine::GetDirectoryAndFileList(const TDesC& aPath)
	{
	if (iFileList)
		{
		delete iFileList;
		iFileList = NULL;
		}
	if (iDirList)
		{
		delete iDirList;
		iDirList = NULL;
		}
	
	TInt result = iFs.GetDir(aPath,
                             KEntryAttNormal | KEntryAttHidden | KEntryAttSystem,
                             ESortByName | EDirsFirst | EAscending,
                             iFileList,
                             iDirList);
	return (result);
	}
	
TInt CRFsEngine::DirectoryCount()
	{
	if (iDirList)
		return (iDirList->Count());
	else
		return (0);
	}
	
	
TInt CRFsEngine::FileCount()
	{
	if (iFileList)
		return (iFileList->Count());
	else
		return (0);
	}
	
	
const TDesC& CRFsEngine::DirectoryName(TInt aDirListIndex)
	{
	if ( (!iDirList) || (iDirList->Count()<=aDirListIndex) )
		return (KNullDesC);
	
	else
		return (iDirList->operator[](aDirListIndex).iName);
	
	}

const TDesC& CRFsEngine::FileName(TInt aFileNameIndex)
	{
	if ( (!iFileList) || (iFileList->Count()<=aFileNameIndex) )
		return (KNullDesC);
	
	else
		return (iFileList->operator[](aFileNameIndex).iName);
	}

void CRFsEngine::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}


CRFsEngine::CRFsEngine()
	{}

