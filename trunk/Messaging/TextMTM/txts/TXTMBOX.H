// TXTMBOX.H
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//


#ifndef __TXTMBOX_H__
#define __TXTMBOX_H__

#include <e32base.h>
#include <F32FILE.H>
#include <MSVSTD.H>
#include <MSVENTRY.H>
#include <E32DEF.H>
#include "txut.h"

//
//	CTxtRefreshMBox: refresher class to synchronize real folder and service
//

class CTxtRefreshMBox : public CBase
	{
	public:
		static CTxtRefreshMBox* NewL(RFs& aFs, TFileName& aRelativePath, 
			TMsvId aCurrentRootEntryId, CMsvServerEntry *aEntry, TMsvId aServiceEntryId,
			const TMTMTxtSettings& aTxtSettings);
		~CTxtRefreshMBox();
		TBool DoStepL(); // Returns ETrue if finished.
	private: // Construction
		CTxtRefreshMBox(RFs& aFs,TFileName& aRelativePath, TMsvId aCurrentRootEntryId, 
			CMsvServerEntry *aEntry, TMsvId aServiceEntryId, const TMTMTxtSettings& aTxtSettings) 
			: iFs(aFs) , iRelativePath(aRelativePath), iCurrentRootEntryId(aCurrentRootEntryId), 
			iEntry(aEntry), iTxtSettings(aTxtSettings), iServiceEntryId(aServiceEntryId)
			{};
		void ConstructL();
	private: // Tools
		TInt CreateChild(const TDesC& aDescription, const TDesC& aDetails, 
			TUid aMessageType, const TTime& aDate, const TInt aSize);
		void DeleteEntryL();
		TMsvId InsertFileL();
	private:
		RFs&  iFs;
		TFileName& iRelativePath;
		TMsvId	iCurrentRootEntryId;
		CDir* iFilelist;
		CMsvServerEntry* iEntry;
		CMsvEntrySelection* iExistingEntries;
		
		TInt iCurrentFile;	// index for file list
		TInt iCurrentEntry;	// index for entry list
		const TMTMTxtSettings& iTxtSettings;
		TMsvId iServiceEntryId;
	};
#endif
