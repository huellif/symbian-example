// mmfrectest.h
//

#ifndef __MMFRECTEST_H
#define __MMFRECTEST_H

void MainL();

/**
This class implements a state machine that records a short clip using each format that
the system supports. For each record format that the system supports, it will record
2s of audio to a file, then 2s of audio into a descriptor. It will then move onto the
next format.

It is necessary to implement this as a state machine as we get asynchronous callbacks
via MoscoStateChangeEvent which we have to wait for before we can continue to the next 
step.

For each record format, we go through these states in order:
1. EInitFile - while the file recording is initialised
2. (after the MoscoStateChangeEvent callback) ERecordFile - on a 2s timer while recording takes place
3. EDoOpenDesc - while recording into a descriptor is initialised
4. (after the MoscoStateChangeEvent callback) ERecordDesc - again on a 2s timer.

The logic implemented here is probably somewhat more complex that would be necessary
for any normal recording, but it demonstrates the principal of how to use the recording
API.
*/
class CMMFRecordTest : public CBase, public MMdaObjectStateChangeObserver
	{
public:
	static CMMFRecordTest* NewLC();
	virtual ~CMMFRecordTest();
	
	void Go();
	
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);	
private:
	enum TState
		{
		ENone,
		EDoOpenFile,
		EInitFile,
		ERecordFile,
		EDoOpenDesc,
		EInitDesc,
		ERecordDesc,
		ENext,
		EDone
		};
private:
	CMMFRecordTest();
	void ConstructL();
	void GetPluginInformationL();
	void Printf(TRefByValue<const TDesC16> aFmt, ...);
	static TInt Callback(TAny* self);
	void DoCallbackL();
	
	void NextState(TState aState);
	
	void InitializeFileL();
	void InitializeDesL();
	
	void DeleteFileL(const TDesC& aFileName);
	TBool GetNextFormatUid();
	void Next();
private:
	/** The current state of the object */
	TState iState;
	/** Used to generate asynchronous callbacks to move between states */
	CAsyncCallBack* iCallBack;
	/** Timer used while recording is in progress */
	CPeriodic* iTimer;
	/** Console used to output messages as the program runs */
	CConsoleBase* iConsole;
	/** The recorder utility which performs all recording */
	CMdaAudioRecorderUtility* iRecorder;
	
	TCallBack iCbFn;
	
	TInt                        iControllerIndex;
	TInt						iFormatIndex;
    TUid                        iControllerUid;
    TUid                        iDestinationFormatUid;
	RMMFControllerImplInfoArray iControllers;
	RFs							iFs;
	
	TInt iFileNum;
	TBuf<0x10> iExtension;
	TBuf<0x20> iFileName;
	
	TBuf<0x100> iFormattingBuf;
	TBuf8<0x1000> iRecBuf;
	};

class TTruncateOverflow : public TDes16Overflow
	{
	virtual void Overflow(TDes16& /*aDes*/) {};
	};
	
#endif //__MMFRECTEST_H