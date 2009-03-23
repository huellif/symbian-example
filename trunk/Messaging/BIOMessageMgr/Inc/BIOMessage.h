// BIOMessage.h
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#ifndef __BIOMESSAGE_H__
#define __BIOMESSAGE_H__


#include "BIOParser.h"

#include <sendas2.h>
#include <csendasaccounts.h>
#include <csendasmessagetypes.h>

// System Includes
#include <e32cons.h>

_LIT(KTestTitle, "BIO Parser Example" );

_LIT(KBodyText, "Every man passes his life in the search after friendship.");
_LIT(KAddress1, "test1@symbian.com");
_LIT(KAddress3, "test3@symbian.com");
_LIT(KAddress4, "test4@symbian.com");
_LIT(KAlias1, "alias1");
_LIT(KAlias2, "alias2");
_LIT(KSubject, "HELLO");

class CDummyObserver;
class CBioMessage : public CBase
{
public:
	static CBioMessage* NewL();
	~CBioMessage();
	void StartL();

	void Connect();
	void CreateL(RSendAsMessage& aMessage);
	void SendL(RSendAsMessage& aMessage);
	void ParseL();

private:
	CBioMessage();
	void ConstructL();
	void CleanMailFolder(TMsvId aFolderId);

private:
	CConsoleBase* 			iConsole;

	RSendAs 				iSendAs;	
	CDummyObserver* 		iObserver;
	CMsvSession* 			iSession;
	CMsvEntry* 				iEntry;
	CMsvEntrySelection* 	iSelection;
};


class CDummyObserver : public CBase, public MMsvSessionObserver
{
public:
	void HandleSessionEventL(TMsvSessionEvent, TAny*, TAny*, TAny*) {};
};

#endif /*__BIOMESSAGE_H__*/
