// TLS1dll.h
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.


// DLL example program (1) to demonstrate the use of Thread local storage.

#include <e32test.h>


class CSetter : public CBase
  	{
public:
	IMPORT_C CSetter(CConsoleBase& aConsole);
	~CSetter();          
	IMPORT_C void   SetStaticTextL(const TDesC& aString);
	IMPORT_C void   ShowStaticText() const;
private: 
	CConsoleBase& iConsole;    // Use the console (not owned)
	};


class CGeneral : public CBase
  	{
public:
	IMPORT_C      CGeneral(CConsoleBase& aConsole);
	IMPORT_C void ShowStaticText() const;
private:
	CConsoleBase& iConsole; // Use the console (not owned)
	};

