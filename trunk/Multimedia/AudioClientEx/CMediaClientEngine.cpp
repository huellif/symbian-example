// CMediaClientEngine.cpp
//
// Copyright (c) 2001-2005 Symbian Software Ltd.  All rights reserved.

#include "CMediaClientEngine.h"

// Global helper function to change extension on a file
void ChangeExtension(const TFileName& aFile, const TDesC& aTargetType,
	TFileName& aTargetFile);
//
/************** CONSTRUCTION AND DESTRUCTION ******************/
//

CMediaClientEngine::CMediaClientEngine(RFs& aFs)
	: iFs(aFs),
	iState(CMediaClientEngine::EIdle),
	iVolume(CMediaClientEngine::EMedium)
	{
	}

CMediaClientEngine::~CMediaClientEngine()
	{
	iEngineObserver = 0;
	Stop();
	}

//
/************** GENERAL FUNCTIONS ******************/
//

void CMediaClientEngine::SetObserver(MExEngineObserver& aObserver)
	{
	iEngineObserver = &aObserver;
	}


// Terminate current operation
void CMediaClientEngine::Stop()
	{
	// Take appropriate action depending on what the state is
	switch (iState)
		{
		case ETonePrepare:
			iAudioToneUtility->CancelPrepare();
			ToneCleanup();
			break;
		case ETonePlaying:
	        iAudioToneUtility->CancelPlay();
			ToneCleanup();
			break;
		case EPlayFilePrepare:
			PlayCleanup();
			break;
		case EPlayFilePlaying:
			iAudioPlayUtility->Stop();
			PlayCleanup();
			break;
		case ERecordPrepare:
			RecordCleanup();
			break;
		case ERecording:
			iAudioRecorderUtility -> Stop();
			RecordCleanup();
			break;
		case EConvert:
			ConvertCleanup();
			break;
		case EStreamPrepare:
			StreamCleanup();
			break;
		case EStreamStarted:
		case ENextStreamBuf:
			iAudioStreamPlayer -> Stop(); // Calls MaoscBufferCopied() with KErrAbort
			iState = EStreamStopping;
			break;
		default:
			break;
		};
	// And tell the UI about the state change
	if (iEngineObserver)
		iEngineObserver -> HandleEngineState(iState,KErrNone);
	}

// Get volume
CMediaClientEngine::TExVolume CMediaClientEngine::Volume() const
	{
	return iVolume;
	}

// Set volume
void CMediaClientEngine::SetVolume(CMediaClientEngine::TExVolume aVolume)
	{
	iVolume = aVolume;
	if (iAudioStreamPlayer) // Streaming is in progress
		{
		iAudioStreamPlayer -> SetVolume(iAudioStreamPlayer -> MaxVolume()/iVolume);
		}	
	}

// Get state
CMediaClientEngine::TState CMediaClientEngine::Status() const
	{
	return iState;
	}

// Panic
void CMediaClientEngine::Panic(CMediaClientEngine::TPanics aPanic)
	{
	_LIT(KPanic,"MEDIA_AUDIO_EX");
	User::Panic(KPanic,aPanic);
	}


//
/************** TONE PLAYING ******************/
//

// Play a tone of specified frequency and duration
void CMediaClientEngine::PlayL(TInt aFrequency, 
							   const TTimeIntervalMicroSeconds& aDuration)
	{
	__ASSERT_DEBUG(iState == CMediaClientEngine::EIdle, Panic(CMediaClientEngine::EInProgress));
	
	iAudioToneUtility = CMdaAudioToneUtility::NewL(*this);
	iState = ETonePrepare;
	iAudioToneUtility -> PrepareToPlayTone(aFrequency, aDuration);
	// This will call back MatoPrepareComplete() when preparation is complete
	}

// Audio is now prepared to play a tone
// from MMdaAudioToneObserver
void CMediaClientEngine::MatoPrepareComplete(TInt aError)
	{
	__ASSERT_DEBUG(iEngineObserver, Panic(CMediaClientEngine::ENullObserver));
	__ASSERT_DEBUG(iAudioToneUtility, Panic(CMediaClientEngine::ENullTonePlayer));
	__ASSERT_DEBUG(iState == ETonePrepare, Panic(CMediaClientEngine::EWrongState));
	
	// If preparation was sucessful, play the tone
	if (aError)
		ToneCleanup();
	else
		{
		iState = ETonePlaying;
		iAudioToneUtility -> SetVolume(iAudioToneUtility -> MaxVolume()/iVolume);
		iAudioToneUtility -> Play();
		}
	iEngineObserver -> HandleEngineState(iState,aError);
	}

// Audio has completed playing the tone
// from MMdaAudioToneObserver
void CMediaClientEngine::MatoPlayComplete(TInt aError)
	{
	__ASSERT_DEBUG(iState == ETonePlaying, Panic(CMediaClientEngine::ENotReady));
	__ASSERT_DEBUG(iEngineObserver, Panic(CMediaClientEngine::ENullObserver));
	
	// Tell observer sound is complete, and clean up
	ToneCleanup();
	iEngineObserver -> HandleEngineState(iState,aError);
	}

// Clean up
void CMediaClientEngine::ToneCleanup()
	{
	iState = EIdle;
	delete iAudioToneUtility;
	iAudioToneUtility = NULL;
	}


//
/************** SOUND FILE PLAYING ******************/
//

// Play a sound file
void CMediaClientEngine::PlayL(const TDesC &aFile)
	{
	__ASSERT_DEBUG(iState == CMediaClientEngine::EIdle, Panic(CMediaClientEngine::EInProgress));
	
	iAudioPlayUtility = CMdaAudioPlayerUtility::NewFilePlayerL(aFile, *this);
	iState = EPlayFilePrepare;
	// This will call back MapcInitComplete() when preparation is complete
	}

// Audio is now prepared to play a file
// from MMdaAudioPlayerCallback
void CMediaClientEngine::MapcInitComplete(TInt aError, 
										  const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	__ASSERT_DEBUG(iEngineObserver, Panic(CMediaClientEngine::ENullObserver));
	__ASSERT_DEBUG(iAudioPlayUtility, Panic(CMediaClientEngine::ENullPlayerUtility));
	__ASSERT_DEBUG(iState == EPlayFilePrepare || iState == EGetMetaData1, Panic(CMediaClientEngine::EWrongState));
	
	// If preparation was sucessful, play the tone
	if (aError)
		{
		PlayCleanup();
		iEngineObserver -> HandleEngineState(iState,aError);
		return;
		}
	if (iState == EPlayFilePrepare)
		{
		iState = EPlayFilePlaying;
		iAudioPlayUtility -> SetVolume(iAudioPlayUtility -> MaxVolume()/iVolume);
		iAudioPlayUtility->Play();		
		iEngineObserver -> HandleEngineState(iState,aError);
		return;
		}
	if (iState == EGetMetaData1)
		{
		TInt numEntries = 0;
		iAudioPlayUtility -> GetNumberOfMetaDataEntries(numEntries);
		for (TInt i=0; i<numEntries; i++)
			iMetaData->Append(iAudioPlayUtility->GetMetaDataEntryL(i));
		PlayCleanup();
		iEngineObserver -> HandleEngineState(EGetMetaData2,aError);
		iState = EIdle;
		return;
		}
	}

// Audio has completed playing the tone
// from MMdaAudioPlayerCallback
void CMediaClientEngine::MapcPlayComplete(TInt aError)
	{
	__ASSERT_DEBUG(iState == EPlayFilePlaying, Panic(CMediaClientEngine::ENotReady));
	__ASSERT_DEBUG(iEngineObserver, Panic(CMediaClientEngine::ENullObserver));
	
	// Tell observer sound is complete, and clean up
	PlayCleanup();
	iEngineObserver -> HandleEngineState(iState,aError);
	}

// Clean up
void CMediaClientEngine::PlayCleanup()
	{
	iState = EIdle;
	delete iAudioPlayUtility;
	iAudioPlayUtility = NULL;
	}


//
/************** SOUND RECORDING/CONVERSION ******************/
//

// Get a list of possible record formats
void CMediaClientEngine::RecordTypesL(CDesCArray& aTypeArray)
	{
	// get a list of mmf plug-ins that can read the file's format
	RMMFControllerImplInfoArray controllers;
	CleanupResetAndDestroyPushL(controllers);

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelectRead = CMMFFormatSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelectWrite = CMMFFormatSelectionParameters::NewLC();

	RArray<TUid> mediaIds;
	mediaIds.Append(KUidMediaTypeAudio);
	CleanupClosePushL(mediaIds);
	cSelect->SetMediaIdsL(mediaIds, CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds);
	cSelect->SetRequiredPlayFormatSupportL(*fSelectRead);
	cSelect->SetRequiredRecordFormatSupportL(*fSelectWrite); // need to specify this as otherwise the supported record formats won't be retrieved
	cSelect->ListImplementationsL(controllers);

	TBuf<10> buf;
	// Loop through each returned plug-in and get their record formats
	for (TInt i=0; i<controllers.Count(); i++)
		{
		const RMMFFormatImplInfoArray& recordInfo =  controllers[i]->RecordFormats();
		// Get array of supported file extensions.
		for (TInt j=0; j<recordInfo.Count(); j++)
			{
			const CDesC8Array& extensions = recordInfo[j]->SupportedFileExtensions();
			// and finally add each extension to the array
			for (TInt k=0; k<extensions.Count(); k++)
				{
				buf.Copy(extensions[k]);
				aTypeArray.AppendL(buf);
				} // end of k
			} // end of j
		} // end of i
	CleanupStack::PopAndDestroy(5);
	} // end of function


// Record into a file
void CMediaClientEngine::RecordL(const TDesC &aFile, const TRecordFormatOptions& aRecordFormatOptions)
	{
	__ASSERT_DEBUG(iState == CMediaClientEngine::EIdle, Panic(CMediaClientEngine::EInProgress));
	
	iRecordFormatOptions = &aRecordFormatOptions;
	// Set up file to record to, the recording format, codec, and settings
	iAudioRecorderUtility = CMdaAudioRecorderUtility::NewL(*this);
	iAudioRecorderUtility -> OpenFileL(aFile);
	iState = ERecordPrepare;
	iEngineObserver -> HandleEngineState(iState,KErrNone);
	// This will call back MoscoStateChangeEvent() when preparation is complete
	}

// Set options and start recording
void CMediaClientEngine::Record2L()
	{
	// Set the options
	iAudioRecorderUtility -> SetAudioDeviceMode(CMdaAudioRecorderUtility::EDefault);
	if (iRecordFormatOptions->iGainSet)
		iAudioRecorderUtility -> SetGain(iRecordFormatOptions->iGain);
	if (iRecordFormatOptions->iBalanceSet)
		iAudioRecorderUtility -> SetPlaybackBalance(iRecordFormatOptions->iBalance);
	if (iRecordFormatOptions->i4CCSet)
		iAudioRecorderUtility -> SetDestinationDataTypeL(iRecordFormatOptions->i4CC);
	if (iRecordFormatOptions->iBitRateSet)
		iAudioRecorderUtility -> SetDestinationBitRateL(iRecordFormatOptions->iBitRate);
	if (iRecordFormatOptions->iSampleRateSet)
		iAudioRecorderUtility -> SetDestinationSampleRateL(iRecordFormatOptions->iSampleRate);
	if (iRecordFormatOptions->iChannelSet)
		iAudioRecorderUtility -> SetDestinationNumberOfChannelsL(iRecordFormatOptions->iChannel);

	// and start recording
	iAudioRecorderUtility -> RecordL();
	iState = ERecording;
	}

// Clean up recorder
void CMediaClientEngine::RecordCleanup()
	{
	iState = EIdle;
	delete iAudioRecorderUtility;
	iAudioRecorderUtility = NULL;
	}

// Gets a recorder for specified target type
CMdaAudioRecorderUtility* CMediaClientEngine::NewRecorderL(const TDesC& aTargetType)
	{
	__ASSERT_DEBUG(iState == CMediaClientEngine::EIdle, Panic(CMediaClientEngine::EInProgress));

	return CRecorderCreator::NewRecorderL(aTargetType);
	}

// ************** CRecorderCreator **************
//

CRecorderCreator::CRecorderCreator()
:CActive(EPriorityStandard)
	{}

CMdaAudioRecorderUtility* CRecorderCreator::NewRecorderL(const TDesC& aTargetType)
	{
	CRecorderCreator* creator = new (ELeave) CRecorderCreator();
	CleanupStack::PushL(creator);
	CMdaAudioRecorderUtility* record = creator->ConstructL(aTargetType);
	CleanupStack::PopAndDestroy(creator);
	return record;
	}

CMdaAudioRecorderUtility* CRecorderCreator::ConstructL(const TDesC& aTargetType)
	{
	_LIT(KDummyName, "c:\\dummy");
	TFileName name(KDummyName);
	name.Append(aTargetType);

	// create converter
	iRecorderUtility = CMdaAudioRecorderUtility::NewL(*this);
	iRecorderUtility->OpenFileL(name);
	CActiveScheduler::Add(this);
	SetActive();
	CActiveScheduler::Start();
	return iRecorderUtility;
	}

void CRecorderCreator::RunL()
	{
	if (iStatus != KErrNone)
		{
		delete iRecorderUtility;
		iRecorderUtility = NULL;
		}
	CActiveScheduler::Stop();
	}

void CRecorderCreator::DoCancel()
	{
	delete iRecorderUtility;
	iRecorderUtility = NULL;
	CActiveScheduler::Stop();
	}

void CRecorderCreator::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, 
	TInt /*aCurrentState*/, TInt aErrorCode)
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aErrorCode);
	}

// Record/convert events handler
// from MMdaObjectStateChangeObserver
void CMediaClientEngine::MoscoStateChangeEvent(CBase* aObject, 
		TInt /*aPreviousState*/, TInt aCurrentState, TInt aErrorCode)
	{
	__ASSERT_ALWAYS(aObject == iAudioRecorderUtility || aObject == iConvertUtility, Panic(CMediaClientEngine::EWrongState));
	if (aObject == iAudioRecorderUtility)
		{
		__ASSERT_DEBUG(iState == CMediaClientEngine::ERecordPrepare || iState == CMediaClientEngine::ERecording, 
			Panic(CMediaClientEngine::EWrongState));

		if (aErrorCode)
			RecordCleanup();
		else if (aCurrentState == CMdaAudioClipUtility::EOpen)
			Record2L();
		iEngineObserver -> HandleEngineState(iState,aErrorCode);
		}
	// Handle converter event
	if (aObject == iConvertUtility)
		{
		__ASSERT_DEBUG(iState == CMediaClientEngine::EConvert || iState == CMediaClientEngine::EConvertComplete, 
			Panic(CMediaClientEngine::EWrongState));
		// converter has finished
		if (aErrorCode || iState == CMediaClientEngine::EConvertComplete)
			ConvertCleanup();
		else // converter has opened file, move to next stage
			Convert2L();
		iEngineObserver -> HandleEngineState(iState,aErrorCode);
		}
	}

// Get a list of possible destination types
void CMediaClientEngine::DestinationTypesL(const TFileName& aFile, CDesCArray& aTypeArray)
	{
	// get a list of mmf plug-ins that can read the file's format
	RMMFControllerImplInfoArray controllers;
	CleanupResetAndDestroyPushL(controllers);
	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelectRead = CMMFFormatSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelectWrite = CMMFFormatSelectionParameters::NewLC();
	fSelectRead->SetMatchToFileNameL(aFile); // Specify the source file format
	cSelect->SetRequiredPlayFormatSupportL(*fSelectRead);
	cSelect->SetRequiredRecordFormatSupportL(*fSelectWrite); // need to specify this as otherwise the supported record formats won't be retrieved
	cSelect->ListImplementationsL(controllers);

	TBuf<10> buf;
	TParsePtrC parse(aFile);
	TPtrC ext = parse.Ext();

	// Loop through each returned plug-in and get their record formats
	for (TInt i=0; i<controllers.Count(); i++)
		{
		const RMMFFormatImplInfoArray& recordInfo =  controllers[i]->RecordFormats();
		// Get array of supported file extensions.
		for (TInt j=0; j<recordInfo.Count(); j++)
			{
			const CDesC8Array& extensions = recordInfo[j]->SupportedFileExtensions();
			// and finally add each extension to the array
			for (TInt k=0; k<extensions.Count(); k++)
				{
				buf.Copy(extensions[k]);
				if (buf != ext)
					aTypeArray.AppendL(buf);
				} // end of k
			} // end of j
		} // end of i
	CleanupStack::PopAndDestroy(4);
	} // end of function

// Create and open a converter
void CMediaClientEngine::ConvertL(const TFileName& aFile, const TDesC& aTargetType, const TFormatOptions& aOptions)
	{
	__ASSERT_DEBUG(iState == CMediaClientEngine::EIdle, Panic(CMediaClientEngine::EInProgress));
	
	iOptions = &aOptions;
	ChangeExtension(aFile, aTargetType, iTargetFile);
	// create converter
	iConvertUtility = CMdaAudioConvertUtility::NewL(*this);
	iConvertUtility->OpenL(aFile, iTargetFile);
	iState = EConvert;
	// This will call back MoscoStateChangeEvent() when converter is open
	}

// Set conversion options and start conversion
void CMediaClientEngine::Convert2L()
	{
	if (iOptions->i4CCSet) iConvertUtility->SetDestinationDataTypeL(iOptions->i4CC);
	if (iOptions->iBitRateSet) iConvertUtility->SetDestinationBitRateL(iOptions->iBitRate);
	if (iOptions->iSampleRateSet) iConvertUtility->SetDestinationSampleRateL(iOptions->iSampleRate);
	if (iOptions->iChannelSet) iConvertUtility->SetDestinationNumberOfChannelsL(iOptions->iChannel);
	iConvertUtility->ConvertL();
	iState = EConvertComplete;
	// This will call back MoscoStateChangeEvent() when conversion is complete
	}

// Clean up converter
void CMediaClientEngine::ConvertCleanup()
	{
	iState = EIdle;
	delete iConvertUtility;
	iConvertUtility = NULL;
	}

//
/************** STREAM PLAYING ******************/
//

// Play a stream
void CMediaClientEngine::PlayL()
	{
	__ASSERT_DEBUG(iState == CMediaClientEngine::EIdle, Panic(CMediaClientEngine::EInProgress));
	iAudioStreamPlayer = CMdaAudioOutputStream::NewL(*this);
	iSettings.iVolume = iAudioStreamPlayer -> MaxVolume()/iVolume;
	iSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
	iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;

	iAudioStreamPlayer -> Open(&iSettings);
		// indicates whether the example will use a list to store the sound fragments
	iState = EStreamPrepare;
	iEngineObserver -> HandleEngineState(iState,KErrNone);
	// This will call back MaoscOpenComplete() when preparation is complete
	}

// Audio is now prepared to play a stream
void CMediaClientEngine::MaoscOpenComplete(TInt aError)
	{
	if (aError)
		StreamCleanup();
	else
		iState = EStreamStarted;

	iEngineObserver -> HandleEngineState(iState,aError);
	}

// Audio has accepted data: call back observer so they know to write more data.
// If using a list, just delete the first element
void CMediaClientEngine::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	// Check for error
	if (aError)
		iState = EStreamError;
	else
		iState = ENextStreamBuf;

	iEngineObserver -> HandleEngineState(iState,aError);
	}

// Stream has finished
void CMediaClientEngine::MaoscPlayComplete(TInt aError)
	{
	if (aError == KErrUnderflow) aError=KErrNone;
	StreamCleanup();
	iEngineObserver -> HandleEngineState(iState,aError);
	}

// Write data to the stream
void CMediaClientEngine::WriteToStreamL(const TDesC8& aData)
	{
	if (iState == CMediaClientEngine::EStreamStarted 
		|| iState == CMediaClientEngine::ENextStreamBuf) 
		iAudioStreamPlayer -> WriteL(aData);
	// This will call back MaoscBufferCopied() when the data is copied
	}


// Clean up
void CMediaClientEngine::StreamCleanup()
	{
	iState = EIdle;
	delete iAudioStreamPlayer;
	iAudioStreamPlayer = NULL;
	}


//
// ************** METADATA FOR A FILE **************
//

void CMediaClientEngine::GetMetaDataL(const TFileName& aFile, RMetaDataArray& aMetaData)
	{
	__ASSERT_DEBUG(iState == CMediaClientEngine::EIdle, Panic(CMediaClientEngine::EInProgress));
	
	iAudioPlayUtility = CMdaAudioPlayerUtility::NewFilePlayerL(aFile, *this);
	iMetaData = &aMetaData;
	iState = EGetMetaData1;
	// This will call back MapcInitComplete() when preparation is complete
	}


// Helper classes & functions

//
// ************** CConverterCreator **************
//

CMdaAudioConvertUtility* CMediaClientEngine::NewConverterL(const TFileName& aFile, const TDesC& aTargetType,
	TFileName& aTargetFile)
	{
	__ASSERT_DEBUG(iState == CMediaClientEngine::EIdle, Panic(CMediaClientEngine::EInProgress));

	return CConverterCreator::NewConverterL(aFile, aTargetType, aTargetFile);
	}

CConverterCreator::CConverterCreator()
:CActive(EPriorityStandard)
	{}

CMdaAudioConvertUtility* CConverterCreator::NewConverterL(const TFileName& aFile, const TDesC& aTargetType,
	TFileName& aTargetFile)
	{
	CConverterCreator* creator = new (ELeave) CConverterCreator();
	CleanupStack::PushL(creator);
	CMdaAudioConvertUtility* convert = creator->ConstructL(aFile, aTargetType, aTargetFile);
	CleanupStack::PopAndDestroy(creator);
	return convert;
	}

CMdaAudioConvertUtility* CConverterCreator::ConstructL(const TFileName& aFile, const TDesC& aTargetType,
	TFileName& aTargetFile)
	{
	ChangeExtension(aFile, aTargetType, aTargetFile);
	// create converter
	iConvertUtility = CMdaAudioConvertUtility::NewL(*this);
	iConvertUtility->OpenL(aFile, aTargetFile);
	CActiveScheduler::Add(this);
	SetActive();
	CActiveScheduler::Start();
	return iConvertUtility;
	}

void CConverterCreator::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aErrorCode);
	}

void CConverterCreator::RunL()
	{
	if (iStatus != KErrNone)
		{
		delete iConvertUtility;
		iConvertUtility = NULL;
		}
	CActiveScheduler::Stop();
	}

void CConverterCreator::DoCancel()
	{
	delete iConvertUtility;
	iConvertUtility = NULL;
	CActiveScheduler::Stop();
	}

//
// ************* ChangeExtension *************
//

void ChangeExtension(const TFileName& aFile, const TDesC& aTargetType,
	TFileName& aTargetFile)
	{
	// create target file name as existing file + plus target extension
	aTargetFile = aFile;
	TParsePtr parse(aTargetFile);
	TPtrC ext = parse.Ext();
	aTargetFile.Replace(aTargetFile.Length()-ext.Length(),ext.Length(),aTargetType);
	}
