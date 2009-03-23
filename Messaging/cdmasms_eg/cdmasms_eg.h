//////////////////////////////////////////////////////////////////////////
//
//	cdmasms_eg.h
//
//	Copyright (c) 2004 Symbian Software Ltd.  All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#ifndef CDMASMS_EG_H_
#define CDMASMS_EG_H_

#include <smsclnt.h>
#include <smcmmain.h>
#include <smuthdr.h>
#include <smutset.h>
#include <smscmds.h>
#include <smut.h>
#include <csmsaccount.h>
#include <mtclreg.h>

#include <Smsstd.h>
#include <SmsMessageSettings.h>
#include <SmsMessageWrappers.h>
#include <tia637.h>

class CSMSSession;

/** 
	Active object signalled when asynchronous messaging operation completes
*/
class CSendOp : public CActive
	{
public:
	CSendOp(CSMSSession& aSmsSession);
	~CSendOp();
	void SendL();

private:
	// Implement CActive
	void DoCancel();
	void RunL();

private:
	/** Operation owned by this object */
	CMsvOperation* iMsvOperation;
	/** Session used by object */
	CSMSSession& iSmsSession;
	};


/** Holds a session to the message server and the 
	SMS client MTM object.
 */
class CSMSSession: public CBase 
	{
public:
	CSMSSession(MMsvSessionObserver& aSessionObserver);
	void ConstructL();
	~CSMSSession();
	/** Get SMS client MTM */
	CSmsClientMtm& SmsClientMtm() {return *iSmsClientMtm;}

	CMsvOperation* SendSMSNowL(TRequestStatus& aStatus);
	
	void SimulateIncomingL(TBool aWEMT);
	TMsvId CreateWEMTL(const CSmsSettings& aServiceSettings);
	
private:
	/** Message server session */
	CMsvSession* iMsvSession;	
	/** SMS client MTM object */
	CSmsClientMtm* iSmsClientMtm;
	/** Client MTM registry */
	CClientMtmRegistry* iClientMtmRegistry;
	/** Session observer interface */
	MMsvSessionObserver& iSessionObserver;	
	};

/**
Main UI class to write menus to the console 
and get user input. 

It monitors session events for new incoming
SMS messages.
*/
class CMenuHandler : public CBase, public MMsvSessionObserver
	{
public:
	CMenuHandler(CConsoleBase& aConsole, TDes& aInput);
	void ConstructL();
	~CMenuHandler();

	void ProcessInputL();
	void MenuL();
	
private:
	void HandleSessionEventL(TMsvSessionEvent , TAny* , TAny* , TAny* );
	
	void NextCreateInputL();
	void NextIncomingInputL();
	TBool InputOutOfRange(TInt aHigh,TInt& aVal) const;
	void IncomingMenuL();
	void PrintMessageL();
	void SaveMessageL();
	void PrintAbsoluteTimeL(const TDateTime& absTime);
	void PrintRelativeTime(TSmsRelativeTimeUnit aTimeUnit, TUint32 aRelativeTime, TBool aValidity);
	void PrintAcknowledgementStatus();
	void PrintAckStatus(CSmsNumber::TSmsAckStatus ackStatus);
	CMsvOperation* SaveScheduledL(TMsvId aId,TRequestStatus& aStatus);	

private:
	/** Top-level menu states */
	enum TState
		{
		/** Main menu */
		EMenuChoiceMain,
		/** Sub-menu for incoming messages */
		EMenuChoiceIncoming,
		/** Input parameters for creating a message */
		EInputCreate,
		/** Input parameters for monitoring incoming messages */
		EInputIncoming
		};
	/** States in creating a new message */
	enum TCreateState
		{
		EInit,
		ESetMsgText,
		ESetAddress,
		ESetValidityPeriodAbs,
		ESetDeliveryTimeAbs,
		ESetPriority,
		ESetPrivacy,
		ESetAlert,
		ESetLang,
		ESetSetCallbackNumber,
		EWEMTSetAnimation,
		EWEMTSetSound,		
		ELast	
		};
	/** States in setting options for incoming messages */
	enum TIncomingState
		{
		EIncomingInit,
		ESetTeleservice,
		ESetTextMatch
		};
				
private:
	/** SMS client MTM from  */
	CSmsClientMtm* iMtm;
	CSMSSession* iSmsSession;
	/** CDMA specific fields of current message */
	/** For getting results of a send operation */
	CSendOp* iSendOperation;
	/** Main menu state */	
	TState iState;

	/** Flag for getting incoming messages */
	TBool iGetIncoming;
	/** Incoming options state */
	TInt iIncomingState;

	/** Teleservice to match (0 == match all) */
	tia637::TTeleserviceId iMatchTeleservice; 
	/** Text to match (empty string == match all) */
	TBuf<5> iMatchText;
	/** Create message options state */
	TInt iCreateState;
	/** Flag is true if creating WEMT, false if creating WMT */
	TBool iWEMT;
	/** Console for status messages */
	CConsoleBase& iConsole;
	/** Keyboard input buffer */
	TDes& iInput;
	};

#endif

