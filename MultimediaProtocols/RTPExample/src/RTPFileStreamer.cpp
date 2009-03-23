// RtpFileStreamer.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#include "RtpFileStreamer.h"
#include <CommDbConnPref.h>

_LIT(KGreetingText,"Welcome to RTP source file dump!\n");

/**
Symbian OS 2nd phase constructor to initialise a new send source on the RRtpSession
and register to be called back when a packet is successfully sent by send source.
*/
void CRtpFileSender::ConstructL(const TDesC& aSrcFilename)
	{
			
	User::LeaveIfError(iFile.Replace(iFs,aSrcFilename,EFileWrite|EFileStreamText));
	TPtrC8 representation((TUint8*)(&KGreetingText)->Ptr(), (&KGreetingText)->Size());
	User::LeaveIfError(iFile.Write(representation));
	User::LeaveIfError(iFile.Flush());
	iFile.Close(); 
	
	User::LeaveIfError(iFile.Open(iFs,aSrcFilename,EFileRead|EFileWrite|EFileStreamText));
	iSendSrc = iSession.NewSendSourceL();
	iSendSrc.SetDefaultPayloadSize(iPacketSize);
	
	iSendSrc.PrivRegisterEventCallbackL(ERtpSendSucceeded, (TRtpCallbackFunction)CRtpFileSender::PacketSent, this);
	iSendSrc.PrivRegisterEventCallbackL(ERtpSendFail, (TRtpCallbackFunction)CRtpFileSender::SendError, this);

	iSendPacket = iSendSrc.NewSendPacketL();

	CActiveScheduler::Add(this);
	}

/**
A callback method which CRtpFileSender registers with the RtpSession to be called
when a packet is successfully sent.
*/		
void CRtpFileSender::PacketSent(CRtpFileSender* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->DoPacketSent(aEvent);
	}
	
/**
An instance method which is called by the static callback function to notify 
the observer and start the RTimer such that another packet can be sent 
when the timer expires.
*/
void CRtpFileSender::DoPacketSent(const TRtpEvent& /*aEvent*/)
	{
	if (iObserver)
		{
		iObserver->NotifyPacketSent();
		}
	iSendIntervalTimer.After(iStatus,TTimeIntervalMicroSeconds32(iDelayMicroSecs));
	SetActive();
	}

/**
A callback method to be called when a sent packet fails.
*/
void CRtpFileSender::SendError(CRtpFileSender* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->DoSendError(aEvent);
	}
	
/**
An instance method which is called by the static callback function to notify the
observer of the error while sending the packet.
*/	
void CRtpFileSender::DoSendError(const TRtpEvent& /*aEvent*/)
	{
	if (iObserver)
		{
		iObserver->NotifyError();
		}
	}
		
CRtpFileSender::~CRtpFileSender()
	{
	Cancel();
	iFile.Close();
	iSendIntervalTimer.Close();
	iSendPacket.Close();
	iSendSrc.Close();
	}
CRtpFileSender* CRtpFileSender::NewL(RRtpSession& aSession,RFs& aFs,const TDesC& aSrcFilename, TInt aPacketSize, TInt aDelayMicroSeconds)
	{
	if (!aSession.IsOpen())
		{
		User::Leave(KErrArgument);
		}
	CRtpFileSender* self = new (ELeave) CRtpFileSender(aSession,aFs,aPacketSize,aDelayMicroSeconds);
	CleanupStack::PushL(self);
	self->ConstructL(aSrcFilename);
	CleanupStack::Pop(self);
	return self;
	}
CRtpFileSender::CRtpFileSender(RRtpSession& aSession,RFs& aFs,TInt aPacketSize, TInt aDelayMicroSeconds) :
	CActive(0), iDelayMicroSecs(aDelayMicroSeconds), iPacketSize(aPacketSize), 
	iSession(aSession), iPayloadDesC(NULL,NULL), iFs(aFs)
	{
	}
void CRtpFileSender::StartL()
	{
	User::LeaveIfError(iSendIntervalTimer.CreateLocal());
	iSendIntervalTimer.After(iStatus,TTimeIntervalMicroSeconds32(iDelayMicroSecs));
	SetActive();
	}
	
/**
 Sends the next packet. This does the following events:
	- Get a new RtpPacket to send from the send source
	- Set the RTP timestamp of the packet 
	- Read a block of data from the file
If the data is left in file then send the packet else notify the completion and close the input file.
*/
void CRtpFileSender::RunL()
	{
	iSendPacket.SetTimestamp(User::FastCounter());
	iPayloadDesC.Set(const_cast<TUint8*>(iSendPacket.WritePayload().Ptr()),iPacketSize,iPacketSize);
	User::LeaveIfError(iFile.Read(iPayloadDesC));
	if (iPayloadDesC.Length()>0)
		{
		TInt tmp = iPayloadDesC.Length();
		iSendPacket.WritePayload().SetLength(tmp);
		iSendPacket.Send();
		}
	else
		{
		if (iObserver)
			{
			iObserver->NotifyComplete();
			}
		}	
	}
void CRtpFileSender::DoCancel()
	{
	iSendIntervalTimer.Cancel();
	}
CRtpFileStreamer::CRtpFileStreamer(RSocketServ& aSocketServ, const TInetAddr& aDestAddr, TUint aLocalPort) :
      iSocketServ(aSocketServ),
      iDestAddr(TInetAddr(aDestAddr)),
      iLocalPort(aLocalPort)
   	{
   	}
CRtpFileStreamer* CRtpFileStreamer::NewL(RSocketServ& aSocketServ,
    											const TDesC& aSrcFilename,
    											const TDesC& aDestFilename, 
    											TInt aBlockLen,
    											const TInetAddr& aDestAddr,
    											TUint aLocalPort, TInt aConnId)
	{
	CRtpFileStreamer* self = new (ELeave) CRtpFileStreamer(aSocketServ,aDestAddr,aLocalPort);
	CleanupStack::PushL(self);
	self->ConstructL(aSrcFilename,aDestFilename,aBlockLen,10000,aConnId);
	CleanupStack::Pop(self);
	return self;
	}

/**
Symbian OS 2nd phase constructor to open an Rtp session using the pre-connected socket overload,
register for the ERtpNewSource callback in the Rtp event model, create a 
CRtpFileSender active object to stream aSrcFilename
*/		
void CRtpFileStreamer::ConstructL(const TDesC& aSrcFilename, const TDesC& aDestFilename, TInt aPacketSize, TInt aDelayMicroSeconds, TInt aConnId)
	{
	
	User::LeaveIfError(iRFs.Connect());
	iRFs.CreatePrivatePath(EDriveC);
	iRFs.SetSessionToPrivate(EDriveC);
	User::LeaveIfError(iDestFile.Replace(iRFs,aDestFilename,EFileWrite));
	TInetAddr localAddr;
	localAddr.SetPort(iLocalPort);
	_LIT8(KCname, "test");
		
	if (aConnId!=KErrNotFound)
		{
		User::LeaveIfError(iConnection.Open(iSocketServ));
		TCommDbConnPref prefs;
		prefs.SetIapId(aConnId);
   		prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
   		TRequestStatus stat;
   		iConnection.Start(prefs, stat);
   		User::WaitForRequest( stat );
    	User::LeaveIfError( stat.Int() );
		iRtpSession.OpenL(iSocketServ, localAddr,iDestAddr,aPacketSize+12,iConnection, EPriorityNormal, KCname);	
		}
	else
		{
		User::LeaveIfError(iSocket.Open(iSocketServ, KAfInet,KSockDatagram, KProtocolInetUdp));	
		User::LeaveIfError(iSocket.Bind(localAddr));
		localAddr.SetPort(iLocalPort + 1);
		User::LeaveIfError(iRtcpSocket.Open(iSocketServ, KAfInet,KSockDatagram, KProtocolInetUdp));	
		User::LeaveIfError(iRtcpSocket.Bind(localAddr));
		TRequestStatus stat;
		iSocket.Connect(iDestAddr,stat);
		User::WaitForRequest(stat);
		User::LeaveIfError(stat.Int());
		iDestAddr.SetPort(iDestAddr.Port() + 1);
		iRtcpSocket.Connect(iDestAddr,stat);
		User::WaitForRequest(stat);
		User::LeaveIfError(stat.Int());
		iRtpSession.OpenL(iSocket, aPacketSize+12, iRtcpSocket);
		iRtpSession.SetRTPTimeConversion(100, 100);
		}				
	iRtpSession.PrivRegisterEventCallbackL(ERtpNewSource, (TRtpCallbackFunction)CRtpFileStreamer::NewSource, this);
	iSender = CRtpFileSender::NewL(iRtpSession,iRFs,aSrcFilename, aPacketSize, aDelayMicroSeconds);
	}
CRtpFileStreamer::~CRtpFileStreamer()
	{
	if (iSender)
		{
		delete iSender;	
		}
	iDestFile.Close();
	iRFs.Close();
	iRecvPacket.Close();
	iRtpRecvSrc.Close();
	iRtpSession.Close();
	iSocket.Close();
	iRtcpSocket.Close();
	}

void CRtpFileStreamer::StartL()
	{
	iSender->StartL();
	}
/**
A callback method that is called when a packet with a new SSRC arrives.
*/
void CRtpFileStreamer::NewSource(CRtpFileStreamer* aPtr, const TRtpEvent& aEvent)
	{
	if (aPtr->ReceiveSrc().IsOpen())
		{
		aPtr->ReceiveSrc().Close();
		}
	TRAPD(err,
		aPtr->ReceiveSrc() = aEvent.Session().NewReceiveSourceL();
		aPtr->ReceiveSrc().PrivRegisterEventCallbackL(ERtpPacketReceived, (TRtpCallbackFunction)CRtpFileStreamer::PacketArrived, aPtr);)
	if (err!=KErrNone)
		{
		__DEBUGGER();
		}
	}
	
/**
A callback method that is called on packet arrival.
*/	
void CRtpFileStreamer::PacketArrived(CRtpFileStreamer* aPtr, const TRtpEvent& aEvent)
	{	
	aPtr->iRecvPacket.Close();
	aPtr->iRecvPacket = aEvent.ReceiveSource().Packet();
	if (aEvent.ReceiveSource().Packet().IsOpen())
		{
		__DEBUGGER();
		}
	TRAPD(err,aPtr->HandleReceivedPacketL());
	if (err!=KErrNone)
		{
		__DEBUGGER();
		}
	}
	
/**
An instance method called by the static callback function to
be called on packet arrival.
*/	
void CRtpFileStreamer::HandleReceivedPacketL()
	{
	if (iObserver)
		{
		iObserver->NotifyPacketReceived();
		}
	TPtrC8 writeDesc(iRecvPacket.Payload().Ptr(),iRecvPacket.Payload().Length());
	User::LeaveIfError(iDestFile.Write(writeDesc));
	}
