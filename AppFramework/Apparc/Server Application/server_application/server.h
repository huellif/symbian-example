// server.h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef __SERVER_H
#define __SERVER_H

#include <EikServerApp.h>


//CServAppServer class declaration

class CServAppServer : public CEikAppServer
{
public:
	IMPORT_C CApaAppServiceBase* CreateServiceL(TUid aServiceType) const;
	
	IMPORT_C CServAppServer();
	IMPORT_C ~CServAppServer();	
};

#endif