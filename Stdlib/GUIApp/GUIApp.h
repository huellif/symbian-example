// GUIAppEng.H
//
// Copyright (c) 1997-1999 Symbian Ltd.  All rights reserved.
//

#if !defined(__SLSUMEIK_H__)
#define __SLSUMEIK_H__

//////////////////////////////////////////////////////////////////////////////
//
// Include files containing:
//
// 1. Symbol definitions used by C++ code and resource scripts (the *.hrh)
// 2. Resource ids generated by resource compilation of 
//    resource scripts (the *.rsg)
// 3. Class definitions required by this app (the *.h)
//    
//////////////////////////////////////////////////////////////////////////////

// 1.
#include <eikon.hrh>
#include "GUIApp.hrh"

#include <e32std.h> // required to avoid warning
// C header files
#include <fcntl.h>
#include <unistd.h>
#include <sys/reent.h>
#include "extern.h"

#include <eikon.rsg> // needed for file browse dialog
#include <GUIApp.rsg>

// 3.
#include <eikenv.h>
#include <eikappui.h>
#include <eikproc.h>
#include <eiktbar.h>
#include <eikfsel.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikfnlab.h>
#include <eikfbrow.h>
#include <eikfutil.h>
#include <eiktxlbm.h>

#include <coeutils.h>

#include <basched.h>

  // The unique identifier for this application.
  // NOTE that the number has been arbitrarily chosen for the purpose
  // of the example.
const TUid KUidCksumApp={0xE8000073};

// Number of digits required to hold the maximum length checksum
const TInt KMaxChecksumLength=10; 

//////////////////////////////////////////////////////////////////////////////
//
// -----> CExamplePrefsDialog(definition)
//
//////////////////////////////////////////////////////////////////////////////

class CExampleChecksumDialog : public CEikDialog
	{
public:
	CExampleChecksumDialog(TDes* aFileName, TInt aTitleId=0);
private:	
	void       PreLayoutDynInitL();
	TBool      OkToExitL(TInt aKeycode);
private:
	  // Data members defined by this class.
	TDes* iFileName;
	TInt iTitleId;
	};

class CExampleListBoxDialog : public CEikDialog
    {
public:
	CExampleListBoxDialog(CDesCArrayFlat* aCksumArray); // (change name of parameter)
private: // framework
    void PreLayoutDynInitL();
	void SetSizeAndPositionL(const TSize& aSize);
    TBool OkToExitL(TInt aKeycode);
	CDesCArrayFlat* iCksumArray;
    };


//////////////////////////////////////////////////////////////////////////////
//
// -----> CExampleContainer(definition)
//
//////////////////////////////////////////////////////////////////////////////
class CExampleContainer : public CCoeControl, 
                      public MCoeControlObserver
	{
public:
	  // Construction
	CExampleContainer();
	void ConstructL(const TRect& aRect);
	  // Destruction
	~CExampleContainer();
	
private:
	  // Virtual, defined by CCoeControl; replaces the default implementation
	  // provided by CCoeControl.
	void         Draw(const TRect& aRect) const;

	  // Defined as pure virtual by the mixin class MCoeControlObserver 
	  // inherited by CCoeControl. An empty implementation provided by 
	  // this class (its containees do not report events).
	void         HandleControlEventL(CCoeControl* aControl,
		                             TCoeEvent aEventType
									);
	
public:
	  // Member functions defined by this class
	void			 AppendChecksumL(const TDesC& aFilename);
	CDesCArrayFlat*	 GetChecksumArray() const;


private:
      // Data members defined and used by this class.
	CDesCArrayFlat* iChecksumArray;			// List of filenames and checksums
	};

//////////////////////////////////////////////////////////////////////////////
//
// -----> CExampleAppUi (definition)
//
//////////////////////////////////////////////////////////////////////////////
class CExampleAppUi : public CEikAppUi
	{
public:
	  // Destruction.
	~CExampleAppUi();

public:
	  // Virtual, defined by CEikAppUi; replaces the implementation 
	  // provided by CEikAppUi.
	void ConstructL();
	
private:
	  // Virtual, defined by CEikAppUi; ; empty implementation
	  // provided by CEikAppUi; full implementation provided
	  // by this class. 
	void  HandleCommandL(TInt aCommand);
	
private:
	  // Member functions defined by this class
	void         OnCmdExit();
	void		 OnCmdChecksumL();
	void		 OnCmdViewChecksumsL();
	void         DrawViewL();

private:
	  // Data members defined by this class.
	CExampleContainer* iContainer;      // The container control (ie the view)
	};

//////////////////////////////////////////////////////////////////////////////
//
// -----> CExampleDocument (definition)
//
//////////////////////////////////////////////////////////////////////////////
class CExampleDocument : public CEikDocument
	{
public:
	  // Construction.
	static CExampleDocument* NewL(CEikApplication& aApp);
    CExampleDocument(CEikApplication& aApp);
	  // Destruction.
	~CExampleDocument();

private:
	  // Defined as pure virtual by CEikDocument; full implementation
	  // provided by this class
	CEikAppUi* CreateAppUiL();		// Construct an app.user interface
	};

//////////////////////////////////////////////////////////////////////////////
//
// -----> CExampleApplication (definition)
//
//////////////////////////////////////////////////////////////////////////////
class CExampleApplication : public CEikApplication
	{
private:		
	  // Defined as pure virtual by CApaApplication; implementation
	  // provided by this class 
	TUid          AppDllUid() const; // Returns Uid associated with app

	  // Defined as pure virtual by CEikApplication; implementation
	  // provided by this class.
	CApaDocument* CreateDocumentL(); // Construct new document
	};

#endif