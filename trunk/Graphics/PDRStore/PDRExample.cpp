// PDRExample.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#include "PDRExample.h"


CPdrExample::CPdrExample()
	{}

CPdrExample::~CPdrExample()
	{
	_LIT(KExit,"\n\nPress any key to exit the application ");
	iConsole->Printf(KExit);
	iConsole->Getch();
	
	// Free user defined objects
	delete iDriver;
	delete iStopper;
	RFbsSession::Disconnect();
	delete iConsole;
	}

CPdrExample* CPdrExample::NewL()
 	{
	CPdrExample* self= new (ELeave) CPdrExample();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CPdrExample::ConstructL()
	{
	iConsole = Console::NewL(KTitle,TSize(KConsFullScreen,KConsFullScreen));

	_LIT(KTextPressAKey, "\n\nPress any key to step through the example");
	iConsole->Printf ( KTextPressAKey );
	iConsole->Getch ();
	
	iConsole->Printf ( _L("\nLoading printer driver...\n") );
	
	// Load printer driver
	User::LoadPhysicalDevice(PDD_NAME);
	// Load printer device
	User::LoadLogicalDevice(LDD_NAME);
	FbsStartup();
	
	// Open a session with the font and bitmap server
	RFbsSession::Connect();		
	}
	
// Create a printer driver
void CPdrExample::CreatePrinterDriverL()
	{
		
	iStopper=new(ELeave) CPdrPrint();
	CActiveScheduler::Add(iStopper);

	// Create a printer driver object
	iDriver=CPrinterDriver::NewL();		
	
	TFileName filename( KDriveName );
	filename.Append( KDefaultPrinterDriverPath );
	filename.Append( KEpsonFileName );
	
	iConsole->Printf ( _L("\nOpening printer driver information file...\n") );
	
	// Open a printer driver information(.pdr) file
	TInt ret;
	TRAP(ret,iDriver->OpenPdrL( filename ));
	if ( ret == KErrNone )
		{
		iConsole->Printf(_L("\nNumber of models: %d\n"),iDriver->NumModels());		
		}

	// Get models
	TPrinterModelEntry entry;
	for (TInt i=0; i<iDriver->NumModels(); i++)
		{
		entry=iDriver->Model(i);
		iConsole->Printf(_L("Model: %S\n"),&entry.iModelName);
		}
	
	iConsole->Getch();
	iConsole->Printf(_L("\nSelect model from 1 to %d\n"),iDriver->NumModels());
	iConsole->Printf(_L("\nSelecting... %d\n"),iDriver->NumModels());
	iConsole->Printf(_L("\nPrinter driver properties:\n"));

	// Create a printer device		
	iDevice=NULL;
	entry=iDriver->Model(iDriver->NumModels()-1);
	TRAP(ret,iDevice=iDriver->CreatePrinterDeviceL(entry.iUid));
	if ( ret == KErrNone )
		{
		iConsole->Printf(_L("PDR device: %d\n"),iDevice);
		iConsole->Printf(_L("No. of typefaces: %d\n"),iDevice->NumTypefaces());
		iConsole->Printf(_L("Model name: %S\n"),&(iDevice->Model().iModelName));
		iConsole->Getch();
		
		TPageSpec pagespec(TPageSpec::EPortrait,KA4PaperSizeInTwips);
		iDevice->SelectPageSpecInTwips(pagespec);
	
		}	
	}

// Display supported typefaces 
void CPdrExample::DisplayTypefaces()
	{
	// Get typefaces 
	TTypefaceSupport support;
	iDevice->TypefaceSupport(support,0);
	TBuf<KMaxTypefaceNameLength> name;
		
	iConsole->Printf(_L("\nSupported typefaces:\n"));
	TInt i;
	for (i=0; i<iDevice->NumTypefaces(); i++)
		{
		iDevice->TypefaceSupport(support,i);
		name.Copy(support.iTypeface.iName);
		iConsole->Printf(_L(" %S\n"),&name);
		}
	iConsole->Getch();
	}

// Create a printer control	
void CPdrExample::CreatePrinterControl()
	{	
	CPrinterPort* printerport=NULL;

	if (iDevice->Model().iRequiresPrinterPort)
		{
		TInt ret;
		TRAP(ret,printerport=CFilePrinterPort::NewL(_L("c:\\temp.prn")));
		if (ret==KErrNone)
			{
			iConsole->Printf(_L("\nCreate a printer control to process and terminate the print job\n"));
			TRAP(ret,iDevice->CreateControlL(printerport));
			}		
		}
	}

	
// Create a graphic context, process and terminate the print job
void CPdrExample::PrintPages()
	{
	
	TBandAttributes attributes;
	CPrinterControl::TMoreOnPage moreonpage=CPrinterControl::EMoreOnPage;

	TInt h=iDevice->HorizontalPixelsToTwips(1000);
	TInt v=iDevice->VerticalPixelsToTwips(1000);
	TInt k=4800;

	TInt nuBands = iDevice->iControl->BandsPerPage();
	iConsole->Printf(_L("Number of bands per page: %d\n"), nuBands);
	iConsole->Printf(_L("Printing bands on the page...\n"));
	iConsole->Getch();

	TInt i=0;	
	do
		{
		iConsole->Printf(_L("Band %d\n"),i+1);
		moreonpage=iDevice->iControl->QueueGetBand(iStopper->Queue(),attributes);
		CActiveScheduler::Start();
		
		// Create a graphic context for breaking the drawing into bands
		CGraphicsContext* gc=NULL;
		iDevice->CreateContext((CGraphicsContext *&) gc);
		gc->SetPenColor(KRgbBlack);							 
		gc->SetPenStyle(CGraphicsContext::ESolidPen);
		gc->SetPenSize(TSize(4*k/h,4*k/v));
		gc->DrawEllipse(TRect(391,352,1152,601));	
		delete gc;
		i++;
		}
	while (moreonpage==CPrinterControl::EMoreOnPage);
	
	iConsole->Printf(_L("\nPrinting completed\n"));
	iDevice->iControl->QueueEndPrint(iStopper->Queue());
	CActiveScheduler::Start();
	}

LOCAL_C void MainL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );

	CPdrExample* app = CPdrExample::NewL();
	CleanupStack::PushL(app);
	
	// Load the printer driver
	app->CreatePrinterDriverL();
	// Display supported typefaces 
	app->DisplayTypefaces();
	// Create a printer control for handling the print jobs
	app->CreatePrinterControl();
	// Process and terminate the print job
	app->PrintPages();
	
	CleanupStack::PopAndDestroy(2,scheduler);
	}

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(cleanup == NULL)
    	{
    	return KErrNoMemory;
    	}
    TRAPD(err, MainL());
	if(err != KErrNone)
		{
		User::Panic(_L("Failed to complete"),err);
		}

    delete cleanup;
    __UHEAP_MARKEND;
    return KErrNone;
	}


