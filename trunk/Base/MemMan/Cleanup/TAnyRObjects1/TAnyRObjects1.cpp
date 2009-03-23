// TAnyRObjects1.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

// AIM: To provide examples for the documentation of exceptions & traps
// Situation3 - examples of cleanup-stack support for TAny* and RItems


#include "CommonFramework.h"

// Name of file to be used
_LIT(KFileName,"TAnyandRObjects1.dat");

// Test data to be put into the file.
_LIT(KTestData,"Test data for TAnyandRObjects1\n");

// #include specific files
#include <f32file.h>
#include "euhexdmp.h"


//////////////////////////////////////////////////////////////////////////////
//
// ----->  RFileWithCleanup(definition)
//
// Function Cleanup() and operator TCleanupItem() needed to provide 
// Cleanup Stack for RFile
//
//////////////////////////////////////////////////////////////////////////////
class RFileWithCleanup : public RFile
	{
private:
	static void Cleanup(TAny *aPtr);
public:
	operator TCleanupItem();
	};


//////////////////////////////////////////////////////////////////////////////
//
// ----->  RFileWithCleanup(implementation)
//
//////////////////////////////////////////////////////////////////////////////
void RFileWithCleanup::Cleanup(TAny *aPtr)
	{
	_LIT(KMsgDoingCleanup,"Doing cleanup of file.\n");
	console->Printf(KMsgDoingCleanup);						
	  // Invoke the Close member on the RItem at aPtr
	((RFileWithCleanup *)aPtr)->Close();
	}

RFileWithCleanup::operator TCleanupItem()
	{
	return TCleanupItem(Cleanup,this);
	}

LOCAL_C void createDataFileL()
	{
	  // utility function to create some data which we can later read
	RFs createFileSession;
	RFileWithCleanup createFile;

      // connect to filserver session		
	User::LeaveIfError(createFileSession.Connect());

	  // create the private directory
	  // on drive C:
	  // i.e. "C:\private\0FFFFF01\"
	  // Note that the number 0FFFFF01 is the 
	  // process security id taken from the 2nd UID
	  // specified in the mmp file.
    createFileSession.CreatePrivatePath(EDriveC);
    
      // Set the session path to
      // this private directory on drive C:
    createFileSession.SetSessionToPrivate(EDriveC);

	  // create TAnyandRObjects1.dat and open for writing
	User::LeaveIfError(createFile.Replace(createFileSession,
		                                  KFileName,
					                      EFileWrite|EFileStreamText));

      // Note that Write() requires a TDesC8
	  // type so we need to construct an explicit
	  // TDesC8 type to represent the data contained
	  // in the standard (16-bit) descriptor.
	TPtrC8 representation((TUint8*)(&KTestData)->Ptr(), (&KTestData)->Size());


	  // write and commit text
	User::LeaveIfError(createFile.Write(representation));
	User::LeaveIfError(createFile.Flush());
	_LIT(KMsgDataWritten,"Data written to file\n");
	console->Printf(KMsgDataWritten);

	  // close file and session
	  // (NB. no LeaveIfError due to RFile.close and 
	  // RFs.close guaranteed to complete)
	createFile.Close();	                      // close file
	createFileSession.Close(); 	              // close file server session
	}

void useBufferL(TPtr8& bufferPtr)
	{
	printBuffer(0,bufferPtr);
	  // Remove following comment to force a leave
	  // while using the buffer
	//User::Leave(KErrGeneral);
	}


LOCAL_C void doExampleL()
    {
				// create the datafile for the example
	createDataFileL();

				// create a simple buffer. In real code, you
				// would probably use an HBufC*, or an RBuf.
				// You could also use a TBuf on the stack if it's small.
	TText8* buffer=(TText8*) User::Alloc(100*sizeof(TText8));
				
				// push it to the cleanup stack: treated as TAny*
	CleanupStack::PushL(buffer);

				// create a pointer to the buffer
	TPtr8 bufferPtr(buffer,100); 
	
				// the file session to be used
	RFs fsSession;
	_LIT(KMsgOpeningSession,"Opening session\n");
	console->Printf(KMsgOpeningSession);

				// open the file-server session
	User::LeaveIfError(fsSession.Connect());

				// the file instance myFile
	RFileWithCleanup myFile;
	_LIT(KMsgOpeningFile,"Opening file\n");
	console->Printf(KMsgOpeningFile);

				// open the file
	User::LeaveIfError(myFile.Open(fsSession,KFileName,EFileStreamText|EFileRead));

				// push the file instance to the cleanup stack
	CleanupStack::PushL(myFile);	  	

				// read stuff from the file to the buffer (may leave)
	_LIT(KMsgReadingFile,"Reading file into buffer.\n");
	console->Printf(KMsgReadingFile);
	User::LeaveIfError(myFile.Read(bufferPtr));

      // Remove following comment to force a leave
	  // while using the file
	//User::Leave(KErrGeneral);

				// destroy the file on the cleanup stack
	CleanupStack::PopAndDestroy();		

	fsSession.Close();

				// use the buffer
	useBufferL(bufferPtr);

				// destroy the buffer on the cleanup stack
	CleanupStack::PopAndDestroy();		
	}

