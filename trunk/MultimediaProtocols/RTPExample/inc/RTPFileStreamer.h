// RTPFileStreamer.h
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#ifndef __RTPFILESTREAMER_H__
#define __RTPFILESTREAMER_H__

#include "CommDbConnPref.h"
#include <in_sock.h>
#include <f32file.h>
#include <rtp.h>


class MFileStreamerObserver
/**
A simple observer pattern interface to display the status information.
*/
{
public:
	virtual void NotifyPacketSent() {}
	virtual void NotifyPacketReceived() {}
	virtual void NotifyComplete() {}
	virtual void NotifyError() {}
};



class CRtpFileSender : public CActive
/**
An Active Object which sends a block of a file after every n 
microseconds as an RTP payload. 
*/
{
public:
	//Callbacks for the sender
	static void PacketSent(CRtpFileSender* aPtr, const TRtpEvent& aEvent);
	void DoPacketSent(const TRtpEvent& aEvent);
	static void SendError(CRtpFileSender* aPtr, const TRtpEvent& aEvent);
	void DoSendError(const TRtpEvent& aEvent);
	static CRtpFileSender* NewL(RRtpSession& aSession,RFs& aFs,const TDesC& aSrcFilename, TInt aPacketSize, TInt aDelayMicroSeconds);
	void StartL();
	void SetObserver(MFileStreamerObserver&	aObserver)
		{
		iObserver = &aObserver;
		}
	~CRtpFileSender();
private:
	CRtpFileSender(RRtpSession& aSession,RFs& aFs,TInt aPacketSize, TInt aDelayMicroSeconds);
	void ConstructL(const TDesC& aSrcFilename);
	void RunL();
	void DoCancel();
	

private:
	RTimer	iSendIntervalTimer;
	TInt	iDelayMicroSecs;
	TInt	iPacketSize;
	RRtpSession&	iSession;
	RRtpSendSource	iSendSrc;
	RRtpSendPacket	iSendPacket;
   	TPtr8			iPayloadDesC;
   	MFileStreamerObserver*	iObserver;
	RFile	iFile;
	RFs&	iFs;
};




class CRtpFileStreamer : public CBase
/**
CRtpFileStreamer demonstrates the initialisation of RTP and the method to
register for callbacks in the RTP event model. It handles the receiving of
RTP packets on the RRtpSession it owns.
*/
{
public:

    static CRtpFileStreamer* NewL(	RSocketServ& aRSocketServ,
    								const TDesC& aSrcFilename,
    								const TDesC& aDestFilename, 
    								TInt aBlockLen,
    								const TInetAddr& aDestAddr,
    								TUint aLocalPort,TInt aConnId);
    
    ~CRtpFileStreamer();
	//Callbacks for the receiver
	static void NewSource(CRtpFileStreamer* aPtr, const TRtpEvent& aEvent);
	static void PacketArrived(CRtpFileStreamer* aPtr, const TRtpEvent& aEvent);
	inline RRtpReceiveSource& ReceiveSrc() 
		{
		return iRtpRecvSrc;
		}
	void SendNextPacketL();
	void HandleReceivedPacketL();
	void SetObserver(MFileStreamerObserver& aObserver) 
		{
		iObserver = &aObserver;
		if (iSender) iSender->SetObserver(aObserver);
		}
	void StartL();
	
private:
   CRtpFileStreamer( RSocketServ& aSocketServ, const TInetAddr& aDestAddr, TUint aLocalPort);
   void ConstructL(const TDesC& aSrcFilename, const TDesC& aDestFilename, TInt aPacketSize, TInt aDelayMicroSecs, TInt aConnId);
public:
	RRtpReceivePacket iRecvPacket;
private:
    RSocketServ& iSocketServ;
    RSocket iSocket;
	RSocket iRtcpSocket;
  	RConnection	iConnection;
  	TInetAddr			iDestAddr;
   	TUint				iLocalPort;
    RRtpSession	   	  	iRtpSession;
    RRtpReceiveSource 	iRtpRecvSrc;
	MFileStreamerObserver*	iObserver;
  	CRtpFileSender*	iSender;
   	RFs		iRFs;
   	RFile	iDestFile;
};


#endif //__RTPFILESTREAMER_H__
