// TXTI.CPP
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

// Standard includes
#include <coemain.h>	// CCoeEnv

// Messaging includes
#include <mtclbase.h> 
#include <msvstd.hrh>
#include <mtmdef.hrh>
#include <msvuids.h>
#include <msvids.h>

// Specific includes
#include "txut.h"
#include "txtucmds.h"
#include "txclient.h"
#include "txti.h"
#include "txtipan.h"
#include <txti.rsg>
#include <txti.mbg>
#include <MTMUIDEF.HRH>

// Constants
const TInt KTxtiMtmUdZoomNumberOfStates = 1;

#ifdef __WINS__
// on wins, assume built to z:
_LIT(KTxtiMtmUdResourceFile,"TXTI.RSC");
_LIT(KTxtiMtmUdBitmapFile,"TXTI.MBM");
#else
_LIT(KTxtiMtmUdResourceFile,"c:\\resource\\messaging\\TXTI.RSC");
_LIT(KTxtiMtmUdBitmapFile,"c:\\resource\\messaging\\TXTI.MBM");
#endif //WINS

_LIT(KTxtiReadyToSend,"Ready To Send");
//
//	TTxtiBitmapIndexes: index to bitmap sets
//	3 sets for messages (at different priority levels), and 1 for a service
//
enum TTxtiBitmapIndexes
	{
	ETxtiLowPriority=0,
	ETxtiNormalPriority,
	ETxtiHighPriority,
	ETxtiService,
	ETxtiFolder
	};

// This constant is required for TechView. It's defined in MTMExtendedCapabilities.hrh
// but we define it ourselves to keep this code generic, and not depend on TechView-specific headers
#define KUidMtmQueryCanCreateNewMsgValue 0x10008b24

//
// CTxtiMtmUiData: UI Data MTM
//

//
//	Construction, initialisation, and destruction 
//

CTxtiMtmUiData* CTxtiMtmUiData::NewL(CRegisteredMtmDll& aRegisteredDll)
	{
	CTxtiMtmUiData* base=new(ELeave) CTxtiMtmUiData(aRegisteredDll);
	CleanupStack::PushL(base);
	base->ConstructL();
	CleanupStack::Pop();
	return base;
	}

CTxtiMtmUiData::~CTxtiMtmUiData()
	{ 
	}

void CTxtiMtmUiData::PopulateArraysL()
// Initialise bitmaps and function information 
	{
	// Read MTM-specific operation information 
	ReadFunctionsFromResourceFileL(R_TEXTUD_FUNCTION_ARRAY);

	// Populate bitmap array
	CreateBitmapsL(KTxtiMtmUdZoomNumberOfStates, KTxtiMtmUdBitmapFile, EMbmTxtiTextl1, EMbmTxtiFold1);
	}

CTxtiMtmUiData::CTxtiMtmUiData(CRegisteredMtmDll& aRegisteredDll)
	:	CBaseMtmUiData(aRegisteredDll)
	{}


void CTxtiMtmUiData::GetResourceFileName(TFileName& aFileName) const
// Resource file loading
	{ 
	aFileName=KTxtiMtmUdResourceFile; 
	}


//
//	MTM-specific functionality 
//

TInt CTxtiMtmUiData::OperationSupportedL(TInt aOperationId, const TMsvEntry& aContext) const
// Context-sensitive operation query 
	{
	TInt aReasonResourceId=0;	// 0 means "operation is available"
	
	if (aContext.iMtm!=KUidMsgTypeText) return R_TEXTUD_NOT_SUPPORTED;

	const TBool isMessage=aContext.iType==KUidMsvMessageEntry;
	const TBool isService=aContext.iType==KUidMsvServiceEntry;
	
	if (aOperationId==ETxtuCommandRefreshMBox)
		{
		// Only allow refresh on services
		if (!isService)
    		aReasonResourceId=R_TEXTUD_ONLY_REFRESH_SERVICES;
		}
	else if (aOperationId==ETxtuCommandExportText)
		{
		// Only allow export on local messages
		if ( (!isMessage) || (aContext.iServiceId != KMsvLocalServiceIndexEntryId) )
    		aReasonResourceId=R_TEXTUD_ONLY_MESSAGES;
		}
	return aReasonResourceId;
	}


TInt CTxtiMtmUiData::QueryCapability(TUid aCapability, TInt& aResponse) const
// Query for capability 
	{
	switch (aCapability.iUid)
		{
	// Supported valued capabilities
	case KUidMtmQueryMaxBodySizeValue:
		aResponse=KMaxTextMessageSize;
		break;
	case KUidMtmQueryMaxTotalMsgSizeValue:
		aResponse=KMaxTextMessageSize;
		break;

	// Supported non-valued capabilities
	// boolean returns
	case KUidMtmQuerySupportedBodyValue:	
	case KUidMtmQueryCanSendMsgValue:	
	case KUidMtmQueryOffLineAllowedValue:
	case KUidMtmQueryCanReceiveMsgValue:
	case KUidMtmQueryCanCreateNewMsgValue:
		aResponse=ETrue;
		break;

	// Non-supported capabilities
	// Boolean returns
	case KUidMtmQuerySupportAttachmentsValue:
		aResponse=EFalse;
		break;
	default:
		return KErrNotSupported;
		};
	return KErrNone;
	}

const CBaseMtmUiData::CBitmapArray& CTxtiMtmUiData::ContextIcon(const TMsvEntry& aContext, TInt /*aStateFlags*/) const
// Get context-specific icon
// Text UI Data MTM has 3 sets of message icons relating to the priority of the message
	{
	__ASSERT_ALWAYS(aContext.iMtm==KUidMsgTypeText, Panic(ETxtiMtmUdWrongMtm));
	__ASSERT_ALWAYS(aContext.iType!=KUidMsvAttachmentEntry, Panic(ETxtiMtmUdNoIconForAttachment));

	TInt retIndex=ETxtiLowPriority;
	if (aContext.iType == KUidMsvServiceEntry)
		retIndex=ETxtiService;
	else if (aContext.iType == KUidMsvFolderEntry)
		retIndex=ETxtiFolder;
	else if (aContext.Priority()==EMsvMediumPriority)
		retIndex=ETxtiNormalPriority;
	else if (aContext.Priority()==EMsvHighPriority)
		retIndex=ETxtiHighPriority;
	return *iIconArrays->At(retIndex);
	}

//
// Context-specific information
//

TBool CTxtiMtmUiData::CanOpenEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
	{
	__ASSERT_ALWAYS(aContext.iMtm==KUidMsgTypeText, Panic(ETxtiMtmUdWrongMtm));
	__ASSERT_ALWAYS(aContext.iType!=KUidMsvAttachmentEntry, Panic(ETxtiMtmUdAttachmentsNotSupported));

	aReasonResourceId=0;
	if ( aContext.iType != KUidMsvMessageEntry )
		{
		aReasonResourceId=R_TEXTUD_ONLY_MESSAGES;
		return KErrNotSupported;
		}
	else
		return KErrNone;
	}

TBool CTxtiMtmUiData::CanCloseEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
	{
	__ASSERT_ALWAYS(aContext.iMtm==KUidMsgTypeText, Panic(ETxtiMtmUdWrongMtm));
	__ASSERT_ALWAYS(aContext.iType!=KUidMsvAttachmentEntry, Panic(ETxtiMtmUdAttachmentsNotSupported));

	aReasonResourceId=0;
	if ( aContext.iType != KUidMsvMessageEntry )
		{
		aReasonResourceId=R_TEXTUD_ONLY_MESSAGES;
		return KErrNotSupported;
		}
	else
		return KErrNone;
	}

TBool CTxtiMtmUiData::CanViewEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
	{
	__ASSERT_ALWAYS(aContext.iMtm==KUidMsgTypeText, Panic(ETxtiMtmUdWrongMtm));
	__ASSERT_ALWAYS(aContext.iType!=KUidMsvAttachmentEntry, Panic(ETxtiMtmUdAttachmentsNotSupported));

	aReasonResourceId=0;
	if ( aContext.iType != KUidMsvMessageEntry )
		{
		aReasonResourceId=R_TEXTUD_ONLY_MESSAGES;
		return KErrNotSupported;
		}
	else
		return KErrNone;
	}

TBool CTxtiMtmUiData::CanEditEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
	{
	__ASSERT_ALWAYS(aContext.iMtm==KUidMsgTypeText, Panic(ETxtiMtmUdWrongMtm));
	__ASSERT_ALWAYS(aContext.iType!=KUidMsvAttachmentEntry, Panic(ETxtiMtmUdAttachmentsNotSupported));

	aReasonResourceId=0;
	if ( aContext.iType == KUidMsvFolderEntry )
		{
		aReasonResourceId=R_TEXTUD_CAN_NOT_EDIT_FOLDERS;
		return KErrNotSupported;
		}
	else
		return KErrNone;
	}


TBool CTxtiMtmUiData::CanDeleteFromEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
	{
	__ASSERT_ALWAYS(aContext.iMtm==KUidMsgTypeText, Panic(ETxtiMtmUdWrongMtm));
	__ASSERT_ALWAYS(aContext.iType!=KUidMsvAttachmentEntry, Panic(ETxtiMtmUdAttachmentsNotSupported));
	
	aReasonResourceId=0;
	return KErrNone;
	}

TBool CTxtiMtmUiData::CanCopyMoveToEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
	{
	__ASSERT_ALWAYS(aContext.iMtm==KUidMsgTypeText, Panic(ETxtiMtmUdWrongMtm));
	__ASSERT_ALWAYS(aContext.iType!=KUidMsvAttachmentEntry, Panic(ETxtiMtmUdAttachmentsNotSupported));

	aReasonResourceId=0;
	return KErrNone;
	}

TBool CTxtiMtmUiData::CanCopyMoveFromEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
	{
	__ASSERT_ALWAYS(aContext.iMtm==KUidMsgTypeText, Panic(ETxtiMtmUdWrongMtm));
	__ASSERT_ALWAYS(aContext.iType!=KUidMsvAttachmentEntry, Panic(ETxtiMtmUdAttachmentsNotSupported));

	aReasonResourceId=0;
	return KErrNone;
	}

TBool CTxtiMtmUiData::CanReplyToEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
//
// MTM UI does not support replying.
//
    {
	__ASSERT_ALWAYS(aContext.iMtm==KUidMsgTypeText, Panic(ETxtiMtmUdWrongMtm));
	__ASSERT_ALWAYS(aContext.iType!=KUidMsvAttachmentEntry, Panic(ETxtiMtmUdAttachmentsNotSupported));
	
	aReasonResourceId=R_TEXTUD_NOT_SUPPORTED;
	return KErrNotSupported;
    }

TBool CTxtiMtmUiData::CanForwardEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
//
// MTM UI does not supports forwarding.
//
    {
	__ASSERT_ALWAYS(aContext.iMtm==KUidMsgTypeText, Panic(ETxtiMtmUdWrongMtm));
	__ASSERT_ALWAYS(aContext.iType!=KUidMsvAttachmentEntry, Panic(ETxtiMtmUdAttachmentsNotSupported));

	aReasonResourceId=R_TEXTUD_NOT_SUPPORTED;
	return KErrNotSupported;
    }


TBool CTxtiMtmUiData::CanCreateEntryL(const TMsvEntry& aParent, TMsvEntry& aNewEntry, 
									  TInt& aReasonResourceId) const
	{
	__ASSERT_ALWAYS(aNewEntry.iMtm==KUidMsgTypeText, Panic(ETxtiMtmUdWrongMtm));
	__ASSERT_ALWAYS(aNewEntry.iType!=KUidMsvAttachmentEntry, Panic(ETxtiMtmUdAttachmentsNotSupported));

	aReasonResourceId=0;
	// --- Can create services if they are off root ---
	if (aNewEntry.iType == KUidMsvServiceEntry)
		return (aParent.Id() == KMsvRootIndexEntryIdValue);

	// --- Can create messages in local folders ---
	if (aNewEntry.iType == KUidMsvMessageEntry)
		return (aParent.iMtm.iUid == KMsvLocalServiceIndexEntryIdValue);

	return KErrNotSupported;
	}

TBool CTxtiMtmUiData::CanDeleteServiceL(const TMsvEntry& aService, TInt& aReasonResourceId)const
	{
	__ASSERT_ALWAYS(aService.iMtm==KUidMsgTypeText, Panic(ETxtiMtmUdWrongMtm));

	aReasonResourceId=0;
	return KErrNone;
	}

TBool CTxtiMtmUiData::CanCancelL(const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
	{
	return EFalse;	
	}

HBufC* CTxtiMtmUiData::StatusTextL(const TMsvEntry& /*aContext*/) const
	{
	HBufC* statusBuffer = HBufC::NewL(15);
	*statusBuffer = KTxtiReadyToSend;
	return statusBuffer;
	}
