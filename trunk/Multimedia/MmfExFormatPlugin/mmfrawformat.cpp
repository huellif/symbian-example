// Copyright (c) 2002-2005 Symbian Software Ltd.  All rights reserved.

// MMF framework headers
#include <mmfFile.h>
#include <ecom.h>
#include <mmfFormatImplementationUIDs.hrh>
#include <mmffourcc.h>

#include "mmfrawformat.h"
#include "UIDs.hrh"

const TUint KFormatDefaultFrameSize(0x1000); //Set default frame size to 4 K
const TUint KDefineIOBufferSize(0x0200); //easy to read clip buffer size
const TUint KOneSecondInMicroSeconds(1000000); //1 Second
const TUint KMono(1);
const TUint KStereo(2);
//this defines the valid sample rates for RAW
const TUint KRawSampleRates[] = { 8000, 11025, 22050, 44100 };


//
// CMMFRawFormatRead
//

// Factory function
CMMFFormatDecode* CMMFRawFormatRead::NewL(MDataSource* aSource)
	{
	if ((aSource->DataSourceType()==KUidMmfDescriptorSource)||
			(aSource->DataSourceType()==KUidMmfFileSource))
		{//currently only files and descriptor MDataSources are supported
		CMMFRawFormatRead* self = new(ELeave)CMMFRawFormatRead;
		CleanupStack::PushL(self);
		self->ConstructL(aSource);
		CleanupStack::Pop();
		return self;
		}
	else return NULL;
	}

// Destructor
CMMFRawFormatRead::~CMMFRawFormatRead()
	{
	delete iBuffer;
	}
	
// Second-phase constructor
void CMMFRawFormatRead::ConstructL(MDataSource* aSource)
	{
	// tell clip we're using it
	iClip = aSource;
	User::LeaveIfError(iClip->SourceThreadLogon(*this));
	iClip->SourcePrimeL();
	iFrameSize = KFormatDefaultFrameSize;
	iClipLength = (static_cast<CMMFClip*>(iClip))->Size();
	}

// Implementing MDataSource

// Handle request to fill buffer with data from clip
void CMMFRawFormatRead::FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId )
	{	
	// check media id & pass onto the clip
	if (aMediaId.iMediaType != KUidMediaTypeAudio) User::Leave(KErrNotSupported); 
	iDataPath = aConsumer;
	//assumes first frame is frame 1
	TUint position = ((aBuffer->FrameNumber()-1)*iFrameSize)+iStartPosition;
	(static_cast<CMMFClip*>(iClip))->ReadBufferL(aBuffer, position, this);
	// notified of when buffer is full by BufferFilledL
	}

// creates the buffer for the source
// This overload supplies the sink buffer, as optimal source buffer size creation may depend on this
CMMFBuffer* CMMFRawFormatRead::CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& aSinkBuffer, TBool &aReference)
	{
	if (aMediaId.iMediaType == KUidMediaTypeAudio) 
		{
		NegotiateSourceBufferL(aSinkBuffer); //sets frame size to match sink buffer
		return CreateSourceBufferL(aMediaId, aReference);
		}
	else User::Leave(KErrNotSupported);
	return NULL;
	}

// creates the buffer for the source
CMMFBuffer* CMMFRawFormatRead::CreateSourceBufferL(TMediaId aMediaId, TBool &aReference)
	{
	if (aMediaId.iMediaType == KUidMediaTypeAudio) 
		{
		// assume default frame size if haven't determined a better one
		if (!iFrameSize) iFrameSize = KFormatDefaultFrameSize;
		// sets aReference to false if a new CMMFBuffer is returned
		aReference = EFalse;
		return CreateSourceBufferOfSizeL(iFrameSize);
		}
	else User::Leave(KErrNotSupported);
	return NULL;
	}

// Helper function to create and zero fill a buffer of specified size
CMMFDataBuffer* CMMFRawFormatRead::CreateSourceBufferOfSizeL(TUint aSize)
	{
	//needs to create source buffer
	CMMFDataBuffer* buffer = CMMFDataBuffer::NewL(aSize);
	buffer->Data().FillZ(aSize);
	return buffer;
	}

// Helper function to determine best source buffer size
void CMMFRawFormatRead::NegotiateSourceBufferL(CMMFBuffer& aSinkBuffer)
	{
	// if sink buffer has a fixed size use this to determine source buffer size
	if (aSinkBuffer.Type() == KUidMmfDataBuffer)
		{
		// RAW is linear data, so can set target buffer to sink buffer size
		TUint sinkBufferLength = (static_cast<CMMFDataBuffer&>(aSinkBuffer)).Data().MaxLength();
		if (sinkBufferLength == 0) sinkBufferLength = KFormatDefaultFrameSize;
		iFrameSize = sinkBufferLength; 
		CalculateFrameTimeInterval();
		}
	else 
		User::Leave(KErrNotSupported);
	}

// returns the codec FourCC code for the mediaId
TFourCC CMMFRawFormatRead::SourceDataTypeCode(TMediaId aMediaId)
	{
	if (aMediaId.iMediaType == KUidMediaTypeAudio) return iFourCC;
	else return TFourCC(); //defaults to 'NULL' fourCC
	}

// sets the codec FourCC code for the mediaId
TInt CMMFRawFormatRead::SetSourceDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId)
	{
	if (aMediaId.iMediaType != KUidMediaTypeAudio) return KErrNotSupported;
	else iFourCC = aSinkFourCC;
	
	if ((iFourCC == KMMFFourCCCodePCM16) || 
		(iFourCC == KMMFFourCCCodePCM16B) || 
		(iFourCC == KMMFFourCCCodePCMU16)) 
			iBitsPerSample = 16;
	else if ((iFourCC == KMMFFourCCCodeIMAD) || 
		(iFourCC == KMMFFourCCCodeIMAS)) 
			iBitsPerSample = 4;
	else 
			iBitsPerSample = 8; //default to 8
	return KErrNone;
	}

// Initiate use of the interface 
TInt CMMFRawFormatRead::SourceThreadLogon(MAsyncEventHandler& aEventHandler)
	{//pass through to source clip
	return(iClip->SourceThreadLogon(aEventHandler));
	}

// Prepare clip
void CMMFRawFormatRead::SourcePrimeL()
	{
	iClip->SourcePrimeL();
	CalculateFrameTimeInterval();
	}

// Play clip
void CMMFRawFormatRead::SourcePlayL()
	{
	iClip->SourcePlayL();
	}

// Pause clip
void CMMFRawFormatRead::SourcePauseL()
	{
	iClip->SourcePauseL(); //propagate state change down to clip
	}

// Stop clip
void CMMFRawFormatRead::SourceStopL()
	{
	iClip->SourceStopL();
	}

// End use of the interface
void CMMFRawFormatRead::SourceThreadLogoff()
	{
	iClip->SourceThreadLogoff();
	}

// from MDataSink

// called by MDataSource to pass back full buffer to the sink
void CMMFRawFormatRead::BufferFilledL(CMMFBuffer* aBuffer)
	{
	//set position
	TTimeIntervalMicroSeconds position = //assumes frame numbers begin at frame 1
		TTimeIntervalMicroSeconds(TInt64(aBuffer->FrameNumber()-1)*iFrameTimeInterval.Int64());
	aBuffer->SetTimeToPlay(position);
	iDataPath->BufferFilledL(aBuffer);	
	}


// from CMMFFormatDecode

// returns number of streams
TUint CMMFRawFormatRead::Streams(TUid aMediaType) const
	{
	//need to check aMediaType for audio
	if (aMediaType == KUidMediaTypeAudio) return 1; //raw files can only have 1 audio stream
	else return 0;
	}

// returns the time interval for one frame
TTimeIntervalMicroSeconds CMMFRawFormatRead::FrameTimeInterval(TMediaId aMediaId) const
	{
	if (aMediaId.iMediaType == KUidMediaTypeAudio) return iFrameTimeInterval;
	else return TTimeIntervalMicroSeconds(0);
	}

// returns the duration of the source clip
TTimeIntervalMicroSeconds CMMFRawFormatRead::Duration(TMediaId aMediaId) const
	{
	if ((aMediaId.iMediaType == KUidMediaTypeAudio) && 
		(iClipLength) && (iSampleRate) && (iBitsPerSample) && (iChannels))
		{//we have enough values to calculate the duration
		TInt64 clipLength(iClipLength);
		clipLength*=KOneSecondInMicroSeconds;
		TTimeIntervalMicroSeconds duration = TTimeIntervalMicroSeconds(clipLength/iSampleRate);
		duration = TTimeIntervalMicroSeconds(duration.Int64()/(iBitsPerSample*iChannels));
		duration = TTimeIntervalMicroSeconds(duration.Int64()*8);
		return duration;
		}
	else return TTimeIntervalMicroSeconds(0);
	}

// helper function: calculates time between frames
void CMMFRawFormatRead::CalculateFrameTimeInterval()
	{
	if ((iFrameSize) && (iSampleRate) && (iBitsPerSample) && (iChannels))
		{
		iFrameTimeInterval = TTimeIntervalMicroSeconds((iFrameSize*KOneSecondInMicroSeconds)/iSampleRate);
		iFrameTimeInterval = 
			TTimeIntervalMicroSeconds(iFrameTimeInterval.Int64()/(iBitsPerSample*iChannels));
		iFrameTimeInterval = TTimeIntervalMicroSeconds(iFrameTimeInterval.Int64()*8);
		}
	}

// called by sink to suggest a source buffer size
void CMMFRawFormatRead::SuggestSourceBufferSize(TUint aSuggestedBufferSize)
	{
	iFrameSize = aSuggestedBufferSize; //set source format frame size to buffer size suggested by sink
	CalculateFrameTimeInterval();
	}

// set the number of channels
TInt CMMFRawFormatRead::SetNumChannels(TUint aChannels)
	{
	TInt error = KErrNone;
	if ((aChannels ==  KMono)||(aChannels == KStereo)) iChannels = aChannels;
	else error = KErrNotSupported; //only alow one or two channels
	return error;
	}

// set the sample rate
TInt CMMFRawFormatRead::SetSampleRate(TUint aSampleRate)
	{
	TInt status = KErrNotSupported;
	//we'll iterate through the valid sample table
	TInt i = sizeof(KRawSampleRates) / sizeof(TUint);
		
	while ((i--) && (status != KErrNone))
		{
		if (aSampleRate == KRawSampleRates[i])
			{
			iSampleRate = aSampleRate;
			status = KErrNone;
			}
		}
	return status;
	}

// helper function to read from clip
void CMMFRawFormatRead::DoReadL(TInt aReadPosition)
	{
	STATIC_CAST(CMMFClip*,iClip)->ReadBufferL(iBuffer,aReadPosition);
	}

// get the supported sample rates
void CMMFRawFormatRead::GetSupportedSampleRatesL(RArray<TUint>& aSampleRates)
	{
	aSampleRates.Reset();

	// Iterate through the valid sample table and append each value to aSampleRates
	TInt i = sizeof(KRawSampleRates) / sizeof(TUint);
	
	while (i--)
		{
		User::LeaveIfError(aSampleRates.Append(KRawSampleRates[i]));
		}
	}

// get the supported channel number options
void CMMFRawFormatRead::GetSupportedNumChannelsL(RArray<TUint>& aNumChannels)
	{
	aNumChannels.Reset();
	User::LeaveIfError(aNumChannels.Append(KMono));
	User::LeaveIfError(aNumChannels.Append(KStereo));
	}

// get the supported codecs
void CMMFRawFormatRead::GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes)
	{
	if (aMediaId.iMediaType != KUidMediaTypeAudio)
		User::Leave(KErrNotSupported);
	aDataTypes.Reset();
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM16));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM16B));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCMU16));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeIMAD));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeIMAS));
	}


//
// CMMFRawFormatWrite
//

// Factory function
CMMFFormatEncode* CMMFRawFormatWrite::NewL(MDataSink* aSink)
	{
	if ((aSink->DataSinkType()==KUidMmfDescriptorSink)||
			(aSink->DataSinkType()==KUidMmfFileSink))
		{//currently only files and descriptor MDataSources are supported
		CMMFRawFormatWrite* self = new(ELeave)CMMFRawFormatWrite;
		CleanupStack::PushL(self);
		self->ConstructL(aSink);
		CleanupStack::Pop();
		return STATIC_CAST(CMMFFormatEncode*, self);
		}
	else return NULL;
	}

// destructor
CMMFRawFormatWrite::~CMMFRawFormatWrite()
	{
	delete iBuffer;
	delete iConvertBuffer;
	delete iChannelAndSampleRateConverterFactory;
	}
	
// second-phase construction	
void CMMFRawFormatWrite::ConstructL(MDataSink* aSink)
	{
	iClip = aSink;
	//first need to check if sink clip already exists to get settings.
	User::LeaveIfError(iClip->SinkThreadLogon(*this));
	iClip->SinkPrimeL();
	iBuffer = CreateSinkBufferOfSizeL(KDefineIOBufferSize); //512 easiest file size to read
	DoReadL(0);//read from beginning of clip
	if (iBuffer->Data().Size()> 0)
		{
		iClipAlreadyExists = ETrue;
		//There is no header, so data size is the same as the clip size in this case.
		iDataSize = iClipLength = STATIC_CAST(CMMFClip*,iClip)->Size();
		}
	iFrameSize = KFormatDefaultFrameSize;
	}

// from MDataSink

// sink thread attaches
TInt CMMFRawFormatWrite::SinkThreadLogon(MAsyncEventHandler& aEventHandler)
	{//pass through to sink clip
	return(iClip->SinkThreadLogon(aEventHandler));
	}

// helper function: calculates time between frames
void CMMFRawFormatWrite::CalculateFrameTimeInterval()
	{
	if ((iFrameSize) && (iSampleRate) && (iBitsPerSample) && (iChannels))
		{
		iFrameTimeInterval = TTimeIntervalMicroSeconds((iFrameSize*KOneSecondInMicroSeconds)/iSampleRate);
		iFrameTimeInterval = 
			TTimeIntervalMicroSeconds((iFrameTimeInterval.Int64())/(iBitsPerSample*iChannels));
		iFrameTimeInterval = TTimeIntervalMicroSeconds(iFrameTimeInterval.Int64()*8);
		}
	}

// called if sink setup depends on source
void CMMFRawFormatWrite::Negotiate(MDataSource& aSource)
	{
	if (aSource.DataSourceType() == KUidMmfAudioInput)
		{
		// could  query the audio caps from DevSound for the settings below
		iSourceSampleRate = 8000; // assume 8KHz for now
		iSourceChannels = 1; //assume mono
		iSourceFourCC.Set(KMMFFourCCCodePCM16); //16 bit PCM
		}
	else if (aSource.DataSourceType() == KUidMmfFormatDecode)
		{//source is a clip so for now set sink settings to match source
		iSourceSampleRate = ((CMMFFormatDecode&)aSource).SampleRate();
		iSourceChannels = ((CMMFFormatDecode&)aSource).NumChannels();
		iSourceFourCC.Set(aSource.SourceDataTypeCode(TMediaId(KUidMediaTypeAudio)));
		iSourceWillSampleConvert = STATIC_CAST(CMMFFormatDecode&, aSource).SourceSampleConvert();
		((CMMFFormatDecode&)aSource).SuggestSourceBufferSize(iFrameSize); //for now suggest format src takes same buf size as sink??
		//make the start position the end of the clip
		}
	else return;
	//set default sink parameters to be the same as the source
	if (iClipAlreadyExists) iStartPosition = iClipLength;
	if (!iSampleRate) iSampleRate = iSourceSampleRate; //might have already been set by custom command
	if (!iChannels) iChannels = iSourceChannels;
	if (!iBitsPerSample)
		{
		iFourCC.Set(iSourceFourCC);
		if ((iFourCC == KMMFFourCCCodePCM16) ||
			(iFourCC == KMMFFourCCCodePCM16B) ||
			(iFourCC == KMMFFourCCCodePCMU16))
				iBitsPerSample = 16;		
		else if ((iFourCC == KMMFFourCCCodeIMAD) || 
			(iFourCC == KMMFFourCCCodeIMAS))
				iBitsPerSample = 4;	
		else 
				iBitsPerSample = 8; //default to 8
		}
	CalculateFrameTimeInterval();
	}

// Prime the sink to be accessed
void CMMFRawFormatWrite::SinkPrimeL()
	{
	iClip->SinkPrimeL(); //propagate state change down to clip
	CalculateFrameTimeInterval();
	}

// Play the sink
void CMMFRawFormatWrite::SinkPlayL()
	{
	iClip->SinkPlayL(); //propagate state change down to clip
	if ((iChannels != iSourceChannels) || (iSampleRate != iSourceSampleRate) && (!iSourceWillSampleConvert))
		{//the source channels & sample rate don't match the formats - therefore need to do a conversion 
		//currently only pcm16 is supported so return with an error if format not pcm16
		if (iFourCC != KMMFFourCCCodePCM16) User::Leave(KErrNotSupported);
		iChannelAndSampleRateConverterFactory 
			= new(ELeave)CMMFChannelAndSampleRateConverterFactory;
		iChannelAndSampleRateConverter = 
			iChannelAndSampleRateConverterFactory->CreateConverterL( iSourceSampleRate, iSourceChannels, 
																	iSampleRate, iChannels);
		//need to create an intermediate buffer in which to place the converted data
		TUint convertedBufferFrameSize = (iFrameSize*iChannels)/iSourceChannels;
		iConvertBuffer = CreateSinkBufferOfSizeL(convertedBufferFrameSize);
		}
	iFileHasChanged = ETrue; //file will change if we start playing to it
	}

// Pause the sink
void CMMFRawFormatWrite::SinkPauseL()
	{
	iClip->SinkPauseL(); //propagate state change down to clip
	}

// Stop the sink
void CMMFRawFormatWrite::SinkStopL()
	{
	iClip->SinkStopL(); //propagate state change down to clip
	}

// Detach from the sink
void CMMFRawFormatWrite::SinkThreadLogoff()
	{
	iClip->SinkThreadLogoff(); //propagate down to clip
	}

// Called by the CMMFDataPath to add a buffer to a clip
void CMMFRawFormatWrite::EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId)
	{
	//since raw always contains linear audio the sink buffer size can set the source buffer size

	//check media id &pass onto clip
	if (aMediaId.iMediaType!=KUidMediaTypeAudio) User::Leave(KErrNotSupported); 
	iDataPath = aSupplier;

	// Check we haven't exceeded any set maximum on our clip length
	if (iMaximumClipSize > 0)
		{
		// Find the current clip size
		TInt currentClipLength = STATIC_CAST(CMMFClip*, iClip)->Size();
		TInt bufferSize = aBuffer->BufferSize();
		if ((currentClipLength + bufferSize) >= iMaximumClipSize)
			User::Leave(KErrEof);
		}

	//assumes first frame is frame 1
	iBufferToEmpty = aBuffer; //save this so it can be returned to datapath
	TInt position = ((aBuffer->FrameNumber()-1)*iFrameSize)+iStartPosition;
	if (position < (TInt)iStartPosition) position = iStartPosition; //can't write before start of header
	if ((iChannelAndSampleRateConverter) && (!iSourceWillSampleConvert))
		{//need to perform channel & sample rate conversion before writing to clip
		iFrameSize = iChannelAndSampleRateConverter->Convert(*(CMMFDataBuffer*)aBuffer,*iConvertBuffer);
		STATIC_CAST(CMMFClip*,iClip)->WriteBufferL(iConvertBuffer, position, this);
		}
	else
		{//no need to convert the data
		STATIC_CAST(CMMFClip*,iClip)->WriteBufferL(aBuffer, position, this);
		}
	iPos = position; //save current write position
	}

// helper function to create buffer of specficed size
CMMFDataBuffer* CMMFRawFormatWrite::CreateSinkBufferOfSizeL(TUint aSize)
	{
	//needs to create source buffer
	CMMFDataBuffer* buffer = CMMFDataBuffer::NewL(aSize);
	buffer->Data().FillZ(aSize);
	iBufferCreated = ETrue;
	return buffer;
	}

// create buffer to receive data
CMMFBuffer* CMMFRawFormatWrite::CreateSinkBufferL(TMediaId aMediaId, TBool &aReference)
	{
	if (aMediaId.iMediaType == KUidMediaTypeAudio) 
		{
		if (!iFrameSize) iFrameSize = KFormatDefaultFrameSize;
		aReference = EFalse;
		return CreateSinkBufferOfSizeL(iFrameSize);
		}
	else User::Leave(KErrNotSupported);
	return NULL;
	}

// gets the codec type
TFourCC CMMFRawFormatWrite::SinkDataTypeCode(TMediaId aMediaId)
	{
	if (aMediaId.iMediaType == KUidMediaTypeAudio) return iFourCC;
	else return TFourCC(); //defaults to 'NULL' fourCC
	}

// sets the codec type
TInt CMMFRawFormatWrite::SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId)
	{
	if (aMediaId.iMediaType != KUidMediaTypeAudio) return KErrNotSupported;
	else iFourCC = aSinkFourCC;
	
	if ((iFourCC == KMMFFourCCCodePCM16) || 
		(iFourCC == KMMFFourCCCodePCM16B) ||
		(iFourCC == KMMFFourCCCodePCMU16)) 
			iBitsPerSample = 16;
	else if ((iFourCC == KMMFFourCCCodeIMAD) ||
		(iFourCC == KMMFFourCCCodeIMAS)) 
			iBitsPerSample = 4;
	else 
			iBitsPerSample = 8; //default to 8

	return KErrNone;
	}

// helper function to read data from clip
void CMMFRawFormatWrite::DoReadL(TInt aReadPosition)
	{
	STATIC_CAST(CMMFClip*,iClip)->ReadBufferL(iBuffer,aReadPosition);
	}

// helper function to write data to clip
void CMMFRawFormatWrite::DoWriteL(TInt aWritePosition)
	{
	STATIC_CAST(CMMFClip*,iClip)->WriteBufferL(iBuffer,aWritePosition);
	}


// from MDataSource

// called by MDataSink to pass back emptied buffer to the source
void CMMFRawFormatWrite::BufferEmptiedL(CMMFBuffer* aBuffer)
	{
	iDataSize+=aBuffer->BufferSize(); //total bytes written
	iPos += aBuffer->BufferSize(); //total bytes written so far - iPos is not always = iDataSize due to repositions
	if (iMaxPos < iPos) iMaxPos = iPos; //need iMaxPos incase we write data then repos to an earlier pos in the clip
	if (iBufferToEmpty != aBuffer) iDataPath->BufferEmptiedL(iBufferToEmpty); //need to return same buffer
	else iDataPath->BufferEmptiedL(aBuffer);
	}


// from CMMFFormatEncode

// set the number of channels 
TInt CMMFRawFormatWrite::SetNumChannels(TUint aChannels)
	{
	TInt error = KErrNone;
	if ((aChannels ==  KMono)||(aChannels == KStereo)) iChannels = aChannels;
	else error = KErrNotSupported; //only alow one or two channels
	return error;
	}

// set the sample rate
TInt CMMFRawFormatWrite::SetSampleRate(TUint aSampleRate)
	{
	TInt status = KErrNotSupported;
	//we'll iterate through the valid sample table
	TInt i = sizeof(KRawSampleRates) / sizeof(TUint);
		
	while ((i--) && (status != KErrNone))
		{
		if (aSampleRate == KRawSampleRates[i])
			{
			iSampleRate = aSampleRate;
			status = KErrNone;
			}
		}
	return status;
	}

// get the frame interval
TTimeIntervalMicroSeconds CMMFRawFormatWrite::FrameTimeInterval(TMediaId aMediaId) const
	{
	if (aMediaId.iMediaType == KUidMediaTypeAudio) return iFrameTimeInterval;
	else return TTimeIntervalMicroSeconds(0);
	}

// returns the duration of the source clip
TTimeIntervalMicroSeconds CMMFRawFormatWrite::Duration(TMediaId aMediaId) const
	{
	if ((aMediaId.iMediaType == KUidMediaTypeAudio) 
		&& (iDataSize) && (iSampleRate) && (iBitsPerSample) && (iChannels))
		{
		TInt64 clipLength(iDataSize);
		clipLength*=KOneSecondInMicroSeconds;
		TTimeIntervalMicroSeconds duration = TTimeIntervalMicroSeconds(clipLength/iSampleRate);
		duration = 
			TTimeIntervalMicroSeconds(duration.Int64()/(iBitsPerSample*iChannels));
		duration = TTimeIntervalMicroSeconds(duration.Int64()*8);
		return duration;
		}
	else return TTimeIntervalMicroSeconds(0);
	}

// Calculate and return the number of bytes used for on second of audio.
TInt64 CMMFRawFormatWrite::BytesPerSecond() 
	{
	TInt64 bitsPerSecond = iSampleRate * iBitsPerSample * iChannels ;
	TInt64 bytesPerSecond = bitsPerSecond/8;
	return bytesPerSecond ;
	}

// Shortens the clip from the position specified to the end specified.
void CMMFRawFormatWrite::CropL(TTimeIntervalMicroSeconds aPosition, TBool aToEnd )
	{
	// Does clip have any size to crop
	if (!(STATIC_CAST(CMMFClip*,iClip)->Size())) User::Leave(KErrNotFound); //no clip to crop or clip is 0 bytes.


	// Is aPosition between the start and the end?
	if ( ( aPosition < TTimeIntervalMicroSeconds(0) ) || ( aPosition >= Duration( KUidMediaTypeAudio) ) ) 
		User::Leave( KErrArgument ) ;

	// Convert aPostion to cropPosition in bytes

	TInt64 cropPosition64 = 
		TInt64( ( aPosition.Int64() * iSampleRate * (iBitsPerSample/8) * iChannels ) /KOneSecondInMicroSeconds);
	TUint cropPosition = I64INT(cropPosition64);

	// Does cropPosition need adjustment to retain integrity?  (assume not)

	TUint dataSize ;  // This will be the size of the data left after cropping.

	if ( !aToEnd )
		{
		// Shift the data physically
		// move the data in blocks
		// Create a CMMFDataBuffer and use CMMFClip to shift the data
		dataSize = iMaxPos - cropPosition ;
		if (( dataSize > 0 ) && (aPosition != TTimeIntervalMicroSeconds(0)))
			{
			TUint bufSize = ( dataSize < KDefineIOBufferSize ? dataSize : KDefineIOBufferSize ) ; //max bufSize 512
			CMMFDataBuffer* buffer = CMMFDataBuffer::NewL(bufSize) ;
			CleanupStack::PushL( buffer ) ;

			TUint rPos = cropPosition ; // read position
			TUint wPos = 0;
			TInt dataToShift = ETrue ;
			while ( dataToShift )
				{
				STATIC_CAST( CMMFClip*, iClip )->ReadBufferL( buffer, rPos ) ;  // synchronous calls
				STATIC_CAST( CMMFClip*, iClip )->WriteBufferL( buffer, wPos ) ;
				if ( rPos > iMaxPos ) 
					dataToShift = EFalse ;  // past the end:  Done
				else
					{ // shift the pointers
					rPos += bufSize ;
					wPos += bufSize ;
					}
				}// while data to shift
			CleanupStack::PopAndDestroy( ) ; // buffer
			}// if data to shift
		}// crop to start
	else // crop to end
		dataSize = cropPosition ;

	iDataSize = dataSize ;
	iMaxPos = dataSize ;

	// Do the physical chop
	if ( iClip->DataSinkType() == KUidMmfFileSink )
		{
		STATIC_CAST( CMMFFile*, iClip )->FileL().SetSize( iMaxPos ) ;
		iClipLength = iMaxPos; 
		}
	}

// get the supported sample rates
void CMMFRawFormatWrite::GetSupportedSampleRatesL(RArray<TUint>& aSampleRates)
	{
	aSampleRates.Reset();

	// Iterate through the valid sample table and append each value to aSampleRates
	TInt i = sizeof(KRawSampleRates) / sizeof(TUint);
	
	while (i--)
		{
		User::LeaveIfError(aSampleRates.Append(KRawSampleRates[i]));
		}
	}

// get the supported channel number options
void CMMFRawFormatWrite::GetSupportedNumChannelsL(RArray<TUint>& aNumChannels)
	{
	aNumChannels.Reset();
	User::LeaveIfError(aNumChannels.Append(KMono));
	User::LeaveIfError(aNumChannels.Append(KStereo));
	}

// set maximum clip size
void CMMFRawFormatWrite::SetMaximumClipSize(TInt aBytes)
	{
	iMaximumClipSize = aBytes;
	}

// get the supported codecs
void CMMFRawFormatWrite::GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes)
	{
	if (aMediaId.iMediaType != KUidMediaTypeAudio)
		User::Leave(KErrNotSupported);
	aDataTypes.Reset();
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM16));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM16B));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCMU16));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeIMAD));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeIMAS));
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KRawDecoder,	CMMFRawFormatRead::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KRawEncoder,	CMMFRawFormatWrite::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
