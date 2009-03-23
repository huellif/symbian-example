// TXUT.H
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#if !defined (__TXUT_H__)
#define __TXUT_H__

#if !defined(__BADESCA_H__)
#include <badesca.h>
#endif
#if !defined(__MSVAPI_H__)
#include <msvapi.h>
#endif



// TextMTM UIDs
LOCAL_D const TInt KUidMsgTypeTextValue = 0x10005247;
LOCAL_D const TUid KUidMsgTypeText = { KUidMsgTypeTextValue};
LOCAL_D const TUid theTechnologyTypeUid	= { KUidMsgTypeTextValue};

LOCAL_D const TUid KUidMsvMtmTextServerDLL	={0xE800005F};
LOCAL_D const TUid KUidMsvMtmTextClientDLL	={0xE800005D};
LOCAL_D const TUid KUidMsvMtmTextUiDLL		={0xE8000060};
LOCAL_D const TUid KUidMsvMtmTextUiDataDLL	={0x10003C61};
LOCAL_D const TUid KUidTextMtmVersion1		={ KUidMsgTypeTextValue};

// Constants
LOCAL_D const TInt KUidTxtMTMSettingsValue = 0x1000525A;
LOCAL_D const TUid KUidTxtMTMSettings={KUidTxtMTMSettingsValue};
LOCAL_D const TInt KMaxTextMessageSize = 0xFFFF;


//
//	TTxtProgress: Operation progress information
//

class TTxtProgress
	{
public:
	TInt		iTotalMsgs;
	TInt		iMsgsProcessed;
	TInt		iErrorCode; // client side needs to be informed of an error
	TMsvId		iNewId;	// Used with CreateL, or any copy/move, storing the last moved message id.
	};

typedef TPckgBuf<TTxtProgress> TTxtProgressBuf;

//
// TxtUtils: Generic static utility functions
//

class TxtUtils
	{
public:
	// Build file name for associated entry, given its entry.
	IMPORT_C static void GetEntryFileNameL(TFileName& aFileName, TMsvEntry& aEntry);
	};

//
// TMTMTxtSettings: contains the settings for a service.
// In pre 9.0 releases, settings are stored in the CMsvStore
// associated with the service entry.
//
// From 9.0, the CMTMTxtSettings class should be used to
// store/load settings into the central repository.

class CMsvStore;
class TMTMTxtSettings
	{
public:
	TFileName RootFolder() const;
	void SetRootFolder(const TDesC& aRootFolder);
private:
	TFileName iRootFolder;
	};

inline void TMTMTxtSettings::SetRootFolder(const TDesC& aRootFolder)
	{
	iRootFolder = aRootFolder;
	}

inline TFileName TMTMTxtSettings::RootFolder() const
	{
	return iRootFolder;
	}

class CRepository;

/** From 9.0, this class is used to
 store/load settings into the central repository.
 It also saves/loads a setting to identify a default textmtm service.
 */
class CMTMTxtSettings: public CBase
	{
public:
	IMPORT_C static CMTMTxtSettings* NewL();
	~CMTMTxtSettings();
	// load, save, and delete text mtm settings
	IMPORT_C void LoadSettingsL(TMsvId aServiceId, TMTMTxtSettings& aSettings) const;
	IMPORT_C void SaveSettingsL(TMsvId aServiceId, const TMTMTxtSettings& aSettings);
	IMPORT_C void DeleteSettingsL(TMsvId aServiceId);
	// handle a setting that specifies what the default service is
	IMPORT_C TMsvId DefaultServiceL() const;
	IMPORT_C void SetDefaultServiceL(TMsvId aService);
	IMPORT_C void DeleteDefaultServiceSettingL();

private:
	void ConstructL();
	void CreateOrSetL(TUint aKey, const TDesC& aValue);
	void CreateOrSetL(TUint aKey, TInt aValue);
	TUint32 FindAccountL(TMsvId aService) const;
	TUint GetNextAccountSlotL();
	void DeleteSettingL(TUint32 settingId);

private:
	CRepository* iRepository;
	};

#endif
