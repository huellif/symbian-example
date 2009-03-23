// mmfrectest.cpp
//

#include <e32base.h>
#include <e32cons.h>
#include <mdaaudiosampleeditor.h>
#include <bautils.h>
#include <ecom\ecom.h>
#include "mmfrectest.h"

_LIT(KFileNameFormat, "c:\\record%03d%S");

const TInt KRecordTimeMicros = 2000000; //2s

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,MainL());
		delete cleanup;
		}
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	return r;
	}
	
void MainL()
	{
	// The recorder utility requires an active scheduler to be running in the thread before
	// it can be used. This would normally be done by the application framework
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	
	CMMFRecordTest* test = CMMFRecordTest::NewLC();
	
	test->Go();

	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2);
	}
	

//______________________________________________________________________________
//						CMMFRecordTest
CMMFRecordTest* CMMFRecordTest::NewLC()
	{
	CMMFRecordTest* self = new(ELeave)CMMFRecordTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMMFRecordTest::~CMMFRecordTest()
	{
	delete iRecorder;
	delete iConsole;
	delete iCallBack;
	delete iTimer;
	iFs.Close();
	for (TInt i=0; i<iControllers.Count(); ++i)
		{
		delete iControllers[i];
		}
	iControllers.Close();
	}
	
void CMMFRecordTest::NextState(TState aState)
	{
	iState = aState;
	iCallBack->Set(iCbFn);
	iCallBack->CallBack();
	}

void CMMFRecordTest::Go()
	{
	NextState(ENone);
	}

/**
This is called by the recorder utility whenever some event occurs relating to the recording. For example,
it is called after initialisation when the utility is ready to begin recording. It would be called again
if there was a problem during recording that meant it had to be terminated.
*/
void CMMFRecordTest::MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	ASSERT(aObject == iRecorder);
	
	switch (iState)
		{
	case EInitFile:
		Printf(_L("Initialising file:"));
		break;
	case EInitDesc:
		Printf(_L("Initialising des:"));
		break;
		}
	Printf(_L("MoscoStateChangeEvent(%d, %d, %d)"), aPreviousState, aCurrentState, aErrorCode);	

	TTimeIntervalMicroSeconds32 delay(KRecordTimeMicros);

	switch (iState)
		{
	case EInitFile:
		if (aErrorCode == KErrNone)
			{ // file was opened successfully
			Printf(_L("call: RecordL() (to file)"));
			iState = ERecordFile;
			iRecorder->RecordL();
			iTimer->Start(delay, delay, iCbFn);
			}
		else
			{
			Printf(_L("OpenFile failed with error %d"), aErrorCode);
			// move onto next stage: record to descriptor
			NextState(EDoOpenDesc);
			}
		break;
	case EInitDesc:
		if (aErrorCode == KErrNone)
			{
			Printf(_L("call: RecordL() (to des)"));
			iState = ERecordDesc;
			iRecorder->RecordL();
			iTimer->Start(delay, delay, iCbFn);
			}
		else
			{
			Printf(_L("OpenDesc failed with error %d"), aErrorCode);
			// move onto next stage: try next format
			NextState(ENext);
			}			
		break;
	case ERecordFile:
		// an error has occurred during recording. Stop the recording and move onto the next test.
		if (aCurrentState!=CMdaAudioClipUtility::ERecording)
			{
			iTimer->Cancel();
			Printf(_L("call: Stop()"));
			iRecorder->Stop();
			NextState(EDoOpenDesc);
			}
		break;
	case ERecordDesc:
		// an error has occurred during recording. Stop the recording and move onto the next test.
		if (aCurrentState!=CMdaAudioClipUtility::ERecording)
			{
			iTimer->Cancel();
			Printf(_L("call: Stop()"));
			iRecorder->Stop();
			NextState(ENext);
			}
		break;
		}

	}
	
CMMFRecordTest::CMMFRecordTest()
	{
	}

void CMMFRecordTest::ConstructL()
	{
	iCallBack = new(ELeave)CAsyncCallBack(CActive::EPriorityStandard);
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	iConsole = Console::NewL(_L(" RecTest "), TSize(KConsFullScreen, KConsFullScreen));
	User::LeaveIfNull(iConsole);
	iRecorder = CMdaAudioRecorderUtility::NewL(*this);
	User::LeaveIfError(iFs.Connect());
	
	iCbFn = TCallBack(Callback, this);
	}

/**
This function finds details of all the plugins and supported formats that can be used
for audio recording. Once this has been done, each record format is excercised in turn.
*/
void CMMFRecordTest::GetPluginInformationL()
	{
	CMMFControllerPluginSelectionParameters* controllerSelection = NULL;
	CMMFFormatSelectionParameters* formatSelect = NULL;

	iControllers.ResetAndDestroy();
	iControllers.Close();

	controllerSelection = CMMFControllerPluginSelectionParameters::NewLC();

	RArray<TUid> mediaIds; //Array to hold media types we wish to support
	CleanupClosePushL(mediaIds);
	
	// specify that we're interested in audio
	mediaIds.AppendL(KUidMediaTypeAudio);
	controllerSelection->SetMediaIdsL(
		mediaIds, CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds);

	formatSelect = CMMFFormatSelectionParameters::NewLC();
	/*
	specify that were only interested in formats for recording.
	If you were intersted in recording to a specific format rather than just excercising
	every record format supported by the system, you could (for example) specify which 
	format by calling formatSelect->SetMatchToMimeTypeL().	
	*/
	controllerSelection->SetRequiredRecordFormatSupportL(*formatSelect);
	
	// get the list of controller plugins that support audio recording in one or 
	// more formats.
	controllerSelection->ListImplementationsL(iControllers);
	
	// iControllers now contains details of all audio controller plugins that support
	// audio recording. Each of the controller in the array will support one or more
	// recording formats.
	Printf(_L("GetPluginInformationL() - iControllers.Count() : %d"), iControllers.Count());

	CleanupStack::PopAndDestroy(3, controllerSelection);
	}

void CMMFRecordTest::Printf(TRefByValue<const TDesC16> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);

	TTruncateOverflow overflow;
	iFormattingBuf.Zero();
	iFormattingBuf.AppendFormatList(aFmt, list, &overflow);
	
	// output the message to the console on the screen, and also to debug output
	iConsole->Printf(iFormattingBuf);
	iConsole->Printf(_L("\n"));
	RDebug::RawPrint(iFormattingBuf);
	}
	
TInt CMMFRecordTest::Callback(TAny* self)
	{
	TState state = ((CMMFRecordTest*)self)->iState;
	TRAPD(err, ((CMMFRecordTest*)self)->DoCallbackL());
	if (err!=KErrNone)
		{
		((CMMFRecordTest*)self)->Printf(_L("Left with %d in state %d"), err, state);
		}
	return err;		
	}
	
void CMMFRecordTest::DoCallbackL()
	{
	iTimer->Cancel();
	switch (iState)
		{
	case ENone:
		// start by getting details of all the 
		GetPluginInformationL();
		iControllerIndex = 0;
		iFormatIndex = 0;
		// get the detail of the first format UID
		if (GetNextFormatUid())
			{
			// and begin initialization
			NextState(EDoOpenFile);
			}
		else
			{
			// if this fails for some reason, move onto the next controller
			Next();
			}
		break;
		
	case EDoOpenFile:
		InitializeFileL();
		break;
		
	case EDoOpenDesc:
		InitializeDesL();
		break;
		
	case ERecordFile:
		// stop recording to a file after the timer expires
		Printf(_L("call: Stop()"));
		iRecorder->Stop();
		NextState(EDoOpenDesc);
		break;

	case ERecordDesc:
		// stop recording to a descriptor after the timer expires
		Printf(_L("call: Stop()"));
		iRecorder->Stop();
		NextState(ENext);
		break;

	case ENext:
		Next();
		break;
		
	case EDone:
		CActiveScheduler::Stop();
		break;
		};
	
	
	}
	
/**
Moves onto the next controller, if any more exist.
*/
void CMMFRecordTest::Next()
	{
	if (iControllerIndex >= iControllers.Count())
		{
		NextState(EDone);
		return;
		}
		
	CMMFControllerImplementationInformation* info = iControllers[iControllerIndex];
	
	++iFormatIndex;
	
	if (iFormatIndex >= info->RecordFormats().Count())
		{
		iFormatIndex = -1;
		++iControllerIndex;
		NextState(ENext);
		return;
		}	
		
	NextState(GetNextFormatUid() ? EDoOpenFile : EDone);	
	}
	
/**
Gets details of the next record format supported by the current controller
*/
TBool CMMFRecordTest::GetNextFormatUid()
	{
	// if we have already excercised all of the controllers, then finish:
	if (iControllerIndex >= iControllers.Count())
		{
		return EFalse;
		}
		
	CMMFControllerImplementationInformation* info = iControllers[iControllerIndex];
	
	// if we have already tried all record formats supported by the current
	// controller, then move onto the next:
	if (iFormatIndex >= info->RecordFormats().Count())
		{
		return EFalse;
		}
		
	// get the unique IDs of the controller and the record format we're using
	iControllerUid = info->Uid();
	iDestinationFormatUid = info->RecordFormats()[iFormatIndex]->Uid();
	
	// find the recommended extension for a file of this format
	iExtension.Zero();
	const CDesC8Array& extns = info->RecordFormats()[iFormatIndex]->SupportedFileExtensions();
	if (extns.Count()>0)
		{
		iExtension.Copy(extns[0]);
		}
	
	// finally display some information about the format we're using
	Printf(_L("Using controller '%S', record format '%S'"), &info->DisplayName(), &info->RecordFormats()[iFormatIndex]->DisplayName());
		
		
	return ETrue;
	}

void CMMFRecordTest::DeleteFileL(const TDesC& aFileName)
	{
	TInt result = BaflUtils::DeleteFile(iFs, aFileName);

	if ((result != KErrNotFound)&&(result!=KErrPathNotFound))
		{
		User::LeaveIfError(result);
		}
	}

/**
Initializing recording to a file. Once this is done, we must wait for a callback into
MoscoStateChange event before we can actually start recording.
*/
void CMMFRecordTest::InitializeFileL()
	{
	iFileName.Zero();
	iFileName.Format(KFileNameFormat, iFileNum, &iExtension);
	
	// delte the file if it already exists
	DeleteFileL(iFileName);
	
	Printf(_L("call: OpenFileL(%S, %x, %x, %x)"), &iFileName, iControllerUid, KNullUid, iDestinationFormatUid);
	iState = EInitFile;
	iRecorder->OpenFileL(iFileName, iControllerUid, KNullUid, iDestinationFormatUid);
	
	++iFileNum;
	}
	
/**
Initializing recording to a descriptor. Once this is done, we must wait for a callback into
MoscoStateChange event before we can actually start recording.
*/
void CMMFRecordTest::InitializeDesL()
	{
	Printf(_L("call: OpenDesL(buf, %x, %x, %x)"), iControllerUid, KNullUid, iDestinationFormatUid);
	iState = EInitDesc;
	iRecorder->OpenDesL(iRecBuf, iControllerUid, KNullUid, iDestinationFormatUid);
	}



	
