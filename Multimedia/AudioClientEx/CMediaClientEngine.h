// CMediaClientEngine.h
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
//

// The engine interfaces to the MMF audio APIs
// to play tones, files, and streams, and record and convert files

#ifndef CMEDIACLIENTENGINE
#define CMEDIACLIENTENGINE

#include <e32std.h>
#include <f32file.h>
#include <badesca.h>

#include <MdaAudioSamplePlayer.h>
#include <MdaAudioTonePlayer.h>
#include <MdaAudioSampleEditor.h>
#include <MdaAudioOutputStream.h>

class MExEngineObserver;

// Defines type for array of media file meta data
typedef RPointerArray<CMMFMetaDataEntry> RMetaDataArray;

/** Conversion format options */
class TFormatOptions
	{
public:
	TFormatOptions() {Reset();}
	virtual void Reset() {i4CCSet = iBitRateSet = iSampleRateSet = iChannelSet = EFalse;}
public:
	// Conversion format option values
	TFourCC i4CC;
	TUint iBitRate;
	TUint iSampleRate;
	TUint iChannel;
	// Flags indicating which options have been set
	TBool i4CCSet;
	TBool iBitRateSet;
	TBool iSampleRateSet;
	TBool iChannelSet;
	};

/** Record format options 
	These are the same as conversion options, plus gain and balance settings */
class TRecordFormatOptions: public TFormatOptions
	{
public:
	TRecordFormatOptions() {Reset();}
	void Reset()
		{
		TFormatOptions::Reset();
		iBalanceSet = iGainSet = EFalse;
		}
public:	
	// recording options
	TInt iBalance;  // recording balance
	TInt iGain;	// gain

public:
	// recording option flags
	TBool iBalanceSet;
	TBool iGainSet;
	};

/** Application engine to play tones, files, & streams, and record to file.
  It inherits:
	 MMdaAudioPlayerCallback  to handles call backs from audio player;
	 MMdaAudioToneObserver  to handles call backs from tone player;
	 MMdaObjectStateChangeObserver  to handles call backs from recorder;
	 MMdaAudioOutputStreamCallback  to handles call backs from stream player.*/
class CMediaClientEngine : public CBase, 
	public MMdaAudioPlayerCallback,	
	public MMdaAudioToneObserver,	
	public MMdaObjectStateChangeObserver, 
	public MMdaAudioOutputStreamCallback
    {
public:
	/** Volume levels */
	enum TExVolume
		{
		/** Loud volume */
		ELoud=1,
		/** Medium volume  */
		EMedium,
		/** Low volume */
		EQuiet
		};
	
	/** Gain levels */
	enum TExRecordGain
		{
		/** High gain */
		EGainMax,
		/** Medium gain */
		EGainMedium,
		/** Low gain */
		EGainLow
		};
		
	/** Engine status */
	enum TState
        {
        /** No operation */
		EIdle,
		/** Preparing to play a tone */
		ETonePrepare,
        /** Playing a tone */
		ETonePlaying,
		/** Preparing to play a file*/
		EPlayFilePrepare,
        /** Playing a file */
		EPlayFilePlaying,
		/** Preparing to record */
		ERecordPrepare,
		/** Recording */
		ERecording,
		/** Getting meta data */
		EGetMetaData1,
		/** Retrieved meta data */
		EGetMetaData2,
		/** Converting file */
		EConvert,
		/** Completed conversion */
		EConvertComplete,
		/** Preparing to play a stream */
		EStreamPrepare,
		/** Stream initiated */
		EStreamStarted,
		/** Requires new stream data */
		ENextStreamBuf,
		/** Error in playing stream */
		EStreamError,
		/** Stream closing down */
		EStreamStopping
		};

public:
	/** Constructor.
	@param File server session */
	CMediaClientEngine(RFs& aFs);
	/** Destructor.*/
	~CMediaClientEngine();
	/** Set the engine observer.
	@param aObserver The engine observer */
	void SetObserver(MExEngineObserver& aObserver);
	/** Plays a sound file.
	@param aFile File to play
	@panic MEDIA_AUDIO_EX 0 Engine is already playing a sound */
	void PlayL(const TDesC &aFile);
	/** Plays a sound tone.
	@param aFrequency Frequency in Hz 
	@param aDuration Duration in ms
	@panic MEDIA_AUDIO_EX 0 Engine is already playing a sound */
    void PlayL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);
	/** Plays a stream.
	@panic MEDIA_AUDIO_EX 0 Engine is already playing a sound */
	void PlayL();
	/** Writes data to the stream.
	@param aData Data to write */
	void WriteToStreamL(const TDesC8& aData);
	/** Gets a recorder for specified target type */
	CMdaAudioRecorderUtility* NewRecorderL(const TDesC& aTargetType);
	/** Get possible record types */
	void RecordTypesL(CDesCArray& aTypeArray);
	/** Records sound to a file.
	@param aFile File to record to */
	void RecordL(const TDesC &aFile, const TRecordFormatOptions& aRecordOptions);
	/** Terminates any current operation. */
	void Stop();
	/** Gets meta data for a file */
	void GetMetaDataL(const TFileName& aFile, RMetaDataArray& aMetaData);
	/** Gets an array of possible conversion types for specified file */
	static void DestinationTypesL(const TFileName& aFile, CDesCArray& aExt);
	/** Gets a converter for specified file and target */
	CMdaAudioConvertUtility* NewConverterL(const TFileName& aFile, const TDesC& aTargetType, 
		TFileName& aTargetFile);
	/** Does conversion for specified file and target */
	void ConvertL(const TFileName& aFile, const TDesC& aTargetType, const TFormatOptions& aOptions);
	/** Gets the volume. */
	TExVolume Volume() const;
	/** Sets the volume. 
	@param aVolume The new volume */
	void SetVolume(TExVolume aVolume);

	/** Gets the engine state 
	@return The engine state*/
	TState Status() const;

private:
	/** Engine panic codes */
	enum TPanics
		{
		/** Sound is already playing */
		EInProgress,
		/** No observer set */
		ENullObserver,
		/** Sound preparation is not complete. */
		ENotReady,
		/** No tone player constructed */
		ENullTonePlayer,
		/** Engine is in the wrong state */
		EWrongState,
		/** No file player constructed  */
		ENullPlayerUtility
		};

private:

	/** File is ready to play.
	@param aError Any error in setup
	@param aDuration The duration of the audio sample */
    void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);

	/** File has completed playing.
	@param aError Any error in playing */
    void MapcPlayComplete(TInt aError);
	/** Tone is ready to play.
	@param aError Any error in setup */
	void MatoPrepareComplete(TInt aError);
	/** Tone has completed playing.
	@param aError Any error in playing */
	void MatoPlayComplete(TInt aError);

	// from MMdaObjectStateChangeObserver
	/** Change in sound recorder state.
	@param aObject Recorder object
	@param aPreviousState Last state
	@param aCurrentState Current state
	@param aErrorCode Any error that has occurred */
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, 
		TInt aCurrentState, TInt aErrorCode);

	/** Stream has completed playing.
	@param aError Any error in playing */
	void MaoscPlayComplete(TInt aError);
	/** Stream has completed use of specified data slice.
	@param aError Any error in handling the slice */
	void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	/** Stream is ready to play.
	@param aError Any error in setup */
	void MaoscOpenComplete(TInt aError);

	/** Conversion stage 2*/
	void Convert2L();
	/** Recording stage 2*/
	void Record2L();

	/** Clean up after tone playing */
	void ToneCleanup();
	/** Clean up after file playing */
	void PlayCleanup();
	/** Clean up after recording */
	void RecordCleanup();
	/** Clean up after stream playing */
	void StreamCleanup();
	/** Clean up after convert */
	void ConvertCleanup();

	/** Panic engine 
	@param aPanic Panic code */
	void Panic(TPanics aPanic);

private:
	/** File to play */
	TFileName iFileName;
	/** Target conversion file */
	TFileName iTargetFile;
	/** File server session */
	RFs& iFs;
    /** Engine state */
	TState iState;
	/** Sound volume */
	TExVolume iVolume;
	/** Sound gain */
	TExRecordGain iGain;
	/** Engine observer */
	MExEngineObserver* iEngineObserver;
		
	/** Tone player */
	CMdaAudioToneUtility* iAudioToneUtility;
	/** File player */
    CMdaAudioPlayerUtility* iAudioPlayUtility;
	/** File recorder */
	CMdaAudioRecorderUtility* iAudioRecorderUtility;
	/** Stream player */
	CMdaAudioOutputStream* iAudioStreamPlayer;
	/** Converter */
	CMdaAudioConvertUtility* iConvertUtility;

	/** Additional sound settings */
	TMdaAudioDataSettings iSettings;
	/** Meta data array */
	RMetaDataArray* iMetaData;
	/** Conversion format options */
	const TFormatOptions* iOptions;
	/** Record format options */
	const TRecordFormatOptions* iRecordFormatOptions;
    };

/** Engine observer for the UI to implement */
class MExEngineObserver
	{
public:
/** Reports an engine event.
@param aState New engine state
@param aError Error condition */
	virtual void HandleEngineState(CMediaClientEngine::TState aState, TInt aError) = 0;
	};

/** Engine helper to create recorder object in a seeming synchronous way */
class CRecorderCreator: public CActive, public MMdaObjectStateChangeObserver
	{
public:
	static CMdaAudioRecorderUtility* NewRecorderL(const TDesC& aTargetType);
	
private:
	CRecorderCreator();
	CMdaAudioRecorderUtility* ConstructL(const TDesC& aTargetType);
	void RunL();
	void DoCancel();
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	CMdaAudioRecorderUtility* iRecorderUtility;
	};


/** Engine helper to create converter object in a seeming synchronous way */
class CConverterCreator: public CActive, public MMdaObjectStateChangeObserver
	{
public:
	static CMdaAudioConvertUtility* NewConverterL(const TFileName& aFile, const TDesC& aTargetType,
		TFileName& aTargetFile);
	
private:
	CConverterCreator();
	CMdaAudioConvertUtility* ConstructL(const TFileName& aFile, const TDesC& aTargetType,
		TFileName& aTargetFile);
	void RunL();
	void DoCancel();
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	CMdaAudioConvertUtility* iConvertUtility;
	};


#endif // CMEDIACLIENTENGINE
