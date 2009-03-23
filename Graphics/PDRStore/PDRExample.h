// PDRExample.h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef __PDREXAMPLE_H__
#define __PDREXAMPLE_H__

#include <e32base.h>
#include <e32cons.h>

#include <pdrport.h>
#include <gdi.h>
#include <fbs.h>
#include <prnsetup.h>
#include <metafile.h>

#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")

_LIT( KTitle, "PdrExample" );
_LIT( KDriveName, "z:" );
_LIT( KEpsonFileName, "epson.pdr" );

class CPdrPrint;

// Demonstrates how to load of printer driver information files and creation
// of printer drivers
class CPdrExample :public CBase
	{
public:
	static CPdrExample* NewL();
	~CPdrExample();
	
	void CreatePrinterDriverL();	
	void DisplayTypefaces();
	void CreatePrinterControl();
	void PrintPages();
	

private:
	CPdrExample();
	void ConstructL();

private:
	CPdrPrint*  	iStopper;

	CConsoleBase* 	iConsole;	
	CPrinterDriver* iDriver;
	CPrinterDevice* iDevice;
    };
    
// Wrapper class of CActive
class CPdrPrint : public CActive
	{
public:
	CPdrPrint(): CActive(EPriorityLow) {}
	TRequestStatus& Status();	
	void DoCancel() {}
	TRequestStatus&  Queue(); 
	void RunL(); 
	};

// Inline implementation
void CPdrPrint::RunL()
	{
	CActiveScheduler::Stop();
	}
			
TRequestStatus& CPdrPrint::Queue()
	{
	SetActive();
	return iStatus;
	}
			
TRequestStatus& CPdrPrint::Status()
	{
	return iStatus;
	}

#endif //__PDREXAMPLE_H__