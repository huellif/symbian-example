// panconnection.H
//
// Copyright (c) Symbian Software Ltd  2006.  All rights reserved.
//

#ifndef PANCONNECTION_H
#define PANCONNECTION_H


#include <e32base.h>
#include <e32cons.h> 
#include <bt_sock.h> 
#include <es_sock.h>
#include <in_sock.h>
#include <btsdp.h>

// Enum for determining which side we want to change role on
enum TSide
	{
	ELocalRole,
	EPeerRole
	};

// SDP constants

const TInt KIpUUID = 0x0009; // IP UUID for SDP
const TInt KTcpUUID = 0x0004;// TCP UUID for SDP 

/* Attribute ID for service description is calculated from the sum of offset and language base */
static const TSdpAttributeID KSdpAttrIdServiceDescription 
							= KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceDescription;
/* Attribute ID for service description is calculated from the sum of offset and language base */
static const TSdpAttributeID KSdpAttrIdServiceName 
							= KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceName;

_LIT(KServiceName, "PAN Profile Engine");
_LIT(KServiceDescription, "This device is ready to use PAN profile");

/**
 * The CPanConnections class is responsible for setting up the underlying
 * PAN. It's duties are to update the CommDb and use it to start an IAP,
 * it allows for devices to be connected together in a network using Bluetooth.
 * If the user wishes to be the dealer in the game (PAN-Gn) then they will be required to 
 * select the player (via Bluetooth address).
 */
class CPanConnections : public CActive
	{
public:
	static CPanConnections* NewL(CConsoleBase & aConsole);
	
	~CPanConnections();
	void CancelRecvFrom();
	TInt StartConnection(TBTDevAddr* aDevAddr, TBool aUsePANNotifier, TBool aUseTcpTransport);
	TInt StopConnection();
	TInt SetActivePanConn(TUint aConnIndex);
	void SetLocalIpAddrL(TUint32 aAddr);
	void SetFixedRoleL(TSide aSide, TUint aRole);
	void PrintConns();
	TInt TcpIpBindAndListen();
	TInt TcpIpBindAndConnect();
	void ListenDoCancel();
	void ConnectDoCancel();
	TInt CloseTcpIpSocketConnect();
	TUint32 GetRemoteIpAddr() const;
	void SetRemoteIpAddr(TUint32 aAddr);
	TInt AcceptNewSocket();
	void PrintTcpSocketConnections();
	TInt GetLocalRole() const;
	void CancelIap();
	TInt StopIAP();
	TInt CloseAllTcpIpSockets();
	TInt CloseCurrentTcpIpSocket();
	TInt ActiveConnCount();
	void CancelAllOperations();
	TInt UdpBind();
	void RunL();
	void DoCancel();
	
	RArray<TInetAddr>& RemoteNames();
	RSocket& GetSocket();
	RArray<RSocket>& GetSocketArray();
	
private:
	CPanConnections(CConsoleBase & aConsole);
	void SetTransportLayerProtocol(TUint aProtocol);
	void IapStarted();
	void TcpIpConnectionComplete();
	void TcpIpListenComplete();
	TInt StartIAP();
	void PrintIAPL();
	TInt UpdateCurrentConnections();	
	TInt Initialise();
	TUint32 GetLocalIpAddr();
	void ConfigureIAPL(TBool aIsListening, TBTDevAddr* aDevAddr, TBool aUsePANNotifier);
	void RecvTcpIpPacket();
	void RegisterSdpL();	
	void ReceiveRemoteName();

public:
	TBool iIapStarted;
	TBool iIapLoading;
	TBool iListening;	

private:
	enum TActiveMode
		{
		EIAPStart,
		ESocketAccept,
		ESocketConnect,
		EUdpReceive
		};

	// Used to select different IAPs from commdb
	enum PanProfileIAPs
		{
		EGeneralIAP = 1,
		EUnknownConnUnknownRolesListen,
		EUnknownConnUnknownRoles,
		EKnownConnUnknownRolesListen
		};	
		
	RSocket iSocket;
	RArray<RSocket> iSocketArray;
	TInt iActiveTcpSocket;
	TPckgBuf<TInt> iBuff;
	TInetAddr iSrcAddr;
	TInetAddr iDstAddr;
	TUint iLocalRole;
	TUint iPeerRole;
	TBool iUseTcpTransport;
	TActiveMode iActiveMode;
	TBTDevAddr iControlAddr;
	TInt iActivePanConn;	
	RConnection iConnection;
	RSocketServ iSockSvr;
	CConsoleBase& iConsole;
	RBTDevAddrArray iActiveConnections;// array of TBTDevAddrs on PAN
	TUint32 iRemoteIpAddr;
	TUint32 iLocalIpAddr;
	RSdp iSdp;
	RSdpDatabase iSdpdb;
	TInetAddr iRemoteName;
	RArray<TInetAddr> iRemoteNames;

	};
	

#endif // PANCONNECTION_H



