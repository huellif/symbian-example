// Currency.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

#include "CommonFramework.h" // standard example framework

// advance declarations
void printCurrency();
void formatCurrency(TDes &aBuffer, TReal currencyAmount);


LOCAL_C void doExampleL()
    {
		// construct and initialize application data
		// Locale information includes whether there is a space between 
		// currency symbol and amount, whether negative currency amounts 
		// are enclosed in brackets, and whether digits to left of decimal 
		// separator are grouped in threes ("Triads"). 
	TLocale locale;	// locale information
	TCurrencySymbol currencySymbol;
	currencySymbol.Set();	// Get system wide currency symbol setting
	locale.SetCurrencySymbolPosition(ELocaleBefore); 
	locale.SetCurrencySpaceBetween(EFalse);	
	locale.SetNegativeCurrencyFormat(TLocale::ELeadingMinusSign);
	locale.SetCurrencyDecimalPlaces(2);	
	locale.SetCurrencyTriadsAllowed(ETrue);	
	locale.SetThousandsSeparator(',');
	locale.SetDecimalSeparator('.');
	locale.Set();			// set system default settings
	printCurrency();
	}

void printCurrency()
	{
	TBuf<30> aBuffer; // receives formatted currency string
	aBuffer.Zero(); // empty buffer
	TReal currencyAmount=-12345678.119;
	formatCurrency(aBuffer, currencyAmount);
	_LIT(KFormat1,"Currency value is: %S\n");
	console->Printf(KFormat1,&aBuffer);
	}
		
void formatCurrency(TDes &aBuffer, TReal currencyAmount)
	{
		//
		// Format the currency starting with the currency symbol 
		//
	TLocale locale;			// System locale settings
	TRealFormat realFormat; 
		//
		// Set up a TRealFormat object from locale information.
		// This involves setting decimal and thousands separators, 
		// whether triads are allowed or not and number of decimal places.
		//
	realFormat.iType=KRealFormatFixed; // converts number to the form
	                                   //"nnn.ddd" (n=integer, d=decimal) 
	realFormat.iWidth=30;	           // Max. number of characters allowed
									   // to  represent the number
	realFormat.iPlaces=locale.CurrencyDecimalPlaces(); 
	realFormat.iPoint=locale.DecimalSeparator(); 
	realFormat.iTriad=locale.ThousandsSeparator();
	realFormat.iTriLen=(locale.CurrencyTriadsAllowed() ? 1 : 0); 
	TCurrencySymbol symbol;			  // get currency symbol from
									  // system setting
			
	_LIT(KTxtOpenBra,"(");
	_LIT(KTxtSpace," ");
	_LIT(KTxtCloseBra,")");
	_LIT(KTxtMinusSign,"-");
									  // Negative currency amounts may
									  // be enclosed in brackets.
									  // Currency symbol can appear before or
	                                  // after the value.
	                                  // We can have spaces between the currency
	                                  // symbol and the value.
	                                 
                                      
    TUint currencySymbolAtFront;
    TUint spaceBetweenSymbolAndValue;

                                      //
                                      // setup some useful values.    
                                      //
    currencySymbolAtFront      = ((locale.CurrencySymbolPosition()==ELocaleBefore) ? 0x01 : 0x00);
    spaceBetweenSymbolAndValue = (locale.CurrencySpaceBetween() ? 0x01 : 0x00);
                                      
                                      //
                                      // Deal with negative values
                                      //
    if (currencyAmount<0)
        {
                                      // Check if position of currency symbol needs to swap for 
                                      // for negavtive values
        currencySymbolAtFront ^= (locale.NegativeCurrencySymbolOpposite() ? 0x01 : 0x00);
        
                                      // Check if we need a space between currency symbol
                                      // and value.
        if (spaceBetweenSymbolAndValue && locale.NegativeLoseSpace())
            {
            spaceBetweenSymbolAndValue = 0x00;
            }
        
                                      // Now lay out the negative value as instructed. 
        switch (locale.NegativeCurrencyFormat())
            {
            case TLocale::ELeadingMinusSign :
                {
                aBuffer.Append(KTxtMinusSign);
	            if (currencySymbolAtFront)
	                {
	                aBuffer.Append(symbol);
	                if (spaceBetweenSymbolAndValue)
	                    {
	                    aBuffer.Append(KTxtSpace);
	                    }
	                aBuffer.AppendNum(-currencyAmount,realFormat);
	                }
	            else
	                {
	                aBuffer.AppendNum(-currencyAmount,realFormat);
	                if (spaceBetweenSymbolAndValue)
	                    {
	                    aBuffer.Append(KTxtSpace);
	                    }
	                aBuffer.Append(symbol);
	                }
                break;
                }
               
            case TLocale::ETrailingMinusSign :
                {
                if (currencySymbolAtFront)
                    {
                    aBuffer.Append(symbol);
                    if (spaceBetweenSymbolAndValue)
	                    {
	                    aBuffer.Append(KTxtSpace);
	                    }
	                aBuffer.AppendNum(-currencyAmount,realFormat);
	                aBuffer.Append(KTxtMinusSign);
                    }
                else
                    {
                    aBuffer.AppendNum(-currencyAmount,realFormat);
	                aBuffer.Append(KTxtMinusSign);
	                if (spaceBetweenSymbolAndValue)
	                    {
	                    aBuffer.Append(KTxtSpace);
	                    }
	                aBuffer.Append(symbol);
                    }
                break;
                }
            
            case TLocale::EInterveningMinusSign :
                {
                if (currencySymbolAtFront)
                    {
                    aBuffer.Append(symbol);
                    if (spaceBetweenSymbolAndValue)
	                    {
	                    aBuffer.Append(KTxtSpace);
	                    }
	                aBuffer.AppendNum(currencyAmount,realFormat);        
                    }
                else
                    {
                    aBuffer.AppendNum(currencyAmount,realFormat);        
                    if (spaceBetweenSymbolAndValue)
	                    {
	                    aBuffer.Append(KTxtSpace);
	                    }
	                aBuffer.Append(symbol);
                    }
                break;
                }
                
            default : // EInBrackets is the only remaining option
                {
                aBuffer.Append(KTxtOpenBra);
                if (currencySymbolAtFront)
                    {
                    aBuffer.Append(symbol);
                    if (spaceBetweenSymbolAndValue)
	                    {
	                    aBuffer.Append(KTxtSpace);
	                    }
                    aBuffer.AppendNum(-currencyAmount,realFormat);
                    }
                else
                    {
                    aBuffer.AppendNum(-currencyAmount,realFormat);
                    if (spaceBetweenSymbolAndValue)
	                    {
	                    aBuffer.Append(KTxtSpace);
	                    }
                    aBuffer.Append(symbol);
                    }
		        aBuffer.Append(KTxtCloseBra);
		        break;
                }
            
            }
        }
        
                                      //
                                      // Deal with zero or postive values
                                      //
    else
        {
        if (currencySymbolAtFront)
            {
            aBuffer.Append(symbol);
            if (spaceBetweenSymbolAndValue)
	            {
	            aBuffer.Append(KTxtSpace);
	            }
            aBuffer.AppendNum(currencyAmount,realFormat);
            }
        else
            {
            aBuffer.AppendNum(currencyAmount,realFormat);
            if (spaceBetweenSymbolAndValue)
	            {
	            aBuffer.Append(KTxtSpace);
	            }
            aBuffer.Append(symbol);
            }
        }
        	
 	}	
