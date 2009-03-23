// TXCLIENT.H
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//


#if !defined (__TXCLIENT_H__)
#define __TXCLIENT_H__

#include <e32base.h>
#include <badesca.h>
#include <msvapi.h>
#include <mtclbase.h>

const TUint KAddresseeListResizeGranularity = 5;

#include "txut.h"

// 
// CTextMtmClient
//		Implements CBaseMtm to provide Client-side Text MTM		
//

class CTextMtmClient : public CBaseMtm
	{
public:
	//Construction and destruction 
	IMPORT_C static CTextMtmClient* NewL(CRegisteredMtmDll& aRegisteredMtmDll,CMsvSession& aMsvSession);
	~CTextMtmClient();

	// Store and restore entry data 
	void SaveMessageL(); 
	void LoadMessageL();

	// Response message creation 
	CMsvOperation* ReplyL  (TMsvId aReplyEntryId, TMsvPartList aPartlist, TRequestStatus& aCompletionStatus);
	CMsvOperation* ForwardL(TMsvId aForwardEntryId, TMsvPartList aPartList, TRequestStatus& aCompletionStatus);

	// Validation and search 
	TMsvPartList ValidateMessage(TUint aPartList);
	TMsvPartList Find(const TDesC& aTextToFind, TMsvPartList aPartList);

	// Addressees have no meaning in the text mtm.
	void AddAddresseeL(const TDesC& aRealAddress);
	void AddAddresseeL(const TDesC& aRealAddress, const TDesC& aAlias);
	void RemoveAddressee(TInt aIndex);

	// MTM-specific functionality
	TInt QueryCapability(TUid aCapability, TInt& aResponse);
	void InvokeSyncFunctionL(TInt aFunctionId,const CMsvEntrySelection& aSelection, TDes8& aParameter);
	CMsvOperation* InvokeAsyncFunctionL(TInt aFunctionId,const CMsvEntrySelection& aSelection, TDes8& aParameter, TRequestStatus& aCompletionStatus);

	void CreateAttachmentL(const TDesC& aFilePath, const TDesC8& aMimeType, TRequestStatus& aStatus);
	void CreateAttachmentL(RFile& aFile, const TDesC8& aMimeType, TRequestStatus& aStatus);
	void CreateLinkedAttachmentL(const TDesC& aFilePath, const TDesC8& aMimeType, TRequestStatus& aStatus);
	void CreateMessageAttachmentL(TMsvId aAttachmentId, TRequestStatus& aStatus);
	
	TMsvId DefaultServiceL() const;
	void RemoveDefaultServiceL();
	void ChangeDefaultServiceL(const TMsvId& aService);

	// Text-specific
	TMTMTxtSettings& TxtSettings();
	CMTMTxtSettings& Settings();

protected:
	CTextMtmClient(CRegisteredMtmDll& aRegisteredMtmDll,CMsvSession& aMsvSession);
	void ContextEntrySwitched(); 
	void ConstructL();

private: // Properties
	TMTMTxtSettings iTxtSettings;
	CMTMTxtSettings* iSettings;
	};

// Addresses are not used in this MTM
inline void CTextMtmClient::AddAddresseeL(const TDesC& /*aRealAddress*/) {};
inline void CTextMtmClient::AddAddresseeL(const TDesC& /*aRealAddress*/, const TDesC& /*aAlias*/) {};
inline void CTextMtmClient::RemoveAddressee(TInt /*aIndex*/) {};

inline TMTMTxtSettings& CTextMtmClient::TxtSettings()
	{
	return iTxtSettings;
	}

inline CMTMTxtSettings& CTextMtmClient::Settings()
	{
	return *iSettings;
	}

#endif // __TXCLIENT_H__
