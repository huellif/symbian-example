// server.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#include "server.h"
#include "definitions.h"
#include "serv_app_minimal.h"


// Constructor
EXPORT_C CServAppServer::CServAppServer()
{
}

// Destructor
EXPORT_C CServAppServer::~CServAppServer()
{
}

// Identifies the type of service (through the UID input parameter) 
// the client has requested and creates an instance of the session
// that implements this service. In case of unidentified service type,
// the base class implementation of this method is called.                                           
EXPORT_C CApaAppServiceBase* CServAppServer::CreateServiceL(TUid aServiceType) const
{
	if (aServiceType==KServiceType)
		return new(ELeave) CMinimalSession();
	else
		return CEikAppServer::CreateServiceL(aServiceType);
}

