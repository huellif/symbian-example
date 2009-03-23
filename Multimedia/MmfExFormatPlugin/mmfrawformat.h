
#ifndef __MMF_RAW_FORMAT_H__
#define __MMF_RAW_FORMAT_H__

// MMF framework headers
#include <mmfformat.h>
#include <mmfdatabuffer.h>
#include <mmfclip.h>
#include <ImplementationProxy.h>
#include <mmfutilities.h>

/* Audio decoder plug-in to read .RAW audio.
 It implements MDataSource to pass data to the controller, and MDataSink to read data from 
 the source clip.
 Also implements MAsyncEventHandler to send an event to the client */
class CMMFRawFormatRead : public CMMFFormatDecode, public MAsyncEventHandler
	{
public:
	// Factory function
	static CMMFFormatDecode* NewL(MDataSource* aSource);
	~CMMFRawFormatRead();

	//from MDataSource
	void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId);
	CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, TBool &aReference); 
	CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& aSinkBuffer, TBool &aReference);
	TFourCC SourceDataTypeCode(TMediaId aMediaId);
	TInt SetSourceDataTypeCode(TFourCC aSourceFourCC, TMediaId aMediaId); 
	TInt SourceThreadLogon(MAsyncEventHandler& aEventHandler);
	void SourceThreadLogoff();
	void SourcePrimeL();
	void SourcePlayL();
	void SourcePauseL();
	void SourceStopL();

	//from MDataSink
	void BufferFilledL(CMMFBuffer* aBuffer);

	// from CMMFFormatDecode
	TUint Streams(TUid aMediaType) const;
	TTimeIntervalMicroSeconds FrameTimeInterval(TMediaId aMediaType) const;
	TTimeIntervalMicroSeconds Duration(TMediaId aMediaType) const;
	TUint NumChannels() {return iChannels;};
	TUint SampleRate() {return iSampleRate;};
	TUint BitRate() {return iSampleRate * iBitsPerSample;};
	TInt SetNumChannels(TUint aChannels);
	TInt SetSampleRate(TUint aSampleRate);
	void GetSupportedSampleRatesL(RArray<TUint>& aSampleRates);
	void GetSupportedNumChannelsL(RArray<TUint>& aNumChannels);
	void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);
	void SuggestSourceBufferSize(TUint aSuggestedBufferSize);	

	//from MAsyncEventHandler
	TInt SendEventToClient(const TMMFEvent& /*aEvent*/) {return KErrNone;}

private:
	// Construction
	void ConstructL(MDataSource* aSource);
	// Helper functions
	CMMFDataBuffer* CreateSourceBufferOfSizeL(TUint aSize);
	void DoReadL(TInt aReadPosition);
	void NegotiateSourceBufferL(CMMFBuffer& aBuffer);
	void CalculateFrameTimeInterval();

protected:
	MDataSource* iClip; //for decode format MDatasource; for encode format MDataSink
	MDataSink* iDataPath; //for decode format MDataSink; for encode format MDataSource
	TFourCC iFourCC;	

private:
	CMMFDataBuffer* iBuffer;
	TUint iStartPosition;
	TUint iPos;
	TUint iChannels;
	TUint iSampleRate;
	TUint iBitsPerSample;
	TTimeIntervalMicroSeconds iFrameTimeInterval;
	TUint iFrameSize;
	TUint iClipLength;
	};


/* Audio encoder plug-in to write .RAW audio.
 It implements MDataSink to get data from the controller, and MDataSource to write data to 
 the target clip.
 Also implements MAsyncEventHandler to send an event to the client */
class CMMFRawFormatWrite : public CMMFFormatEncode, public MAsyncEventHandler
	{
public:
	// Factory function
	static CMMFFormatEncode* NewL(MDataSink* aSink);
	~CMMFRawFormatWrite();

	//from MDataSink
	CMMFBuffer* CreateSinkBufferL(TMediaId aMediaId, TBool &aReference); 
	TFourCC SinkDataTypeCode(TMediaId aMediaId); //returns FourCC code for the mediaId
	TInt SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId); 
	TInt SinkThreadLogon(MAsyncEventHandler& aEventHandler);
	void SinkThreadLogoff();
	void Negotiate(MDataSource& aSource);
	void SinkPrimeL();
	void SinkPlayL();
	void SinkPauseL();
	void SinkStopL();
	void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId);

	//from MDataSource
	void BufferEmptiedL(CMMFBuffer* aBuffer);

	// from CMMFFormatEncode
	TTimeIntervalMicroSeconds FrameTimeInterval(TMediaId aMediaType) const;
	TTimeIntervalMicroSeconds Duration(TMediaId aMediaType) const;
	TInt SetNumChannels(TUint aChannels);
	TInt SetSampleRate(TUint aSampleRate);
	TUint NumChannels() {return iChannels;};
	TUint SampleRate() {return iSampleRate;};
	TUint BitRate() {return iSampleRate * iBitsPerSample;};
	TInt64 BytesPerSecond()  ;
	void GetSupportedSampleRatesL(RArray<TUint>& aSampleRates);
	void GetSupportedNumChannelsL(RArray<TUint>& aNumChannels);
	void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);
	void SetMaximumClipSize(TInt aBytes);
	void CropL(TTimeIntervalMicroSeconds aPosition, TBool aToEnd ) ;

	//from MAsyncEventHandler
	TInt SendEventToClient(const TMMFEvent& /*aEvent*/) {return KErrNone;}

private:
	// construction
	void ConstructL(MDataSink* aSink);
	// helper functions
	CMMFDataBuffer* CreateSinkBufferOfSizeL(TUint aSize);
	void CalculateFrameTimeInterval();
	void DoReadL(TInt aReadPosition);
	void DoWriteL(TInt aWritePosition);

private:
	MDataSink* iClip; //for decode format MDatasource; for encode format MDataSink
	MDataSource* iDataPath; //for decode format MDataSink; for encode format MDataSource
	TFourCC iFourCC;	
	CMMFDataBuffer* iBuffer;
	TBool iClipAlreadyExists;
	TBool iBufferCreated;
	TBool iFileHasChanged;
	TUint iStartPosition;
	TUint iDataLength;
	TUint iPos;
	TUint iMaxPos;
	TUint iChannels;
	TUint iSampleRate;
	TUint iSourceChannels;
	TUint iSourceSampleRate;
	TFourCC iSourceFourCC;
	TUint iBitsPerSample;
	TTimeIntervalMicroSeconds iFrameTimeInterval;
	TUint iFrameSize;
	TUint iClipLength;
	TUint iDataSize;
	CMMFChannelAndSampleRateConverterFactory* iChannelAndSampleRateConverterFactory;
	CMMFChannelAndSampleRateConverter* iChannelAndSampleRateConverter; 
	CMMFDataBuffer* iConvertBuffer;
	CMMFBuffer* iBufferToEmpty; //pointer to store buffr to empty - needed if buffer passed to clip is different
	TBool iSourceWillSampleConvert;
	TInt iMaximumClipSize;
	};

#endif

