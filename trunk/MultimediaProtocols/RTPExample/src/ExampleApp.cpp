// ExampleApp.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//


#include "RTPFileStreamer.h"
#include "ExampleApp.h"


//CActiveConsole
CActiveConsole::CActiveConsole(MActiveConsoleNotify& aNotify):CActive(EPriorityStandard), iNotify(aNotify)
	{
	CActiveScheduler::Add(this);
	}

CActiveConsole::~CActiveConsole()
	{
	delete iConsole;
	Cancel();
	}

CActiveConsole* CActiveConsole::NewL(MActiveConsoleNotify& aNotify,const TDesC& aTitle,const TSize& aSize)
	{
	CActiveConsole* console = new (ELeave) CActiveConsole(aNotify);
	CleanupStack::PushL(console);
	console->ConstructL(aTitle,aSize);
	CleanupStack::Pop();
	return console;
	}

void CActiveConsole::ConstructL(const TDesC& aTitle,const TSize& aSize)
	{
	iConsole = Console::NewL(aTitle,aSize);
	}

void CActiveConsole::DoCancel()
	{
	iConsole->ReadCancel();
	}

void CActiveConsole::RequestKey()
	{
	DrawCursor();
	iConsole->Read(iStatus);
	SetActive();
	}

void CActiveConsole::DrawCursor()
	{
	iConsole->Printf(_L(">>"));
	}

void CActiveConsole::RunL()
	{
	TChar ch = iConsole->KeyCode();
	iNotify.KeyPressed(ch);
	}

//CExampleApp
CExampleApp::CExampleApp()
	{
	}
	
CExampleApp::~CExampleApp()
	{
	delete iActiveConsole;
	delete iMonitorConsole;
	iSockServ.Close();
	}
	
CExampleApp* CExampleApp::NewL()
	{
	CExampleApp* thisapp = new (ELeave) CExampleApp();
	CleanupStack::PushL(thisapp);
	thisapp->ConstructL();
	CleanupStack::Pop();
	return thisapp;
	}

static TSize gMainConsole(KConsFullScreen,KConsFullScreen);
static TSize gTinyConsole(40,10);

void CExampleApp::ConstructL()
	{
	iActiveConsole = CActiveConsole::NewL(*this,_L("Status"),gMainConsole);
	iMonitorConsole = CActiveConsole::NewL(*this,_L("RtpExample"),gTinyConsole);
	User::LeaveIfError(iSockServ.Connect());
	}
	
void CExampleApp::StartL()
	{
	_LIT(KTextWelcome, " ### RTP Example ### \n");
	iMonitorConsole->Console().Printf(KTextWelcome);
	
	_LIT(KTextStartApp, "\n\n Starting the RTP application");
	iMonitorConsole->Console().Printf ( KTextStartApp );
	
	_LIT(KTextPressAKey, "\n\n Press any key to step through the example");
	iMonitorConsole->Console().Printf ( KTextPressAKey );
	iMonitorConsole->Console().Getch ();	

	_LIT(KDAddr,"\n\n Local address: ");
	iMonitorConsole->Console().Printf(KDAddr);
	iMonitorConsole->Console().Printf(KDestAddr);
	iMonitorConsole->Console().Getch ();
	_LIT(KLPort,"\n\n Address family: ");
	iMonitorConsole->Console().Printf(KLPort);
	iMonitorConsole->Console().Printf(KLocalPort);
	iMonitorConsole->Console().Getch ();
	_LIT(KDPort,"\n\n RTP port number: ");
	iMonitorConsole->Console().Printf(KDPort);
	iMonitorConsole->Console().Printf(KDestPort);
	iMonitorConsole->Console().Getch ();
	_LIT(KSFile,"\n\n Source file name: ");
	iMonitorConsole->Console().Printf(KSFile);
	iMonitorConsole->Console().Printf(KsFileName);
	iMonitorConsole->Console().Getch ();
	_LIT(KTFile,"\n\n Target file name: ");
	iMonitorConsole->Console().Printf(KTFile);
	iMonitorConsole->Console().Printf(KdFileName);
	iMonitorConsole->Console().Getch ();
	_LIT(KBufSize,"\n\n Buffer size: ");
	iMonitorConsole->Console().Printf(KBufSize);
	iMonitorConsole->Console().Printf(KSize);
	iMonitorConsole->Console().Getch ();

	TInt dport;
	TInt lport;
	TInt psize;
	TInetAddr daddr;
	TLex parser;
	parser.Assign(KDestPort);
	User::LeaveIfError(parser.Val(dport));
	parser.Assign(KSize);
	User::LeaveIfError(parser.Val(psize));
	parser.Assign(KLocalPort);
	User::LeaveIfError(parser.Val(lport));

	RHostResolver resolver;
	User::LeaveIfError(resolver.Open(iSockServ,KAfInet,KProtocolInetUdp));
	CleanupClosePushL(resolver);
	TNameEntry entry;
	TRequestStatus status;
	resolver.GetByName(KDestAddr,entry,status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	CleanupStack::PopAndDestroy(1);//close resolver

	TSockAddr remote = entry().iAddr;
	remote.SetPort(dport);

	TInt connId=KErrNotFound;
	iMonitorConsole->Console().Printf(_L("\n\n Sending RTP packet from %S file to %S file."),&KsFileName,&KdFileName);		
	iMonitorConsole->Console().Getch();
	iStreamer = CRtpFileStreamer::NewL(iSockServ,KsFileName,KdFileName,psize,remote,lport,connId);
	iStreamer->SetObserver(*this);
	iStreamer->StartL();
	CActiveScheduler::Start();
	}
	
void CExampleApp::Stop()
	{
	if (iStreamer)
		{
		delete iStreamer;
		}
	CActiveScheduler::Stop();
	}
	
void CExampleApp::DrawMonitor()
	{
	iMonitorConsole->Console().Printf(_L("\n Sent: %d\n Recv: %d"),iSent,iRecv);
	}
	
void CExampleApp::NotifyPacketSent()
	{
	iSent++;
	DrawMonitor();
	iActiveConsole->Console().Printf(_L("s"));
	}
	
void CExampleApp::NotifyPacketReceived()
	{
	iRecv++;
	DrawMonitor();
	iActiveConsole->Console().Printf(_L("r"));
	}
	
void CExampleApp::NotifyComplete()
	{
	iMonitorConsole->Console().Printf(_L("\n\n Successfully sent %d RTP packet(s)"),iRecv);
	iMonitorConsole->Console().Getch();
	
	_LIT(KExit,"\n\n Press any key to exit the application ");
	iMonitorConsole->Console().Printf(KExit); 
	iMonitorConsole->Console().Getch();
	
	if (iStreamer)
		{
		delete iStreamer;
		}
	CActiveScheduler::Stop();
	}
	
void CExampleApp::NotifyError()
	{
	iMonitorConsole->Console().Printf(_L("Error\n"));
	iMonitorConsole->Console().Getch();
	iActiveConsole->Console().Printf(_L("E"));
	}
	
void CExampleApp::KeyPressed(TChar /*aKey*/)
	{
	}

void MainL()
	{
	CExampleApp* app = CExampleApp::NewL();
	CleanupStack::PushL(app);
	app->StartL();
	CleanupStack::PopAndDestroy(app);
	}
	
TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();
	CActiveScheduler* activescheduler=new CActiveScheduler;
	CActiveScheduler::Install(activescheduler);

	TRAPD(err, MainL());
	_LIT(KTxtEPOC32EX,"EXAMPLES");
	__ASSERT_ALWAYS(!err,User::Panic(KTxtEPOC32EX,err));
	
	delete activescheduler;
	delete cleanupStack;
	__UHEAP_MARKEND;
	return KErrNone;
	}
