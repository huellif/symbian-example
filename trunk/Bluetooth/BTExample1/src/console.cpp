// console.cpp
//
// Copyright (c) Symbian Software Ltd  2006.  All rights reserved.
//



#include <e32base.h>
#include <e32cons.h>
#include <bttypes.h>
#include <BTExtNotifiers.h>
#include <btsdp.h>

#include "console.h"
#include "panconnection.h"
#include "dealer.h"
#include "player.h"


/**
 * Function to panic the thread.
 */
void Panic(TInt aPanicCode)
	{
	User::Panic(_L("PanConnConsole Panic: "), aPanicCode);
	}
	

/**
 * Constructor.
 */
CActiveConsole::CActiveConsole(CConsoleBase* aConsole) 
	: CActive(EPriorityNormal),
	  iConsole(aConsole)	 
	{	
	}

/**
 * NewLC function, calls 2nd phase constructor.
 */
CActiveConsole* CActiveConsole::NewLC(CConsoleBase* aConsole)
    {
    CActiveConsole* self = new (ELeave) CActiveConsole(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL(aConsole);
    return self;
    }


/**
 * 2nd phase construction.
 */
void CActiveConsole::ConstructL (CConsoleBase* aConsole)
    { 
    CActiveScheduler::Add(this);
    iPanConnection = CPanConnections::NewL(*aConsole);
    }



/**
 * Destructor.
 */
CActiveConsole::~CActiveConsole()
	{
	Cancel();// Cancel Active Object
	delete iPanConnection;
	}

/**
 * RequestCharacter function, this is responsible for diplaying a menu to the user.
 */
void CActiveConsole::RequestCharacter()
    {
    
   	iConsole->ClearScreen();
	if (iPanConnection->iIapStarted && iPlayerRole == EPlayer)
		{
		iMode = EScabbyQueen;
		}
	
	if (iMode == EPan)
		{
		// Mode is PAN Profile, at this stage devices withh need to connect using Bluetooth	
    	iPanConnection->PrintConns();
    	MenuLine();
		iConsole->Printf(_L("*****************PAN Profile menu*****************\n"));
		MenuLine();
		iConsole->Printf(_L("*   n - Invite a new player (Dealer Only)        *\n"));
	//	iConsole->Printf(_L("*   N - Open new connection using PAN notifier   *\n"));
		iConsole->Printf(_L("*   c - Remove current active player             *\n"));
	//	iConsole->Printf(_L("*   L - Start listening (for player or dealer)   *\n"));
		iConsole->Printf(_L("*   Y - Be a Player                              *\n"));
		iConsole->Printf(_L("*   Z - Be a Dealer                              *\n"));
		iConsole->Printf(_L("*   b - Start Game (Dealer Only)                 *\n"));
		iConsole->Printf(_L("*   s - Stop Game                                *\n"));
	//	iConsole->Printf(_L("*   x - Switch to TCP/IP                         *\n"));
		iConsole->Printf(_L("*   1..7 - Select current active player          *\n"));		
		iConsole->Printf(_L("*   Esc Escape                                   *\n"));
		MenuLine();
		}
	else if (iMode == EScabbyQueen)
		{
		if (iPlayerRole == EDealer)
			{
			// Dealer will already be created
			MenuLine();
    		iConsole->Printf(_L("******************* Dealer Menu ******************\n"));
    		MenuLine();
    		iConsole->Printf(_L("*             a - Deal                           *\n"));
    		iConsole->Printf(_L("*             b - Show Deck                      *\n"));
    		iConsole->Printf(_L("*             x - Exit Game                      *\n"));
    		MenuLine();
			}
		else
			{
			if (!iPlayer)
				{
				iPlayer = CScabbyQueenPlayer::NewL(*iConsole, iPanConnection->GetSocket());
				}
			iConsole->Printf(_L("%dPlayer Number: %d\n\n"),iPlayer->iPlayerNum(),iPlayer->iPlayerNum());

    		MenuLine();
    		iConsole->Printf(_L("******************* Player Menu ******************\n"));
    		MenuLine();
    		iConsole->Printf(_L("*          a - Show Cards                        *\n"));
    		if (iPlayer->iCurrentPlayer)
    			{
    			iConsole->Printf(_L("*          b - Take card from right              *\n"));
    			}
    		iConsole->Printf(_L("*          x - Exit Game                         *\n"));
    		MenuLine();
    		
    		if (iPlayer->iCurrentPlayer)
    			{
    			iConsole->Printf(_L("\nYou need to select a card from the other player\n"));
    			}
			}
		}
    
    iConsole->Read(iStatus);// Read the key press, RunL will be called when iStatus completes 
    SetActive();
    
    }
    



/**
 * The active object DoCancel function, if invoked will cancel an outstanding read.
 */
void  CActiveConsole::DoCancel()
    {
    iConsole->ReadCancel();
    }



/**
 * Just a line.
 */
void CActiveConsole::MenuLine()
	{
	iConsole->Printf(_L("**************************************************\n"));
	}

/**
 * The active object RunL function. It is responsible for routing the users
 * selection by calling the ProcessKeyPressL function.
 */
void  CActiveConsole::RunL()
    {
	TRAPD(err,ProcessKeyPressL(TChar(iConsole->KeyCode())));
	if(err != KErrNone)
		{
		iConsole->Printf(_L("Failed. Error: %d\r\n"),err);
		RequestCharacter();
		}	
    }


/**
 * This function is called after the user has made a selection.
 * Depending on the mode we are in and the user choise we call the appropriate 
 * function.
 */
void CActiveConsole::ProcessKeyPressL(TChar aChar)
    {
    TInt rerr = KErrNone;
	TBool redraw = ETrue;
	
 
   
    if (iMode == EPan)
    	{
    		
		TBTDevAddr devAddr;
//	TBTDevAddr devAddr(MAKE_TINT64(0x0002, 0x5b019a2c));
    	if (aChar == EKeyEscape)
			{
			CActiveScheduler::Stop();
			return;
			}
		else
			{
			switch(aChar)
				{
				case 'n':
					{	
					iMode = EPan;
					//Ask user which device address we should connect to...
					RNotifier notify;
					User::LeaveIfError(notify.Connect());
					TBTDeviceSelectionParamsPckg pckg;
					TBTDeviceResponseParamsPckg resPckg;
					TRequestStatus stat;
					notify.StartNotifierAndGetResponse(stat, KDeviceSelectionNotifierUid, pckg, resPckg);
					User::WaitForRequest(stat);
					notify.CancelNotifier(KDeviceSelectionNotifierUid);
					notify.Close();
					User::LeaveIfError(stat.Int()); 
					devAddr = resPckg().BDAddr();
			
		
					rerr = iPanConnection->StartConnection(&devAddr, EFalse, EFalse);
					}
				break;
			
		//		case 'N':
		//			{	
		//			// Need to open connection with pan profile here
		//			iMode = EPan;
		//			rerr = iPanConnection->StartConnection(&devAddr, ETrue, EFalse);
		//			}
		//		break;
								
		//		case 'l':
		//		case 'L':
		//			rerr = StartListening();
		//		break;
				
				case 'y':
				case 'Y':
					// User will be a player, get them to enter IP address,
					// Local and peer role will be changed here, and the remote address
					// of the dealer will be set.
					iMode = EPan;
					iPlayerRole = EPlayer;
					iConsole->Printf(_L("\nEnter your IP address (11.11.11.???): "));
					TRAP(rerr, PlayerProcessL());
					iPanConnection->SetRemoteIpAddr(KDealerIpAddr);
					rerr = StartListening();
				break;
			
				case 'z':
				case 'Z':
					// User will be a dealer, set the local and peer role,
					// and the local IP address. 
					iMode = EPan;
					iPlayerRole = EDealer;
					iPanConnection->SetLocalIpAddrL(KDealerIpAddr);
					iPanConnection->SetFixedRoleL(ELocalRole, KPanGnUUID);
					iPanConnection->SetFixedRoleL(EPeerRole, KPanUUUID);
					rerr = KErrNone;
				break;
				
			//	case 'h':
			//	rerr = iPanConnection->UdpBind();
			//	break;
				
				case 'c':
				case 'C':
					iMode = EPan;
					// close current connection
					rerr = iPanConnection->StopConnection();
				break;
			
				case 's':
				case 'S':
					iMode = EPan;
					//stop iap
					iPanConnection->StopIAP();
				break;

			
			//	case 'x':
			//	case 'X':
					// Switch to the TCP, will allow for players to join game
					// and for the dealer to host.
			//		iMode = ETcpIp;
			//		rerr = StartTcpMenu();
			//	break;	
				
				case 'b':
				case 'B':
					if (iPlayerRole == EDealer)
						{
						iMode = EScabbyQueen;
						iPanConnection->CancelRecvFrom();
						if (!iDealer)
							{
							TRAP (rerr,iDealer = CScabbyQueenDealer::NewL(*iConsole, iPanConnection->RemoteNames(), iPanConnection->GetSocket()));
							}
							iDealer->SendPlayerNumbers();
						}
					else
						{
						rerr = KErrNone;
						}
				break;
				
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					if (iPanConnection->iIapStarted)
						rerr = iPanConnection->SetActivePanConn(aChar.GetNumericValue());
					else
						rerr = KErrNone;
				break;		
			
    			default:
				break; 
				
				}
			}
    	}
     else if (iMode == EScabbyQueen)
    	{
    	if (iPlayerRole == EDealer)
    		{
    		switch(aChar)
    			{
    			case 'a':
    				// Dealing the deck will initiate the game.
    				
    				TRAP(rerr, iDealer->DealDeckL());
    				if (rerr != KErrNone)
    					{
    					iConsole->Printf(_L("\nFailed to deal deck"));
    					}
    				else
    					{
    					iDealer->StartTheGame();
    					}
    			break;
    			
    			case 'b':
    				TRAP(rerr, iDealer->ShowDeckL());
    			break;
    			
    			case 'x':
    				iMode = EPan;
    				iDealer->BaseCancelAll();
    			break;
    			default:
    			break;
    			}
    		}
    	else if (iPlayerRole == EPlayer)
    		{
    		switch(aChar)
    			{
    			case 'a':
    				TRAP(rerr, iPlayer->ShowHand());
    			break;
    			
    			case 'b':
    				{
    				iPlayer->GetRightHandPlayerCard();
    				iCardNum = GetCardNumber();
    				rerr = iPlayer->SendCardNum(iCardNum);
    				}
    			break;
    			
    
    			
    			case 'x':
    				iMode = EPan;
    				iPlayer->BaseCancelAll();
  				break;
  				
    			default:
    			break;
    			}
    		}
    	
    	}	
    
	if(redraw && rerr == KErrNone)
		{
		RequestCharacter();
		}
    
    }


/**
 * Function used to access the console from outside this class.
 */
CConsoleBase* CActiveConsole::Console()
	{
	return iConsole;
	}

void CActiveConsole::PlayerProcessL()
	{
	iPanConnection->SetLocalIpAddrL(GetIpAddress());
	iPanConnection->SetFixedRoleL(ELocalRole, KPanUUUID);
	iPanConnection->SetFixedRoleL(EPeerRole, KPanGnUUID);
	}

/**
 * Function that allows for entry of an IP address.
 * Adress will be in the form xxx.xxx.xxx.xxx
 */
TUint32 CActiveConsole::GetIpAddress()
	{
	TBuf<KMaxBufferSize> buf;
	TKeyCode key;
	
	while((key=iConsole->Getch())!=EKeyEnter)
		{
		buf.Append(key);
		iConsole->Printf(_L("%c"),key);
		}
	TInetAddr addr;
	addr.Input(buf);
	return addr.Address();
	}

/**
 * Function that allows the user to enter a card number.
 */
TInt CActiveConsole::GetCardNumber()
	{
	TBuf<KMaxBufferSize> buf;
	TKeyCode key;
	while((key=iConsole->Getch())!=EKeyEnter)
		{
		TChar test = key;
		if (test.IsDigit())
			{	
			buf.Append(key);	
			iConsole->Printf(_L("%c"),key);
			}
		}
	// Convert Desc into a TInt
	TLex16 lex;
	lex.Assign(buf);
	TInt val;
	lex.Val(val);
	val = 2*val;
	return val;
	}


/**
 * Function that will start a listening connection (if we arn't already)
 */
TInt CActiveConsole::StartListening()
	{
	TInt rerr = KErrNone;
	if(iPanConnection->iListening)
		{
		__ASSERT_ALWAYS(iPanConnection->iIapStarted || iPanConnection->iIapLoading, Panic(EListeningWithoutIAP));
		iConsole->Printf(_L("Listening Connection already exists."));
		rerr = KErrAlreadyExists;
		}
	else
		{
		if(iPanConnection->iIapStarted || iPanConnection->iIapLoading)
			{
			iConsole->Printf(_L("Non listening connection already exists."));
			rerr = KErrAlreadyExists;
			}
		}
	iPanConnection->iListening = ETrue;	
	rerr = iPanConnection->StartConnection(NULL, EFalse, EFalse);// Call to start the listen
	return rerr;
	}
	




	
	
/**
 * This is the function called by the entry point code.
 * It exists to perform initialisation
 */
void RunAppL(void)
	{
	CActiveScheduler *myScheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(myScheduler);
	CActiveScheduler::Install(myScheduler); 
	CConsoleBase* console = 	
	Console::NewL(_L("PAN Profile Example Application"),TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(console);
	CActiveConsole* my_console = CActiveConsole::NewLC(console);// New active console
	my_console->RequestCharacter();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(3); 
	}


/**
 * The E32main function is the main entry point for the
 * code.
 */

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,RunAppL()); // more initialization, then do example
	__ASSERT_ALWAYS(!error,User::Panic(_L("PAN Profile Example Application"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;

	return 0; // and return
	}

