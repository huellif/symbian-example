// WriteToFile.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

// This example writes the text "hello world!" to a file
// Use it as the basis of all examples which depend on just E32 and F32

#include "CommonFramework.h"
#include <f32file.h>

_LIT(KFileName,"WriteToFile.dat");
_LIT(KGreetingText,"Hello World!\n");

// Do the example
LOCAL_C void doExampleL()
    {
                                             // The file server session.
	RFs fsSession;                           
	                                         // Connect to the file server session.
	User::LeaveIfError(fsSession.Connect()); 
	
	                                         // create the private directory
	                                         // on drive C:
	                                         // i.e. "C:\private\0FFFFF00\"
	                                         // Note that the number 0FFFFF00 is the 
	                                         // process security id taken from the 2nd UID
	                                         // specified in the mmp file.
    fsSession.CreatePrivatePath(EDriveC);
    
                                             // Set the session path to
                                             // this private directory on drive C:
    fsSession.SetSessionToPrivate(EDriveC);
                                                 
                                             // Use this object to represent
                                             // the file to be written to.
	RFile file;                                  
	                                             
	User::LeaveIfError(file.Replace(fsSession,KFileName,EFileWrite|EFileStreamText));

		
		                                     // Note that Write() requires a TDesC8
		                                     // type so we need to construct an explicit
		                                     // TDesC8 type to represent the data contained
		                                     // in the standard (16-bit) descriptor.
	TPtrC8 representation((TUint8*)(&KGreetingText)->Ptr(), (&KGreetingText)->Size());
	
	                                         // Write the text ...
	User::LeaveIfError(file.Write(representation));
	                                             
	                                         // ... and commit the text.
	User::LeaveIfError(file.Flush());
	
	_LIT(KTxt1,"Data written to file\n");
	console->Printf(KTxt1);
		                                     // Close file
	file.Close(); 
		                                     // Delete file (remove comment if you want
		                                     // to do this)
	//User::LeaveIfError(fsSession.Delete(KFileName));
	
		                                         // close file server session
	fsSession.Close(); 
	}
