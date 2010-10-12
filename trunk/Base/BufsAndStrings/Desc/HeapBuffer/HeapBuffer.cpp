// HeapBuffer.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.
// Examples to demonstrate the basic ideas of heap buffer descriptors.
#include "CommonFramework.h"
//
// Common literal text
//
_LIT(KTxtHelloWorld,"Hello World!");
_LIT(KTxtHelloWorldMorning,"Hello World! Morning");
_LIT(KTxtAndHi," & Hi");
//
// Common Format strings
//
_LIT(KTDesFormat,"Size()=%d;\nMaxLength()=%d\n");
_LIT(KTDesCFormat,"Ptr()=%x; Length()=%d; ");
_LIT(KDesCFormat_Ptr_Length_Size,"Ptr()=%x; Length()=%d; Size()=%d\n");

void example_hbufc_init(HBufC* &aHBufC)
	{
	// Construct an HBufC.
	// This descriptor can hold up to 15 data 
	// items.
	// The current length is zero

	aHBufC = HBufC::NewL(15);
	CleanupStack::PushL(aHBufC);
	// Note, we could replace the above two lines of code
	// with the single line: aHBufC  = HBufC::NewLC(15);
	// Show 1. Content
	//      2. address of descriptor
	//      3. address of descriptor data area
	//      4. length of descriptor
	//      5. size of descriptor
	// The address of the descriptor data area
	// is offset 4 from the start of the 
	// descriptor itself.
	_LIT(KFormat10,"\"%S\"; descriptor at %x; ");
	console->Printf(KFormat10, aHBufC, aHBufC);
	console->Printf(KDesCFormat_Ptr_Length_Size, aHBufC->Ptr(),
			aHBufC->Length(), aHBufC->Size());
	}

void example_hbufc_setupdata(HBufC* &aHBufC)
	{
	// Set up some data into the HBufC 
	*aHBufC = KTxtHelloWorld; // "Hello World!"

	// Show 1. Content
	//      2. length of descriptor
	//      3. size of descriptor
	_LIT(KFormat9,"\"%S\"; ");
	console->Printf(KFormat9, aHBufC);
	_LIT(KFormat8,"Length()=%d; Size()=%d\n");
	console->Printf(KFormat8, aHBufC->Length(), aHBufC->Size());
	}

void example_hbufc_assign_new_text(HBufC* &aHBufC)
	{
	// Now want to replace the text with:
	// "Hello World! Morning"
	// Resulting length would be > 15
	// So, reallocate the HBufc first
	// to make it bigger

	aHBufC = aHBufC->ReAllocL(20);

	// Assign the new text.
	*aHBufC = KTxtHelloWorldMorning; // "Hello World! Morning"

	// Show it
	_LIT(KFormat7,"\n\"%S\"; \n(1st realloc') desc'at %x; ");
	console->Printf(KFormat7, aHBufC, aHBufC);
	console->Printf(KDesCFormat_Ptr_Length_Size, aHBufC->Ptr(),
			aHBufC->Length(), aHBufC->Size());
	}

// Do the example
LOCAL_C void doExampleL()
	{

	// An HBufC is always constructed on the heap
	// using the static New(), NewL() or NewLC()
	HBufC* buf;

	example_hbufc_init(buf);
	example_hbufc_setupdata(buf);
	example_hbufc_assign_new_text(buf);

	// buf may point to the same area as before.
	// In general, this is not the case so DO 
	// NOT ASSUME. 

	buf = buf->ReAllocL(22);
	_LIT(KFormat6,"\n\"%S\"; \n(2nd realloc') desc'at %x; ");
	console->Printf(KFormat6, buf, buf);
	console->Printf(KDesCFormat_Ptr_Length_Size, buf->Ptr(), buf->Length(),
			buf->Size());

	// The Des() function returns a TPtr to the
	// HBufC.
	// The HBufC data can be changed through 
	// the TPtr.
	// The maximum length of the TPtr is 
	// determined from the size of the cell 
	// allocated to the data area of the HBufC.
	// In this example, the value has been rounded 
	// up to 24.
	TPtr ptr = buf->Des();
	_LIT(KFormat11,"TPtr descriptor at %x; ");
	console->Printf(KFormat11, &ptr);
	console->Printf(KTDesCFormat, ptr.Ptr(), ptr.Length());
	console->Printf(KTDesFormat, ptr.Size(), ptr.MaxLength());

	// Now change the HBufC data through
	// the TPtr. This is OK provided the length
	// of the changed data does not exceed the 
	// maximum length.
	//
	// The following change deletes the last 
	// nine characters and then appends 
	// the characters " & Hi".
	//
	// Note that the length of both the HBufC 
	// and the TPtr reflect the changed data.

	ptr.Delete((ptr.Length() - 9), 9);
	ptr.Append(KTxtAndHi); // " & Hi"

	// Look at it from HBufC's viewpoint
	_LIT(KFormat4,"\n\"%S\";\nHBufC descriptor at %x; ");
	console->Printf(KFormat4, buf, buf);

	console->Printf(KDesCFormat_Ptr_Length_Size, buf->Ptr(), buf->Length(),
			buf->Size());
	// Look at it from TPtr's viewpoint
	_LIT(KFormat3,"\"%S\"; \nTPtr  descriptor at %x; ");
	console->Printf(KFormat3, &ptr, &ptr);
	console->Printf(KTDesCFormat, ptr.Ptr(), ptr.Length());
	console->Printf(KTDesFormat, ptr.Size(), ptr.MaxLength());
	// Pop the HBufC off the cleanup stack
	// and destroy it (i.e. the HBufC)
	CleanupStack::PopAndDestroy();
	}

