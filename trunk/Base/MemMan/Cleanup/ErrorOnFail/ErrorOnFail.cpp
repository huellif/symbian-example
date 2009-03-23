// ErrorOnFail.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.
//
// Example shows attempt to construct an object and return an
// appropriate error code on failure.
//
// NOTE: the structure of this example is different to standard E32 examples


#include <e32cons.h>


//
// Common formats
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
LOCAL_C TInt doExample();      
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
	  // Initialize and call the example code under cleanup stack
	_LIT(KMsgExampleCode,"Symbian OS Example Code");
	console = Console::NewL(KMsgExampleCode,TSize(KConsFullScreen,KConsFullScreen));
	  // Put console onto the cleanup stack
	CleanupStack::PushL(console);

	  // Perform the example function
	TInt retVal;
	retVal = doExample();

	  // Show the value returned from the example
	_LIT(KFormat2,"Return code=%d.\n");
	console->Printf(KFormat2, retVal);
	_LIT(KMsgPressAnyKey," [press any key]");
	console->Printf(KMsgPressAnyKey);
	console->Getch();

	  // Remove the console object from the cleanupstack
	  // and destroy it 
	CleanupStack::PopAndDestroy();
    }


//////////////////////////////////////////////////////////////////////////////
//
// Do the example
//
//////////////////////////////////////////////////////////////////////////////
TInt doExample()
	{
	  // Memory alloc fails on the 'allocFailNumber' attempt.
	__UHEAP_SETFAIL(RHeap::EDeterministic,allocFailNumber);
	  // Allocate and test
	CExample* myExample = new CExample;
	if (!myExample) return(KErrNoMemory); 
	  // Do something with the CExample object
	myExample->iInt = 5;
	  //
	console->Printf(KCommonFormat1,myExample->iInt);
	  // Delete the CExample object
	delete myExample;
	  // Completed OK
	return KErrNone;
	}








