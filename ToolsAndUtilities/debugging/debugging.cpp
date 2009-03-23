// debugging.cpp
//
// Copyright (c) 1997-2003 Symbian Software Ltd.  All rights reserved.
//
// For demonstration purposes only.
//
/* 
This program demonstrates some deliberate device hardware only crashes.
To use, pick one of the options, each of which causes a different
type of crash:

option 1 - data abort - accessing non-existent memory
option 2 - data abort - stack overflow
option 3 - data abort - pointer in deleted heap cell
option 4 - data abort - uninitialised pointer on stack
option 5 - data abort - misaligned access to 32 bit word
option 6 - data abort - misaligned access to 16 bit word
option 7 - prefetch abort
option 8 - undefined instruction
*/

#include <e32base.h>
#include <e32cons.h>

_LIT(KTxtPressAnyKey," [press any key]\n");


LOCAL_D CConsoleBase* console;

void BadFunction1()
	{
	BadFunction1();
	}

#ifdef __ARMCC__
#pragma Ono_inline // prevent compile time errors
#endif

void BadFunction2()
	{
	// uninitialised pointer on stack - may not crash
	TInt* p;
	*p = 42; // warning here is intentional
	}
	
void BadFunction3(TUint8 *ps)
	{	
	TInt32* p = (TInt32*)ps;
	*p = 0x42;
	}
	
void BadFunction4(TUint8 *ps)
	{	
	TInt16* p = (TInt16*)ps;
	*p = 0x42;
	}

TInt E32Main()
	{
	typedef void (*TPfn)();
	CTrapCleanup* cleanupStack=CTrapCleanup::New();
	console = Console::NewL(_L("Debugging Demo"),TSize(KConsFullScreen, KConsFullScreen));
	console->Printf(_L("Hit 1-8 to determine function call\n"));
	TInt key = console->Getch();	
	console->Printf(_L("Key Hit %i\n"),key);

	switch (key)
		{
	default:
	case 49:
		{
		// option 1 - data abort - accessing non-existent memory
		TInt* r = (TInt*) 0x1000;
		*r = 0x42;
		}
		break;
	case 50:
		{
		// option 2 - data abort - stack overflow
		BadFunction1();
		}
		break;
	case 51:
		{
		// option 3 - data abort - pointer in deleted heap cell
		// May not crash on UREL builds
		struct S { TInt* iPtr; };
		S* p = new S;
		p->iPtr = new TInt;
		delete p->iPtr;
		delete p;
		*(p->iPtr) = 42;
		}
		break;
	case 52:
		{
		// option 4 - data abort - uninitialised pointer on stack
		BadFunction2();
		}
		break;
	case 53:
		{
		// option 5 - data abort - misaligned access to 32 bit word
		TUint8 buffer[16];
		BadFunction3(buffer+2);
		}
		break;
	case 54:
		{
		// option 6 - data abort - misaligned access to 16 bit word
		TUint8 buffer[16];
		BadFunction4(buffer+1);
 		}
		break;
	case 55:
		{
		// option 7 - prefetch abort
		TPfn f = NULL;
		f();
		}
		break;
	case 56:
		{
		// option 8 - undefined instruction
		TUint32 undef = 0xE6000010;
		TPfn f = (TPfn) &undef;
		f();
		}
		break;
		};
	
	console->Printf(KTxtPressAnyKey);
	console->Getch();
	delete cleanupStack;
	return 0;
	}
