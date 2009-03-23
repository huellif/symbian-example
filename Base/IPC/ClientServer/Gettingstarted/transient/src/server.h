// server.h
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.
//

// Transient server example - server classes

#include <e32base.h>
#include "clientserver.h"

enum TMyPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction,
	EPanicAlreadyReceiving
	};

void PanicClient(const RMessagePtr2& aMessage,TMyPanic TMyPanic);

class CShutdown : public CTimer
	{
	enum {KMyShutdownDelay=0x200000};	// approx 2s
public:
	inline CShutdown();
	inline void ConstructL();
	inline void Start();
private:
	void RunL();
	};

class CMyServer : public CServer2
	{
public:
	static CServer2* NewLC();
	void AddSession();
	void DropSession();
	void Send(const TDesC& aMessage);
private:
	CMyServer();
	void ConstructL();
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
	TInt iSessionCount;
	CShutdown iShutdown;
	};

class CMySession : public CSession2
	{
public:
	CMySession();
	void CreateL();
	void Send(const TDesC& aMessage);
private:
	~CMySession();
	inline CMyServer& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);
	inline TBool ReceivePending() const;
private:
	RMessagePtr2 iReceiveMsg;
	TInt iReceiveLen;
	};

