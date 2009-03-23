// ComplexServer.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

#include "ComplexClientAndServer.h"
#include "ComplexServer.h"
#include <e32svr.h>
#include <e32uid.h>



//**********************************
//CCountServer - implementations
//**********************************

/**
A utility function to panic the server.
*/
void CCountServer::PanicServer(TCountServPanic aPanic)
	{
	_LIT(KTxtCountServer,"CountServer");
	User::Panic(KTxtCountServer,aPanic);
	}


/**
First phase construction
*/
CCountServer* CCountServer::NewL(CActive::TPriority aActiveObjectPriority)
	{
	CCountServer* self=new (ELeave) CCountServer(aActiveObjectPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


/**
Creates and returns a new object container
using the server's object container index.

This is a service that is used by a session.
*/
CObjectCon* CCountServer::NewContainerL()
	{
	return iContainerIndex->CreateL();
	}


/**
Constructor takes the server priority value. 

The server is an active object, and the priority value is the priority
of this active object.

It passes the priority value to the base class in the Ctor list.
By default, the session is not sharable, which is what we want here
so no second parameter is passed to the CServer2 constructor.
*/
CCountServer::CCountServer(CActive::TPriority aActiveObjectPriority)
	: CServer2(aActiveObjectPriority)
	{
	}


/**
Second-phase constructor - creates the object container index.
*/
void CCountServer::ConstructL()
	{
	iContainerIndex=CObjectConIx::NewL();
	}


/**
Desctructor - deletes the object container index.
*/
CCountServer::~CCountServer()
	{
	delete iContainerIndex; 
	}





/**
Creates a new session with the server.
*/
CSession2* CCountServer::NewSessionL(const TVersion &aVersion,const RMessage2& /*aMessage*/) const
	{
	  // Check that the version is OK
	TVersion v(KCountServMajorVersionNumber,KCountServMinorVersionNumber,KCountServBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		User::Leave(KErrNotSupported);
	
	// CAN USE THE aMessage argument to check client's security and identity
	// can make use of this later but for now ignore. AH 4/5/05
	// the connect message is delivered via the RMessage2 object passed. 
	
	  // Create the session.
	return new (ELeave) CCountSession;
	}
	
	
