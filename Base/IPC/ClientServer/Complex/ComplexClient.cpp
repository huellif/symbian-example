// ComplexClient.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.


#include "ComplexClientAndServer.h"
#include "ComplexClient.h"
#include "CommonFramework.h"



/**
This is the main body of the example. It makes a connection
connects with the server, causing it to be created if it does not
already exist. 

It continues by making various requests on the server, displaying
and the results of those requests.

**NOTE**: The example does not demonstrate any security features - its purpose is simply
          to demonstrate the basic principles of client/server interaction.
*/

LOCAL_C void doExampleL()
	{
	_LIT(KTxtTestingCountServer,"Testing the count server test with 2 client subsessions; these represent independent counters \n\n");
	_LIT(KTxtInitCounterAWith,"\nInitialize counter A with : ");
	_LIT(KTxtInitCounterBWith,"\nInitialize counter B with : ");
	_LIT(KTxtInitCounterFailed,"\nSetting the counter from string failed: non-numeric character detected\n");
	_LIT(KTxtInitCounterSucceeded,"\nSetting the counter from string succeededd\n");
    _LIT(KTxtGetCounterAValue,"Getting counterA value from server: %d \n");
    _LIT(KTxtGetCounterBValue,"Getting counterB value from server: %d \n");	
    _LIT(KMsgPressAnyKey," (press any key to continue)\n");
	  // Useful integer variable.
	TInt ret;
			
	  // Say that we are testing the count server.
	console->Printf(KTxtTestingCountServer);
	
	  // This is our handle to the server.
    RCountSession countserv;

 	  // Connect to the count server, starting it up if we need to 
	  // (which in this example we do need to).
	  // This creates a session with the server.
	User::LeaveIfError(countserv.Connect());
	

	  // Set up the first subsession with the count server.
	  // We need to pass our handle to the server.
	RCountSubSession counterA;
	counterA.Open(countserv);
	console->Printf(KTxtInitCounterAWith);
	
	
      // Initialise the counter by passing an illegal string - this 
      // should fail - but we will check anyway before reporting
      // a failure.
	_LIT(KTxtIllegalString,"2a");
	console->Printf(KTxtIllegalString);
	ret = counterA.SetFromString(KTxtIllegalString);
	if (ret==ENonNumericString)
	    {
		console->Printf(KTxtInitCounterFailed);
	    }
	else
	    {
	    console->Printf(KTxtInitCounterSucceeded); 	
	    }
	
		
	  // Set up the second subsession with the count server.
	  // We need to pass our handle to the server.
	RCountSubSession counterB;
	counterB.Open(countserv);
	console->Printf(KTxtInitCounterBWith);
	
	
      // Initialise the counter by passing a legal string.
	_LIT(KTxtLegalString,"100");
	console->Printf(KTxtLegalString);
	ret = counterB.SetFromString(KTxtLegalString);
	if (ret==ENonNumericString)
	    {
		console->Printf(KTxtInitCounterFailed);
	    }
	else
	    {
	    console->Printf(KTxtInitCounterSucceeded); 	
	    }
	    
	console->Printf(KMsgPressAnyKey);
	console->Getch();
	console->ClearScreen();
	
  
	  // Now get the initial values back from the server.
	  // The 1st subsession should have a default value (because we initialised it with an illegal value).
	  // The 2nd subsession should have the value we specified (because we initialised it witha legal value).
	console->Printf(KTxtGetCounterAValue,counterA.CounterValue());  
	console->Printf(KTxtGetCounterBValue,counterB.CounterValue()); 
	

		// Increase CounterA by the default value
	_LIT(KTxt1,"\nIncrease counterA by default value (i.e. 1)..\n");
	console->Printf(KTxt1);
	counterA.Increase();
	console->Printf(KTxtGetCounterAValue,counterA.CounterValue());  

		// Increase CounterA by 2
	_LIT(KTxt2,"\nIncrease counterA by 2..\n");
	console->Printf(KTxt2);
	counterA.IncreaseBy(2);
	console->Printf(KTxtGetCounterAValue,counterA.CounterValue());  
	
	  // Increase CounterB by the default value
    _LIT(KTxt3,"\nIncrease counterB by default value (i.e. 1)..\n");
	console->Printf(KTxt3);
	counterB.Increase();
	console->Printf(KTxtGetCounterBValue,counterB.CounterValue());  
	
      // Increase CounterA by 7	
	_LIT(KTxt4,"\nIncrease counterA by 7..\n");
	console->Printf(KTxt4);
	counterA.IncreaseBy(7);
    console->Printf(KTxtGetCounterAValue,counterA.CounterValue());  	
	
	  // Increase CounterB by 5	
	_LIT(KTxt5,"\nIncrease counterB by 5..\n");
	console->Printf(KTxt5);
	counterB.IncreaseBy(5);
    console->Printf(KTxtGetCounterBValue,counterB.CounterValue());  	
	
	  // Decrease CounterA by the default value
	_LIT(KTxt6,"\nDecrease counterA..\n");
	console->Printf(KTxt6);
	counterA.Decrease();
    console->Printf(KTxtGetCounterAValue,counterA.CounterValue());  	
   
      // Decrease CounterB by 3
	_LIT(KTxt7,"\nDecrease counterB by 3..\n");
	console->Printf(KTxt7);
	counterB.DecreaseBy(3);
	console->Printf(KTxtGetCounterBValue,counterB.CounterValue());  	

      // Show the number of resources in use.
	_LIT(KTxt8,"\nResource count is.. %d \n");
    console->Printf(KTxt8,countserv.ResourceCount());

		//close both subsessions
		
	_LIT(KTxt9,"\nClosing counterA and then CounterB..\n");
	console->Printf(KTxt9);
	counterA.Close();
	counterB.Close();
	

	  // Close the sesssion with the count server.
	countserv.Close();
	
	  // NB in this example, it is possible to close the session before closing the 
	  // subsessions. This is because the subsessions are themsleves closed as a
	  // consequence of closing the session - also the subsessions offer a simple
	  // (almost trivial) synchronous service.
	  // In more complex cases, you would need to think through the consequences
	  // of closing a session, while a subsession is sill open.
	}

