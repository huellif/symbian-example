//
// Interface.inl
// Copyright © 1997-2001 Symbian Ltd. All rights reserved.
//

// Set default to be first implementation found by resolver
_LIT8(KDefaultImplementation,"*");

inline CExampleInterface::CExampleInterface()
	{
	}

inline CExampleInterface::~CExampleInterface()
	{
	// Destroy any instance variables and then
	// inform the framework that this specific 
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

inline CExampleInterface* CExampleInterface::NewL()
	{
	// Set up the interface find for the default resolver.
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(KDefaultImplementation());
	resolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching

	// Set up some empty initialisation parameters
	TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	const TUid KExResolverUid = {0x10009DD0};
	return REINTERPRET_CAST(CExampleInterface*, 
							REComSession::CreateImplementationL(KCExampleInterfaceUid, 
															   _FOFF(CExampleInterface,iDtor_ID_Key),
															   &initParams,
															   resolverParams,
															   KExResolverUid));
	}

inline CExampleInterface* CExampleInterface::NewL(const TDesC8& aMatchString)
	{
	// Set up the interface find for the default resolver.
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(aMatchString);
	resolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching

	// Set up some empty initialisation parameters
	TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	return REINTERPRET_CAST(CExampleInterface*, 
							REComSession::CreateImplementationL(KCExampleInterfaceUid, 
															   _FOFF(CExampleInterface,iDtor_ID_Key), 
															   &initParams,
															   resolverParams));
	}

inline CExampleInterface* CExampleInterface::NewL(const TDesC8& aMatchString, TExampleInterfaceInitParams& aParams)
	{
	// Set up the interface find for the default resolver.
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(aMatchString);
	resolverParams.SetWildcardMatch(ETrue);		// Allow wildcard matching

	// The CreateImplementationL method will return
	// the created item.
	return REINTERPRET_CAST(CExampleInterface*, REComSession::CreateImplementationL(KCExampleInterfaceUid,
												_FOFF(CExampleInterface,iDtor_ID_Key),
												&aParams,
												resolverParams));
	}

inline void CExampleInterface::ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray)
	{
	REComSession::ListImplementationsL(KCExampleInterfaceUid, aImplInfoArray);
	}
