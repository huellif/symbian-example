//
// Interface.h
// Copyright © 1997-2001 Symbian Ltd. All rights reserved.
//

#ifndef _CEXAMPLEINTERFACE__
#define _CEXAMPLEINTERFACE__

#include <e32base.h>
#include <ECom.h>
#include <badesca.h>

// UID of this interface
const TUid KCExampleInterfaceUid = {0x10009DC0};

/**
	An example abstract class being representative of the
	concrete class which the client wishes to use.

	It acts as a base, for a real class to provide all the 
	functionality that a client requires.  
	It supplies instantiation & destruction by using
	the ECom framework, and functional services
	by using the methods of the actual class.
 */
class CExampleInterface : public CBase
	{
public:
	// The interface for passing initialisation parameters
	// to the derived class constructor.
	struct TExampleInterfaceInitParams
		{
		TInt integer;
		const TDesC* descriptor;
		};

	// Instantiates an object of this type 
	static CExampleInterface* NewL();

	// Instantiates an object of this type 
	// using the aMatchString as the resolver parameters.
	static CExampleInterface* NewL(const TDesC8& aMatchString);

	// Instantiates an object of this type
	// using the aMatchString as the resolver parameters
	// and passing the aParams parameters 
	static CExampleInterface* NewL(const TDesC8& aMatchString, TExampleInterfaceInitParams& aParams);

	// Destructor.
	virtual ~CExampleInterface();

	// Request a list of all available implementations which 
	// satisfy this given interface.
	static void ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray);

	// Pure interface method
	// Representative of a method provided on the interface by 
	// the interface definer.
	virtual void DoMethodL(TDes& aString) = 0;

protected:
	//Default c'tor
	inline CExampleInterface();

private:
	// Unique instance identifier key
	TUid iDtor_ID_Key;
	};

#include "Interface.inl"

#endif 

