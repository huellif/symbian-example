// TXTI.H
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#if !defined(__TXTI_H__)
#define __TXTI_H__

#if !defined(__MSVSTD_H__)
#include <MSVSTD.H>
#endif

#if !defined(__MTUDBAS_H__)
#include <MTUDCBAS.H>
#endif

//
// CTxtiMtmUiData: UI Data MTM
//

class CTxtiMtmUiData : public CBaseMtmUiData
	{
public:
	// Construction, initialisation, and destruction 
	static CTxtiMtmUiData* NewL(CRegisteredMtmDll& aRegisteredDll);
	~CTxtiMtmUiData();
	void PopulateArraysL();
	void GetResourceFileName(TFileName& aFileName) const;
	
	// MTM-specific functionality 
	TInt OperationSupportedL(TInt aOperationId, const TMsvEntry& aContext) const;
	TInt QueryCapability(TUid aCapability, TInt& aResponse) const;

	// Context-specific information 
	TBool CanEditEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	TBool CanViewEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	TBool CanOpenEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	TBool CanCloseEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	TBool CanDeleteFromEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	TBool CanCopyMoveToEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	TBool CanCopyMoveFromEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	TBool CanReplyToEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	TBool CanForwardEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	TBool CanCreateEntryL(const TMsvEntry& aContext,TMsvEntry& aNewEntry, TInt& aReasonResourceId) const;
	TBool CanDeleteServiceL(const TMsvEntry& aService, TInt& aReasonResourceId) const;
	TBool CanCancelL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	
	HBufC* StatusTextL(const TMsvEntry& aContext) const;

// Context-specific icons 
	const CBitmapArray& ContextIcon(const TMsvEntry& aContext,TInt aStateFlags) const;
private:
	CTxtiMtmUiData(CRegisteredMtmDll& aRegisteredDll);
	};

#endif // __TXTI_H__
