// clientserver.h
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.
//

// Transient server example - shared client/server definitions

#include <e32std.h>

_LIT(KMyServerName,"t-server");
_LIT(KMyServerImg,"t-server");		// EXE name
const TUid KServerUid3={0x01000000};

const TInt KMaxMyMessage=100;

enum TMyMessages
	{
	ESend,
	EReceive,
	ECancelReceive
	};

