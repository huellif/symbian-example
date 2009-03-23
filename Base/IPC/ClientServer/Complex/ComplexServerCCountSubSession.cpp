// ComplexServer.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

#include "ComplexClientAndServer.h"
#include "ComplexServer.h"
#include <e32svr.h>
#include <e32uid.h>




//**********************************
//CCountSubSession implementation
//**********************************

// construction
CCountSubSession::CCountSubSession(CCountSession* aSession)
 : iSession(aSession)
 {
 }


/**
Initialize the counter with the numeric equivalent of the descriptor contents
This function is here to demonstrate reading from the client address space.
Note that in this example, the client and the server are part of the same process,
*/
void CCountSubSession::SetFromStringL(const RMessage2& aMessage)
	{
	   // length of passed descriptor (1st parameter passed from client)
	TInt deslen = aMessage.GetDesLength(0);
	
	  // Passed data will be saved in this descriptor.
    RBuf buffer;
      
      // Max length set to the value of "deslen", but current length is zero
    buffer.CreateL(deslen);
      
      // Do the right cleanup if anything subsequently goes wrong
    buffer.CleanupClosePushL();
    
      // Copy the client's descriptor data into our buffer.
    aMessage.ReadL(0,buffer,0);
    
      // Now do a validation to make sure that the string only has digits
    if (buffer.Length() == 0)
        {
    	User::Leave(ENonNumericString);
        }
    
    TLex16 lexer;
    
    lexer.Assign(buffer);
    while (!lexer.Eos())
        {
        TChar thechar;
        
        thechar = lexer.Peek();
        if (!thechar.IsDigit())
            {
        	User::Leave(ENonNumericString);
            }
        lexer.Inc();
        }
       
      // Convert to a simple TInt value. 
    lexer.Assign(buffer);           
    if (lexer.Val(iCount))
        {
    	User::Leave(ENonNumericString);
        }
    
	  // Clean up the memory acquired by the RBuf variable "buffer"
	CleanupStack::PopAndDestroy();
	}


/**
Increases the session counter by default (1)
*/
void CCountSubSession::Increase()
	{
	iCount++;
	}


/**
Increases the session counter by an integer.
*/
void CCountSubSession::IncreaseBy(const RMessage2& aMessage)
	{
	iCount+=aMessage.Int0();
	}


/**
Decreases the session counter by default (1)
*/
void CCountSubSession::Decrease()
	{
	iCount--;
	}


/**
Decreases the session counter by an integer.
*/
void CCountSubSession::DecreaseBy(const RMessage2& aMessage)
	{
	iCount-=aMessage.Int0();
	}


/**
Resets the session counter.
*/
void CCountSubSession::Reset()
	{
	iCount=0;
	}


/**
Writes the counter value to a descriptor in the client address space.
This function demonstrates writing to the client.
*/
void CCountSubSession::CounterValueL(const RMessage2& aMessage)
	{
	TPckgBuf<TInt> p(iCount);
	aMessage.WriteL(0,p);
	}


