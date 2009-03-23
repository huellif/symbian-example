// Connecting.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.


// NB. THE FOLLOWING CODE CREATES A DIRECTORY CALLED F32TEST IN THE DEFAULT 
// DIRECTORY AND DELETES IT BEFORE EXITING.

#include <f32file.h>
#include "CommonFramework.h"
	
LOCAL_D RFs fsSession;

LOCAL_C void doExampleL()
    {
	// Define descriptor constants using the _LIT macro 
	_LIT(KMessage1,"Default path for fsSession is %S\n");
	_LIT(KMessage2,"Session path for fsSession is now %S\n");
	_LIT(KMessage3,"Session path for fsSession2 is %S\n");
    _LIT(KTestDir,"f32test\\");

	  // Open file server session
	User::LeaveIfError(fsSession.Connect());
	
	  // Get the default session path, and display it.
	  // It should be the private path for this (process)
	  // i.e. c:\private\0fffff04.
	TFileName path;
	User::LeaveIfError(fsSession.SessionPath(path));
	console->Printf(KMessage1,&path);

	  // Create a directory within the default session path.
	  // and then set the default path to the full directory.
	  //
	  // Steps.
	  // 
	  // 1  Create the private path - it is not automatically created by Symbian OS.
	  // 2. Get the name of the private path
	  // 3. Create the new directory level f32test\ within the private path
	  // 4. Set the session path to \private\0fffff04\f32test.
	User::LeaveIfError(fsSession.CreatePrivatePath(EDriveC)); // <--- 1.
	fsSession.PrivatePath(path);   // <------------------------------ 2.
	path.Append(KTestDir);
	User::LeaveIfError(fsSession.MkDir(path));// <------------------- 3.
	User::LeaveIfError(fsSession.SetSessionPath(path)); // <--------- 4.
		
	  // Retrieve the session path to make sure that the system
	  // agrees with what we believe is the new session path.
	  // Clear the path descriptor; this is NOT necessary, but just
	  // shows that we are not cheating !
    path.SetLength(0);
    User::LeaveIfError(fsSession.SessionPath(path));
	console->Printf(KMessage2,&path);
	 
	  // Make another connection to the file server and show  
	  // its default session path.It should be the original 
	  // private path for this (process) i.e. c:\private\0fffff04.
	RFs fsSession2;
	User::LeaveIfError(fsSession2.Connect());
	fsSession2.SessionPath(path);
	console->Printf(KMessage3,&path);

      // Remove the \f32test\ directory	
	fsSession.PrivatePath(path);       
	path.Append(KTestDir);
  	User::LeaveIfError(fsSession.RmDir(path));
	
	  // Close these two sessions with the file server.
	fsSession2.Close();
	fsSession.Close();
	}
