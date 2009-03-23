// client_api.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#include "client_api.h"
#include "definitions.h"

// Constructor
EXPORT_C RServAppService::RServAppService()
{
}

// Destructor
EXPORT_C RServAppService::~RServAppService()
{
}

// Sends a request for service to the server side.
// The message input parameter (contained in a descriptor)
// is wrapped into a TIpcArgs type message.		
EXPORT_C TInt RServAppService::Send(const TDesC & aMessage)
{
	return SendReceive(EServAppServExample,TIpcArgs(&aMessage));
}

// Returns the UID of the service the client has requested
TUid RServAppService::ServiceUid() const
{	
	return KServiceType;
}