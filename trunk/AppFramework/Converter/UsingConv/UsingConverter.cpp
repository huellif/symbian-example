// UsingConverter.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//
///////////////////////////////////////////////////////////////////////
//
// This code demonstrates how to use a file converter.

//////////////////////////////////////////////////////////////////////////
#include <e32base.h>
#include <e32cons.h>
#include <s32file.h>
#include <s32mem.h>
#include <conarc.h>
#include <concnf.h>
#include <conlist.h>
#include <bautils.h>
#include "usingconverter.h"
#include <ecom.h>
static CConsoleBase* gConsole;

_LIT8(KFromType,"example/qp");
_LIT8(KToType,"example/text");
_LIT(KLitOriginalQuotedPrintableFile,    "c:\\Private\\E800009C\\w7allchr.doc.qp");
_LIT(KLitGeneratedDocFileFromQuotedPrintable,    "c:\\Private\\E800009C\\w7allchr.doc.qp.doc");

void CUsingConverter::ConvertFileL(CCnaConverterList& aList, TInt aConverterUid, const TDesC& aInputFile, const TDesC& aOutputFile)
    {
    // Finds the converter with the specified UID, loads its DLL and returns a pointer to it
    CConverterBase* const converter=aList.NewConverterL(TUid::Uid(aConverterUid));
    if(converter==NULL)
        {
        _LIT(KConsoleMessage, "\nConverter not found");
        gConsole->Printf(KConsoleMessage);
        User::Leave(KErrNotFound);
        }
    else
        {
        CleanupStack::PushL(converter);
        if(converter->Uid().iUid==aConverterUid)
            {
            _LIT(KConsoleMessage1, "\nCorrect Uid");
            gConsole->Printf(KConsoleMessage1);
            if(converter->Capabilities()&CConverterBase::EConvertsObjects)
                {
                _LIT(KConsoleMessage2, "\nCorrect Capability");
                gConsole->Printf(KConsoleMessage2);
                
                RFileReadStream inputStream;
                CleanupClosePushL(inputStream);
                RFileWriteStream outputStream;
                CleanupClosePushL(outputStream);
                User::LeaveIfError(inputStream.Open(iFs, aInputFile, EFileShareReadersOnly|EFileStream|EFileRead));
                User::LeaveIfError(outputStream.Replace(iFs, aOutputFile, EFileShareExclusive|EFileStream|EFileWrite));
                // this calls the converter's DoConvertL() and ConvertObjectAL()
                converter->ConvertObjectL(inputStream, outputStream);
                outputStream.CommitL();
                CleanupStack::PopAndDestroy(2,&inputStream);
				_LIT(KConsoleMessage3, "\nFile converted successfully");
                gConsole->Printf(KConsoleMessage3);
                }
            else
                {
                _LIT(KConsoleMessage4, "\nIncorrect Capability");
                gConsole->Printf(KConsoleMessage4);
                }
            }
        else
            {
            _LIT(KConsoleMessage5, "\nIncorrect UID");
            gConsole->Printf(KConsoleMessage5);
            }    
        CleanupStack::PopAndDestroy(converter);
        }
     }

void CUsingConverter::EgConverterL()
    {
    // connects to the file server 
    iFs.Connect();
    // creates the executable's private directory (which stores the input and output files) 
    User::LeaveIfError(iFs.CreatePrivatePath(EDriveC));
    // gets a list of available converters.
    CCnaConverterList* const list=CCnaConverterList::NewLC();
    // Gets the uid of the converter that converts from the source to the target data types
    TUid cuid =list->ConverterL(TDataType(KFromType),TDataType(KToType));
	ConvertFileL(*list,cuid.iUid,KLitOriginalQuotedPrintableFile, KLitGeneratedDocFileFromQuotedPrintable);

    CleanupStack::PopAndDestroy(list);
    }
    

CUsingConverter::~CUsingConverter()
    {
    // Closes the session with the file server
    iFs.Close();
    }
    

CUsingConverter::CUsingConverter()
    {
    }

static void DoExampleL()
    {
    // Create the console to print the messages to.
    _LIT(KConsoleMessageDisplay, "Converter Example");
    _LIT(KConsoleStars,"\n*************************\n");
    gConsole = Console::NewL(KConsoleMessageDisplay,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(gConsole);
    gConsole->Printf(KConsoleMessageDisplay);
    gConsole->Printf(KConsoleStars);
    
    CUsingConverter* example= new(ELeave) CUsingConverter();
    TRAPD(err, example->EgConverterL());
    if (err)
        {
        _LIT(KFailed,"\n\nExample failed: leave code=%d");
        gConsole->Printf(KFailed, err);
        }
    delete example;    
    // wait for user to press a key before destroying gConsole
    _LIT(KMsgPressAnyKey, "\nPress any key to continue");
    gConsole->Printf(KMsgPressAnyKey);
    gConsole->Getch();
     
    //This method is called once all the ECOM implementations
    //are destroyed
    REComSession::FinalClose();
    CleanupStack::PopAndDestroy(gConsole);
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
            TRAP_IGNORE(DoExampleL());
            delete cleanup;
            }
        delete scheduler;
        } 
    __UHEAP_MARKEND;
    return KErrNone;
    }
