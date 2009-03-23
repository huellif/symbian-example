// SendAs2Example.h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//
// @file
// This contains Class CSendAs2Example.

#ifndef __SENDAS2EXAMPLE_H__
#define __SENDAS2EXAMPLE_H__

// epoc include
#include <sendas2.h>
#include <csendasaccounts.h>
#include <csendasmessagetypes.h>
#include <mtmuids.h>
#include <e32cons.h>
#include <msvids.h>

class CDummyObserver;

/**
CSendAs2Example is a class that publicly inherits from CBase. 
CBase is the base class for all classes to be instantiated on the heap.
This class demonstrates- creating a session, connecting to
sendAs server to create a message, displaying the list of SendAs accounts present,
Filtering those messages based on capability and sending the message.
*/
class CSendAs2Example : public CBase
{
public:
	static CSendAs2Example* NewL();
	~CSendAs2Example();
	void StartL();
	void Connect();
	void CreateL(RSendAsMessage& aMessage);
	void DisplayAccountL();
	void CapabilityFilterL();
	void SendL(RSendAsMessage& aMessage);
	
private:
	CSendAs2Example();
	void ConstructL();
	void CleanMessageFolderL();

private:
	/** Pointer to the console interface */
	CConsoleBase* 			iConsole;
	RSendAs 				iSendAs;
	/** Pointer to Session observer used to report whatever notification it receives */	
	CDummyObserver* 		iObserver;
	/** Pointer to the channel of communication between a client thread and the Message Server */
	CMsvSession* 			iSession;
	/** Pointer to a particular Message Server entry */
	CMsvEntry* 				iEntry;
	/** Pointer to the array of entry IDs */
	CMsvEntrySelection* 	iSelection;
};

/**
CDummyObserver is a client implementation of MMsvSessionObserver.
It is a class that publicly inherits from two classes, CBase and MMsvSessionObserver. 
MMsvSessionObserver is the interface for notification of events from a Message Server session.
Objects that need to observe a session view should implement this interface.
*/

class CDummyObserver : public CBase, public MMsvSessionObserver
{
public:
	void HandleSessionEventL(TMsvSessionEvent, TAny*, TAny*, TAny*) {};
};

#endif /*__SENDASEXAMPLE_H__*/