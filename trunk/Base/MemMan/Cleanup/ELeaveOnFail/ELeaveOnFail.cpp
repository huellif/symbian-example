// ELeaveOnFail.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

// Example shows attempt to construct an object and specifying
// (ELeave) after the 'new' operator. 
// Specifying (ELeave) causes a leave on failure to allocate memory 
// for the new object. 

// NOTE: the structure of this example is different to standard E32 examples


#include <e32cons.h>

//
// Common format text
//

_LIT(KCommonFormat1,"Value of iInt is %d.\n");


  // All messages written to this
LOCAL_D CConsoleBase* console;
  
  // Flag which determines whether the doSomething() member function
  // of the CExample class should leave when called.
LOCAL_D TBool leaveFlag = ETrue;

  // Parameter for __UHEAP_SETFAIL
  // Allocation guaranteed to fail at this number of allocation attempts;
  // i.e. if set to n, allocation fails on the nth attempt. 
  // NB only used in debug mode
#ifdef _DEBUG
LOCAL_D TInt allocFailNumber = 1;
#endif
  
  // Function prototypes
LOCAL_C void doExampleL();      
LOCAL_C void callExampleL();



//////////////////////////////////////////////////////////////////////////////
//
// -----> CExample (definition)
//
// The class is used by the example code
//
//////////////////////////////////////////////////////////////////////////////
class CExample : public CBase
	{
public :
	void DoSomethingL();
public :
	TInt iInt;
	};


//////////////////////////////////////////////////////////////////////////////
//
// -----> CExample (implementation)
//
//////////////////////////////////////////////////////////////////////////////
void CExample::DoSomethingL() 
	{
	  // Leave if the global flag is set
	if (leaveFlag)
		{
		_LIT(KMsgLeaving,"DoSomethingL leaving.\n");
		console->Printf(KMsgLeaving);
		User::Leave(KErrGeneral);	 	
		}
	console->Printf(KCommonFormat1,iInt);
	}

//////////////////////////////////////////////////////////////////////////////
//
// Main function called by E32
//
//////////////////////////////////////////////////////////////////////////////
GLDEF_C TInt E32Main()
    {
	  // Get cleanup stack
	CTrapCleanup* cleanup=CTrapCleanup::New();

	  // Some more initialization, then do the example
	TRAPD(error,callExampleL());
	  
	  // callExampleL() should never leave.
	_LIT(KMsgPanicEpoc32ex,"EPOC32EX");
	__ASSERT_ALWAYS(!error,User::Panic(KMsgPanicEpoc32ex,error));

	  // destroy the cleanup stack
	delete cleanup;
	
	  // return
	return 0;
    }


//////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////
LOCAL_C void callExampleL() 
    {
	  // Initialize and call the example code under cleanup stack.
	_LIT(KMsgExampleCode,"Symbian OS Example Code");
	console = Console::NewL(KMsgExampleCode,TSize(KConsFullScreen,KConsFullScreen));
	  // Put console onto the cleanup stack.
	CleanupStack::PushL(console);

	  // Perform the example function under the protection of a 
	  // TRAP harness.
	TRAPD(error,doExampleL());
	_LIT(KMsgOK,"ok");
	_LIT(KFormat2,"failed: leave code = %d");
	if (error)
		console->Printf(KFormat2,error);
	else
		console->Printf(KMsgOK);
	  
	  // Continue
	_LIT(KMsgPressAnyKey," [press any key]");
	console->Printf(KMsgPressAnyKey);
	console->Getch();

	  // Remove the console object from the cleanupstack
	  // and destroy it. 
	CleanupStack::PopAndDestroy();
    }


//////////////////////////////////////////////////////////////////////////////
//
// Do the example
//
//////////////////////////////////////////////////////////////////////////////
void doExampleL()
	{
	  // Memory alloc fails on the 'allocFailNumber' attempt. 
	  //
	  // Note that if you set this to some low value sucah as 1 or 2, then as a side effect,
	  // you may also see a dialog box stating "Not enough memory".
	__UHEAP_SETFAIL(RHeap::EDeterministic,allocFailNumber);
	  // Allocate - leave if allocation fails.
	  // The (ELeave) causes a leave if allocation fails; replaces
	  // a call to User::LeaveIfNull(myExample);
	  // Compare with the code in LeaveOnFailure
	  //
	  // Note that the memory allocation will fail and the "new" operation
	  // will leave if allocFailNumber is 1.
	CExample* myExample = new (ELeave) CExample;
	  // Do something with the CExample object
	myExample->iInt = 5;
	  //
	console->Printf(KCommonFormat1,myExample->iInt);
	  // Delete the CExample object
	delete myExample;
	}








