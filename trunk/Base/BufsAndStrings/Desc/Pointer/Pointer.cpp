// Pointer.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

// Examples to demonstrate the basic ideas of
// pointer descriptors. 
//

#include "CommonFramework.h"

//
// Common literal text
//
_LIT(KTxtHiTHere,"Hi there");
_LIT(KTxtHaveNiceDay,"Have a nice day!");

//
// Common format strings
//
_LIT(KCommonFormat1,"\"%s\"\n");
_LIT(KCommonFormat3,"length=%d; size=%d;\n");
_LIT(KCommonFormat5,"Ptr()=%x; Length()=%d; Size()=%d\n");
_LIT(KCommonFormat16,"MaxLength()=%d\n");
_LIT(KCommonFormat17,"\"%S\"; ");
_LIT(KCommonFormat18,"Length()=%d; Size()=%d; ");

// Do the example
LOCAL_C void doExampleL()
    {
    				// Define a constant C style (ASCII) string
	const TText8* cstr8 = (TText8*)"Hello World!";

	  			// Look at it.

	TBuf<12> temp;
	temp.Copy(TPtrC8(cstr8));
	console->Printf(temp);

				// A TPtrC8 descriptor represents the
		        // ASCII text; contrast this with the
		        // basic C string  
	TPtrC8 ptrC8(cstr8);

				// Look at:
				//   1. Address of C string
				//   2. Length of the C string
				//   3. Size of the string.
				// Size is 13 bytes to allow for 
				// the terminating NULL.
	            // 
	_LIT(KFormat2,"\nNarrow C string at %x; ");
	console->Printf(KFormat2,cstr8);
	console->Printf(KCommonFormat3,12,sizeof("Hello World!"));

				// Look at:
				//   1. Address of descriptor
				//   2. Address of descriptor data area
				//   3. Length  of descriptor
				//   4. Size of descriptor
				// Address of descriptor data area is the 
				// same as the address of the C string. 
				// The Size of the descriptor data is only
				// 12 bytes (1 byte for each character).
	_LIT(KFormat4,"8-bit pointer-descriptor at %x; ");
	console->Printf(KFormat4,&ptrC8);
	console->Printf(KCommonFormat5,ptrC8.Ptr(),ptrC8.Length(),ptrC8.Size());		
								
		
				// Define a constant C style (Wide) string
	const TText16*  cstr16 = (TText16*)L"Hello World!";

				// A TPtrC16 descriptor represents the
		        // wide (i.e. double-byte character) text;
		        // contrast this with the basic C string.  
	TPtrC16 ptrC16(cstr16);
	
	_LIT(KFormat7,"\nWide C string at %x; ");
	console->Printf(KFormat7,cstr16);
	console->Printf(KCommonFormat3,12,sizeof(L"Hello World!"));
	
				// Look at:
				//   1. Address of descriptor
				//   2. Address of descriptor data area
				//   3. Length  of descriptor
				//   4. Size of descriptor
				// Address of descriptor data area is the
				// same as the address of the C string.
				// The size of descriptor data is only
				// 24 bytes (2 bytes for each character).
	_LIT(KFormat6,"16-bit pointer-descriptor at %x; ");
	console->Printf(KFormat6,&ptrC16);
	console->Printf(KCommonFormat5,ptrC16.Ptr(),ptrC16.Length(),ptrC16.Size());		
	
		
				// Use the _S macro to define a constant 
				// C style string of the appropriate width.
				// The TText variant is defined at build
				// time	as  TText16 
				// (In earlier times this could also have been TText8,
				//  but in all current versions, TText16 is the standard).
	const TText* cstr = _S("Hello World!"); 

				// TPtrC descriptor represents the text;
				// the TPtrC variant is defined at build 
				// time as TPtrC16.
	TPtrC ptrc(cstr);
	
	_LIT(KFormat8,"\nBuild-dependent TText at %x; ");
	console->Printf(KFormat8,cstr);
	console->Printf(KCommonFormat3,12,sizeof( 
					   		          #ifdef _UNICODE
							          L"Hello world!"
							          #else
							          "Hello world!"
							          #endif
	                   		          )
				   );
	
	 			// Look at descriptor basics.
	_LIT(KTxtBuildDependentptdesc,"Build-dependent pointer-descriptor");
	console->Printf(KTxtBuildDependentptdesc);
	
	_LIT(KFormat9," at %x; ");
	console->Printf(KFormat9,&ptrc);
	_LIT(KFormat10,"Ptr()=%x;\n");
	console->Printf(KFormat10,ptrc.Ptr());
	_LIT(KFormat11," Length()=%d; Size()=%d\n");
	console->Printf(KFormat11,ptrc.Length(),ptrc.Size());

				// The _LIT macro is most useful. It constructs
	            // a family of constant descriptor classes TLitC<Tint>. 
	            // The constant generated by the _LIT macro:
	            //   1. can be passed as 'const TDesC&' type
	            //   2. has an address-of operator 'operator&()' which returns a 'const TDesC*' type
	            //   3. can be passed as a TRefByValue<const TDesC>
				//
				// The _L macro constructs a TPtrC but should be avoided 
	            // where possible on the grounds of efficiency.
	            // Use the _LIT macro instead.
				
	console->Printf(_L("\nThe _L macro constructs a TPtrC"));
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	_LIT(KTxtTPtrBasicConcepts,"\n-->TPtr basic concepts");
	console->Printf(KTxtTPtrBasicConcepts);
	_LIT(KTxtPressToContinue," (press any key to continue)\n");
	console->Printf(KTxtPressToContinue);
	console->Getch();
					
				// Set up an area and initialise to a 
				// C style string (including the NULL).
				// The need for the NULL forces cstr to
				// have a length of 16.
	TText str[16] =  {'H', 'a', 'v', 'e', ' ', 'a',
					  ' ', 'n', 'i', 'c', 'e',
					  ' ', 'd', 'a', 'y', '\0'};

				// Look at it.
	console->Printf(KCommonFormat1,&str[0]);

 				// TPtr descriptor represents the text.
 				// Descriptor length is 15 but max length
 				// is 16. The descriptor does not need the
 				// terminating NULL, so in this example,
 				// the last data position is spare. 
 	TPtr  ptr(&str[0],15,16);

				// Look at:
				//   1. Address of the C string
				//   2. Length of the C string
				//   3. Size of the string
				// Size is 16 bytes to allow for
				// the terminating NULL.
	_LIT(KFormat12,"C string at %x; ");
	console->Printf(KFormat12,str);
	_LIT(KFormat13,"length=%d; size=%d\n");
	console->Printf(KFormat13,15,sizeof(str));

     			// Look at:
				//   1. Address of descriptor
				//   2. Address of descriptor area
				//   3. Length of descriptor
				//   4. Size of descriptor
				//   5. Max length of descriptor 
				// Address of descriptor data area is the 
				// same as the address of cstr[]. The
				// descriptor length is 15 but the maximum
				// length is 16.
	_LIT(KFormat14,"Descriptor at %x; ");
	console->Printf(KFormat14,&ptr);
	_LIT(KFormat15,"Ptr()=%x; Length()=%d; Size()=%d; ");
	console->Printf(KFormat15,ptr.Ptr(),ptr.Length(),ptr.Size());
	console->Printf(KCommonFormat16,ptr.MaxLength());

				// The data can be replaced using the 
				// assignment operator. Note the reference to
	            // the TLitC<> KTxtHiTHere constructed using the _LIT macro.
	ptr = KTxtHiTHere;			 

				// Length of descriptor is now 8 but maximum
				// length remains at 16.
				// Size is 16 
				// Text in ptr's data area (i.e. in the area 
				// defined by str[]) is 
				// now "Hi there")
	console->Printf(KCommonFormat17,&ptr);
	console->Printf(KCommonFormat18,ptr.Length(),ptr.Size());
	console->Printf(KCommonFormat16,ptr.MaxLength());
					
				// Length can be changed; data represented 
				// by the descriptor is now "Hi"
	ptr.SetLength(2);
	            
	console->Printf(KCommonFormat17,&ptr);
	console->Printf(KCommonFormat18,ptr.Length(),ptr.Size());
	console->Printf(KCommonFormat16,ptr.MaxLength());

				// Length can be set to zero; NO data 
				// is now represented by the
				// descriptor but maximum length is still 16
	ptr.Zero();
	console->Printf(KCommonFormat17,&ptr);
	console->Printf(KCommonFormat18,ptr.Length(),ptr.Size());
	console->Printf(KCommonFormat16,ptr.MaxLength());

				// Replace text with text of length 16, 
				// the maximum. 
	ptr = KTxtHaveNiceDay;
 	console->Printf(KCommonFormat17,&ptr);
	console->Printf(KCommonFormat18,ptr.Length(),ptr.Size());
	console->Printf(KCommonFormat16,ptr.MaxLength());

				// adding another character causes panic !!
				// length would be > maximum.
				// 
				// Remove the "//" marks on the next line
				// to see this happen
	//ptr.Append('@');
    }


    




	
	