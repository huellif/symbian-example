// txin.cpp
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

// includes
#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#if !defined(__E32CONS_H__)
#include <e32cons.h>
#endif
#if !defined(__MSVAPI_H__)
#include <msvapi.h>
#endif

LOCAL_D CConsoleBase* console; 
LOCAL_C void consoleUIInstallL(); 
LOCAL_C void DoInstallL();

// constants
#ifdef __WINS__
_LIT(KMTMDataFullName,"z:\\Resource\\messaging\\mtm\\txtmtm.RSC");
#else
_LIT(KMTMDataFullName,"c:\\Resource\\messaging\\mtm\\txtmtm.RSC");
#endif

// Dummy observer class to absorb Message Server session events
class TDummyObserver : public MMsvSessionObserver
	{
public:
	void HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/) {};
	};

LOCAL_C void DoInstallL()
// Set up Message Server session and install Text MTM group
	{
	TDummyObserver ob;
	CMsvSession* session = CMsvSession::OpenSyncL(ob);
	CleanupStack::PushL(session);
	TInt err=session->DeInstallMtmGroup(KMTMDataFullName);
	if (err!=KErrNone && err!=KErrNotFound)
		User::LeaveIfError(err);
	User::LeaveIfError(session->InstallMtmGroup(KMTMDataFullName));

	CleanupStack::PopAndDestroy(); // session
	}

LOCAL_C void consoleUIInstallL() 
// Set up simple console UI
    {
	_LIT(KTxtTitle,"TextMTM install");
	_LIT(KFormatFailed,"failed: leave code=%d");

	// Construct and install the active scheduler
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler); 

	console=Console::NewL(KTxtTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	TRAPD(error,DoInstallL()); // call install function
	if (error)
		console->Printf(KFormatFailed, error);
	else 
	CleanupStack::PopAndDestroy(2); // scheduler, console
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,consoleUIInstallL()); 
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return error; 
    }
