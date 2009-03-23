// test.cpp
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.
//

// Transient server example - test program

#include <e32base.h>
#include <e32test.h>
#include <f32file.h>
#include "testclient.h"

_LIT(KTest,"Transient server test");
static RTest test(KTest);

const TInt KMaxMessage=100;
typedef TBuf<KMaxMessage> TMessage;

_LIT(KMessage1,"message 1");
_LIT(KMessage2,"message 2");
_LIT(KMessage3,"message 3");

_LIT(KThread1,"Thread 1");
_LIT(KThread2,"Thread 2");

static CClient* NewClientL()
	{
	_LIT(KClientDll,"t-testc");
	return CClient::NewL(KClientDll);
	}

static CClient* NewClientLC()
	{
	CClient* c=NewClientL();
	CleanupClosePushL(*c);
	return c;
	}

typedef void (*ThreadTestL)(void);

static TInt ThreadFunc(TAny* aFuncL)
	{
	ThreadTestL f=ThreadTestL(aFuncL);
	CTrapCleanup* c=CTrapCleanup::New();
	ASSERT(c!=0);
#ifdef _DEBUG
	TRAPD(r,f());
	ASSERT(r==0);
#else
	TRAP_IGNORE(f());
#endif
	delete c;
	return KErrNone;
	}

static RThread TestThread(const TDesC& aName,ThreadTestL aTestL,TThreadPriority aPriority=EPriorityNormal)
	{
	const TInt KStackSize=0x2000;			//  8KB
	const TInt KInitHeapSize=0x1000;		//  4KB
	const TInt KHeapSize=0x1000000;			// 16MB

	RThread t;
	test (t.Create(aName,&ThreadFunc,KStackSize,KInitHeapSize,KHeapSize,(TAny*)aTestL)==KErrNone);
	t.SetPriority(aPriority);
	t.Resume();
	return t;
	}

static RThread FindServer()
	{
	RThread t;
	t.SetHandle(0);
	_LIT(KServerName,"*t-server");
	TFindThread find(KServerName);
	TFullName n;
	if (find.Next(n)==KErrNone)
		t.Open(find);
	return t;
	}

static void WaitForClose()
	{
	RThread t(FindServer());
	if (t.Handle()!=0)
		{
		TRequestStatus s;
		t.Logon(s);
		if (t.ExitType()==EExitPending || s!=KRequestPending)
			User::WaitForRequest(s);
		t.Close();
		}
	}

static void CheckInterfaceL()
	{
	test.Start(_L("Single session"));
	CClient* c=NewClientLC();
	test (c->Send(KMessage1)==KErrNone);
	TRequestStatus s;
	TMessage msg;
	c->Receive(s,msg);
	test (s==KRequestPending);
	test (c->Send(KMessage2)==KErrNone);
	test (s==KErrNone);
	test (msg==KMessage2);
	test (c->Send(KMessage1)==KErrNone);
	test (s==KErrNone);
	test (msg==KMessage2);
	User::WaitForRequest(s);
	c->CancelReceive();
	test (s==KErrNone);
	test (msg==KMessage2);
	c->Receive(s,msg);
	test (s==KRequestPending);
	c->CancelReceive();
	test (s==KErrCancel);
	test (c->Send(KMessage1)==KErrNone);
	test (s==KErrCancel);
	User::WaitForRequest(s);
	c->CancelReceive();
	CleanupStack::PopAndDestroy(); // c
//
	test.Next(_L("Second session"));
	c=NewClientLC();
	CClient* c2=NewClientLC();
	c->Receive(s,msg);
	test (s==KRequestPending);
	test (c2->Send(KMessage3)==KErrNone);
	test (s==KErrNone);
	test (msg==KMessage3);
	TRequestStatus s2;
	TMessage msg2;
	c->Receive(s,msg);
	test (s==KRequestPending);
	c2->Receive(s2,msg2);
	test (s2==KRequestPending);
	test (c->Send(KMessage1)==KErrNone);
	test (s==KErrNone);
	test (msg==KMessage1);
	test (s2==KErrNone);
	test (msg2==KMessage1);
//
	CleanupStack::PopAndDestroy(2); // c2, c
	WaitForClose();
	test.End();
	}

static RSemaphore StartSem;
static RSemaphore GoSem;

static void Start1L()
	{
	StartSem.Wait();
	CClient* c=NewClientLC();
	GoSem.Wait();
	c->Send(KMessage1);
	TMessage m;
	TRequestStatus s;
	c->Receive(s,m);
	GoSem.Signal();
	User::WaitForRequest(s);
	ASSERT(m==KMessage2);
	CleanupStack::PopAndDestroy();	// c
	}

static void Start2L()
	{
	StartSem.Wait();
	CClient* c=NewClientLC();
	TMessage m;
	TRequestStatus s;
	c->Receive(s,m);
	GoSem.Signal();
	User::WaitForRequest(s);
	ASSERT(m==KMessage1);
	GoSem.Wait();
	c->Send(KMessage2);
	CleanupStack::PopAndDestroy();	// c
	}

static void SimultaneousStartL()
	{
	test.Start(_L("Set up threads"));
	test (StartSem.CreateLocal(0,EOwnerProcess)==KErrNone);
	test (GoSem.CreateLocal(0,EOwnerProcess)==KErrNone);
	RThread t=TestThread(KThread1,&Start1L,EPriorityMore);
	TRequestStatus s1;
	t.Logon(s1);
	t.Close();
	t=TestThread(KThread2,&Start2L,EPriorityMore);
	TRequestStatus s2;
	t.Logon(s2);
	t.Close();
//
	test.Next(_L("Set them off"));
	StartSem.Signal(2);
	User::WaitForRequest(s1);
	test (s1==KErrNone);
	User::WaitForRequest(s2);
	test (s2==KErrNone);
//
	GoSem.Close();
	StartSem.Close();
	WaitForClose();
	test.End();
	}

static void StartWhileStoppingL()
	{
	test.Start(_L("Start & stop server and wait for exit"));
	NewClientL()->Close();
	RThread t(FindServer());
	test (t.Handle()!=0);
	TRequestStatus s;
	t.Logon(s);
	if (t.ExitType()==EExitPending || s!=KRequestPending)
		User::WaitForRequest(s);
	test.Next(_L("attempt to restart with dead thread/server"));
	TRAPD(r,NewClientL()->Close());
	test (r==KErrNone);
	test.Next(_L("attempt to restart after cleanup"));
	t.Close();
	WaitForClose();
	TRAP(r,NewClientL()->Close());
	test (r==KErrNone);
	WaitForClose();
	test.End();
	}

static void MainL()
	{
	test.Next(_L("Validate interface"));
	CheckInterfaceL();
	test.Next(_L("Simultaneous Start"));
	SimultaneousStartL();
	test.Next(_L("Start while stopping"));
	StartWhileStoppingL();
	}

TInt E32Main()
	{
	test.Title();
	test.Start(_L("initialising"));
	// start the loader
	RFs fs;
	test (fs.Connect()==KErrNone);
	fs.Close();
	CTrapCleanup* c=CTrapCleanup::New();
	test (c!=0);
	TRAPD(r,MainL());
	test (r==0);
	delete c;
	test.End();
	test.Close();
	return KErrNone;
	}

