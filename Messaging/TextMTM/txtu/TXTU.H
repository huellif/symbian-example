// TXTU.H
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.


#if !defined(__TXTU_H__)
#define __TXTU_H__

#if !defined(__MTMUIBAS_H__)
#include <MTMUIBAS.H>
#endif

#if !defined(__EIKCFDLG_H__)
#include <eikcfdlg.H>//save as dialog
#endif
#if !defined(__TXUT_H__)
#include "txut.h"	 //service settings
#endif

//
//	CTextMtmExportToFileDialog: export to file dialog
//

class CTextMtmExportToFileDialog : public CEikFileSaveAsDialog
	{
public:
	CTextMtmExportToFileDialog(TDes* aFileName, const TDesC* aTitle);
	void PreLayoutDynInitL();
private:
	const TDesC& iTitleBuf;
	};


//
//	CTextMtmEditServiceDialog: service setup dialog
//

class CTextMtmEditServiceDialog : public CEikDialog
	{
public:
	CTextMtmEditServiceDialog(TMTMTxtSettings& aTxtSettings);
	void PreLayoutDynInitL();
private:
	TBool OkToExitL(TInt aButtonId);
private:
	TMTMTxtSettings& iTxtSettings;
	};

//
// CTextMtmUi: User Interface MTM
//

class CTextMtmUi : public CBaseMtmUi
	{
public:
	// Construction
	static CTextMtmUi* NewL(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll);

	// Destruction
	~CTextMtmUi();
	CMsvOperation* CancelL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);
	// Context-specific
	CMsvOperation* OpenL(TRequestStatus& aStatus); 
	CMsvOperation* OpenL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection); 

	CMsvOperation* CloseL(TRequestStatus& aStatus); 
	CMsvOperation* CloseL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);

	// Launches editor/settings dialog as appropriate  
	CMsvOperation* EditL(TRequestStatus& aStatus);
	CMsvOperation* EditL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection) ;

	// Launches viewer/settings dialog as appropriate  
	CMsvOperation* ViewL(TRequestStatus& aStatus);
	CMsvOperation* ViewL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);

	// Copy and move
	CMsvOperation* CopyToL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	CMsvOperation* MoveToL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	CMsvOperation* CopyFromL(const CMsvEntrySelection& aSelection, TMsvId aTargetId, 
		TRequestStatus& aStatus);
	CMsvOperation* MoveFromL(const CMsvEntrySelection& aSelection, TMsvId aTargetId, 
		TRequestStatus& aStatus);

	// Create/delete/undelete
	CMsvOperation* CreateL(const TMsvEntry& aEntry, CMsvEntry& aParent, TRequestStatus& aStatus);
	CMsvOperation* DeleteFromL(const CMsvEntrySelection& aSelection,TRequestStatus& aStatus);
	CMsvOperation* DeleteServiceL(const TMsvEntry& aService, TRequestStatus& aStatus);

	// MTM-specific
	void InvokeSyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TDes8& aParameter);
	CMsvOperation* InvokeAsyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TRequestStatus& aCompletionStatus, 
		TDes8& aParameter);

	TInt GetProgress(const TDesC8& aProgress, TBuf<EProgressStringMaxLen>& aReturnString, TInt& aTotalEntryCount, TInt& aEntriesDone,
		TInt& aCurrentEntrySize, TInt& aCurrentBytesTrans) const;

	CMsvOperation* ReplyL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus);
	CMsvOperation* ForwardL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus);


public:
	// TextMtm-specific
	CMsvOperation* LaunchEditorApplicationL(TRequestStatus& aStatus, TMsvId aId, TBool aReadOnly);

protected: 
	// from CBaseMtmUi
	void GetResourceFileName(TFileName& aFileName) const;

protected:
	CTextMtmUi(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll);
	void ConstructL();

private:
	CMsvOperation* DoCopyMoveToL(const CMsvEntrySelection& aSelection, 
		TRequestStatus& aStatus, TBool aAction);
	CMsvOperation* DoCopyMoveFromL(const CMsvEntrySelection& aSelection, 
		TMsvId aTargetId, TRequestStatus& aStatus, TBool aAction);
	void CheckValidCopyMoveDelSelectionL(const CMsvEntry& aParent,
										 const CMsvEntrySelection& aSelection);
	// Editing routines
	CMsvOperation* MessageEditL(TRequestStatus& aStatus);
	CMsvOperation* ServiceEditL(TRequestStatus& aStatus);
	void CreateBodyIfNoneL();

	void DoExportTextFromMessageL(TMsvId anId);

	CMsvEntry* GetMessageEntryLC(TMsvId anId) const;
	CRichText* GetTextFromMessageStoreL(CMsvStore* aStore) const;
	void SetProgressSuccess(TPckgBuf<TMsvLocalOperationProgress>& aProgress,TMsvId aId);

private:
	TMsvId iServiceId;
	};


//
//	CEditRemoteOperation: remote edit operation
//

class CEditRemoteOperation : public CMsvOperation
	{
	enum TEditRemoteState
		{
		EStateFinished=0,
		EStateEditing,
		EStateCopying,
		};
	
	public:
		CEditRemoteOperation(CTextMtmUi& aMtmUi, CMsvSession& aMsvSession, 
			TInt aPriority, TRequestStatus& aObserverRequestStatus);
		~CEditRemoteOperation();
	public:	// from CMsvOperation
		void Start(TMsvEntry& aSourceEntry);
		const TDesC8& ProgressL();
	public:	// from CActive
		void DoCancel();
		void RunL();
	private:
		CTextMtmUi& iMtmUi;
		TBuf8<10> progress;
		CMsvOperation* iCopyOperation;
		CMsvOperation* iEditOperation;
		TEditRemoteState iState;
	};

#endif // __TXTU_H__
