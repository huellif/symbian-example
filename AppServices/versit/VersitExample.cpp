// VersitExample.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.

// This example demonstrates a vCard parser


#include <e32base.h>
#include <e32cons.h>
#include <vcard.h>
#include <versit.h>
#include "VersitExample.h"

// Global definition
static CConsoleBase* gConsole;

// name of file to write the vCard to
_LIT(KVCardFileJIS,"c:\\Private\\E8000094\\charsetJIS.vcf");

// string for display
_LIT(KMsgPressAnyKey,"\n\nPress any key to continue\n");

CExampleVersit* CExampleVersit::NewL()
	{
	CExampleVersit* self = new (ELeave) CExampleVersit();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CExampleVersit::ConstructL()
	{
	iParser = CParserVCard::NewL();
	}	
	
CExampleVersit::~CExampleVersit()	
	{
	delete iParser;
	iFsSession .Close();
	}

// Externalises a vCard to a file
void CExampleVersit::ExternalizeToFileL(const TDesC& aFileName)
	{
	RFile file;
	iFsSession.Connect();
	// create the private directory (c:\Private\E8000094\)
	User::LeaveIfError(iFsSession.CreatePrivatePath(EDriveC));
	User::LeaveIfError(file.Replace(iFsSession, aFileName, EFileWrite));
	CleanupClosePushL(file);
	iParser->ExternalizeL(file);
	CleanupStack::PopAndDestroy(&file);
	}

// Internalises a vCard from a file
void CExampleVersit::InternalizeFromFileL(RFile& aInputFile)
	{
	TInt size;
	if (aInputFile.Size(size)==KErrNone)
		{
		delete iParser;
		iParser = NULL;

		iParser = CParserVCard::NewL();
		RFileReadStream stream(aInputFile);
		CleanupClosePushL(stream);
		iParser->InternalizeL(stream);
		CleanupStack::PopAndDestroy(&stream);
		}
	}

// Creates a vCard containing a note property and a character set property parameter.
// Then externalizes the vCard to a file.
void CExampleVersit::CreateAndExternalizeVCardL()
	{
	//create a property value to hold some text
	_LIT(KNote,"\x4e79\x4f19\x5032");
	CParserPropertyValue* value=CParserPropertyValueHBufC::NewL(KNote);
	CleanupStack::PushL(value);

	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave)CArrayPtrFlat<CParserParam>(5);
	CleanupStack::PushL(arrayOfParams);

	// Add a character set property parameter
	CParserParam* parserParam=CParserParam::NewL(KVersitTokenCHARSET,KVersitTokenJIS);
	CleanupStack::PushL(parserParam);
	arrayOfParams->AppendL(parserParam);
	CleanupStack::Pop(parserParam);

	// create the NOTE property
	CParserGroupedProperty* property=CParserGroupedProperty::NewL(*value,KVersitTokenNOTE,NULL,arrayOfParams);

	CleanupStack::Pop(2,value); // value, arrayOfParams
	CleanupStack::PushL(property);

	// Add the property to the vCard
	iParser->AddPropertyL(property);
	CleanupStack::Pop(property);
	//Sets the default transformation format
	iParser->SetDefaultCharSet(Versit::EJISCharSet);
	ExternalizeToFileL(KVCardFileJIS);
	}
	
//Internalize the VCard
void CExampleVersit::InternalizeVCardL()
	{
	RFile file;
	TInt err=file.Open(iFsSession,KVCardFileJIS,EFileRead);
	CleanupClosePushL(file);
	InternalizeFromFileL(file);
	CleanupStack::PopAndDestroy(&file);
	
	_LIT(KConsoleMessage1, "vCard has been successfully internalised from a file");
	gConsole->Printf(KConsoleMessage1);
	}

void CExampleVersit::EgVersitL()
	{
	CreateAndExternalizeVCardL();
	InternalizeVCardL();
	}

static void DoExampleL()
	{
	// Create the console to print the messages to.
	_LIT(KConsoleMessageDisplay, "Versit Example");
	_LIT(KConsoleStars,"\n*************************\n");
	gConsole = Console::NewL(KConsoleMessageDisplay,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(gConsole);
	gConsole->Printf(KConsoleMessageDisplay);
	gConsole->Printf(KConsoleStars);

	CExampleVersit* egVersit= CExampleVersit::NewL();
	TRAPD(err, egVersit->EgVersitL());
	if (err)
		{
		_LIT(KFailed,"\n\nExample failed: leave code=%d");
		gConsole->Printf(KFailed, err);
		}
	delete egVersit;	
	// wait for user to press a key before destroying gConsole
 	gConsole->Printf(KMsgPressAnyKey);
 	gConsole->Getch();
	CleanupStack::PopAndDestroy(gConsole);
	}

// Standard entry point function
TInt E32Main()
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
			TRAP_IGNORE(DoExampleL());
			delete cleanup;
			}
		delete scheduler;
		} 
	__UHEAP_MARKEND;
	return KErrNone;
	}
