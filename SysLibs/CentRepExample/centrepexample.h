// CentRepExample .h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

/**
@file
Contains the CCentRepExample class.
*/
#ifndef __CENTREPEXAMPLE_H__
#define __CENTREPEXAMPLE_H__

// Symbian OS includes
#include <centralrepository.h> 

/**
CCentRepExample demonstrates some uses of the CRepository class.
CRepository provides access to the Symbian OS central repository,
which is used to store system and application settings. 

The class demonstrates how to open a repository, read, write,
and delete settings, perform operations in a transaction, 
find settings, restore default settings, and request 
change notifications.
*/
class CCentRepExample :public CBase
	{
public:
	static CCentRepExample* NewLC();

	~CCentRepExample();
	void ResetL();
	void OpenRepositoryL();
	void RdChgSetL();
	void ReadSettingsL();
	void ChangeSettingsL();
	void MoveSettingsL();
	void ResetAndNotifyL();
	void FindSettingsL();
	void TransactionFuncL();
	void DeleteL();
	
private:
	CCentRepExample();
	void ConstructL();
private:
	/** Pointer to the console interface */
	CConsoleBase* 		iConsole;
	/** Pointer to the class that provides access to a repository. */
	CRepository* iRepository;
    };

#endif //__CENTREPEXAMPLE_H__
