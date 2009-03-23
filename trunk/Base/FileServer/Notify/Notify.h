// Notify.h
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.
#ifndef __NOTIFY_H
#define __NOTIFY_H

#include "CommonFramework.h"
#include <f32file.h>
	
/** 
	Monitors file copy events, and prints out the progress.
*/
class TFileCopyProgressMonitor : public MFileManObserver
	{
public:
	/** 
		Constructor.
		@param aFileMan Used to obtain the number of bytes currently copied 
	*/
	TFileCopyProgressMonitor(CFileMan& aFileMan);
private:
	TControl NotifyFileManStarted();
	TControl NotifyFileManOperation();
	TControl NotifyFileManEnded();	
private:
	CFileMan& iFileMan;
	};

#endif
