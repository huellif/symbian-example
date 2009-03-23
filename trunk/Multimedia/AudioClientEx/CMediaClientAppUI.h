// CMediaClientAppUi.h
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
//

#ifndef CMEDIACLIENTAPPUI
#define CMEDIACLIENTAPPUI

#include <eikappui.h>
#include <eikmsg.h>
#include <badesca.h>

#include "CMediaClientEngine.h"

class CMediaClientView;
class CAudioStreamSource;
class CAudioStreamSourceWithQueue;

/** Handles user commands to drive the engine.
  It implements the <code>MExEngineObserver</code> interface to get
  events back from the engine. */
class CMediaClientAppUi : public CEikAppUi, public MExEngineObserver
    {
public:
	/** Performs second-phase construction.*/
    void ConstructL();
	/** Destructor. */
    ~CMediaClientAppUi();
	/** Exits the application. 
	This is a static callback function, which is called back asynchronously
	to allow the application to exit after any media server active objects
	have completed running.
	@return	Unused error code
	@param aUI Pointer to the app UI object itself */
	static TInt DoExit(TAny* aUI);

private:
	/** Handles user commands.
	@param aCommand */
    void HandleCommandL(TInt aCommand);
	/** Dynamically modifies menu pane so that only appropriate commands
	are available.
	@param aResourceId ID of the menu pane
	@param aMenuPane Pointer to the menu pane */
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
	/** Gets the application engine.
	@return The application engine */
	CMediaClientEngine& MediaClientEngine() const;
	/** Plays a tone. */
	void ToneDialogL();
	/** Plays a file. */
	void PlayFileDialogL();
	/** Records a file. */
	void RecordFileDialogL();
	/** Requests file metadata */
	void FileMetaDataL();
	/** Displays retrieved file metadata */
	void DisplayMetaDataL();
	/** Converts a file to another format */
	void ConvertFileDialogL();

	// From MExEngineObserver
	/** Handles engine events.
	@param aState The current engine state
	@param aError The error code produced by the last engine operation */
	void HandleEngineState(CMediaClientEngine::TState aState, TInt aError);

	/** Cleans up <code>iAudioStreamSource</code> and <code>iAudioStreamSourceWithQueue</code>. */
	void StreamSourceCleanUp();

private:
    /** The application view */
	CMediaClientView* iAppView;
	/** File name buffer */
	TFileName iFileName;
	/** Synchronous stream data source */
	CAudioStreamSource* iAudioStreamSource;
	/** Aynchronous stream data source */
	CAudioStreamSourceWithQueue* iAudioStreamSourceWithQueue;
	/** Stores meta data about a file */
	RMetaDataArray iMetaData;
	/** The engine state */
	CMediaClientEngine::TState iState;
	/** The callback to exit function */
	CAsyncCallBack* iExitCallBack;
	/** Conversion format options */
	TFormatOptions iFormatOptions;
	/** Record format options */
	TRecordFormatOptions iRecordFormatOptions;
	/** Conversion format type */
	TBuf<5> iTargetType;
    };


#include <eikdialg.h>

/** A dialog to allow the user to the set a tone to play. */
class CToneDialog: public CEikDialog
	{
public:
/** Runs the dialog to get the frequency and duration of the tone to play.
@return True if the dialog was OK'ed, false if cancelled
@param aFrequency On return, frequency in Hz 
@param aDuration On return, duration in ms */
	static TBool RunDlgLD(TInt& aFrequency, TTimeIntervalMicroSeconds& aDuration);

private:
/** Constructor.
@param aFrequency Frequency
@param aDuration Duration */
	CToneDialog(TInt& aFrequency, TTimeIntervalMicroSeconds& aDuration);
/** Called when user pressses a button. From CEikDialog.
@return True if can now leave, false to retain the dialog
@param aKeycode Button presssed */
	TBool OkToExitL(TInt aKeycode);

private:
	/** Frequency */
	TInt& iFrequency;
	/** Duration */
	TTimeIntervalMicroSeconds& iDuration;
	};


/** A dialog to allow the user to set how to record a sound file */
class CRecordDialog: public CEikDialog
	{
public:
/** Runs the dialog to get the target file name, format, and options
@return True if the dialog was OK'ed, false if cancelled */
	static TBool RunDlgLD(
		TFileName& aFileName, CMediaClientEngine& aMediaClientEngine,
		TRecordFormatOptions& aFormatOptions);
	~CRecordDialog();

private:
	// construct/destruct
	CRecordDialog(TFileName& aFileName, CMediaClientEngine& aMediaClientEngine,
		TRecordFormatOptions& aFormatOptions);
	// from CEikDialog
	TBool OkToExitL(TInt aKeycode);
	void PreLayoutDynInitL();
	void HandleControlStateChangeL(TInt aControlId);

private:
	TFileName& iFileName;
	CMediaClientEngine& iMediaClientEngine;
	// Format options
	TRecordFormatOptions& iRecordFormatOptions;
	// Flag for options set
	TInt iOptionsSetFor;
	};


/** A dialog to allow the user to set how to convert a file */
class CConverterDialog: public CEikDialog
	{
public:
/** Runs the dialog to get the target file name, format, and options
@return True if the dialog was OK'ed, false if cancelled */
	static TBool RunDlgLD(
		// input parameters
		const TFileName& aFileName, CMediaClientEngine& aMediaClientEngine,
		TFormatOptions& aFormatOptions, TDes& aTargetType);
	~CConverterDialog();

private:
	// construct/destruct
	CConverterDialog(const TFileName& aFileName, CMediaClientEngine& aMediaClientEngine,
		TFormatOptions& aFormatOptions, TDes& aTargetType);
	// from CEikDialog
	TBool OkToExitL(TInt aKeycode);
	void PreLayoutDynInitL();
	void HandleControlStateChangeL(TInt aControlId);

private:
	// Input params
	const TFileName& iFileName;
	CMediaClientEngine& iMediaClientEngine;
	// Format options
	TFormatOptions& iFormatOptions;
	// Format type
	TDes& iTargetType;
	// Flag for options set
	TInt iOptionsSetFor;
	};


// Format options dialog
class CFormatOptionsDialog: public CEikDialog
	{
public:
	static TBool RunDlgLD(const TFileName& aInputFile, const TDesC& aTargetType, 
		CMediaClientEngine& aMediaClientEngine, TFormatOptions& aFormatOptions);
	~CFormatOptionsDialog();

protected:
	// construct/destruct
	CFormatOptionsDialog(const TFileName& aInputFile, const TDesC& aTargetType, 
		CMediaClientEngine& aMediaClientEngine, TFormatOptions& aFormatOptions);
	void ConstructL();
	void TUintChoiceList(TInt aResourceId, const RArray<TUint>& aArray);
	// from CEikDialog
	TBool OkToExitL(TInt aKeycode);
	void PreLayoutDynInitL();

protected:
	// Input params
	const TFileName& iFileName;
	const TDesC& iTargetType;
	CMediaClientEngine& iMediaClientEngine;
	// Output
	TFormatOptions& iFormatOptions;

	// Cached info
	RArray<TFourCC> iDataTypes;
	RArray<TUint> iBitRates;
	RArray<TUint> iSampleRates;
	RArray<TUint> iChannels;
	};

// Recording format options dialog
class CRecordFormatOptionsDialog: public CFormatOptionsDialog
	{
public:
	static TBool RunDlgLD(const TDesC& aTargetType, 
		CMediaClientEngine& aMediaClientEngine, TRecordFormatOptions& aFormatOptions);
	~CRecordFormatOptionsDialog();

private:
	// construct/destruct
	CRecordFormatOptionsDialog(const TDesC& aTargetType, 
		CMediaClientEngine& aMediaClientEngine, TRecordFormatOptions& aFormatOptions);
	void ConstructL();
	// from CEikDialog
	TBool OkToExitL(TInt aKeycode);
	void PreLayoutDynInitL();

private:
	// Cached info
	TInt iMaxGain;
	// Output
	TRecordFormatOptions& iRecordFormatOptions;
	// 
	TBuf<1> iBuf;
	};


/** Size of each slice read from the file during streaming */
const TInt KSliceSize = 4096;
/** Buffer to  */
typedef TBuf8<KSliceSize> TSoundSlice;

/** Provides an effectively synchronous artificial audio stream source.
  It gets data in slices from a file as soon as the stream player
  is ready for them. */
class CAudioStreamSource: public CActive
	{
public:

/** Allocates and constructs a new synchronous artificial audio stream source.
@cat Construction and destruction
@return New synchronous artificial audio stream source object
@param aFs File server session
@param aEngine Application engine */
	static CAudioStreamSource* NewL(RFs& aFs,CMediaClientEngine& aEngine);

/** Asynchronously feeds the next slice to the stream player. */
	void Next();

/** Destructor.
@cat Construction and destruction */
	~CAudioStreamSource();

private:
	/** Second phase constructor.
@cat Construction and destruction */
	void ConstructL();

/** Constructor.
@cat Construction and destruction
@param aFs File server session
@param aEngine Application engine */
	CAudioStreamSource(RFs& aFs, CMediaClientEngine& aEngine);

/** Gets the next slice of data and feeds it to the stream player.
@cat Active object protocol */
	void RunL();
/** Cancels the operation.
@cat Active object protocol */
	void DoCancel();

private:
	/** The file from which to read the stream data */
	RFile iSourceFile;
	/** File server session */
	RFs& iFs;
	/** Application engine */
	CMediaClientEngine& iEngine;
	/** Data slice to feed to stream player */
	TSoundSlice iBuf;
	};

/**
A sound slice read in from the file.

  These objects are held in a queue. They are removed from the queue in the callback 
  function MaoscBufferCopied().
*/
class CSoundSlice: public CBase
	{
public:

/**
Allocates and constructs a new synchronous artificial audio stream source.
@cat Construction and destruction
@return New synchronous artificial audio stream source object
@param aData Data for the slice
*/
	static CSoundSlice* NewL(const TDesC8& aData);
/**
Destructor.
*/
	~CSoundSlice();
	const TDesC8& GetData();
public:
    static const TInt iOffset; 
private:
/**
Constructor.
@cat Construction and destruction
*/
	CSoundSlice();
/**
Second phase constructor.
@param aData Data for the slice
*/
	void ConstructL(const TDesC8& aData);
private:
	/** Data for the slice */
	HBufC8* iData;
	/** Allows objects to be part of a linked list of slices */
	TSglQueLink iLink;
	};


/** Provides an artificial audio stream source.

Gets data in slices from a file and stores them in a queue
until they are safe to be deleted.
A timer is used to indicate when to read the next slice.*/
class CAudioStreamSourceWithQueue: public CTimer
	{
public:
/**
Allocates and constructs a new asynchronous artificial audio stream source.
@return New asynchronous artificial audio stream source object
@param aFs File server session
@param aEngine Application engine
*/	static CAudioStreamSourceWithQueue* NewL(RFs& aFs,CMediaClientEngine& aEngine);
/**
Destructor.
*/
	~CAudioStreamSourceWithQueue();
/**
Asynchronously feeds the next slice to the stream player.
*/
	void Next();

/**
Removes a used slice from the queue.

*/
	void RemoveFromQueue();

private:
/**
Constructor.
@param aFs File server session
@param aEngine Application engine
*/
	CAudioStreamSourceWithQueue(RFs& aFs, CMediaClientEngine& aEngine);
/**
Second phase constructor.
*/
	void ConstructL();
/**
Gets the next slice of data and feeds it to the stream player.
*/
	void RunL();
/**
Cancels the operation.
*/
	void DoCancel();

private:
	/** The file from which to read the stream data */
	RFile iSourceFile;
	/** File server session */
	RFs& iFs;
	/** Application engine */
	CMediaClientEngine& iEngine;
	/** Data slice to feed to stream player */
	TSoundSlice iBuf;
	/** Header for the linked list of slices */
	TSglQue<CSoundSlice> iQueue;
	/** List iterator - needed to delete all the items */
	TSglQueIter<CSoundSlice> iQueueIter;
	};


#endif // CMEDIACLIENTAPPUI
