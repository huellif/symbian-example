// session.h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef __SESSION_H
#define __SESSION_H

#include <ApaServerApp.h>

//CServAppSession class definition

class CServAppSession: public CApaAppServiceBase
{
public:	
	IMPORT_C ~CServAppSession();
	
	IMPORT_C virtual void ServiceL (const RMessage2& aMessage);

protected:
	IMPORT_C CServAppSession();

private:		
	virtual void Display(const TDesC & aMessage) = 0;
};

#endif  //__SESSION_H