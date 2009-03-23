// ExampleApp.h
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//


#ifndef __EXAMPLEAPP_H__
#define __EXAMPLEAPP_H__

#include <e32test.h>
#include <bacline.h>
#include "RtpFileStreamer.h"

_LIT(KTitle,"RTCP App" );
_LIT(KDestAddr,"2130706433" );
_LIT(KDestPort,"9000" ); 
_LIT(KLocalPort,"9000" ); 
_LIT(KsFileName,"source.dat" ); 
_LIT(KdFileName,"target.dat" );
_LIT(KSize,"100" );

/**
RTP active console observer
*/
class MActiveConsoleNotify
	{
public:
	virtual void KeyPressed(TChar aKey) =0;
	};
/**
RTP active console
*/
class CActiveConsole : public CActive
	{
	public:
		static CActiveConsole* NewL(MActiveConsoleNotify& aNotify,const TDesC& aTitle,const TSize& aSize);
		void RequestKey();		
		inline CConsoleBase& Console() const 
			{
			return *iConsole;
			};
		~CActiveConsole();
	private:
		void RunL();
		void DoCancel();
		void DrawCursor();
		CActiveConsole(MActiveConsoleNotify& aNotify);
		void ConstructL(const TDesC& aTitle,const TSize& aSize);
	private:
		CConsoleBase*			iConsole;
		MActiveConsoleNotify&	iNotify;
	};
/**
RTP example application	
*/
class CExampleApp : public CBase, public MActiveConsoleNotify, public MFileStreamerObserver
	{
public:
	static CExampleApp* NewL();

	void StartL();
	void Stop();
	~CExampleApp();
	void KeyPressed(TChar aKey);
	void ReadError(TInt aError);
public:
	virtual void NotifyPacketSent();
	virtual void NotifyPacketReceived();
	virtual void NotifyComplete();
	virtual void NotifyError();
private:
	CExampleApp();
	void ConstructL();
	void DrawMonitor();
private:
	CActiveConsole*			iActiveConsole;
	CActiveConsole*			iMonitorConsole;
	TInt					iSent;
	TInt					iRecv;
	CRtpFileStreamer*		iStreamer;
	RSocketServ				iSockServ;
	};

#endif // __EXAMPLEAPP_H__
