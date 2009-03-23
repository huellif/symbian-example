// ContactViews.cpp
//
// Copyright (c) 2005 Symbian Software Ltd.  All rights reserved.
//
//
//
///////////////////////////////////////////////////////////////////////
//
// Find and filtered views 
// -----------------------
//
// This example demonstrates CContactFindView and CContactFilteredView
//
//__________________________________________________________________
//
// This code creates a contact database, "exampleViews.cdb", adds an item 
// to it containing four fields, then uses a find view and filtered view 
// to search/filter the database. The contents of all matching items are 
// printed to the console.
////////////////////////////////////////////////////////////////////////


#include <e32base.h>
#include <e32cons.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
// User include
#include "ContactViews.h"

// stores the total number of views created in the example
const TInt KTotalViews = 3;
 
LOCAL_D CConsoleBase* console;
// name of contact database to be created
_LIT(KContactsFilename,"c:exampleViews.cdb");
// pointer to the database
LOCAL_C CContactDatabase* db;

CExampleViews* CExampleViews::NewL(CContactDatabase& aDb)
	{
	CExampleViews* self=new(ELeave) CExampleViews(aDb);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CExampleViews::~CExampleViews()
	{
	if (iFilterView!=NULL) 
		{
		iFilterView->Close(*this);	
		}
	if (iFindView!=NULL) 
		{
		iFindView->Close(*this);	
		}
	if (iLocalView!=NULL)
		{
		iLocalView->Close(*this);	
		}
	iSortOrder.Close();
	}
	

CExampleViews::CExampleViews(CContactDatabase& aDb)
:	CActive(EPriorityStandard),	iDb(aDb) {}

void CExampleViews::ConstructL()
	{
	// iSortOrder defines the fields that are used in the view
	iSortOrder.AppendL(KUidContactFieldGivenName);
	iSortOrder.AppendL(KUidContactFieldFamilyName);
	iSortOrder.AppendL(KUidContactFieldPhoneNumber);
	iSortOrder.AppendL(KUidContactFieldEMail);
	CActiveScheduler::Add(this);
	}

// Handles the active object’s request completion event.
void CExampleViews::RunL()
	{
	iLocalView=CContactLocalView::NewL(*this,iDb,iSortOrder,EContactsOnly);
	// creates the search string for the find view
	CPtrCArray* desArray =new (ELeave) CPtrCArray(1);
	_LIT(KSearchString,"Smith");
    TPtrC searchString(KSearchString);
	desArray->AppendL(searchString);
	// Creates a find view based on the local view
	iFindView = CContactFindView::NewL(iDb, *iLocalView,*this, desArray);
	desArray->Reset();
    delete desArray;
    // Creates a filtered view based on the local view
    // Filters out items without a telephone number
	iFilterView = CContactFilteredView::NewL(*this, iDb, *iLocalView, CContactDatabase::EPhonable);
	}

// invoked by the active object mechanism
void CExampleViews::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
 	{
	if (aEvent.iEventType==TContactViewEvent::EReady)
		{
		iNumViewsCreated++;
		if (&aView == iFindView)
		// find view is ready to be used
			{
			_LIT(KConsoleMessage,"Contents of find view:\n");
			console->Printf(KConsoleMessage);			
			for (TInt count = 0; count < iFindView->CountL(); count++)
				{
				// prints out contents of all fields for all items in the find view
				_LIT(KFieldSeparator,",");
				HBufC* text = iFindView->AllFieldsLC(count,KFieldSeparator);
				_LIT(KConsoleMessage2,"%S\n");
				console->Printf(KConsoleMessage2,text);
				CleanupStack::PopAndDestroy(); // text
				}
			}
		else if (&aView == iFilterView)
		// filter view is ready	
			{
			_LIT(KConsoleMessage,"Contents of filter view:\n");
			console->Printf(KConsoleMessage);
			for (TInt count = 0; count < iFilterView->CountL(); count++)
				{
				// prints out contents of all fields for all items in the filter view
				_LIT(KFieldSeparator,",");
				HBufC* text = iFilterView->AllFieldsLC(count,KFieldSeparator);
				_LIT(KConsoleMessage2,"%S\n");
				console->Printf(KConsoleMessage2,text);
				CleanupStack::PopAndDestroy(); // text
				}
			}
		// if all three views have been created, stop the active scheduler
		if (iNumViewsCreated == KTotalViews)
			{
			CActiveScheduler::Stop();
			}
		}
	}
	
	
void CExampleViews::DoCancel()
	{
	//empty implementation.
	}

void CExampleViews::Start()
	{
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}


LOCAL_C void CreateExampleViewL()
// Create the example views
	{
	CExampleViews *exampleViews = CExampleViews::NewL(*db);
	CleanupStack::PushL(exampleViews);
	exampleViews->Start();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(exampleViews);
	// close and cleanup database
	delete db;
	db=NULL;
	}

// Adds a contact item to the contact database. 
LOCAL_C void AddEntryL()
	{
	_LIT(KForename,"John"); 
	_LIT(KSurname,"Smith"); 
	_LIT(KPhoneNumber,"+441617779700"); 
	_LIT(KEmailAddress,"john.smith@symbian.com"); 
	
	// Create a  contact card to contain the data
	CContactCard* newCard = CContactCard::NewLC();
    
	// Create the firstName field and add the data to it
	CContactItemField* firstName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
	firstName->TextStorage()->SetTextL(KForename);
	newCard->AddFieldL(*firstName);
  	CleanupStack::Pop(firstName);
  	
	// Create the lastName field and add the data to it
   	CContactItemField* lastName= CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
  	lastName ->TextStorage()->SetTextL(KSurname);
  	newCard->AddFieldL(*lastName);
  	CleanupStack::Pop(lastName);
  	
	// Create the emailAddress field and add the data to it
	CContactItemField* emailAddr = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldEMail);
	emailAddr->SetMapping(KUidContactFieldVCardMapEMAILINTERNET);
  	emailAddr ->TextStorage()->SetTextL(KEmailAddress);
  	newCard->AddFieldL(*emailAddr);
  	CleanupStack::Pop(emailAddr);
    	
	// Create the phoneNo field and add the data to it
  	CContactItemField* phoneNumber = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPhoneNumber);
	phoneNumber->SetMapping(KUidContactFieldVCardMapTEL);
	phoneNumber ->TextStorage()->SetTextL(KPhoneNumber);
	newCard->AddFieldL(*phoneNumber);
  	CleanupStack::Pop(phoneNumber);
	
	// Add newCard to the database
    const TContactItemId contactId = db->AddNewContactL(*newCard);
  	CleanupStack::PopAndDestroy(newCard);
	}


// Creates the contact database and opens it
LOCAL_C void CreateDatabaseL()
	{
	TRAPD(err,db=CContactDatabase::ReplaceL(KContactsFilename));
         
	// Checks if the database was created successfully
	// if not, exits with an error message.
	
	if(err!=KErrNone)
		{
		_LIT(KConsoleMessage,"Not able to create the database"); 
		// Failed to create the database
		console->Printf(KConsoleMessage);
		User::Leave(err);
		}
	else
		{
		_LIT(KConsoleMessage,"Successfully created the database\n"); 
		console->Printf(KConsoleMessage);
		// Add a contact entry to the database
		AddEntryL();
		
		}
	}


LOCAL_C void DoExampleL()
	{
	_LIT(KTxtConsoleTitle,"Contact views example");
	// Create a console
	console = Console::NewL(KTxtConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	
	// Create the database 
   	CreateDatabaseL();
   	
   	// Create the views
	CreateExampleViewL();
   	    
	// wait for user to press a key before destroying console
 	_LIT(KMsgPressAnyKey,"Press any key to continue\n\n");
	console->Printf(KMsgPressAnyKey);
 	console->Getch();
	CleanupStack::PopAndDestroy(console);
 	}

// Standard entry point function
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	// Active scheduler required as this is a console app
	CActiveScheduler* scheduler=new CActiveScheduler;

	// If active scheduler has been created, install it.
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler); 
		// Cleanup stack needed
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
            // Call the function DoExampleL()
		    TRAP_IGNORE(DoExampleL());
			delete cleanup;
			}
		delete scheduler;
		}

	__UHEAP_MARKEND;
	return KErrNone;
	}
