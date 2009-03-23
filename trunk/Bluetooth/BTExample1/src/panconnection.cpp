// panconnection.cpp
//
// Copyright (c) Symbian Software Ltd  2006.  All rights reserved.
//


#include <e32cons.h>
#include <comms-infras/nifif.h>
#include <comms-infras/nifagt.h>
#include <panprog.h>
#include <CommDbConnPref.h>
#include <bt_sock.h>
#include <es_sock.h>
#include <in_sock.h>
#include <c32comm.h>


#include "panconnection.h"
#include "gameconstants.h"



/**
 * CPanConnections NewL function
 */
CPanConnections* CPanConnections::NewL(CConsoleBase & aConsole)
	{
	CPanConnections* self = new CPanConnections(aConsole);
	return self;
	}

/**
 * Primary construction.
 */
CPanConnections::CPanConnections(CConsoleBase & aConsole) 
	: CActive(EPriorityNormal),
	iIapStarted(EFalse), 
	iActivePanConn(KErrNotFound), 
	iConsole(aConsole), 
	iRemoteIpAddr(0)
	{	
	CActiveScheduler::Add(this);
	}

/**
 * Destruction.
 */
CPanConnections::~CPanConnections()
	{

	for (TInt i=0; i < iSocketArray.Count();i++)
		{
		iSocketArray[i].Close();
		}	
	iSocketArray.Close();
	iSocket.Close();
	
	if (iIapStarted != EFalse)
		{
		iConnection.Stop();
		iConnection.Close();
		iIapStarted = EFalse;
		}
	
	iIapLoading = EFalse;

	iActiveConnections.Close();
	iSockSvr.Close();
	iSdp.Close();
	iSdpdb.Close();
	


	}

/**
 * Function to load the Bluetooth stack.
 */
TInt CPanConnections::Initialise()
	{
	
	#if defined (__WINS__)
		_LIT(KPDDName, "ECDRV");
	#else		
		_LIT(KPDDName, "EUART1");		
	#endif
		_LIT(KLDDName, "ECOMM");	
	TInt rerr = KErrNone;
	rerr = StartC32();// For any Serial comms
	// Load required drivers
	rerr = User::LoadPhysicalDevice(KPDDName);
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}
		
	rerr = User::LoadLogicalDevice(KLDDName);
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}

	rerr = iSockSvr.Connect();
	if(rerr)
		{
		return rerr;
		}

	TProtocolDesc iProtocolInfo;
	TRequestStatus status;

	rerr = iSockSvr.FindProtocol(_L("L2CAP"), iProtocolInfo);
	if(rerr)
		{
		return rerr;
		}
	// After Socket server is connected, start L2CAP protocol
	iSockSvr.StartProtocol(iProtocolInfo.iAddrFamily, iProtocolInfo.iSockType, iProtocolInfo.iProtocol, status);
	User::WaitForRequest(status);
	// Register SDP record.
	TRAP(rerr, RegisterSdpL());
	
	return rerr;
	}

void CPanConnections::RegisterSdpL()
	{
	User::LeaveIfError(iSdp.Connect());
	User::LeaveIfError(iSdpdb.Open(iSdp));
		
	TSdpServRecordHandle handle = 0;
		
	switch (iLocalRole)
		{
		case KPanUUUID:
			iSdpdb.CreateServiceRecordL(KPanUUUID, handle);
		break;
		
		case KPanGnUUID:
			iSdpdb.CreateServiceRecordL(KPanGnUUID, handle);
		break;
		
		default:
			iSdpdb.CreateServiceRecordL(KPanUUUID, handle);
		break;
		}
	
		
	// A list of protocols supported
	CSdpAttrValueDES* protocols = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(protocols);
		protocols
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(KL2CAPUUID)
				->BuildUUIDL(KBnepUUID)
				->BuildUUIDL(KIpUUID)
				->BuildUUIDL(KTcpUUID) 
			->EndListL()
			
		->EndListL();
		
		
	iSdpdb.UpdateAttributeL(handle, KSdpAttrIdProtocolDescriptorList,*protocols);
	CleanupStack::PopAndDestroy(protocols);
		
	// Service Name attribute, a universal attribute that defines the name of this service.
	iSdpdb.UpdateAttributeL(handle, KSdpAttrIdServiceName, KServiceName);
	// Service description attribute
	iSdpdb.UpdateAttributeL(handle, KSdpAttrIdServiceDescription, KServiceDescription);			
	
	}


/**
 * Function called after the CActivePanConn completes and the IAP has started.
 */
void CPanConnections::IapStarted()
	{		
	TInt rerr;
	if (!iIapLoading)
		{

		}
	
	iIapLoading = EFalse;
	iIapStarted = ETrue;
	// Open TCP socket with interface to existing RConnection

	if (iUseTcpTransport)
		{
		rerr = iSocket.Open(iSockSvr, KAfInet, KSockStream, KProtocolInetTcp, iConnection);
		}
	else
		{
		// User wants a UDP socket
		rerr = iSocket.Open(iSockSvr, KAfInet, KSockDatagram, KProtocolInetUdp, iConnection);
		}
		
	if (rerr != KErrNone)
		{
		return;
		}
	// Start TCP connection sequence			
	if (iListening && iUseTcpTransport)
		{
		TcpIpBindAndListen();
		AcceptNewSocket();
		}
	// If we are a UDP socket then bind, a wait is required	
	if (!iUseTcpTransport)
		{
		TInt duration;
		if (iLocalRole == KPanGnUUID)
			{
			duration = 4000000;
			}
		else 
			{
			duration = 6000000;
			}
			
		RTimer timer;
		TRequestStatus status;
		timer.CreateLocal();
		timer.After(status, duration);
		User::WaitForRequest(status);
		timer.Close();
		rerr = UdpBind();
		if (iLocalRole == KPanGnUUID)
			{
			ReceiveRemoteName();
			}
		else
			{
		
			TPckgBuf<TInt> buff = 1;
			TRequestStatus status;
			TInetAddr addr(KDealerIpAddr,KGenericPort);
			iSocket.SendTo(buff, addr, 0, status);
			User::WaitForRequest(status); 
			iConsole.Printf(_L("\nJoined the PAN, press ENTER to continue..."));
			}
			
		
		}
		

	}


/**
 * When the IAP connection has connected asyncronously
 * our RunL will be called. 
 */
void CPanConnections::RunL()
	{
	// Something wrong with RConnection start
	if (iStatus != KErrNone)
		{
		// PRINT SOMETHING HERE
				iConsole.Printf(_L("\nError eith iStatus : %d\n"), iStatus.Int());
		}
	else
		{
		switch (iActiveMode)
			{
			case EIAPStart:
				// Start the IAP
				IapStarted();
			break;
		
			case ESocketAccept:
				TcpIpListenComplete();
			break;
			
			case ESocketConnect:
				TcpIpConnectionComplete();
			break;
		
			case EUdpReceive:
				iRemoteNames.AppendL(iRemoteName);
				iConsole.Printf(_L("\nReady to connect another device"));
			break;
		
			default:
			break;
			}

		}

	}

/**
 * If the request to start a new connection is cancelled before completion
 * this function will be called. 
 */
void CPanConnections::DoCancel()
	{
	CancelIap();
	}
		



/**
 * Function called to Start a connection (IAP).
 */	
TInt CPanConnections::StartConnection(TBTDevAddr* aDevAddr, TBool aUsePANNotifier, TBool aUseTcpTransport)
	{
	TInt err = KErrNone;
	iUseTcpTransport = aUseTcpTransport;
	
	// Check if there is an existing IAP.
	if(iIapLoading == EFalse && iIapStarted == EFalse)
		{
		// No existing connection so configure IAP in CommDb and start IAP,
		// this is done by overriding RConnection's preferences with our IAP
		// information
		TRAP(err, ConfigureIAPL(ETrue, aDevAddr, aUsePANNotifier));
		if(err != KErrNone)
			{
			iConsole.Printf(_L("IAP Configuration Failed - %d.\n"), err);
			}
		else
			{
			err = StartIAP();
			if(err != KErrNone)
				{
				iConsole.Printf(_L("Failed to start IAP - %d.\n"), err);
				}
			}
		}
	else
		{
		#ifdef __NO_CONTROL_DURING_START__
			if(iIapLoading && aDevAddr != NULL)
				{
				// A new connection is required but a listening IAP is pending.
				Cancel();
				iConnection.Close();
			
				TRAP(err, ConfigureIAPL(ETrue, aDevAddr, aUsePANNotifier));
				if(err != KErrNone)
					{
					iConsole.Printf(_L("IAP Configuration Failed - %d.\n"), err);
					}
				else
					{
					err = StartIAP();
					if(err != KErrNone)
						{
						iConsole.Printf(_L("Failed to start IAP - %d.\n"), err);
						}
					}
				return err;
				}
		#endif			
		// An IAP already exists.
		// Make sure that we can store another connection
		if(iActiveConnections.Count() == 7)
			{
			err = KErrOverflow;
			}
		else if(aDevAddr)
			{
			// Add the BT address of the device to the existing RConnection
			iControlAddr = *aDevAddr;
			TPtr8 ptr = iControlAddr.Des();
			err = iConnection.Control(KCOLAgent, KCOAgentPanConnectDevice, ptr);
			if(err != KErrNone)
				{
				__DEBUGGER();
				}
			if (!iUseTcpTransport)
				{
				ReceiveRemoteName();
				}
			}
		}	
	return err;
	}
	
	
/**
 * Function that opens and starts an RConnection.
 */	
TInt CPanConnections::StartIAP()
	{
	TInt rerr = KErrNone;
	// Create a set of connection preferences to override existing
	// set in CommDb when RConnection::Start is called.
	TCommDbConnPref connPref;
	PanProfileIAPs iap(EGeneralIAP);
		
	connPref.SetIapId(iap);
	connPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	
	// load BT stack	
	rerr = Initialise();
	if(rerr == KErrNone)
		{
		// Open the connection
		rerr = iConnection.Open(iSockSvr);
		if(rerr == KErrNone)
			{
			// Start the connection
			// use the iStatus of active object, if it doesn't complete (we are listening)
			// then the active object deals with it, otherwise AO calls IapStarted function on this class
			// Something wrong with RConnection start
			iActiveMode = EIAPStart;	
			iConnection.Start(connPref, iStatus);
			SetActive();
			iIapLoading = ETrue;
			}
		}
	return rerr;
	}
	
	
/**
 * Function that accesses the PAN Service Table in the CommsDb and configures
 * the IAP to the required settings.
 */		
void CPanConnections::ConfigureIAPL(TBool aIsListening, TBTDevAddr* aDevAddr, TBool aUsePANNotifier)
	{
	// Open CommDb and get a view of the PAN service extentions table.
	CCommsDatabase* db = CCommsDatabase::NewL();
	CleanupStack::PushL(db);
	CCommsDbTableView* tableView = db->OpenTableLC(TPtrC(PAN_SERVICE_EXTENSIONS));

	TBuf<KMaxBufferSize> tableName;
	TInt err = tableView->GotoFirstRecord();
	if(err == KErrNone)
		{
		tableView->ReadTextL(TPtrC(COMMDB_NAME), tableName);
		if(tableName == TPtrC(_S("PANServiceExtensionsTable1")))
			{
			
			User::LeaveIfError(tableView->UpdateRecord());// Start update
			//enable listening mode	
			tableView->WriteBoolL(TPtrC(PAN_ALLOW_INCOMING),aIsListening);
			tableView->WriteBoolL(TPtrC(PAN_DISABLE_SDP_QUERY), 0);

			tableView->WriteBoolL(TPtrC(PAN_PROMPT_FOR_REMOTE_DEVICES), aUsePANNotifier);

			if(aDevAddr)
				{
				TBuf<KMaxBufferSize> buf;
				aDevAddr->GetReadable(buf);

				tableView->WriteTextL(TPtrC(PAN_PEER_MAC_ADDRESSES), buf);
				}
			else
				{
				tableView->WriteTextL(TPtrC(PAN_PEER_MAC_ADDRESSES), _L(""));
				}
			// Finalise changes made.
			User::LeaveIfError(tableView->PutRecordChanges());// Finish update
			User::LeaveIfError(db->CommitTransaction());

			CleanupStack::PopAndDestroy(2); // db & tableView
			return;
			}
		}
	User::Leave(KErrNotFound);
	}
	
	
/**
 * Function that takes an IP address from user input, accesses the
 * Bluetooth PAN Profile table in the CommDb and updates the IpAddr
 * field with the supplied address.
 */		
void CPanConnections::SetLocalIpAddrL(TUint32 addr)
	{
	iLocalIpAddr = addr;
	iSrcAddr.SetAddress(iLocalIpAddr);
	CCommsDatabase* db = CCommsDatabase::NewL();
	CleanupStack::PushL(db);
	// Get the LAN service table
	CCommsDbTableView* tableView = db->OpenTableLC(TPtrC(LAN_SERVICE));

	TBuf<KMaxBufferSize> tableName;
	TInt err = tableView->GotoFirstRecord();
	if(err == KErrNone)
		{
		// Get the name of the table
		tableView->ReadTextL(TPtrC(COMMDB_NAME), tableName);
		if(tableName == TPtrC(_S("BluetoothPANProfile")))
			{
			TInetAddr tempAddr;
			TBuf<KMaxBufferSize> dispBuf;
			tempAddr.SetAddress(iLocalIpAddr);
			tempAddr.Output(dispBuf);
			
			User::LeaveIfError(tableView->UpdateRecord());	
			tableView->WriteTextL(_L("IpAddr"), dispBuf);
		
			User::LeaveIfError(tableView->PutRecordChanges());
			User::LeaveIfError(db->CommitTransaction());	
			}
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(2);//db & tableView 
	}
	
	
	
/**
 * Function that accesses the PAN Service table in the CommDb 
 * and applies the appropriate role, this function will be used for
 * local or peer as well as the PAN role (U or Gn).
 */	
void CPanConnections::SetFixedRoleL(TSide aSide, TUint aRole)
	{
	if (aSide == ELocalRole)
		iLocalRole = aRole;
	else
		iPeerRole = aRole;
	
	CCommsDatabase* db = CCommsDatabase::NewL();
	CleanupStack::PushL(db);
	CCommsDbTableView* tableView = db->OpenTableLC(TPtrC(PAN_SERVICE_EXTENSIONS));

	TBuf<KMaxBufferSize> tableName;
	TInt err = tableView->GotoFirstRecord();
	if(err == KErrNone)
		{
		tableView->ReadTextL(TPtrC(COMMDB_NAME), tableName);
		if(tableName == TPtrC(_S("PANServiceExtensionsTable1")))
			{
			User::LeaveIfError(tableView->UpdateRecord());	
			if(aSide == ELocalRole)
				{
				tableView->WriteBoolL(TPtrC(PAN_LOCAL_ROLE), aRole);
				}
			else 
				{
				tableView->WriteBoolL(TPtrC(PAN_PEER_ROLE), aRole);
				}
			User::LeaveIfError(tableView->PutRecordChanges());
			User::LeaveIfError(db->CommitTransaction());

			CleanupStack::PopAndDestroy(2);
			return;
			}
		}
	User::Leave(KErrNotFound);
	}
	
/**
 * Function that will access the PAN service table in the CommDb
 * and use the values contained to printout the current state.
 */		
void CPanConnections::PrintIAPL()
	{
	CCommsDatabase* db = CCommsDatabase::NewL();
	CleanupStack::PushL(db);
	CCommsDbTableView* tableView = db->OpenTableLC(TPtrC(PAN_SERVICE_EXTENSIONS));

	TInt err = tableView->GotoFirstRecord();
	
	if(err == KErrNone)
		{
		// Print the IAP
		TUint32 uVal;
		TBool bVal;
		TBuf<KMaxBufferSize> sVal;
	

		iConsole.Printf(_L("------------------- CURRENT IAP ------------------\n"));
		
		tableView->ReadUintL(TPtrC(PAN_LOCAL_ROLE), uVal);
		iConsole.Printf(_L("Local Role: %d, "), uVal);			
		tableView->ReadUintL(TPtrC(PAN_PEER_ROLE), uVal);
		iConsole.Printf(_L("Peer Role: %d\n"), uVal);
		tableView->ReadBoolL(TPtrC(PAN_PROMPT_FOR_REMOTE_DEVICES), bVal);
		iConsole.Printf(_L("Peer Prompt: %d, "), bVal);			
		tableView->ReadBoolL(TPtrC(PAN_DISABLE_SDP_QUERY), bVal);
		iConsole.Printf(_L("Disable SDP: %d, "), bVal);			
		tableView->ReadBoolL(TPtrC(PAN_ALLOW_INCOMING), bVal);
		iConsole.Printf(_L("Listening: %d\n"), bVal);			
		tableView->ReadTextL(TPtrC(PAN_PEER_MAC_ADDRESSES), sVal);
		iConsole.Printf(_L("Peer MAC Addr: %S\n"), &sVal);

		iConsole.Printf(_L("--------------------------------------------------\n"));
		CleanupStack::PopAndDestroy(2);
		return;
		}
	User::Leave(KErrNotFound);
	}


/**
 * Function to stop the connection to the IAP.
 */		
TInt CPanConnections::StopIAP()
	{
	TInt rerr = KErrNone;
	if(iIapLoading)
		{
		Cancel();
		}
	else
		{
		if(iIapStarted)
			{
			rerr = iConnection.Stop();
			if(rerr != KErrNone)
				{
				iConsole.Printf(_L("Failed to STOP IAP.  Err %d.\n"), rerr);
				}
			iIapStarted = EFalse;
			}
		}
	return rerr;
	}
	
	
/**
 * Function called is an outstanding connection request is canceled. 
 * 
 */		
void CPanConnections::CancelIap()
	{
	iIapLoading = EFalse;
	iConnection.Stop();
	}
	
/**
 * Stops the currently active connection, this is the connection that the user has
 * selected from the numeric menu option, therefore only one device will be
 * disconnected at a time.
 */	

TInt CPanConnections::StopConnection()
	{
	TInt rerr = KErrNone;
	if(iActivePanConn >= iActiveConnections.Count())
		{
		rerr = KErrArgument;
		}
	else if(iActivePanConn != KErrNotFound)
		{
		// get the deivce we wish to disconnect
		TPtr8 ptr = iActiveConnections[iActivePanConn].Des();
		rerr = iConnection.Control(KCOLAgent, KCOAgentPanDisconnectDevice, ptr);
		if(rerr != KErrNone)
			{
			__DEBUGGER();
			}
		}
	else 
		{
		rerr = KErrNotFound;
		}
	return rerr;
	}
	
	
/**
 * Function that is passed the numeric option selected by the user,
 * the iActivePanConn member is set so that we can perform tasks
 * with this device, i.e. diconnect from RConnection.
 */	
TInt CPanConnections::SetActivePanConn(TUint aConnIndex)
	{
	TInt rerr = KErrArgument;
	
	if(aConnIndex < iActiveConnections.Count())
		{
		iActivePanConn = aConnIndex;
		rerr = KErrNone;
		}
	return rerr;
	}



/**
 * Function that will look at all the devices on the RConnection
 * and update the device array accordingly.
 */		
TInt CPanConnections::UpdateCurrentConnections()
	{
	TInt rerr = KErrNone;
	HBufC8* buffer=0;
	const TInt KAddrLen = sizeof(TBTDevAddr);

	TRAP(rerr, buffer = HBufC8::NewL(7*KAddrLen));// create a buffer to house the device address
	if(rerr == KErrNone)
		{
		TPtr8 ptr = buffer->Des();
		// use RConnection to enumerate all the devices
		rerr = iConnection.Control(KCOLAgent, KCOAgentPanEnumerateDevices, ptr);
		if(rerr == KErrNone)
			{
			iActiveConnections.Reset();
			while(ptr.Length()>=KBTDevAddrSize)
				{
				// inspect the addr's in the descriptor and append them to the array.
				TBTDevAddr parsedAddr(ptr.Mid(ptr.Length()-KAddrLen, KBTDevAddrSize));
				ptr.SetLength(Max(ptr.Length()-KAddrLen, 0));
				iActiveConnections.Append(parsedAddr);
				}
			}
		}
	delete buffer;
	return rerr;
	}

/**
 * Function to print the current connections we have.
 */		
void CPanConnections::PrintConns()
	{
	iConsole.Printf(_L("\nRemotenames : %d\n"), iRemoteNames.Count());
	PrintTcpSocketConnections();
	iConsole.Printf(_L("--------------------------------------------------\n"));
	iConsole.Printf(_L("Listening Mode: "));
	iListening ? iConsole.Printf(_L("Enabled        ")) : iConsole.Printf(_L("Disabled        "));
	if(iIapStarted)
		{
		iConsole.Printf(_L("       IAP Started"));

		TInt err = UpdateCurrentConnections();
		if(err != KErrNone)
			{
			iConsole.Printf(_L("FAILED TO ENUMERATE CONNECTIONS. Err %d\n"), err);
			}
		else
			{
			iConsole.Printf(_L("\n------------------ CONNECTIONS -------------------\n"));
			iConsole.Printf(_L("Index  Remote Addr\n"));
			
			
			for(TInt i=0;i<iActiveConnections.Count();i++)
				{
				TBuf<KMaxBufferSize> dispBuf;
				iActiveConnections[i].GetReadable(dispBuf);

				if(iActivePanConn == i)
					{
					iConsole.Printf(_L("-> "));
					}
				else
					{
					iConsole.Printf(_L("   "));
					}
				iConsole.Printf(_L("%d   0x%S\n"), i, &dispBuf);				
				}
			}
		}
	
	if(iLocalIpAddr == 0)
		{
		iConsole.Printf(_L("\nNo local IP address set\n"));
		}
	else
		{
		TInetAddr tempAddr;
		TBuf<KMaxBufferSize> dispBuf;
		
		tempAddr.SetAddress(iLocalIpAddr);
		tempAddr.Output(dispBuf);
		
		iConsole.Printf(_L("\nLocal IP address: %S\n"), &dispBuf);
		}

	}

/**
 * Return the local IP address
 */	
TUint32 CPanConnections::GetLocalIpAddr()
	{
	return iLocalIpAddr;
	}


	
/**
 * Function called by CTcpIpHandler active object when
 * an attempt is made to connect a socket.
 */	
void CPanConnections::TcpIpConnectionComplete()
	{
	if (iStatus == KErrNone)
		{
		iConsole.Printf(_L("\nSuccessfully joined PAN\n"));
		}
	else
		{
		iConsole.Printf(_L("Join PAN Failed!, Socket error : %d"), iStatus.Int());
		}

	}
	
	

	
/**
 * Function that is called to cancel an outstanding socket connection 
 */		
void CPanConnections::ConnectDoCancel()
	{
	iConsole.Printf(_L("TCP/IP connection Canceled!\n"));
	iSocket.CancelConnect();
	}
	
/**
 * This is a function that will return the array device addresses.
 * This can then be used along with a reference to the socket to use the PAN
 */	
RArray<TInetAddr>& CPanConnections::RemoteNames()
	{
	return iRemoteNames;
	}
	
RSocket& CPanConnections::GetSocket()
	{
	return iSocket;
	}
	
RArray<RSocket>& CPanConnections::GetSocketArray()
	{
	return iSocketArray;
	}
	
/**
 * Function called by CTcpIpHandler active object when
 * an accept on the socket has completed, we will immediately
 * get ready to accept another connection, unless there is no more devices
 * to connect to.
 */		
void CPanConnections::TcpIpListenComplete()
	{
	// iPanEstablished ????
	if (iStatus == KErrNone)
		iConsole.Printf(_L("Player Joined\n"));
	else
		iConsole.Printf(_L("TCP socket failed : %d\n"), iStatus.Int());
	// Call accept for the next device to connect to
	TInt err = AcceptNewSocket();
	}
	
	
/**
 * Function called if the current accept on a socket is canceled
 * before completing.
 */	
void CPanConnections::ListenDoCancel()
	{
	iConsole.Printf(_L("\nTCP/IP accept Canceled!\n"));
	iSocket.CancelAccept();	
	}
	
	
/**
 * Take the address entered and set it as our remote IP address
 * (the device we wish to connect to).
 */	
void CPanConnections::SetRemoteIpAddr(TUint32 aAddr)
	{
	iRemoteIpAddr = aAddr;
	}

/**
 * Function to recall the remote IP address we have set.
 */	
TUint32 CPanConnections::GetRemoteIpAddr() const
	{
	return iRemoteIpAddr;
	}
	
TInt CPanConnections::UdpBind()
	{
	iSrcAddr.SetPort(KGenericPort);
	iSrcAddr.SetAddress(iLocalIpAddr);
	return iSocket.Bind(iSrcAddr);
	}
	
// This function will be used if a PAN-Gn
void CPanConnections::ReceiveRemoteName()
	{
	// Attempt to receive, this will happen one by one due to authorisation 
//	if (iRemoteNames.Count() < iActiveConnections.Count())
//		{
		
		iActiveMode = EUdpReceive;
		iSocket.RecvFrom(iBuff,iRemoteName, 0, iStatus);
		SetActive();
//		}
//	else
//		{
//		iConsole.Printf(_L("\nAll devices have told us:!!!!!!"));
//		}
	}

void CPanConnections::CancelRecvFrom()
	{
	iSocket.CancelRecv();
	}
/**
 * Function used to bind and connect to an existing socket.
 * The socket we connect to has to be ready to Accept us.
 */	
TInt CPanConnections::TcpIpBindAndConnect()
	{
	TInt rerr;

	if (iRemoteIpAddr == 0)
		{
		iConsole.Printf(_L("You need to set a destination IP address.\n"));
		return (KErrArgument);
		}
		
	if (!IsActive()/* && iTcpIpMode != ETcpReceive*/)
		{
		// Setup the TInetAddrs inc. port number
		iDstAddr.SetPort(KGenericPort);
		iDstAddr.SetAddress(iRemoteIpAddr);
		iSrcAddr.SetPort(KGenericPort);
		iSrcAddr.SetAddress(iLocalIpAddr);

		rerr = iSocket.Bind(iSrcAddr);// Bind the socket
		if (rerr != KErrNone)
			{
			iConsole.Printf(_L("Failed : %d"), rerr);
			return (rerr);
			}
		iActiveMode = ESocketConnect; // Set the mode for the active object
		iSocket.Connect(iDstAddr, iStatus);// Attempt connection to dest. addr
		SetActive();
		}
	return KErrNone;
	}
	
/**
 * Function that will bind and listen on a socket
 */		
TInt CPanConnections::TcpIpBindAndListen()
	{
	TInt rerr;
	iDstAddr.SetPort(KGenericPort);
	iDstAddr.SetAddress(iRemoteIpAddr);
	iSrcAddr.SetPort(KGenericPort);
	iSrcAddr.SetAddress(iLocalIpAddr);
	
	rerr = iSocket.Bind(iSrcAddr);
	if (rerr != KErrNone)
		{
		iConsole.Printf(_L("Failed to bind with socket : %d"), rerr);
		return (rerr);
		}
	rerr = iSocket.Listen(7);// maximum devices is 7
	if (rerr != KErrNone)
		{
		iConsole.Printf(_L("Failed to listen on socket : %d"), rerr);
		return (rerr);
		}
	return (KErrNone);
	}

/**
 * Function that creates a new blank socket, when a socket is connected
 * the original socket will hand over control to the once blank socket.
 * The new sockets will be held in an array. 
 * Once all sockets have connected we can proceed to send out the player numbers
 * each player.
 */	
TInt CPanConnections::AcceptNewSocket()
	{

// Accept new socket will only be called if using TCP sockets	
	TInt currentSocketCount = iSocketArray.Count();
	TInt rerr;
	if (currentSocketCount == iActiveConnections.Count())
		{
		// We already have the same number of connections as 
		// devices on the PAN, move on to dish out player numbers
		iConsole.Printf(_L("\nAll players on PAN have joined!!\n"));
		TRequestStatus status;
		for (TInt i=0;i<iSocketArray.Count();i++)
			{
			status = KRequestPending;
		//	iPlayerNum = i;
		//	iSocketArray[i].Send(iPlayerNum, 0, status);
			User::WaitForRequest(status);
			if (status != KErrNone)
				{
				// We have failed to send player number to the player
				// Remove them from the socket array and the network
				iSocketArray[i].Close();
				iSocketArray.Remove(i);
				TPtr8 ptr = iActiveConnections[i].Des();
				iConnection.Control(KCOLAgent, KCOAgentPanDisconnectDevice, ptr);
				i--;
				}
			}
		ListenDoCancel();// Cancel any oustanding Accepts.
		return (KErrNone);
		}
	else
		{
		RSocket socket;
		iSocketArray.Append(socket);
		TInt last = iSocketArray.Count()-1;
		rerr = iSocketArray[last].Open(iSockSvr);
		if (rerr != KErrNone)
			{
			iConsole.Printf(_L("Failed to open 'Service' Socket : %d"), rerr);
			return (rerr);
			}
		iActiveMode = ESocketAccept;// set the mode for the active object 
		iSocket.Accept(iSocketArray[last], iStatus);
		SetActive();	
		return (rerr);
		}
	}
	
/**
 * Function to shutdown the existing socket.
 */	
TInt CPanConnections::CloseTcpIpSocketConnect()
	{
	TRequestStatus status;
	// Shutdown immediately
	iSocket.Shutdown(RSocket::EImmediate, status);
	User::WaitForRequest(status);
	iConsole.Printf(_L("\nConnection closed with error: %d"), status.Int());
	return (status.Int());
	}
	
/**
 * Function to shutdown all sockets in the socket array.
 * This function only applies to a PAN-Gn.
 */	
TInt CPanConnections::CloseAllTcpIpSockets()
	{
	TRequestStatus status;
	for (TInt i=0; i<iSocketArray.Count(); i++)
		{
		iSocketArray[i].Shutdown(RSocket::EImmediate, status);
		User::WaitForRequest(status);
		iConsole.Printf(_L("\nConnection to %d closed with error: %d"), i, status.Int());
		}
	return (KErrNone); 
	}
	
/**
 * Function to Shutdown a socket of the users choice.
 */	
TInt CPanConnections::CloseCurrentTcpIpSocket()
	{
	TRequestStatus status;
	iSocketArray[iActiveTcpSocket].Shutdown(RSocket::EImmediate, status);
	User::WaitForRequest(status);
	iConsole.Printf(_L("\nConnection closed with error: %d"), status.Int());
	return (status.Int());
	}
	
/**
 * Function that prints out the IP addresses of the connected devices.
 */		
void CPanConnections::PrintTcpSocketConnections()
	{
	iConsole.Printf(_L("---------------- UDP CONNECTIONS -----------------\n"));
	iConsole.Printf(_L("Devices on Piconet\n"));
//	if (iSocketArray.Count() <= 1 && IsActive())
//		{
//		// We only have one socket, make sure it is connected
		// as it might still be accepting.
//		iConsole.Printf(_L("\n			NO DEVICES\n"));
//		}
//	else
//		{
		TInt count;
		if (IsActive() && iActiveMode == ESocketAccept)
			count = iRemoteNames.Count()-1;
		else
			count = iRemoteNames.Count();
		for(TInt i=0;i<count;i++)
			{
			TBuf<KMaxBufferSize> dispBuf;
			//TInetAddr address;
		//	iRemoteNames[i].RemoteName(address);
			iRemoteNames[i].Output(dispBuf);
			iConsole.Printf(_L("		%d - %S\n"), i, &dispBuf);				
			}
//		}
	iConsole.Printf(_L("--------------------------------------------------\n"));
	}
	
/**
 * Get the local role
 */	
TInt CPanConnections::GetLocalRole() const
	{
	return (iLocalRole);
	}
	


/**
 * Function that return the number of active connections.
 */	
TInt CPanConnections::ActiveConnCount()
	{
	return iActiveConnections.Count();
	}


void CPanConnections::CancelAllOperations()
	{
	for (TInt i=0;i<iSocketArray.Count();i++)
		{
		iSocketArray[i].CancelAll();
		}
	}
