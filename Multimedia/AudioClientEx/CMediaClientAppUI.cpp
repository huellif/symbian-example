// MediaClientAppUI.cpp
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
//

#include <AudioClientEx.rsg>
#include "MediaClient.hrh"
#include "CMediaClientAppUi.h"
#include "CMediaClientView.h"
#include "CMediaClientDocument.h"
#include "CMediaClientEngine.h"

#include <eikenv.h>
#include <eikproc.h>
#include <eikfnlab.h>
#include <eiktbar.h>
#include <eikapp.h>
#include <eikdialg.h>
#include <eikcfdlg.h>
#include <eikon.rsg>
#include <coeutils.h>
#include <datastor.h>
#include <eikmfne.h>
#include <eikmenup.h>
#include <eiklabel.h>
#include <eikchlst.h>
#include <badesca.h>

/** View text for error state */
_LIT(KError, "Error");
/** View text for idle state */
_LIT(KIdle, "Ready");
/** View text for tone preparation state  */
_LIT(KTonePrepare,"Initiating");
/** View text for tone playing state  */
_LIT(KTonePlaying, "Playing tone");
/** View text for file preparation state  */
_LIT(KPlayFilePrepare, "Initiating");
/** View text for file playing state  */
_LIT(KPlayFilePlaying, "Playing file");
/** View text for recording preparation state  */
_LIT(KRecordPrepare, "Initiating");
/** View text for recording state  */
_LIT(KRecording, "Recording file");
/** View text for stream preparation state  */
_LIT(KPlayStreamPrepare, "Initiating");
/** View text for stream playing state */
_LIT(KStreamPlaying, "Playing stream");
/** View text for convert state */
_LIT(KConverting, "Converting file");

// Initial object setup
void CMediaClientAppUi::ConstructL() 
    {
    BaseConstructL();

	// set up view
    iAppView = CMediaClientView::NewL(ClientRect(), MediaClientEngine());
	iAppView -> SetDescription(KIdle);
    AddToStackL(iAppView); // app view should go onto control stack

	// set up filename buffer
	_LIT(KDefault,"c:\\");
    iFileName = KDefault;

	// set up link to engine
	MediaClientEngine().SetObserver(*this);
	iState = CMediaClientEngine::EIdle;

	// An asychronous call back object used to exit when a stream is playing
	// This allows media server time to clean up stream
	TCallBack callBack(CMediaClientAppUi::DoExit, this);
	iExitCallBack = new (ELeave) CAsyncCallBack(callBack,CActive::EPriorityLow);
    }


CMediaClientAppUi::~CMediaClientAppUi() 
    {
	iMetaData.ResetAndDestroy();
	iMetaData.Close();
	StreamSourceCleanUp();
	RemoveFromStack(iAppView);
    delete iAppView;
	delete iExitCallBack;
	}

TInt CMediaClientAppUi::DoExit(TAny* aUI)
	{
	static_cast<CMediaClientAppUi*>(aUI)->Exit();
	return 0;
	}

// Gets engine
CMediaClientEngine& CMediaClientAppUi::MediaClientEngine() const 
    { 
    return (static_cast<CMediaClientDocument*>(Document()))->Engine(); 
    }

// Handle command requests
void CMediaClientAppUi::HandleCommandL(TInt aCommand) 
    {
    switch (aCommand) 
        {
    case EMPCmdPlayTone:
		ToneDialogL();
        break;

	case EMPCmdPlayFile:
		PlayFileDialogL();
        break;

	case EMPCmdRecordFile:
		RecordFileDialogL();
		break;

	case EMPCmdPlayStream:
		StreamSourceCleanUp();
		iAudioStreamSource = CAudioStreamSource::NewL(iEikonEnv->FsSession(),
			MediaClientEngine());
		MediaClientEngine().PlayL();
		break;

	case EMPCmdPlayStreamWithQueue:
		StreamSourceCleanUp();
		iAudioStreamSourceWithQueue = CAudioStreamSourceWithQueue::NewL(
			iEikonEnv->FsSession(),
			MediaClientEngine());
		MediaClientEngine().PlayL();
		break;

	case EMPCmdShowMetaData:
		FileMetaDataL();
		break;

	case EMPCmdConvertFile:
		ConvertFileDialogL();
		break;

	case EMPCmdVolumeLow:
		MediaClientEngine().SetVolume(CMediaClientEngine::EQuiet);
		break;

	case EMPCmdVolumeMedium:
		MediaClientEngine().SetVolume(CMediaClientEngine::EMedium);
		break;

	case EMPCmdVolumeHigh:
		MediaClientEngine().SetVolume(CMediaClientEngine::ELoud);
		break;

    case EMPCmdStop:
		MediaClientEngine().Stop();
		StreamSourceCleanUp();
		break;

    case EEikCmdExit: 
		MediaClientEngine().Stop();
		if ( (iState == CMediaClientEngine::EStreamStarted)
			|| (iState == CMediaClientEngine::ENextStreamBuf)
			|| (iState == CMediaClientEngine::EStreamError)
			|| (iState == CMediaClientEngine::EStreamStopping) )
			iExitCallBack->CallBack();
		else
			Exit();
        break;
        }
    }

void CMediaClientAppUi::StreamSourceCleanUp()
	{
	delete iAudioStreamSource;
	iAudioStreamSource = NULL;
	delete iAudioStreamSourceWithQueue;
    iAudioStreamSourceWithQueue = NULL;
	}

// Dialog to select tone to play
void CMediaClientAppUi::ToneDialogL()
	{
	TInt frequency;
	TTimeIntervalMicroSeconds duration;

	if (CToneDialog::RunDlgLD(frequency,duration))
		MediaClientEngine().PlayL(frequency,duration);
	}

// Dialog to select file to play
void CMediaClientAppUi::PlayFileDialogL()
    {
	CEikFileOpenDialog* dialog = new (ELeave) CEikFileOpenDialog(&iFileName);
    if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_OPEN))
        MediaClientEngine().PlayL(iFileName);
    }

// Dialog to select file to record to
void CMediaClientAppUi::RecordFileDialogL()
	{
	if (CRecordDialog::RunDlgLD(iFileName, MediaClientEngine(), iRecordFormatOptions))
		// start conversion
		MediaClientEngine().RecordL(iFileName, iRecordFormatOptions);
		// callback to HandleEngineState when conversion complete
	}

// Query for file metadata
void CMediaClientAppUi::FileMetaDataL()
	{
	// get meta data about the file, which we'll display afterwards
	iMetaData.ResetAndDestroy();
	CEikFileOpenDialog* dialog = new (ELeave) CEikFileOpenDialog(&iFileName);
    if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_OPEN))
		MediaClientEngine().GetMetaDataL(iFileName,iMetaData);
	// callback to HandleEngineState when data got
	}

// Display retrieved metadata
void CMediaClientAppUi::DisplayMetaDataL()
	{
	// set string with metadata information
	_LIT(KSeparator, "\t");
	_LIT(KNewLine, "\n");
	_LIT(KNoMetaData, "No metadata retrieved");
	const TInt KMaxDataField = 1024;
    HBufC* labelText = HBufC::NewLC(KMaxDataField);
	TPtr labelTextPtr = labelText->Des();
	for (TInt i=0; i<iMetaData.Count(); i++)
		{
		// be careful not to overrun descriptor end
		TInt fieldLen = iMetaData[i]->Name().Length() + 2 + iMetaData[i]->Value().Length();
		if (labelTextPtr.Length()+fieldLen < KMaxDataField)
			{
			labelTextPtr.Append(iMetaData[i]->Name());
			labelTextPtr.Append(KSeparator);
			labelTextPtr.Append(iMetaData[i]->Value());
			labelTextPtr.Append(KNewLine);
			}
		}
	if (iMetaData.Count() == 0) *labelText = KNoMetaData;
	iAppView -> SetDescription(*labelText);
	CleanupStack::PopAndDestroy(); // labelText
	}

// File format conversion dialog
void CMediaClientAppUi::ConvertFileDialogL()
	{
	CEikFileOpenDialog* dialog = new (ELeave) CEikFileOpenDialog(&iFileName);
    if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_OPEN))
		if (CConverterDialog::RunDlgLD(iFileName, MediaClientEngine(), iFormatOptions, iTargetType))
			// start conversion
			MediaClientEngine().ConvertL(iFileName, iTargetType, iFormatOptions);
			// callback to HandleEngineState when conversion complete
	}

// Dim menu commands when sound is playing
void CMediaClientAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
	{
	if (aResourceId == R_AUDIOEX_ACTIONS_MENU)
		{
		TBool dim = (iState != CMediaClientEngine::EIdle);
		aMenuPane -> SetItemDimmed(EMPCmdPlayTone,dim);
		aMenuPane -> SetItemDimmed(EMPCmdRecordFile,dim);
		aMenuPane -> SetItemDimmed(EMPCmdPlayStream,dim);
		aMenuPane -> SetItemDimmed(EMPCmdPlayStreamWithQueue,dim);
		aMenuPane -> SetItemDimmed(EMPCmdPlayFile,dim);
		aMenuPane -> SetItemDimmed(EMPCmdStop,!dim);
		aMenuPane -> SetItemDimmed(EMPCmdConvertFile,dim);
		aMenuPane -> SetItemDimmed(EMPCmdShowMetaData,dim);	
		}
	}

// Call back from the engine to inform the UI of progress
void CMediaClientAppUi::HandleEngineState(CMediaClientEngine::TState aState, 
										  TInt aError)
	{
	iState = aState;
	if (aError) 
		iAppView -> SetDescription(KError);
	else
		switch(aState)
			{
		case CMediaClientEngine::EIdle:
			iAppView -> SetDescription(KIdle);
			break;
		case CMediaClientEngine::ETonePrepare:
			iAppView -> SetDescription(KTonePrepare);
			break;
		case CMediaClientEngine::ETonePlaying:
			iAppView -> SetDescription(KTonePlaying);
			break;
		case CMediaClientEngine::EPlayFilePrepare:
			iAppView -> SetDescription(KPlayFilePrepare);
			break;
		case CMediaClientEngine::EPlayFilePlaying:
			iAppView -> SetDescription(KPlayFilePlaying);
			break;
		case CMediaClientEngine::EGetMetaData2:
			DisplayMetaDataL();
			iState = CMediaClientEngine::EIdle;
			break;
		case CMediaClientEngine::ERecordPrepare:
			iAppView -> SetDescription(KRecordPrepare);
			break;
		case CMediaClientEngine::EConvert:
			iAppView -> SetDescription(KConverting);
			break;
		case CMediaClientEngine::EConvertComplete:
			iAppView -> SetDescription(KIdle);
			break;
		case CMediaClientEngine::ERecording:
			iAppView -> SetDescription(KRecording);
			break;
		case CMediaClientEngine::EStreamPrepare:
			iAppView -> SetDescription(KPlayStreamPrepare);
			break;
		case CMediaClientEngine::EStreamStarted:
			if (iAudioStreamSourceWithQueue)
				iAudioStreamSourceWithQueue->Next();
			else
				iAudioStreamSource->Next();
			iAppView -> SetDescription(KStreamPlaying);
			break;
		case CMediaClientEngine::ENextStreamBuf:
			if (iAudioStreamSourceWithQueue)
				iAudioStreamSourceWithQueue->RemoveFromQueue();
			else
				iAudioStreamSource->Next();
			break;
		default:
			break;
			}

	// update the UI to reflect the state
	iAppView -> DrawNow();
	}

// ********************************
// CRecordDialog - sets conversion format
// ********************************

const TInt KFormatOptionsNotSet = -1;

TBool CRecordDialog::RunDlgLD(
	TFileName& aFileName, CMediaClientEngine& aMediaClientEngine, // input parameters
	TRecordFormatOptions& aRecordFormatOptions) 
	{
	CRecordDialog* dialog = new (ELeave) CRecordDialog(aFileName, aMediaClientEngine, aRecordFormatOptions);
	return (dialog->ExecuteLD(R_FORMAT_DIALOG));
	}

CRecordDialog::CRecordDialog(TFileName& aFileName, CMediaClientEngine& aMediaClientEngine,
	TRecordFormatOptions& aRecordFormatOptions)
	:iFileName(aFileName), 
	iMediaClientEngine(aMediaClientEngine),
	iRecordFormatOptions(aRecordFormatOptions),
	iOptionsSetFor(KFormatOptionsNotSet)
	{
	}

CRecordDialog::~CRecordDialog()
	{
	}

void CRecordDialog::PreLayoutDynInitL()
	{
	// set choice list with destination types
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(4);
	CleanupStack::PushL(array);
	// get array of conversion types
	iMediaClientEngine.RecordTypesL(*array);
	// set choicelist
    CEikChoiceList* typesList = static_cast<CEikChoiceList*>(Control(ECvtTypeField));
	typesList->SetArrayL(array);
	CleanupStack::Pop(array);
	}

void CRecordDialog::HandleControlStateChangeL(TInt aControlId)
	{
	// Unset format options if the format type changes
	if (aControlId == ECvtTypeField)
		iOptionsSetFor = KFormatOptionsNotSet;
	}

TBool CRecordDialog::OkToExitL(TInt aKeycode)
	{
	_LIT(KRecordFile, "c:\\record-output");
	CEikChoiceList* typesList = static_cast<CEikChoiceList*>(Control(ECvtTypeField));
	iFileName = KRecordFile;
	TPtrC type = typesList->Array()->MdcaPoint(typesList->CurrentItem());
	iFileName.Append(type);

	// if format options button pressed, show dialog
	if (aKeycode == EFormatOptions)
		{
		if (CRecordFormatOptionsDialog::RunDlgLD(type, iMediaClientEngine, iRecordFormatOptions)) 
			iOptionsSetFor = typesList->CurrentItem();
		return EFalse;
		}
	
	return ETrue;
    }


// ********************************
// CRecordFormatOptionsDialog - sets conversion format options
// ********************************

TBool CRecordFormatOptionsDialog::RunDlgLD(const TDesC& aTargetType, 
	CMediaClientEngine& aMediaClientEngine, TRecordFormatOptions& aFormatOptions)
	{
	CRecordFormatOptionsDialog* dialog = new (ELeave) CRecordFormatOptionsDialog(aTargetType, 
		aMediaClientEngine, aFormatOptions);
	dialog->ConstructL();
	return (dialog->ExecuteLD(R_RECORD_OPTIONS_DIALOG));
	}

CRecordFormatOptionsDialog::CRecordFormatOptionsDialog(const TDesC& aTargetType, 
	CMediaClientEngine& aMediaClientEngine, TRecordFormatOptions& aFormatOptions)
	:CFormatOptionsDialog(iBuf, aTargetType, aMediaClientEngine, aFormatOptions),
	iRecordFormatOptions(aFormatOptions)
	{
	aFormatOptions.Reset();
	}

void CRecordFormatOptionsDialog::ConstructL()
	{
	// Get a recorder for the format
	CMdaAudioRecorderUtility* recorder = iMediaClientEngine.NewRecorderL(iTargetType);
	
	if (recorder == NULL) User::Leave(KErrNotSupported);
	// Read available options
	CleanupStack::PushL(recorder);
	iMaxGain = recorder -> MaxGain();

	TRAPD(err,recorder -> GetSupportedDestinationDataTypesL(iDataTypes));
	if (err != KErrNotSupported) User::LeaveIfError(err);
	TRAP(err,recorder -> GetSupportedBitRatesL(iBitRates));
	if (err != KErrNotSupported) User::LeaveIfError(err);
	TRAP(err,recorder -> GetSupportedSampleRatesL(iSampleRates));
	if (err != KErrNotSupported) User::LeaveIfError(err);
	TRAP(err,recorder -> GetSupportedNumberOfChannelsL(iChannels));
	if (err != KErrNotSupported) User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(recorder);
	}

CRecordFormatOptionsDialog::~CRecordFormatOptionsDialog()
	{
	}

void CRecordFormatOptionsDialog::PreLayoutDynInitL()
	{
	// set up bit rate, sample, channel, codec lists
	CFormatOptionsDialog::PreLayoutDynInitL();
	// add gain and balance, which are recording specific parameters
    CEikNumberEditor* gain = static_cast<CEikNumberEditor*>(Control(EGain));
	gain->SetMinimumAndMaximum(0,iMaxGain);
	gain->SetNumber(iMaxGain/2);
    CEikNumberEditor* balance = static_cast<CEikNumberEditor*>(Control(EBalance));
	balance->SetMinimumAndMaximum(KMMFBalanceMaxLeft,KMMFBalanceMaxRight);
	balance->SetNumber(KMMFBalanceCenter);
	}

TBool CRecordFormatOptionsDialog::OkToExitL(TInt /*aKeycode*/)
	{
	// get bit rate, sample, channel, codec settings
	CFormatOptionsDialog::OkToExitL(0);
	// get gain and balance, which are recording specific parameters
    CEikNumberEditor* gain = static_cast<CEikNumberEditor*>(Control(EGain));
	iRecordFormatOptions.iGain = gain->Number();
    CEikNumberEditor* balance = static_cast<CEikNumberEditor*>(Control(EBalance));
	iRecordFormatOptions.iBalance = balance->Number();
	iRecordFormatOptions.iGainSet = iRecordFormatOptions.iBalanceSet = ETrue;

	return ETrue;
	}


// ********************************
// CConverterDialog - sets conversion format
// ********************************

TBool CConverterDialog::RunDlgLD(
	const TFileName& aFileName, CMediaClientEngine& aMediaClientEngine, // input parameters
	TFormatOptions& aFormatOptions, TDes& aTargetType) 
	{
	CConverterDialog* dialog = new (ELeave) CConverterDialog(aFileName, aMediaClientEngine, aFormatOptions, aTargetType);
	return (dialog->ExecuteLD(R_FORMAT_DIALOG));
	}

CConverterDialog::CConverterDialog(const TFileName& aFileName, CMediaClientEngine& aMediaClientEngine,
	TFormatOptions& aFormatOptions, TDes& aTargetType)
	:iFileName(aFileName), 
	iMediaClientEngine(aMediaClientEngine),
	iFormatOptions(aFormatOptions),
	iTargetType(aTargetType),
	iOptionsSetFor(KFormatOptionsNotSet)
	{
	}

CConverterDialog::~CConverterDialog()
	{
	}

void CConverterDialog::PreLayoutDynInitL()
	{
	// set choice list with destination types
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(4);
	CleanupStack::PushL(array);
	// get array of conversion types
	iMediaClientEngine.DestinationTypesL(iFileName, *array);
	// set choicelist
    CEikChoiceList* typesList = static_cast<CEikChoiceList*>(Control(ECvtTypeField));
	typesList->SetArrayL(array);
	CleanupStack::Pop(array);
	}

void CConverterDialog::HandleControlStateChangeL(TInt aControlId)
	{
	// Unset format options if the format type changes
	if (aControlId == ECvtTypeField)
		iOptionsSetFor = KFormatOptionsNotSet;
	}

TBool CConverterDialog::OkToExitL(TInt aKeycode)
	{
	CEikChoiceList* typesList = static_cast<CEikChoiceList*>(Control(ECvtTypeField));
	iTargetType = typesList->Array()->MdcaPoint(typesList->CurrentItem());

	// if format options button pressed, show dialog
	if (aKeycode == EFormatOptions)
		{
		if (CFormatOptionsDialog::RunDlgLD(iFileName, iTargetType, iMediaClientEngine, iFormatOptions)) 
			iOptionsSetFor = typesList->CurrentItem();
		return EFalse;
		}
	
	return ETrue;
    }


// ********************************
// CFormatOptionsDialog - sets conversion format options
// ********************************

TBool CFormatOptionsDialog::RunDlgLD(const TFileName& aInputFile, const TDesC& aTargetType, 
	CMediaClientEngine& aMediaClientEngine, TFormatOptions& aFormatOptions)
	{
	CFormatOptionsDialog* dialog = new (ELeave) CFormatOptionsDialog(aInputFile, aTargetType, 
		aMediaClientEngine, aFormatOptions);
	dialog->ConstructL();
	return (dialog->ExecuteLD(R_CONVERT_OPTIONS_DIALOG));
	}

CFormatOptionsDialog::CFormatOptionsDialog(const TFileName& aInputFile, const TDesC& aTargetType, 
	CMediaClientEngine& aMediaClientEngine, TFormatOptions& aFormatOptions)
	:iFileName(aInputFile), iTargetType(aTargetType), iMediaClientEngine(aMediaClientEngine),
	iFormatOptions(aFormatOptions)
	{
	iFormatOptions.Reset();
	}

void CFormatOptionsDialog::ConstructL()
	{
	// Get a converter for the format
	TFileName targetFile; // don't actually use this
	CMdaAudioConvertUtility* converter = iMediaClientEngine.NewConverterL(iFileName, iTargetType, targetFile);
	if (converter == NULL) User::Leave(KErrNotSupported);
	// Read available options
	CleanupStack::PushL(converter);
	TRAPD(err,converter -> GetSupportedDestinationDataTypesL(iDataTypes));
	if (err != KErrNotSupported) User::LeaveIfError(err);
	TRAP(err,converter -> GetSupportedConversionBitRatesL(iBitRates));
	if (err != KErrNotSupported) User::LeaveIfError(err);
	TRAP(err,converter -> GetSupportedConversionSampleRatesL(iSampleRates));
	if (err != KErrNotSupported) User::LeaveIfError(err);
	TRAP(err,converter -> GetSupportedConversionNumberOfChannelsL(iChannels));
	if (err != KErrNotSupported) User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(converter);
	}

CFormatOptionsDialog::~CFormatOptionsDialog()
	{
	iDataTypes.Close();
	iBitRates.Close();
	iSampleRates.Close();
	iChannels.Close();
	}

void CFormatOptionsDialog::PreLayoutDynInitL()
	{
	// Set dialog fields - these 3 are all TUint lists
	TUintChoiceList(EBitRates, iBitRates);
	TUintChoiceList(ESampleRates, iSampleRates);
	TUintChoiceList(EChannels, iChannels);
	// Codecs list is more complicated
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(4);
	CleanupStack::PushL(array);
	HBufC8* buf8 = HBufC8::NewLC(10);
	TPtr8 ptr = buf8->Des();
	TBuf<5> buf;
	for (TInt i=0; i<iDataTypes.Count(); i++)
		{
		iDataTypes[i].FourCC(&ptr);
		buf.Copy(ptr);
		array->AppendL(buf);
		}
    CEikChoiceList* list = static_cast<CEikChoiceList*>(Control(ECodecs));
	list->SetArrayL(array);	
	CleanupStack::PopAndDestroy(); //buf8
	CleanupStack::Pop(); //array
	}

void CFormatOptionsDialog::TUintChoiceList(TInt aResourceId, const RArray<TUint>& aArray)
	{
	CEikChoiceList* list = static_cast<CEikChoiceList*>(Control(aResourceId));
	if (aArray.Count()==0) 
		{
		list->MakeVisible(EFalse);
		return;
		}
	TBuf <10> buf;
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(4);
	CleanupStack::PushL(array);
	for (TInt i=0; i<aArray.Count(); i++)
		{
		buf.Num(aArray[i]);
		array->AppendL(buf);
		}
	list->SetArrayL(array);	
	CleanupStack::Pop(); //array
	}

TBool CFormatOptionsDialog::OkToExitL(TInt /*aKeycode*/)
	{
	CEikChoiceList* list = static_cast<CEikChoiceList*>(Control(ECodecs));
	if (list->IsVisible()) 
		{iFormatOptions.i4CC = iDataTypes[list->CurrentItem()]; iFormatOptions.i4CCSet = ETrue; }
	list = static_cast<CEikChoiceList*>(Control(EBitRates));
	if (list->IsVisible()) 
		{iFormatOptions.iBitRate = iBitRates[list->CurrentItem()]; iFormatOptions.iBitRateSet = ETrue; }
	list = static_cast<CEikChoiceList*>(Control(ESampleRates));
	if (list->IsVisible()) 
		{iFormatOptions.iSampleRate = iSampleRates[list->CurrentItem()]; iFormatOptions.iSampleRateSet = ETrue; }
	list = static_cast<CEikChoiceList*>(Control(EChannels));
	if (list->IsVisible()) 
		{iFormatOptions.iChannel = iChannels[list->CurrentItem()]; iFormatOptions.iChannelSet = ETrue; }
	return ETrue;
	}


// ********************************
// CToneDialog - sets tone to play
// ********************************

TBool CToneDialog::RunDlgLD(TInt& aFrequency, TTimeIntervalMicroSeconds& aDuration)
	{
	CEikDialog* dialog = new (ELeave) CToneDialog(aFrequency,aDuration);
	return (dialog->ExecuteLD(R_TONE_DIALOG));
	}

CToneDialog::CToneDialog(TInt& aFrequency, TTimeIntervalMicroSeconds& aDuration)
	:iFrequency(aFrequency), 
	iDuration(aDuration)
	{
	}

TBool CToneDialog::OkToExitL(TInt /*aKeycode*/)
	{
    iFrequency = STATIC_CAST(CEikNumberEditor*,Control(EFrequencyField))->Number();
    iDuration = STATIC_CAST(CEikNumberEditor*,Control(EDurationField))->Number() * 1000000;
	return ETrue;
    }

// ********************************
// CAudioStreamSource - an artificial audio stream source
// It asynchronously gets slices from a file and feeds them to the engine
// ********************************

// File to read
_LIT(KSourceName, "c:\\testing123.raw");

CAudioStreamSource* CAudioStreamSource::NewL(RFs& aFs,CMediaClientEngine& aEngine)
	{
	CAudioStreamSource* self=new(ELeave) CAudioStreamSource(aFs,aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CAudioStreamSource::CAudioStreamSource(RFs& aFs, CMediaClientEngine& aEngine)
	:
	CActive(CActive::EPriorityStandard),
	iFs(aFs),
	iEngine(aEngine)
	{
	}

void CAudioStreamSource::ConstructL()
	{
	// Base class second-phase construction.
	// Open the source file
	User::LeaveIfError(iSourceFile.Open(iFs,KSourceName,EFileRead));
	CActiveScheduler::Add(this);
	}

void CAudioStreamSource::RunL()
	{
	// Read a slice from the source file
	User::LeaveIfError(iSourceFile.Read(iBuf));
	// If not at end of file, write the slice to the stream
	if (iBuf.Length())
		iEngine.WriteToStreamL(iBuf);
	}

void CAudioStreamSource::Next()
	{
	TRequestStatus * status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CAudioStreamSource::DoCancel()
	{
	}

CAudioStreamSource::~CAudioStreamSource()
	{
	Cancel();
	// Close the source file
	iSourceFile.Close();
	}

// ********************************
// CAudioStreamSourceWithQueue - an artificial audio stream source.
// It gets slices from a file at a constant rate, using a timer,
// stores them in a queue then feeds them to the engine.
// They are deleted from the queue after they have been received by 
// the media framework
// ********************************

CAudioStreamSourceWithQueue* CAudioStreamSourceWithQueue::NewL(RFs& aFs,CMediaClientEngine& aEngine)
	{
	CAudioStreamSourceWithQueue* self=new(ELeave) CAudioStreamSourceWithQueue(aFs,aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CAudioStreamSourceWithQueue::CAudioStreamSourceWithQueue(RFs& aFs, CMediaClientEngine& aEngine)
	:
	CTimer(CActive::EPriorityStandard),
	iFs(aFs),
	iEngine(aEngine),
	iQueue(CSoundSlice::iOffset),
	iQueueIter(iQueue)
	{
	}

void CAudioStreamSourceWithQueue::ConstructL()
	{
	// Base class second-phase construction.
	CTimer::ConstructL(); 
	// Open the source file
	User::LeaveIfError(iSourceFile.Open(iFs,KSourceName,EFileRead));
	CActiveScheduler::Add(this);
	}


CAudioStreamSourceWithQueue::~CAudioStreamSourceWithQueue()
	{
	Cancel();
	CSoundSlice* soundSlice;
	iQueueIter.SetToFirst();
	while ((soundSlice=iQueueIter++) != NULL)
			{
			iQueue.Remove(*soundSlice);
			delete soundSlice;
			};
	iSourceFile.Close();
	}
	
void CAudioStreamSourceWithQueue::Next()
	{
	// Request a 1/10 second wait
	After(100000);
	}
	

void CAudioStreamSourceWithQueue::DoCancel()
	{
	CTimer::DoCancel();
	}
	
void CAudioStreamSourceWithQueue::RunL()
	{
	// Read a slice from the source file
	User::LeaveIfError(iSourceFile.Read(iBuf));
	// If not at end of file, store the slice until it is safe to delete it
	// and write to the stream.
	// It can be deleted when MaoscBufferCopied() is called, 
	// to indicate that it has been copied to the server.
	if (iBuf.Length())
		{
		iQueue.AddLast(*CSoundSlice::NewL(iBuf)); // Store it in the linked list, at end
		iEngine.WriteToStreamL(iQueue.Last()->GetData());
		// This will call back MaoscBufferCopied() when the data is copied
		// and reissue the wait request
		After(100000); // wait 1/10 second
		}
	}

void CAudioStreamSourceWithQueue::RemoveFromQueue()
	{
	if (!(iQueue.IsEmpty()))
		{
		CSoundSlice* soundSlice=iQueue.First();
		iQueue.Remove(*soundSlice);
		delete soundSlice;
		}
	}

// ********************************
// CSoundSlice
// ********************************

const TInt CSoundSlice::iOffset = _FOFF(CSoundSlice,iLink);

CSoundSlice* CSoundSlice::NewL(const TDesC8& aData)
	{
	CSoundSlice* self=new(ELeave) CSoundSlice;
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	CleanupStack::Pop();
	return self;
	}

void CSoundSlice::ConstructL(const TDesC8& aData)
	{
	iData = aData.AllocL();
	}

CSoundSlice::CSoundSlice()
	{}

CSoundSlice::~CSoundSlice()
	{
	delete iData;
	}


const TDesC8& CSoundSlice::GetData()
	{
	return *iData;
	}
