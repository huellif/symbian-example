// HTTPEXAMPLEUTILS.H
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
// 

#include <e32test.h>


/** CHttpExampleUtils is a class that provides some user input utilities and holds a RTest used throughout
*/
class CHttpExampleUtils : public CBase
	{
public:
	static void InitCommsL();
	static CHttpExampleUtils* NewL(const TDesC& aTestName);
	~CHttpExampleUtils();
	void GetAnEntry(const TDesC& ourPrompt, TDes& currentstring);
	TInt GetSelection(const TDesC& ourPrompt, const TDesC& validChoices);
	void PressAnyKey();
	void LogIt(TRefByValue<const TDesC> aFmt, ...);	
	RTest& Test();
private:
	CHttpExampleUtils(const TDesC& aTestName);

private:
	RTest iTest;
	}; 


