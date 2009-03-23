// InterfaceClient.h
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.

#ifndef __CINTERFACECLIENT_H
#define __CINTERFACECLIENT_H

#include <Interface.h>

// Example ECOM client that uses the InterfaceImplementation plug-ins
// through the InterfaceDefinition (CExampleInterface) interface
class TInterfaceClient
	{
public:
	// Gets the default implementation of CExampleInterface
	void GetDefaultL();

	// Gets a CExampleInterface implementation by requesting
	// a specific types of capability
	void GetBySpecificationL();

	// Gets all the CExampleInterface implementations
	void GetByDiscoveryL();
	};

#endif

