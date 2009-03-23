// console.H
//
// Copyright (c) Symbian Software Ltd 2006.  All rights reserved.
//

#ifndef CONSOLE_H
#define CONSOLE_H

#define __NO_CONTROL_DURING_START__

#include <e32base.h>

class CPanConnections;
class CConsoleBase;
class CScabbyQueenDealer;
class CScabbyQueenPlayer;

/**
 * The CActiveConsole class allows for the user to input commands.
 * It owns a pointer to a CPanConnections for setting up a PAN.
 * It owns a pointer to a CScabbyQueenPlayer/Dealer for playing the game.
 */
class CActiveConsole : public CActive
	{
public:
	static CActiveConsole* NewLC(CConsoleBase* aConsole);
	~CActiveConsole();
	void IapStartedCallBack();
	void DoCancel();
	void RunL();
	void RequestCharacter();
	void ProcessKeyPressL(TChar aChar);
	void MenuLine();
	TUint32 GetIpAddress();
	TInt StartListening();
	CConsoleBase* Console();
	TInt GetCardNumber();
	
private:
	void ConstructL(CConsoleBase* aConsole);
	CActiveConsole(CConsoleBase* aConsole);
	void PlayerProcessL();

private:
	// Data members defined by this class
	TInt iCardNum;
	CConsoleBase*    iConsole;					// A console for reading from
	CPanConnections* iPanConnection;
	CScabbyQueenDealer* iDealer;
	CScabbyQueenPlayer* iPlayer;
	enum TMenuMode
		{// Application modes, used to determine which menu to display
		EPan,
		EScabbyQueen
		};
	enum TPlayerRole
		{// To display menu accordingly
		EDealer,
		EPlayer
		};
	enum
		{
		EListeningWithoutIAP,
		EIAPLoadingAONotActive,
		};
	TMenuMode iMode;
	TPlayerRole iPlayerRole;	

	};
	

#endif // CONSOLE_H