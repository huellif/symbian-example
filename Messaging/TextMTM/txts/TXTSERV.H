// TXTSERV.H
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#ifndef __TXTSERV_H__
#define __TXTSERV_H__

#include <mtsr.h>
#include <MSVSTORE.H>
#include <MSVUIDS.H>
#include <MSVIDS.H>

#include "TXTSPAN.H"
#include "txut.h"


//
//	ETxtOperations: The operations the mtm can be in (iCurrentOperation)
//
enum ETxtOperations 
	{
	ETxtOpIdle,
	ETxtOpCopyMoveDel
	};

class CTxtCopyMoveBase;
class CTxtActiveOper;

//
//	CTextServerMtm: The Server-side MTM implementation
//
class CTextServerMtm : public CBaseServerMtm
	{
public:
	// Constructor, initialisation, and destructor 
	IMPORT_C static CTextServerMtm* NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aInitialEntry);
	~CTextServerMtm();
	void SetInitialEntry(CMsvServerEntry* aEntry);

	// Copy and move 
	void CopyToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void CopyFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void CopyWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);

	// Create, change, delete 
	void DeleteAllL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	void CreateL(TMsvEntry aNewEntry, TRequestStatus& aStatus);
	void ChangeL(TMsvEntry aNewEntry, TRequestStatus& aStatus);

	// Command and progress
	void StartCommandL(CMsvEntrySelection& aSelection, TInt aCommand, 
		const TDesC8& aParameter, TRequestStatus& aStatus);
	TBool CommandExpected();
	const TDesC8& Progress();

protected:
	// Active object completion 
	void DoCancel();
	void DoRunL();
	void DoComplete(TInt aError);

private:
	CTextServerMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aInitialEntry);
	void ConstructL();
	void DoOperationL(CTxtActiveOper* aActiveOperation, const CMsvEntrySelection& aSelection,
		TMsvId aDestination, TRequestStatus& aStatus);
	void ConnectToServiceL();
	void DoRefreshL();

	// General asserts
	void AssertConnected();
	void AssertHasServerEntry();
	void AssertIdle();

private:
	TTxtProgressBuf iProgressBuf;
	TInt iCurrentOperation;
	CMsvServerEntry* iEntry;	
	TRequestStatus* iReportStatus;
	RFs	 iFs;
	TMsvId  iServiceEntryId;
	TMTMTxtSettings* iTxtSettings;

	CTxtCopyMoveBase* iTxtCopyMove; // Needed for copy, move, delete ops
	TParse iParse; // Needed for the move to remote operations, to keep track of the current folder.
	};

// Assert functions
inline void CTextServerMtm::AssertConnected()
	{
	__ASSERT_DEBUG(iServiceEntryId != KMsvNullIndexEntryId, gPanic(ETxtsInvalidService));
	__ASSERT_DEBUG(iTxtSettings != NULL, gPanic(ETxtsInvalidService));
	}

inline void CTextServerMtm::AssertHasServerEntry()
	{
	__ASSERT_DEBUG(iEntry != NULL, gPanic(ETxtsInvalidService));
	}

inline void CTextServerMtm::AssertIdle()
	{
	__ASSERT_DEBUG(iCurrentOperation==ETxtOpIdle,gPanic(ETxtsAlreadyBusy));
	}


#endif
