// ComplexServer.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

#include "ComplexClientAndServer.h"
#include "ComplexServer.h"
#include <e32svr.h>
#include <e32uid.h>

//
//NOTE**: The example does not demonstrate any security features - its purpose is simply
//        to demonstrate the basic principles of client/server interaction.
//


/**
The count server thread function that initialises the server.
*/
TInt CCountServer::ThreadFunction(TAny* /**aStarted*/)
	{
	  // Useful TInt variable
	TInt err;
	
	  // create cleanup stack
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
	    {
		PanicServer(ECreateTrapCleanup);
	    }
		
	  // Create an active scheduler.
	CActiveScheduler* pScheduler=new CActiveScheduler;
	__ASSERT_ALWAYS(pScheduler,PanicServer(EMainSchedulerError));
	  // Install the active scheduler.
	CActiveScheduler::Install(pScheduler);
	
	  // Create the server object.
	CCountServer* pServer = NULL;
	TRAP(err,pServer = CCountServer::NewL(EPriorityStandard));
	__ASSERT_ALWAYS(!err,CCountServer::PanicServer(ESvrCreateServer));
	
	  // Start the server
	err = pServer->Start(KCountServerName);
	if (err != KErrNone)
	    {
		CCountServer::PanicServer(ESvrStartServer);
	    }
	    
      // Let everyone know that we are ready to
      // deal with requests.
    RThread::Rendezvous(KErrNone);
    
      // And start fielding requests from client(s).
	CActiveScheduler::Start();
	
      // Tidy up...
    delete pServer;
    delete pScheduler;
    delete cleanup; 

	  // ...although we should never get here!
	return KErrNone;
	}


/**
Create the thread that will act as the server.
This function is exported from the DLL and called by the client.

Note that a server can also be implemented as a separate
executable (i.e. as a separate process).
*/
EXPORT_C TInt StartThread(RThread& aServerThread)
	{
	TInt res=KErrNone;
	
      // Create the server, if one with this name does not already exist.
	
	TFindServer findCountServer(KCountServerName);
	TFullName   name;
	
	  // Need to check that the server exists.
	if (findCountServer.Next(name)!=KErrNone)
	    {
	      // Create the thread for the server.
		res=aServerThread.Create(KCountServerName,
			CCountServer::ThreadFunction,
			KDefaultStackSize,
			KDefaultHeapSize,
			KDefaultHeapSize,
			NULL
			);
		  // The thread has been created OK so get it started - however
          // we need to make sure that it has started before we continue.
		if (res==KErrNone)
			{
			TRequestStatus rendezvousStatus;
			
			aServerThread.SetPriority(EPriorityNormal);
			aServerThread.Rendezvous(rendezvousStatus);
			aServerThread.Resume();
			User::WaitForRequest(rendezvousStatus);
			}
			
		  // The thread has not been created - clearly there's been a problem.
		else
			{
			aServerThread.Close();
			}
		}
    return res;
	}
