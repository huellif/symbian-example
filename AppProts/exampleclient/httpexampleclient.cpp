// HTTPEXAMPLECLIENT.CPP
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
//


#include <uri8.h>
#include <e32base.h>
#include <http.h>
#include <chttpformencoder.h>

#include "httpexampleclient.h"
#include "httpexampleutils.h"


// format for output of data/time values
_LIT(KDateFormat,"%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S.%C%:3");

// File system root
_LIT(KFileSystemRoot,"C:\\");

// Standard headers used by default
_LIT8(KUserAgent, "HTTPExampleClient (1.0)");
_LIT8(KAccept, "*/*");

_LIT(KHttpExampleClientPanic, "HTTP-EC");

enum THttpExampleClientPanics
	{
	EReqBodySumitBufferNotAllocated,
	KBodyWithInvalidSize,
	KCouldntNotifyBodyDataPart
	};

// Size of buffer used when submitting request bodies
const TInt KMaxSubmitSize = 1024;
const TInt KMaxHeaderNameLen = 32;
const TInt KMaxHeaderValueLen = 128;


//
// Implementation of CHttpClient
//


// Supplied as the name of the test program to CHttpExampleUtils
_LIT(KHttpExampleClientTestName, "HttpExample");

CHttpClient::CHttpClient()
	: iReqBodySubmitBufferPtr(0,0)
	{
	// Initial timestamp is time now
	ResetTimeElapsed();
	}


CHttpClient::~CHttpClient()
	{
	iSess.Close();
	iFileServ.Close();
	delete iReqBodySubmitBuffer;
	delete iTransObs;
	delete iFormEncoder;
	delete iUtils;
	}

CHttpClient* CHttpClient::NewLC()
	{
	CHttpClient* me = new(ELeave) CHttpClient;
	CleanupStack::PushL(me);
	me->ConstructL();
	return me;
	}

CHttpClient* CHttpClient::NewL()
	{
	CHttpClient* me = NewLC();
	CleanupStack::Pop(me);
	return me;
	}

void CHttpClient::ConstructL()
	{
	iUtils = CHttpExampleUtils::NewL(KHttpExampleClientTestName);
	
	// Open the RHTTPSession
	iSess.OpenL();

	// Install this class as the callback for authentication requests
	InstallAuthenticationL(iSess);
	
	User::LeaveIfError(iFileServ.Connect());
	iTransObs = CHttpEventHandler::NewL(*iUtils);
	}

void CHttpClient::StartClientL()
	{
	ResetTimeElapsed();
	TBuf<256> url;
	RStringPool strP = iSess.StringPool();

	// Repeat until user selects quit
	TBool done = EFalse;
	while (!done)
		{
		// Get user command from menu
		iUtils->Test().Console()->ClearScreen();
		_LIT(KSelectOption, " Select an option \n\n");
		iUtils->Test().Printf(KSelectOption);
		_LIT(KPossibleSelectionsText, " 1 Get \n 2 Post \n 3 Head \n 4 Trace \n 5 Toggle Verbose \n 6 Quit \n");
		_LIT(KPossibleSelections,"123456");
		TInt selection = iUtils->GetSelection(KPossibleSelectionsText, KPossibleSelections);
		RStringF method;
		iHasARequestBody = EFalse;

		// Set the method to use and ask for a url
		switch (selection)
			{
		case EGet: 
			method = strP.StringF(HTTP::EGET,RHTTPSession::GetTable());
			break;
		case EPost:
			method = strP.StringF(HTTP::EPOST,RHTTPSession::GetTable());
			iHasARequestBody = ETrue; 
			break;
		case EHead: 
			method = strP.StringF(HTTP::EHEAD,RHTTPSession::GetTable()); 
			break;
		case ETrace:
			method = strP.StringF(HTTP::ETRACE,RHTTPSession::GetTable()); 
			break;
		case EToggleVerbosity: 
			{
			TBool verbose = iTransObs->Verbose();
			iTransObs->SetVerbose(!verbose);
			if (!verbose)
				User::InfoPrint(_L("Verbosity is ON "));
			else
				User::InfoPrint(_L("Verbosity is OFF"));
			}
			break;
		case EQuit:
			done = ETrue;
			break;
		default: 
			 break;
			} 

		if (!done && (selection >= 0) && (selection <= 3))
			{
			// Get the URL
			_LIT(KEnterUrl, "Enter Url");
			iUtils->GetAnEntry(KEnterUrl, url);
			if (url.Length() > 0) // back to main menu if we still have an empty URL
				{
				// Get a filename to submit as our request body, for methods that need one
				if (iHasARequestBody)
					{
					GetRequestBodyL(method);
					}

				// Start the method off
				TBuf8<256> url8;
				url8.Copy(url);
				InvokeHttpMethodL(url8, method);
				}
			else
				{
				_LIT(KEmptyUrlErrorText, "You must supply a non empty url");
				iUtils->Test().Printf(KEmptyUrlErrorText);
				User::After(1000000);
				}
			}

		} // while

	// End of tests
	DisplayTimeElapsed();
	iUtils->PressAnyKey();
}

/** Get the post body data from the user. 
The user supplies name and value pairs. These are then encoded as added by CHTTPFormEncoder which acts as a data supplier
for this request. This is typically how a form body for a html form submission that uses POST might be constructed.
*/
void CHttpClient::GetPostBodyManuallyL()
	{
	if (iFormEncoder)
		{
		delete iFormEncoder;
		iFormEncoder = NULL;
		}

	iFormEncoder = CHTTPFormEncoder::NewL(); 
	TBuf<256> name;
	TBuf<256> value;
	TBuf8<256> name8;
	TBuf8<256> value8;
	
	_LIT(KGetPostName, "Enter Name (END to finish)");
	_LIT(KGetPostValue, "Enter Value ");
	_LIT(KEnd, "END");
	do
		{
		iUtils->GetAnEntry(KGetPostName, name);
		if (name.CompareF(KEnd) != 0)
			{
			iUtils->GetAnEntry(KGetPostValue, value);
			name8.Copy(name);
			value8.Copy(value);
			iFormEncoder->AddFieldL(name8, value8);
			}
		}
		while (name.CompareF(KEnd) != 0);
	}


/** Gets the body that you wish to submit when using a POST Method
This can be supplied as a file or can be input manually in name,value pairs
*/
void CHttpClient::GetRequestBodyL(RStringF& aMethod)
	{
	if (aMethod== iSess.StringPool().StringF(HTTP::EPOST,RHTTPSession::GetTable()))
		{
		// get a post body by file or contruct manually?
		_LIT(KConstructPostManually, "\n> Construct Post Manually?  Yes | No ");
		_LIT(KYesNo, "YyNn");
		TInt selection = iUtils->GetSelection(KConstructPostManually,KYesNo);
		if (selection < 2) // 2 is NO
			{
			iManualPost = ETrue;
			GetPostBodyManuallyL();
			return;
			}
		// else carry on as usual and get post data from a file
		}

	iManualPost = EFalse;
	_LIT(KRequestPathPrompt, "Enter path to request body file: ");
	iUtils->GetAnEntry(KRequestPathPrompt, iReqBodyFileName);
	_LIT(KRequestBodyContentType, "Enter request body content-type: ");
	iUtils->GetAnEntry(KRequestBodyContentType, iReqBodyContentType);
	iParsedFileName.Set(KFileSystemRoot,&iReqBodyFileName,NULL);

	// Check it exists and open a file handle
	if (iFileServ.IsValidName(iReqBodyFileName))
		{
		User::LeaveIfError(iReqBodyFile.Open(iFileServ, iParsedFileName.FullName(), EFileRead));
		delete iReqBodySubmitBuffer;
		iReqBodySubmitBuffer = NULL;
		iReqBodySubmitBuffer = HBufC8::NewMaxL(KMaxSubmitSize);
		iReqBodySubmitBufferPtr.Set(iReqBodySubmitBuffer->Des());
		}
	else
		{
		_LIT(KFileNameNotValid, "The specified filename is not valid!.\n");
		iUtils->Test().Printf(KFileNameNotValid);
		}
	}

TBool CHttpClient::GetNextDataPart(TPtrC8& aDataPart)
	{
	__ASSERT_DEBUG(iReqBodySubmitBuffer, User::Panic(KHttpExampleClientPanic, EReqBodySumitBufferNotAllocated));
	// Read from the request body file
	TBool retVal = EFalse;
	TInt err = iReqBodyFile.Read(iReqBodySubmitBufferPtr);
	if (err == KErrNone)
		{
		aDataPart.Set(iReqBodySubmitBufferPtr);
		++iDataChunkCount;
		retVal = (iReqBodySubmitBufferPtr.Length() == 0);
		}
	return retVal;
	}

void CHttpClient::ReleaseData()
	{
	// Clear out the submit buffer
	TPtr8 buff = iReqBodySubmitBuffer->Des();
	buff.Zero();
	// Notify HTTP of more data available immediately, since it's being read from file
	TRAPD(err, iTrans.NotifyNewRequestBodyPartL());
	if (err != KErrNone)
		User::Panic(KHttpExampleClientPanic, KCouldntNotifyBodyDataPart);
	}

TInt CHttpClient::OverallDataSize()
	{
	TInt size = 0;
	TInt err = iReqBodyFile.Size(size);
	if (err < 0)
		User::Panic(KHttpExampleClientPanic,KBodyWithInvalidSize);

	return size;
	}

TInt CHttpClient::Reset()
	{
	return KErrNotSupported;
	}


/** Invoke the http method
This actually creates the transaction, sets the headers and body and then starts the transaction 
*/
void CHttpClient::InvokeHttpMethodL(const TDesC8& aUri, RStringF aMethod)
	{
	// Set the protocol, before the first transaction gets started
/*	if (iUseWspProtocol)
		{
		RHTTPConnectionInfo ci = iSess.ConnectionInfo();
		ci.SetPropertyL(iSess.StringPool().StringF(HTTP::EProtocol,RHTTPSession::GetTable()),
						THTTPHdrVal(iSess.StringPool().StringF(HTTP::EWSP,RHTTPSession::GetTable())));
		}*/

	iDataChunkCount = 0;
	TUriParser8 uri; 
	uri.Parse(aUri);
	iTrans = iSess.OpenTransactionL(uri, *iTransObs, aMethod);
	RHTTPHeaders hdr = iTrans.Request().GetHeaderCollection();

	// Add headers appropriate to all methods
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EAccept, KAccept);

	// Add headers and body data for methods that use request bodies
	if (iHasARequestBody)
		{
		// Content type header
		TBuf8<KMaxContentTypeSize> contTypeBuf;
		contTypeBuf.Copy(iReqBodyContentType);
		RStringF contTypeStr = iSess.StringPool().OpenFStringL(contTypeBuf);
		THTTPHdrVal contType(contTypeStr);
		hdr.SetFieldL(iSess.StringPool().StringF(HTTP::EContentType,RHTTPSession::GetTable()), contType);
		contTypeStr.Close();
		
		MHTTPDataSupplier* dataSupplier = this;
		if (iManualPost)
			dataSupplier = iFormEncoder;
		iTrans.Request().SetBody(*dataSupplier);
		}

	// submit the transaction
	iTrans.SubmitL();

	// Start the scheduler, once the transaction completes or is cancelled on an error the scheduler will be
	// stopped in the event handler
	CActiveScheduler::Start();

	// all done
	iUtils->PressAnyKey();

	// close the request body file, if one was opened
	if (iHasARequestBody)
		iReqBodyFile.Close();
	}

void CHttpClient::SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue)
	{
	RStringF valStr = iSess.StringPool().OpenFStringL(aHdrValue);
	THTTPHdrVal val(valStr);
	aHeaders.SetFieldL(iSess.StringPool().StringF(aHdrField,RHTTPSession::GetTable()), val);
	valStr.Close();
	}

void CHttpClient::ResetTimeElapsed()
// Resets timestamp to time now
	{
	iLastTimeStamp.UniversalTime(); 
	}


void CHttpClient::DisplayTimeElapsed()
// Calculate elapsed time since last measurement, and display
	{
	TTime timeNow;
	timeNow.UniversalTime();
	TTimeIntervalMicroSeconds elapsedMicroSec =
									timeNow.MicroSecondsFrom(iLastTimeStamp);
	iLastTimeStamp = timeNow;
	iUtils->Test().Printf(
		_L("Time elapsed since last measurement is: %d ms\n"),
		elapsedMicroSec.Int64()/1000
		);
	}


/** Called when a authenticated page is requested
Asks the user for a username and password that would be appropriate for the url that was
supplied.
*/
TBool CHttpClient::GetCredentialsL(const TUriC8& aURI, RString aRealm, 
								   RStringF aAuthenticationType,
								   RString& aUsername, 
								   RString& aPassword)

	{
	TBuf<80> scratch;
	TBuf8<80> scratch8;
	scratch8.Format(_L8("Enter credentials for URL %S, realm %S"), &aURI.UriDes(), &aRealm.DesC());
	scratch.Copy(scratch8);
	iUtils->Test().Printf(_L("%S\n"), &scratch);
	scratch.Copy(aAuthenticationType.DesC());
	iUtils->Test().Printf(_L("Using %S authentication\n"), &scratch);
	iUtils->GetAnEntry(_L("Username (or QUIT to give up): "), scratch);
	scratch8.Copy(scratch);
	if (scratch8.CompareF(_L8("quit")))
		{
		TRAPD(err, aUsername = aRealm.Pool().OpenStringL(scratch8));
		if (!err)
			{
			iUtils->GetAnEntry(_L("Password: "), scratch);
			scratch8.Copy(scratch);
			TRAP(err, aPassword = aRealm.Pool().OpenStringL(scratch8));
			if (!err)
				return ETrue;
			}
		}
	return EFalse;
	}


//
// Implementation of class CHttpEventHandler
//

void CHttpEventHandler::ConstructL()
	{
	User::LeaveIfError(iFileServ.Connect());
	}


CHttpEventHandler::CHttpEventHandler(CHttpExampleUtils& aUtils) :
	iUtils(aUtils)
	{
	}


CHttpEventHandler::~CHttpEventHandler()
	{
	iFileServ.Close();
	}


CHttpEventHandler* CHttpEventHandler::NewLC(CHttpExampleUtils& aUtils)
	{
	CHttpEventHandler* me = new(ELeave)CHttpEventHandler(aUtils);
	CleanupStack::PushL(me);
	me->ConstructL();
	return me;
	}


CHttpEventHandler* CHttpEventHandler::NewL(CHttpExampleUtils& aUtils)
	{
	CHttpEventHandler* me = NewLC(aUtils);
	CleanupStack::Pop(me);
	return me;
	}


void CHttpEventHandler::SetVerbose(TBool aVerbose)
	{
	iVerbose = aVerbose;
	}


TBool CHttpEventHandler::Verbose() const
	{
	return iVerbose;
	}


void CHttpEventHandler::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
	{
	switch (aEvent.iStatus)
		{
		case THTTPEvent::EGotResponseHeaders:
			{
			// HTTP response headers have been received. We can determine now if there is
			// going to be a response body to save.
			RHTTPResponse resp = aTransaction.Response();
			TInt status = resp.StatusCode();
			RStringF statusStr = resp.StatusText();
			TBuf<32> statusStr16;
			statusStr16.Copy(statusStr.DesC());
			iUtils.Test().Printf(_L("Status: %d (%S)\n"), status, &statusStr16);

			// Dump the headers if we're being verbose
			if (iVerbose)
				DumpRespHeadersL(aTransaction);

			// Determine if the body will be saved to disk
			iSavingResponseBody = EFalse;
			TBool cancelling = EFalse;
			if (resp.HasBody() && (status >= 200) && (status < 300) && (status != 204))
				{
				TInt dataSize = resp.Body()->OverallDataSize();
				if (dataSize >= 0)
					iUtils.Test().Printf(_L("Response body size is %d\n"), dataSize);
				else
					iUtils.Test().Printf(_L("Response body size is unknown\n"));

				TInt selection = iUtils.GetSelection(_L("\n> Save response to disk? Yes | No | Cancel"), _L("YyNnCc"));
				iSavingResponseBody = (selection < 2);
				cancelling = (selection == 4) || (selection == 5);
				}

			// If we're cancelling, must do it now..
			if (cancelling)
				{
				iUtils.Test().Printf(_L("\nTransaction Cancelled\n"));
				aTransaction.Close();
				CActiveScheduler::Stop();
				}
			else if (iSavingResponseBody) // If we're saving, then open a file handle for the new file
				{
				iUtils.GetAnEntry(_L("Enter filename including path to save response body"), iRespBodyFileName);
				iParsedFileName.Set(KFileSystemRoot,&iRespBodyFileName,NULL);

				// Check it exists and open a file handle
				TInt valid = iFileServ.IsValidName(iRespBodyFileName);
				if (!valid)
					{
					iUtils.Test().Printf(_L("The specified filename is not valid!.\n"));
					iSavingResponseBody = EFalse;
					}
				else
					{
					TInt err = iRespBodyFile.Replace(iFileServ,
													 iParsedFileName.FullName(),
													 EFileWrite|EFileShareExclusive);
					if (err)
						{
						iSavingResponseBody = EFalse;
						User::Leave(err);
						}
					}
				}

			} break;
		case THTTPEvent::EGotResponseBodyData:
			{
			// Get the body data supplier
			iRespBody = aTransaction.Response().Body();

			// Some (more) body data has been received (in the HTTP response)
			if (iVerbose)
				DumpRespBody(aTransaction);
			else
				iUtils.Test().Printf(_L("*"));

			// Append to the output file if we're saving responses
			if (iSavingResponseBody)
				{
				TPtrC8 bodyData;
				TBool lastChunk = iRespBody->GetNextDataPart(bodyData);
				iRespBodyFile.Write(bodyData);
				if (lastChunk)
					iRespBodyFile.Close();
				}

			// Done with that bit of body data
			iRespBody->ReleaseData();
			} break;
		case THTTPEvent::EResponseComplete:
			{
			// The transaction's response is complete
			iUtils.Test().Printf(_L("\nTransaction Complete\n"));
			} break;
		case THTTPEvent::ESucceeded:
			{
			iUtils.Test().Printf(_L("Transaction Successful\n"));
			aTransaction.Close();
			CActiveScheduler::Stop();
			} break;
		case THTTPEvent::EFailed:
			{
			iUtils.Test().Printf(_L("Transaction Failed\n"));
			aTransaction.Close();
			CActiveScheduler::Stop();
			} break;
		case THTTPEvent::ERedirectedPermanently:
			{
			iUtils.Test().Printf(_L("Permanent Redirection\n"));
			} break;
		case THTTPEvent::ERedirectedTemporarily:
			{
			iUtils.Test().Printf(_L("Temporary Redirection\n"));
			} break;
		default:
			{
			iUtils.Test().Printf(_L("<unrecognised event: %d>\n"), aEvent.iStatus);
			// close off the transaction if it's an error
			if (aEvent.iStatus < 0)
				{
				aTransaction.Close();
				CActiveScheduler::Stop();
				}
			} break;
		}
	}

TInt CHttpEventHandler::MHFRunError(TInt aError, RHTTPTransaction /*aTransaction*/, const THTTPEvent& /*aEvent*/)
	{
	iUtils.Test().Printf(_L("MHFRunError fired with error code %d\n"), aError);

	return KErrNone;
	}


void CHttpEventHandler::DumpRespHeadersL(RHTTPTransaction& aTrans)
	{
	RHTTPResponse resp = aTrans.Response();
	RStringPool strP = aTrans.Session().StringPool();
	RHTTPHeaders hdr = resp.GetHeaderCollection();
	THTTPHdrFieldIter it = hdr.Fields();

	TBuf<KMaxHeaderNameLen>  fieldName16;
	TBuf<KMaxHeaderValueLen> fieldVal16;

	while (it.AtEnd() == EFalse)
		{
		RStringTokenF fieldName = it();
		RStringF fieldNameStr = strP.StringF(fieldName);
		THTTPHdrVal fieldVal;
		if (hdr.GetField(fieldNameStr,0,fieldVal) == KErrNone)
			{
			const TDesC8& fieldNameDesC = fieldNameStr.DesC();
			fieldName16.Copy(fieldNameDesC.Left(KMaxHeaderNameLen));
			switch (fieldVal.Type())
				{
			case THTTPHdrVal::KTIntVal:
				iUtils.Test().Printf(_L("%S: %d\n"), &fieldName16, fieldVal.Int());
				break;
			case THTTPHdrVal::KStrFVal:
				{
				RStringF fieldValStr = strP.StringF(fieldVal.StrF());
				const TDesC8& fieldValDesC = fieldValStr.DesC();
				fieldVal16.Copy(fieldValDesC.Left(KMaxHeaderValueLen));
				iUtils.Test().Printf(_L("%S: %S\n"), &fieldName16, &fieldVal16);
				}
				break;
			case THTTPHdrVal::KStrVal:
				{
				RString fieldValStr = strP.String(fieldVal.Str());
				const TDesC8& fieldValDesC = fieldValStr.DesC();
				fieldVal16.Copy(fieldValDesC.Left(KMaxHeaderValueLen));
				iUtils.Test().Printf(_L("%S: %S\n"), &fieldName16, &fieldVal16);
				}
				break;
			case THTTPHdrVal::KDateVal:
				{
				TDateTime date = fieldVal.DateTime();
				TBuf<40> dateTimeString;
				TTime t(date);
				t.FormatL(dateTimeString,KDateFormat);

				iUtils.Test().Printf(_L("%S: %S\n"), &fieldName16, &dateTimeString);
				} 
				break;
			default:
				iUtils.Test().Printf(_L("%S: <unrecognised value type>\n"), &fieldName16);
				break;
				}

			// Display realm for WWW-Authenticate header
			RStringF wwwAuth = strP.StringF(HTTP::EWWWAuthenticate,RHTTPSession::GetTable());
			if (fieldNameStr == wwwAuth)
				{
				// check the auth scheme is 'basic'
				RStringF basic = strP.StringF(HTTP::EBasic,RHTTPSession::GetTable());
				RStringF realm = strP.StringF(HTTP::ERealm,RHTTPSession::GetTable());
				THTTPHdrVal realmVal;
				if ((fieldVal.StrF() == basic) && 
					(!hdr.GetParam(wwwAuth, realm, realmVal)))
					{
					RStringF realmValStr = strP.StringF(realmVal.StrF());
					fieldVal16.Copy(realmValStr.DesC());
					iUtils.Test().Printf(_L("Realm is: %S\n"), &fieldVal16);
					}
				}
			}
		++it;
		}
	}

void CHttpEventHandler::DumpRespBody(RHTTPTransaction& aTrans)
	{
	MHTTPDataSupplier* body = aTrans.Response().Body();
	TPtrC8 dataChunk;
	TBool isLast = body->GetNextDataPart(dataChunk);
	DumpIt(dataChunk);
	if (isLast)
		iUtils.Test().Printf(_L("Got last data chunk.\n"));
	}


void CHttpEventHandler::DumpIt(const TDesC8& aData)
//Do a formatted dump of binary data
	{
	// Iterate the supplied block of data in blocks of cols=80 bytes
	const TInt cols=16;
	TInt pos = 0;
	TBuf<KMaxFileName - 2> logLine;
	TBuf<KMaxFileName - 2> anEntry;
	const TInt dataLength = aData.Length();
	while (pos < dataLength)
		{
		//start-line hexadecimal( a 4 digit number)
		anEntry.Format(TRefByValue<const TDesC>_L("%04x : "), pos);
		logLine.Append(anEntry);

		// Hex output
		TInt offset;
		for (offset = 0; offset < cols; ++offset)
			{
			if (pos + offset < aData.Length())
				{
				TInt nextByte = aData[pos + offset];
				anEntry.Format(TRefByValue<const TDesC>_L("%02x "), nextByte);
				logLine.Append(anEntry);
				}
			else
				{
				//fill the remaining spaces with blanks untill the cols-th Hex number 
				anEntry.Format(TRefByValue<const TDesC>_L("   "));
				logLine.Append(anEntry);
				}
			}
			anEntry.Format(TRefByValue<const TDesC>_L(": "));
			logLine.Append(anEntry);

		// Char output
		for (offset = 0; offset < cols; ++offset)
			{
			if (pos + offset < aData.Length())
				{
				TInt nextByte = aData[pos + offset];
				if ((nextByte >= ' ') && (nextByte <= '~'))
					{
					anEntry.Format(TRefByValue<const TDesC>_L("%c"), nextByte);
					logLine.Append(anEntry);
					}
				else
					{
					anEntry.Format(TRefByValue<const TDesC>_L("."));
					logLine.Append(anEntry);
					}
				}
			else
				{
				anEntry.Format(TRefByValue<const TDesC>_L(" "));
				logLine.Append(anEntry);
				}
			}
			iUtils.Test().Printf(TRefByValue<const TDesC>_L("%S\n"), &logLine);	
			logLine.Zero();

		// Advance to next  byte segment (1 seg= cols)
		pos += cols;
		}
	}


//
// Main implementation
//

LOCAL_D void TestL()
// Create a test object, invoke the tests using it and remove
	{
	// Start C32 and initalize some device drivers. This is necessary when running a test console as these won't 
	// have been started
	CHttpExampleUtils::InitCommsL();

	// create an active scheduler to use
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create and start the client
	CHttpClient* httpCli = CHttpClient::NewLC();
	httpCli->StartClientL();

	CleanupStack::PopAndDestroy(2); // httpCli, scheduler
	}


GLDEF_C TInt E32Main()
// Main program - run the tests within a TRAP harness, reporting any errors that
// occur via the panic mechanism. Test for memory leaks using heap marking.
	{
	__UHEAP_MARK;
	CTrapCleanup* tc=CTrapCleanup::New();

	TRAPD(err,TestL());
	if (err!=KErrNone)
		User::Panic(_L("Test failed with error code: %i"), err);
	delete tc;
	__UHEAP_MARKEND;
	return KErrNone;
	}



