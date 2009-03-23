// standalone.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//


#include <e32std.h>
#include <eikstart.h>

const TUid KAppEmbeddableUid = {0xE800008F}; 

// Standard entry point function.
// Calls embedded application 
TInt E32Main()
	{
	return EikStart::RunApplication(KAppEmbeddableUid);
	}
