// main_serv_app.cpp
//
// Copyright (c) Symbian Software Ltd 2006.  All rights reserved.
//

#include <e32base.h>

#include "client_api.h"
#include "session.h"

const TUid KAppUid = { 0xE1111556 };

LOCAL_C void callExampleL();

GLDEF_C TInt  E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();

	TRAPD(error,callExampleL());
	_LIT(KTxtTrainingTest,"TrainingApp");	
	__ASSERT_ALWAYS(!error,User::Panic(KTxtTrainingTest,error));
	delete cleanup;
	__UHEAP_MARKEND; // Check the test program doesn't leak memory.
	return KErrNone;
	}
	
LOCAL_C void callExampleL()
    {
    RServAppService exampleSess;
    
    _LIT(KHelloWorld,"Hello World!");
    
    TBufC<12> exampleBuf(KHelloWorld);
    
  	exampleSess.ConnectNewAppL(KAppUid);
    exampleSess.Send(exampleBuf);
    exampleSess.Close();
    }