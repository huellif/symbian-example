// centrepexample.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//
/** 
@file
This example program demonstrates the use of the Central Repository. 

The code demonstrates how to open this repository, read its settings, 
change them, perform read-write operations in a transaction, 
find settings, restore default settings, request change notifications 
and delete settings.

The program requires that before it is run a data file representing
a repository is present in the private\10202be9 directory on the
emulator.

The example defines an example repository in the E80000AD.txt file.
This text file can be installed directly, but it is recommended
that repository files are first converted to binary format using the 
CentRepConv tool.

To do this:  

- Copy E80000AD.txt file to the epoc32\winscw\c location on your kit.

- Start a command line prompt, cd to epoc32\RELEASE\WINSCW\UDEB, and run the
command CentRepConv C:\E80000AD.txt 

- This then displays that the file has been converted to the new binary file 
E80000AD.cre. 

- Copy this file to the epoc32\RELEASE\WINSCW\UDEB\Z\private\10202be9
directory on your kit.
*/
#include "centrepexample.h"
#include <e32cons.h>
#include "asyncwaiter.h"

_LIT(KTitle, "Central repository example");
_LIT(KTextPressAKey, "\n\nPress any key to step through the example");
_LIT(KExit,"\nPress any key to exit the application ");
_LIT(KPressAKey,"\nPress any key to continue \n");
_LIT(KErr,"\nThe repository file has not been set up. Please see the instructions in centrepexample.cpp for how to do this");

_LIT(KOpen,"\n\nOpening the repository and displaying some initial settings");
_LIT(KChangeSet,"\nChanging some settings in the repository");
_LIT(KReadSet,"\nReading the changed settings");
_LIT(KTransact,"\n\nPerforming read and write transactions");
_LIT(KDelete,"\n\nCreating integer and real number settings and deleting them");
_LIT(KMove,"\n\nMoving the key to target position");
_LIT(KReset,"\nResetting settings to default values, and getting notifications that the changes have occurred");
_LIT(KFind,"\nFinding settings for simple and structured data");
_LIT(KNonExisting,"\n\nValue of new setting is %d");
_LIT(KInt,"\nValue of setting with key x01 is %d");
_LIT(KReal,"\nValue of setting with key x02 is %f");
_LIT(KInteg,"\nValue of setting with key x06 is %d \n");
_LIT(KIntBefore,"\nValue of setting with key x01 before reset is %d");
_LIT(KIntAfter,"\nValue of setting with key x01 after reset is %d");
_LIT(KIdsFound,"\nFound %d settings");
_LIT(KStringName,"Another string");
_LIT(KString1_UpdatedValue, "Value of setting with key x05 is %s");

static const TUid KUidRepository = { 0xE80000AD };
const TUint32 KNonExistentSetting = 0x10;
const TUint32 KInt1 = 0x01;
const TUint32 KInt3 = 0x06;
const TUint32 KReal1 = 0x02;
const TUint32 KString1 = 0x05;
const TInt KInt1_InitialValue = 1;
const TInt KInt1_UpdatedValue = 73;
const TReal KReal1_InitialValue = 14.91;
const TReal KReal1_UpdatedValue = 72.8;
const TUint32 KMoveTarget = 0x30;

/**
Allocates and constructs a CCentRepExample object and 
leaves it on the cleanup stack.
Initialises all member data to their default values.
*/	
CCentRepExample* CCentRepExample::NewLC()
	{
	CCentRepExample* rep = new(ELeave) CCentRepExample();
	CleanupStack::PushL(rep);
	rep->ConstructL();
	return rep;
	}
	
/**
Constructor
*/
CCentRepExample::CCentRepExample()
	{
	}	

void CCentRepExample::ConstructL()
	{
	
	iConsole = Console::NewL(KTitle,TSize(KConsFullScreen,KConsFullScreen));
	iConsole->Printf ( KTextPressAKey );
	iConsole->Getch ();
	}

/**
Destructor
*/
CCentRepExample::~CCentRepExample()
	{
	iConsole->Printf(KExit);
	iConsole->Getch();
	
	delete iConsole;
	delete iRepository;
	}


/**
Resets the whole repository to the state of the initialization file 
originally used to set it up.
@leave KErrNotSupported, system-wide error codes.
*/ 

void CCentRepExample::ResetL()
 	{
 	CRepository* repository = NULL;
 	TRAPD(err, repository = CRepository::NewL(KUidRepository););
 	CleanupStack::PushL(repository);
 	// test if an error has occurred because the repository
 	// has not been set up: see instructions at the top
 	// of this file if this occurs.
 	if (err == KErrNotFound) 
  		{
  		iConsole->Printf(KErr); 
  		}
 	User::LeaveIfError(err);
 	User::LeaveIfError(err = repository->Reset());
 	CleanupStack::PopAndDestroy(repository);
 	}
	
/**
Creates a CRepository object for accessing the 
repository with the specified UID and displays some of the initial settings.
@leave KErrNotFound, KErrAbort, KErrPermissionDenied,
KErrArgument, system-wide error codes.
*/
void CCentRepExample::OpenRepositoryL()
	{
	TInt i, k;
	TReal j;
	TBuf<50> tooShort;
	iConsole->Printf(KOpen);
	iRepository = CRepository::NewL(KUidRepository);
	
	User::LeaveIfError(iRepository->Get(KInt1, i));
	iConsole->Printf(KInt,i);

	User::LeaveIfError(iRepository->Get(KReal1, j));
	iConsole->Printf(KReal,j);
	
	User::LeaveIfError(iRepository->Get(KInt3, k));
	iConsole->Printf(KInteg,k);
	
	User::LeaveIfError(iRepository->Get(KString1, tooShort));
	iConsole->Printf(KString1_UpdatedValue,tooShort.PtrZ());
	
	iConsole->Printf(KPressAKey);
	iConsole->Getch();
		
	}
	
/**
Sets an existing setting to a new value or 
creates a new setting with a value if the setting doesn't exist. 
@leave KErrAbort, KErrPermissionDenied, KErrArgument, system-wide error codes. 
*/
void CCentRepExample::ChangeSettingsL()
	{
	iConsole->Printf(KChangeSet);
	User::LeaveIfError(iRepository->Set(KNonExistentSetting, 10));
	
	User::LeaveIfError(iRepository->Set(KInt1, KInt1_UpdatedValue));
	
	User::LeaveIfError(iRepository->Set(KReal1, KReal1_InitialValue));
		
	User::LeaveIfError(iRepository->Set(KInt3, KInt1_InitialValue));

	User::LeaveIfError(iRepository->Set(KString1, KStringName));

	}
	
/**
Reads the Changed settings and prints them.
@leave KErrNotFound, KErrAbort, KErrPermissionDenied,
KErrArgument, system-wide error codes.
*/
void CCentRepExample::ReadSettingsL()
	{
	TInt m, i, k;
	TReal j;
	TBuf<50> tooShort;
	iConsole->Printf(KReadSet);
	
	User::LeaveIfError(iRepository->Get(KNonExistentSetting, m));

	iConsole->Printf(KNonExisting,m);
	
	User::LeaveIfError(iRepository->Get(KInt1, i));
	
	iConsole->Printf(KInt,i);

	User::LeaveIfError(iRepository->Get(KReal1, j));

	iConsole->Printf(KReal,j);
	
	User::LeaveIfError(iRepository->Get(KInt3, k));
	
	iConsole->Printf(KInteg,k);
	
	User::LeaveIfError(iRepository->Get(KString1, tooShort));
	
	iConsole->Printf(KString1_UpdatedValue,tooShort.PtrZ());
	
	iConsole->Printf(KPressAKey);
	iConsole->Getch();

	}
	
/**
Finds all the settings for both simple and structured data.
Finds all the settings that exist and match the specification given by 
a partialKey and a mask. 
Matches occur whenever (key & mask) == (partialKey & mask).
@leave KErrAbort, KErrPermissionDenied, KErrArgument, system-wide error codes.
*/	
void CCentRepExample::FindSettingsL()
	{
	RArray<TUint32> foundIds;

	// Finds all the settings that exist and match the specification 
	//given by partialKey and mask. 
	iConsole->Printf(KFind);
	
	// These values will instruct Find to return the all settings keys which 
	//match the pattern 000001XXh where X indicates a ‘don’t care’ state.
	
 	User::LeaveIfError(iRepository->FindL(0x100 /*partialKey*/, 0xF00/*mask*/, foundIds));
 
	iConsole->Printf(KIdsFound,foundIds.Count()); 
	foundIds.Reset();

	// Finds all the settings that contain a given integer and 
	//match the specification given by partialKey and mask. 

	User::LeaveIfError(iRepository->FindEqL(0x00/*partialKey*/, 0x00/*mask*/,KInt1_InitialValue /*integer value*/, foundIds));
	
	iConsole->Printf(KIdsFound,foundIds.Count()); 
	foundIds.Reset();

	// Finds all the settings that contain the given floating point value 
 	//and match the specification given by partialKey and mask. 
	User::LeaveIfError(iRepository->FindEqL(0x00, 0x00,KReal1_InitialValue/*Real value*/, foundIds));

	iConsole->Printf(KIdsFound,foundIds.Count());
	foundIds.Reset();

	// Finds all the settings that contain a given string value 
	//and match the specification given by partialKey and mask. 
	
	User::LeaveIfError(iRepository->FindEqL(0x00, 0x00, KStringName/*string*/, foundIds));

	iConsole->Printf(KIdsFound,foundIds.Count()); 
	foundIds.Reset();

	// Finds all the settings that match the specification given 
	// by partialKey and mask, but are either not integer values or 
	//do not have the given value. 
	
	User::LeaveIfError(iRepository->FindNeqL(0x00/*partial key*/, 0x00/*mask*/, KInt1_UpdatedValue, foundIds));
	
	iConsole->Printf(KIdsFound,foundIds.Count()); 
	foundIds.Reset();

	// Finds all the settings that match the specification given by 
	//partialKey and mask, but are either not floating point values 
	//or do not have the given value.
	
	User::LeaveIfError(iRepository->FindNeqL(0x100, 0x0F0, KReal1_UpdatedValue, foundIds));
	
	iConsole->Printf(KIdsFound,foundIds.Count()); 
	foundIds.Reset();
	
	}
/**
Sets KInt1 to a different value, resets it and gets 
notification that this value has changed.
@leave KErrAlreadyExists error from IPC or server side resource allocation failure
*/
void CCentRepExample::ResetAndNotifyL()
	{
	TInt x;
	
	// Ensure KInt1 is set to a different value to its initial value
	// First change to setting should cause notification
	iConsole->Printf(KReset);
	
	User::LeaveIfError(iRepository->Set(KInt1, KInt1_InitialValue+10));

	
	User::LeaveIfError(iRepository->Get(KInt1, x));
	
	iConsole->Printf(KIntBefore, x);
	
	CAsyncWaiter* waiter = CAsyncWaiter::NewL();
	CleanupStack::PushL(waiter);
	
	User::LeaveIfError(iRepository->NotifyRequest(KInt1, waiter->iStatus));
	
	// Get a notification on a reset as well
	
	User::LeaveIfError(iRepository->Reset(KInt1));
	
	// Check we got a notification
	waiter->StartAndWait();
	User::LeaveIfError(waiter->Result());
	
	// Check KInt1 now has the right value
	User::LeaveIfError(iRepository->Get(KInt1, x));
	iConsole->Printf(KIntAfter, x);

	CleanupStack::PopAndDestroy(waiter);
	}
	
/**
Moves a key from one location to another.
@leave KErrAbort, KErrPermissionDenied, KErrArgument, system-wide error codes.
*/	
void CCentRepExample::MoveSettingsL()
	{
	TUint32 keyInfo;
		
	iConsole->Printf(KMove);
	iConsole->Printf(KPressAKey);
	iConsole->Getch();
	// Move the key to the target position
	User::LeaveIfError(iRepository->Move(KInt3, KMoveTarget, 0xFFFFFFFF,
			keyInfo));
	}	
	
/**
Perform read and write operations using transactions.
@leave KErrLocked, panic.
*/
void CCentRepExample::TransactionFuncL()
	{
	TUint32 keyId;
	TInt intVal;
	
	iConsole->Printf(KTransact);
	
	// Attempts to start a read write transaction.
	User::LeaveIfError(iRepository->StartTransaction(CRepository::EReadWriteTransaction));
	
	// Calls FailTransaction if activated by a Leave or PopAndDestroy.
	iRepository->CleanupCancelTransactionPushL();

	// Creating the variable KNewInt at 0x16
	const TUint32 KNewInt = 0x16;
	const TInt KIntValue = 1201;
	intVal = KIntValue +33;
	
	// Perform some write operations.
	// Creating KNewInt with the value KIntValue.
	User::LeaveIfError(iRepository->Create(KNewInt, KIntValue));
	// Setting KNewInt to the new value
	User::LeaveIfError(iRepository->Set(KNewInt,intVal));
	
	// Persistence of all values read and written during the transaction 
	// is only guaranteed after a successful return from CommitTransaction.
	User::LeaveIfError(iRepository->CommitTransaction(keyId));
	
	// Read the data written.
	User::LeaveIfError(iRepository->Get(KNewInt, intVal));
	
	// Deleting the data after reading it.
	User::LeaveIfError(iRepository->Delete(KNewInt));
	CleanupStack::Pop(); 

	CRepository* repository1;
	User::LeaveIfNull(repository1 = CRepository::NewLC(KUidRepository));
	
	// Begin read transaction for repository1
	User::LeaveIfError(repository1->StartTransaction(CRepository::EReadTransaction));
	
	CRepository* repository2;
	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidRepository));
	
	// Should be able to start another read transaction (Multiple read transactions)
	User::LeaveIfError(repository2->StartTransaction(CRepository::EReadTransaction));
	
	// Perform some gets using the open transactions and repositories
	// Read operation
	User::LeaveIfError(repository1->Get(KInt1, intVal));
	// Ensure transaction cancelled if following code Leaves:
    repository1->CleanupCancelTransactionPushL();
    
    // Commit the transaction 
    User::LeaveIfError(repository1->CommitTransaction(keyId));
    CleanupStack::Pop(); 
	
	// Calls FailTransaction if activated by a Leave or PopAndDestroy.
	repository2->CleanupCancelTransactionPushL();
	User::LeaveIfError(repository2->Get(KInt1, intVal));
	User::LeaveIfError(repository2->CommitTransaction(keyId));
	
	// Pop the transaction
	CleanupStack::Pop();
	CleanupStack::PopAndDestroy(2,repository1);
	

	}
	
/**
Deletes 3 settings KInt1_InitialValue, KMoveTarget and KReal1
@leave KErrAbort, KErrPermissionDenied, KErrArgument, system-wide error codes.
*/	
void CCentRepExample::DeleteL()
	{
	TInt x;
	TReal y;
	
	iConsole->Printf(KDelete);
			
	User::LeaveIfError(iRepository->Get(KInt1_InitialValue, x));
	User::LeaveIfError(iRepository->Get(KReal1, y));
	User::LeaveIfError(iRepository->Delete(KInt1_InitialValue));
	User::LeaveIfError(iRepository->Delete(KMoveTarget));
	User::LeaveIfError(iRepository->Delete(KReal1));
	}
				
LOCAL_C void MainL()
	{
	// Create an Active Scheduler to handle asychronous calls
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );
	CCentRepExample * app = CCentRepExample ::NewLC();

	// Reset the repository
	app->ResetL();
	
	// Open the repository
	app->OpenRepositoryL();
	
	// Change the existing settings
	app->ChangeSettingsL();
	
	// Read the changed settings
	app->ReadSettingsL();
	
	// Find the settings using FindL, FindEqL and FindNeqL
	app->FindSettingsL();
	
	// Move settings
	app->MoveSettingsL();
	
	// Reset settings to default values and get notification
	app->ResetAndNotifyL();

	// Perform multiple operations in a transaction
	app->TransactionFuncL();
	
	// Delete settings
	app->DeleteL();
	
	CleanupStack::PopAndDestroy(2); //app, scheduler

	}

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(cleanup == NULL)
    	{
    	return KErrNoMemory;
    	}
    TRAPD(err, MainL());
	if(err != KErrNone)
		{
		User::Panic(_L("Failed to complete"),err);
		}

    delete cleanup;
    __UHEAP_MARKEND;
    return KErrNone;
	}
