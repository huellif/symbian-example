// TXUT.CPP
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#include <msvstd.h>		// TMsvEntry
#include <msvstore.h>	// CMsvStore
#include <MSVUIDS.H>	//KUidMsvFolderEntry
#include <centralrepository.h>

#include "txut.h"

const TUint32 KNullId = 0x00000000;
const TUint32 KIncrementAccount = 0x00100000;
const TInt KMaxAccount = 2048;
const TUint32 KDefaultServiceId = 0x80000000; // set top bit
const TUint32 EAccountMask = 0x800FFFFF;

// Key IDs for particular text mtm settings in repository
enum 
	{
	EServiceId = 0,
	EFolderSettingId = 1
	};

// standard NewL
EXPORT_C CMTMTxtSettings* CMTMTxtSettings::NewL()
	{
	CMTMTxtSettings* self = new (ELeave) CMTMTxtSettings();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;	
	}

// destructor	
CMTMTxtSettings::~CMTMTxtSettings()
	{
	delete iRepository;
	}

// delete all settings for specified service
EXPORT_C void CMTMTxtSettings::DeleteSettingsL(TMsvId aServiceId)
	{
	TUint32 serviceKey = FindAccountL(aServiceId);
	DeleteSettingL(serviceKey);
	DeleteSettingL(serviceKey + EFolderSettingId);
	}	

/**
Loads settings for textmtm service.

@param	aServiceId ID of service entry

@param	aSettings Service settings 
*/		
EXPORT_C void CMTMTxtSettings::LoadSettingsL(TMsvId aServiceId, TMTMTxtSettings& aSettings) const
	{
	TUint32 serviceKey = FindAccountL(aServiceId);
	TFileName rootFolder;
	User::LeaveIfError(iRepository->Get(serviceKey + EFolderSettingId, rootFolder));
	aSettings.SetRootFolder(rootFolder);
	}

/**
Stores settings for textmtm service.

@param	aServiceId ID of service entry

@param	aSettings Service settings 
*/	
EXPORT_C void CMTMTxtSettings::SaveSettingsL(TMsvId aServiceId, const TMTMTxtSettings& aSettings)
	{
	TUint32 accountId = 0;
	TInt error = 0;
	// see if account already exists
	TRAP(error, accountId = FindAccountL(aServiceId));
	if (error != KErrUnknown) User::LeaveIfError(error);
	// doesn't already exist, so get id of new account
	if (error == KErrUnknown) accountId = GetNextAccountSlotL();
	 	
	TRAP( error,
		// Save settings to CenRep		
		CreateOrSetL(accountId, static_cast<TInt>(aServiceId));
		CreateOrSetL(accountId + EFolderSettingId, aSettings.RootFolder());		
		);
	if (error != KErrNone)
		{
		// saving settings to CenRep failed, so cleanup account and leave
		DeleteSettingsL(aServiceId);
		User::Leave(error);
		}	
	}

/**
Sets the default textmtm service.

@param	aService The default service
*/	
EXPORT_C void CMTMTxtSettings::SetDefaultServiceL(TMsvId aService)
	{
	CreateOrSetL(KDefaultServiceId, static_cast<TInt>(aService));
	}

/**
Gets the default textmtm service.

@return The default service

@leave KErrNotFound If default service setting does not exist.
*/
EXPORT_C TMsvId CMTMTxtSettings::DefaultServiceL() const
	{
	// Get the service Id from CenRep
	TInt temp = 0;
	User::LeaveIfError(iRepository->Get(KDefaultServiceId, temp));				
	return static_cast<TMsvId>(temp);
	}

/**
Remove the default service setting.
*/	
EXPORT_C void CMTMTxtSettings::DeleteDefaultServiceSettingL()
	{
	DeleteSettingL(KDefaultServiceId);				
	}

// create cenrep repository in which to store textmtm settings 
void CMTMTxtSettings::ConstructL()
	{
	iRepository = CRepository::NewL(KUidMsgTypeText);
	}

// sets (or creates if it does not already exist) a string key
void CMTMTxtSettings::CreateOrSetL(TUint aKey, const TDesC& aValue)
	{
	TInt error = iRepository->Set(aKey, aValue);
	if (error == KErrNotFound)
		{
		// setting does not exist, so create it
		User::LeaveIfError(iRepository->Create(aKey, aValue));
		}
	else
		{
		User::LeaveIfError(error);
		}		
	}

// sets (or creates if it does not already exist) an integer key
void CMTMTxtSettings::CreateOrSetL(TUint aKey, TInt aValue)
	{
	TInt error = iRepository->Set(aKey, aValue);
	if (error == KErrNotFound)
		{
		// setting does not exist, so create it
		User::LeaveIfError(iRepository->Create(aKey, aValue));
		}
	else
		{
		User::LeaveIfError(error);
		}		
	}

// Leaves with KErrUnknown if account does not exist
TUint32 CMTMTxtSettings::FindAccountL(TMsvId aService) const
	{
	RArray<TUint32> accounts;
	CleanupClosePushL(accounts);	
	TInt error = iRepository->FindEqL(KNullId, static_cast<TUint32>(EAccountMask), static_cast<TInt>(aService), accounts);
	if (error == KErrNotFound)
		{
		// account does not exist
		User::Leave(KErrUnknown);		
		}
	else
		{
		User::LeaveIfError(error);	
		}
	
	if (accounts.Count()>1)
		{
		// There should be only one account for the service
		User::Leave(KErrOverflow);
		}
	
	TUint32 account = accounts[0];
	CleanupStack::PopAndDestroy(&accounts);
	return account;
	}

// get a base (account) id to identify all keys for a particular service
TUint CMTMTxtSettings::GetNextAccountSlotL()
	{
	TUint32 accountId = KNullId;
	TInt serviceId = 0;
	TInt error = 0;
	TBool found = EFalse;
	
	for (TInt count = 0; count < KMaxAccount; ++count)
		{		
		accountId = accountId + KIncrementAccount;
		error = iRepository->Get(accountId, serviceId);		
		if (error == KErrNotFound)
			{
			found = ETrue;				
			break;
			}
		else
			{
			User::LeaveIfError(error);
			}
		}

	if (found == EFalse)
		{
		// No empty slot available		
		User::Leave(KErrNotFound);
		}
	
	return accountId;
	}

// delete a setting, and don't give an error if it doesn't exist
void CMTMTxtSettings::DeleteSettingL(TUint32 settingId)
	{
	TInt error = iRepository->Delete(settingId);
	if (error != KErrNotFound)
		{
		User::LeaveIfError(error);
		}
	}
	
//
// TxtUtils: Generic static utility functions
// 

EXPORT_C void TxtUtils::GetEntryFileNameL(TFileName& aFileName, TMsvEntry& aEntry) 
// Create absolute file name: default path + aEntry.iDetails + aEntry.iDescription
	{
	CMTMTxtSettings* settings = CMTMTxtSettings::NewL();
	CleanupStack::PushL(settings);
	TMTMTxtSettings root;
	settings->LoadSettingsL(aEntry.iServiceId, root);
	CleanupStack::PopAndDestroy(); //settings
	aFileName = root.RootFolder();		
	aFileName.Append(aEntry.iDetails);
	aFileName.Append(aEntry.iDescription);
	if (aEntry.iType == KUidMsvFolderEntry)
		aFileName.Append(KPathDelimiter);
	}

