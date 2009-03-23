// session.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#include "session.h"
#include "definitions.h"

// Constructor
EXPORT_C CServAppSession::CServAppSession()
{
}

// Destructor
EXPORT_C CServAppSession::~CServAppSession()
{
}

// Implements the virtual function of the base class.
// According to the service type the client requested,
// the appropriate service support code is called.
// In this case, where there is just one service type, the 1st argument
// of the RMessage2 object is extracted and HandleDisplayMessage() method is called
// to handle this argument.

// In case of unidentified requested service, the ServiceL() method
// of base class is called.
EXPORT_C void CServAppSession::ServiceL (const RMessage2& aMessage)
{
	TInt ret = KErrNone;
	switch(aMessage.Function())
	{
		case EServAppServExample:
			HBufC* buf;
			buf = HBufC::NewLC(aMessage.GetDesLength(0));
			TPtr ptr (buf->Des());
			ret = aMessage.Read(0,ptr);
			Display(ptr);  
			CleanupStack::PopAndDestroy(buf);  //free the heap memory that was allocated with HBufC::NewLC()
			break;
		default:
			CApaAppServiceBase::ServiceL(aMessage);
			break;
	}
	aMessage.Complete(ret);
}
