// Notify.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

#include "Notify.h"

_LIT(KStarted,"Copy started\n");
_LIT(KComplete,"Copy complete\n");
_LIT(KProgress,"%d bytes copied\n");

// 
// TFileCopyProgressMonitor
// 

TFileCopyProgressMonitor::TFileCopyProgressMonitor(CFileMan& aFileMan)
:iFileMan(aFileMan)
	{	
	}

// Called when copy operation started
MFileManObserver::TControl TFileCopyProgressMonitor::NotifyFileManStarted()
	{
	console->Printf(KStarted);
	return EContinue;
	}

// Called when copy operation is in progress
MFileManObserver::TControl TFileCopyProgressMonitor::NotifyFileManOperation()
	{
	console->Printf(KProgress,iFileMan.BytesTransferredByCopyStep());
	return EContinue;
	}

// Called when copy operation is complete
MFileManObserver::TControl TFileCopyProgressMonitor::NotifyFileManEnded()
	{
	console->Printf(KComplete);
	return EContinue;
	}
	

//
// Do the example
//

LOCAL_C void doExampleL()
    {
	_LIT(KSourcePath,"dataFile\\");
	_LIT(KDestinationPath,"dataFileCopy\\");
	
	TInt err;

	  // Connect session
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	
	  // create the private directory
	fsSession.CreatePrivatePath(EDriveC);
	
	  // Set the session path to the private directory
    fsSession.SetSessionToPrivate(EDriveC);
	
	  // Get the private directory name
	TFileName path;
	fsSession.PrivatePath(path);
  
      // Create a source and a target directory
	RBuf pathSource;
	RBuf pathDestination;
	
	pathSource.CreateL(sizeof(TFileName));
	pathSource.CleanupClosePushL();
	pathSource.Append(path);
	pathSource.Append(KSourcePath);
	
	pathDestination.CreateL(sizeof(TFileName));
	pathDestination.CleanupClosePushL();
	pathDestination.Append(path);
	pathDestination.Append(KDestinationPath);
	
	err=fsSession.MkDir(pathSource);
	if (err!=KErrAlreadyExists)
		User::LeaveIfError(err);
	
	err=fsSession.MkDir(pathDestination);
	if (err!=KErrAlreadyExists)
		User::LeaveIfError(err);
	
	  // Create 2 source files in the source directory
	  // so that we have something to copy.
    _LIT(KFile1,"dataFile1.txt");
    _LIT(KFile2,"dataFile2.txt");
    _LIT8(KFile1Data,"File data qwertyu");
    _LIT8(KFile2Data,"File data iop");
    
    fsSession.SetSessionPath(pathSource);
    RFile file;
    
    User::LeaveIfError(file.Replace(fsSession,KFile1,EFileWrite));
	User::LeaveIfError(file.Write(KFile1Data));
	User::LeaveIfError(file.Flush()); // Commit data
	file.Close(); // close file having finished with it
	
	User::LeaveIfError(file.Replace(fsSession,KFile2,EFileWrite));
	User::LeaveIfError(file.Write(KFile2Data));
	User::LeaveIfError(file.Flush()); // Commit data
	file.Close(); // close file having finished with it
    
    	// Create file management object
	CFileMan* fileMan = CFileMan::NewL(fsSession);
	CleanupStack::PushL(fileMan); 

	   // Create file management notification object and set to observe
	TFileCopyProgressMonitor fileCopyProgressMonitor(*fileMan);
	fileMan->SetObserver(&fileCopyProgressMonitor);
	
	  // Do copy (here synchronously)
	fileMan->Copy(pathSource,pathDestination);
	
	  // Clean up 3 items 
	CleanupStack::PopAndDestroy(3);
	
	  // Close file server session and note that we have not deleted
	  // any files or directories created here.
	fsSession.Close();
	}

