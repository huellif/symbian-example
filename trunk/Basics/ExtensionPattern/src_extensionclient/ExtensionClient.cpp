// ExtensionClient.cpp
//
// Copyright (c) 1997-2003 Symbian Ltd.  All rights reserved.
//
// Implements a simple console application to use functionality from both OriginalDll
// and the ExtensionDll
// Should work without rebuild for both versions of OriginalDll & ExtensionDll
//
#include <e32base.h>
#include <e32cons.h>
#include <numberstore.h>

_LIT(KHello,"Hello World!\r\n"); 
_LIT(KTxtEPOC32EX,"EPOC32EX");
_LIT(KTxtExampleCode,"Symbian OS Example Code");
_LIT(KFormatFailed,"failed: leave code=%d");
_LIT(KTxtOK,"ok");
_LIT(KTxtPressAnyKey," [press any key]");

LOCAL_D CConsoleBase* console;
LOCAL_C void callExampleL(); 
LOCAL_C void doExampleL(); 

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); 
	TRAPD(error,callExampleL()); 
	__ASSERT_ALWAYS(!error,User::Panic(KTxtEPOC32EX,error));
	delete cleanup; 
	__UHEAP_MARKEND;
	return 0; 
    }

LOCAL_C void callExampleL()
    {
	console=Console::NewL(KTxtExampleCode,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	TRAPD(error,doExampleL()); 
	if (error)
		console->Printf(KFormatFailed, error);
	else
		console->Printf(KTxtOK);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); 
	CleanupStack::PopAndDestroy();
    }
    
LOCAL_C void doExampleL()
    {
	console->Printf(KHello);
	TInt num1=3;
	TInt num2=7;

	//Create & initialise the number store using original functionality
	CNumberStore* numberstore=new (ELeave) CNumberStore();
	numberstore->SetNumber1(num1);
	numberstore->SetNumber2(num2);
	console->Printf(_L("Created CNumberStore with values %d & %d\n"),num1,num2);
	num1=numberstore->Number1();
	num2=numberstore->Number2();
	console->Printf(_L("CNumberStore has values %d & %d\n"),num1,num2);

	TInt total=-1;
	TInt product=-1;
	
	//Use extension functionality

	//The following line will work as expected, using the implementation in the extension dll
	total=numberstore->AddNumbers();

	//The following line will fail to compile, as it attempts to call a private function
	//product=numberstore->DoMultiplyNumbers();

	//The following line will work as expected, using the implementation in the extension dll
	product=numberstore->MultiplyNumbers();

	console->Printf(_L("CNumberStore extension gives total %d\n"),total);
	console->Printf(_L("CNumberStore extension gives product with magic multiplyer %d\n"),product);


	delete numberstore;
	}
