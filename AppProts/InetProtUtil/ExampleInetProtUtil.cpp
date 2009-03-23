// ExampleInetProtUtil.CPP
//
// Copyright (c) Symbian Software Ltd 2005.  All rights reserved.
// 

/**
	ExampleInetProtUtil is a sample code for understanding the applicability of InetProtUtil. 

  	It is intended as an example and introduction to the INETPROTUTIL API's.

  	Users should have a brief understanding of URIs and the different concepts
  	associated with it, like, URLs and URI components. 
*/


// System includes
#include <e32base.h>
#include <e32cons.h>
// Local include
#include "ExampleInetProtUtil.h"

_LIT ( KTest, "InetProtUtil Example" );
_LIT ( KInetProtUtilExamplePanic, "EXAMPLES");
_LIT ( KDisplayAll, "\n%d %S %d %S " );
_LIT ( KLeaveALine, "\n" );

CExampleInetProtUtil::CExampleInetProtUtil()
	{
	}
	
CExampleInetProtUtil::~CExampleInetProtUtil()
	{
	delete iConsole;	
	}
	
void CExampleInetProtUtil::NewL()
	{
	CExampleInetProtUtil *self = new (ELeave) CExampleInetProtUtil;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::PopAndDestroy(self);
	}

void CExampleInetProtUtil::ConstructL()
	{	
	
	iConsole = Console::NewL(KTest,TSize(KConsFullScreen,KConsFullScreen));
	iConsole->Printf ( KTest );
	_LIT(KTextStart, "\nPress any key to step through the example");
	iConsole->Printf ( KTextStart );
	iConsole->Getch ();

	//Create an URI
	CreateUriL();  

	//Modify URI components
	ModifyUriComponentsL();
	
	//Resolve
	ResolveUriL();
	
	//Parse the URI
	ParseUriL();
	
	//Validate URI components
	ValidateUriComponentsL();
	
	//Extract URI components
	ExtractUriComponentsL();
	
	//Retrieve Filename from a given Uri
	RetrieveFileNameL();
	
	//Add and Trim delimiters
	ModifyDelimiterL();
	
	//Text Utilities
	WhiteSpaceRemover();
	
	//Escape encode and decode
	EscapeEncodeDecodeL();	
	}
	
	
//Create an URI
void CExampleInetProtUtil::CreateUriL()
	{
	//Set the physical path of the file
	
	_LIT(KText1, "\n\n\nThe Physical location of the file is....");
	iConsole->Printf ( KText1 );
	
	_LIT(KFullUriName, "K:\\ws\\direct\\direct.mmp");

	//display it 
	TBuf<40> desFullUriName(KFullUriName);
	iConsole->Printf ( KLeaveALine );
	iConsole->Printf ( KFullUriName );
	
	//create the Uri for the path
	CUri8* uri8 = CUri8::CreateFileUriL(desFullUriName); 
	
	const TDesC8& desUriDisplay = uri8->Uri().UriDes();
	TBuf16<100> desCreateUri;
	desCreateUri.Copy (desUriDisplay);
	
	//display it 
	_LIT(KText2, "And its Uri is....");
	iConsole->Printf ( KLeaveALine );
	iConsole->Printf ( KText2 );
	
	iConsole->Printf ( KLeaveALine );
	iConsole->Printf ( desCreateUri );
	
	delete uri8;
	
	iConsole->Getch ();
	}
	
//Modify URI components
void CExampleInetProtUtil::ModifyUriComponentsL()
	{	
	TUriParser8 parser1;
	CUri8* aUri = CUri8::NewL(parser1);
	
	_LIT(KText3, "\n\n\nAdding Uri components one by one....");
	iConsole->Printf ( KText3 );

	// Adding components to the Uri
	
	//Setting and displaying SCHEME
	_LIT8(KScheme, "http");
	_LIT(KScheme1, "\nSCHEME : http");
	iConsole->Printf ( KScheme1 );
	aUri->SetComponentL(KScheme, EUriScheme);
	
	//Setting and displaying HOST
	_LIT8(KHost, "www.symbian.com");
	_LIT(KHost1, "\nHOST : www.symbian.com");
	iConsole->Printf ( KHost1 );
	aUri->SetComponentL(KHost, EUriHost);
	
	//Setting and displaying PORT
	_LIT8(KPort, "80");
	_LIT(KPort1, "\nPORT : 80");
	iConsole->Printf ( KPort1 );
	aUri->SetComponentL(KPort, EUriPort);
	
	//Setting and displaying PATH
	_LIT8(KPath, "/developer/techlib/turic8class.html");
	_LIT(KPath1, "\nPATH : /developer/techlib/turic8class.html");
	iConsole->Printf ( KPath1 );
	aUri->SetComponentL(KPath, EUriPath);
	
	//Display the constucted Uri
	_LIT(KText4, "\nThe fully constructed Uri....");
	iConsole->Printf ( KText4 );
	const TDesC8& desUriDisplay = aUri->Uri().UriDes();
	TBuf16<100> desFullUri;
	desFullUri.Copy (desUriDisplay);
	
	iConsole->Printf( KLeaveALine );
	iConsole->Printf ( desFullUri );
	
	// Removal of component from the Uri
	iConsole->Getch ();
	_LIT(KText5, "\n\nUri with the Port number removed....\n");
	iConsole->Printf ( KText5 );
	aUri->RemoveComponentL(EUriPort);

	//Display the modified Uri
	const TDesC8& desRemovedComponentDisplay =aUri->Uri().UriDes();
	TBuf16<100> desRemovedComponent;
	desRemovedComponent.Copy (desRemovedComponentDisplay);
	iConsole->Printf ( desRemovedComponent );
	
	delete aUri;
	
	iConsole->Getch ();
	}
	
	
//Parse the URI
void CExampleInetProtUtil::ParseUriL()
	{
	//Create a Uri
	_LIT(KText6, "\n\n\nParsing the Uri for....\n");
	iConsole->Printf ( KText6 );

	_LIT(KFullUriName,"K:\\ws\\direct\\direct.mmp");
	TBuf<40> desFullUriName(KFullUriName);
	iConsole->Printf ( desFullUriName );
	
	//Convert from Unicode format to UTF-8 format
	HBufC8* convert8 = EscapeUtils::ConvertFromUnicodeToUtf8L(desFullUriName);
	TUriParser8 parser;

	//Parse the Uri
	TInt errResult = parser.Parse(*convert8);
	
	if (errResult==KErrNone)
	{
		_LIT(KTextParsing, "\nThis Uri has been parsed successfully");
		iConsole->Printf ( KTextParsing );
	}
	
	delete convert8;

	iConsole->Getch();
	}

//Validate URI components
void CExampleInetProtUtil::ValidateUriComponentsL()
	{
	TUriParser8 parser1;
	CUri8* aUri = CUri8::NewL(parser1);
	
	_LIT(KTextf, "\n\n\nValidating the Uri....\n");
	iConsole->Printf ( KTextf );
	
	// Adding components to the Uri
	
	//Adding Scheme
	_LIT8(KScheme, "http");
	aUri->SetComponentL(KScheme,EUriScheme);
	
	//Adding Host
	_LIT8(KHost, "waterlang.org");
	aUri->SetComponentL(KHost,EUriHost);
	
	//Adding Port
	_LIT8(KPort, "90");
	aUri->SetComponentL(KPort,EUriPort);
	
	//Adding Path
	_LIT8(KPath, "/turic8class.html");
	aUri->SetComponentL(KPath,EUriPath);
	
	//Adding Query
	_LIT8(KQuery, "bar=2&x=3");
	aUri->SetComponentL(KQuery,EUriQuery);
	
	//Adding Fragment
	_LIT8(KFragment, "fragment");
	aUri->SetComponentL(KFragment,EUriFragment);
	
	//Display the constructed Uri
	const TDesC8& desUriDisplays =aUri->Uri().UriDes();
	TBuf16<100> desValidate;
	desValidate.Copy (desUriDisplays);
	iConsole->Printf ( desValidate );
	
	// Validate() is not supported for HTTP, but only SIP and SIPS. 
	//The Parse() function itself validates the components and returns the 
	//appropriate result.
	TInt res = parser1.Parse(desUriDisplays);
	if (res==KErrNone)
	{
		_LIT(KText8, "\nThis Uri is VALID");
		iConsole->Printf ( KText8 );
	}
	
	delete aUri;
	
	iConsole->Getch();
	}

//Extract URI components
void CExampleInetProtUtil::ExtractUriComponentsL()
	{
	_LIT(KTextExtract, "\n\n\nExtracting from....\n");
	iConsole->Printf ( KTextExtract );
	
	//Create a Uri 
	_LIT(KUriName, "K:\\ws\\direct\\direct.mmp");
	TBuf<40> desUriName(KUriName);
	CUri8* uriName = CUri8::CreateFileUriL(desUriName);  
	
	//Display the Uri
	const TDesC8& uriDisplay = uriName->Uri().UriDes();
	TBuf16<100> desExtract;
	desExtract.Copy (uriDisplay);
	iConsole->Printf ( desExtract );
	
	//Parse the Uri
	TUriParser8* uriComponent = new(ELeave) TUriParser8();
	uriComponent->Parse(uriDisplay);
	
	//Extract the Scheme component from this Uri
	const TDesC8& host = uriComponent->Extract(EUriScheme);
	TBuf16<100> desEx;
	desEx.Copy (host);
		
	//Display the Component extracted
	_LIT(KTextEx, "\nThe extracted Scheme component is....\n");
	iConsole->Printf ( KTextEx );
	iConsole->Printf ( desEx );
	
	//delete fileName;
	delete uriComponent;
	delete uriName;
	
	iConsole->Getch();
	}
	
	
//Extract URI components
void CExampleInetProtUtil::RetrieveFileNameL()
	{
	_LIT(KTextRet, "\n\n\nRetrieving filename from....\n");
	iConsole->Printf ( KTextRet );
	
	//Create a Uri 
	_LIT(KUriRetName, "K:\\ws\\direct\\direct.mmp");
	TBuf<40> desUriRetName(KUriRetName);
	CUri8* uriRetName = CUri8::CreateFileUriL(desUriRetName);  
	
	//Display the Uri
	const TDesC8& uriDisp = uriRetName->Uri().UriDes();
	TBuf16<100> desRetrieve;
	desRetrieve.Copy (uriDisp);
	iConsole->Printf ( desRetrieve );
	
	//Parse the Uri
	TUriParser8* uriComp = new(ELeave) TUriParser8();
	uriComp->Parse(uriDisp);
	
	//Get or Extract the Filename from the Uri
	_LIT(KTextGetFilename, "\nGetting the filename....\n");
	iConsole->Printf ( KTextGetFilename );
	
	HBufC* fileName = uriComp->GetFileNameL();
	TPtr uriFileNameDisplay = fileName->Des();
	TBuf16<100> desFileName;
	desFileName.Copy (uriFileNameDisplay);
	iConsole->Printf ( desFileName );

	delete fileName;
	delete uriComp;
	delete uriRetName;
	
	iConsole->Getch();
	}
	
//Modify the Uri w.r.t Delimiters	
void CExampleInetProtUtil::ModifyDelimiterL()
	{
	// First set the delimiter before performing any operation
	_LIT(KTextDelimit1, "\n\n\nThe Delimiter is...\n");
	iConsole->Printf ( KTextDelimit1 );
	CExampleDelimiterModifier* delimiterModifyingObj = new(ELeave) CExampleDelimiterModifier;
	delimiterModifyingObj->SetDelimiter(TChar(';'));
	
	//Display the delimiter
	_LIT(KTextDelimit2, ";\n");
	iConsole->Printf ( KTextDelimit2 );
	
	_LIT(KTextDelimit, "to be checked in...\n");
	iConsole->Printf ( KTextDelimit );
	_LIT(KFullUriName,"K:\\ws\\direct\\direct.mmp;");
	TBuf<40> desFullUriName(KFullUriName);
	iConsole->Printf ( desFullUriName );
	
	//Then parse the Uri
	HBufC8* convert8 = EscapeUtils::ConvertFromUnicodeToUtf8L(desFullUriName);
	delimiterModifyingObj->Parse(*convert8);
	
	//Check if the delimiter is present in the front
	TBool checkFrontDelimiter = delimiterModifyingObj->CheckFrontDelimiter();
	
	//Check if the delimiter is present at the back
	TBool checkBackDelimiter = delimiterModifyingObj->CheckBackDelimiter();
	
	//Display the result
	if (!checkFrontDelimiter)
	{
		_LIT(KTextDelimit3, "\nNo delimiter in the front");
		iConsole->Printf ( KTextDelimit3 );
	}
	else
	{
		_LIT(KTextDelimit4, "\nDelimiter is present in the front");
		iConsole->Printf ( KTextDelimit4 );
	}
	if (!checkBackDelimiter)
	{
		_LIT(KTextDelimit5, "\nNo delimiter at the back");
		iConsole->Printf ( KTextDelimit5 );
	}
	else
	{
		_LIT(KTextDelimit6, "\nDelimiter is present at the back");
		iConsole->Printf ( KTextDelimit6 );
	}

	delete convert8;
	delete delimiterModifyingObj;
	
	iConsole->Getch();
	}
	
//Check for white spaces in the Uri
void CExampleInetProtUtil::WhiteSpaceRemover()
	{
	//Take an eg file and insert white spaces in the front and rear
	_LIT(KTextWhiteSpace, "\n\n\nThe Uri containing white spaces....\n");
	iConsole->Printf ( KTextWhiteSpace );
	
	_LIT(KFullUriPath,"     K:\\ws\\direct\\direct.mmp  ");
	TBuf<40> desFullUriPath(KFullUriPath);
	iConsole->Printf ( desFullUriPath );
	
	//Gets the Whitespaces on the right and left of the Uri
	TPtrC desFullUriName(KFullUriPath);
	
	//Check for white spaces in the front or on the left of the Uri
	TInt consumedWhiteSpacesLeft = InetProtTextUtils::RemoveWhiteSpace(desFullUriName,InetProtTextUtils::ERemoveLeft);
	
	//Check for white spaces at the back or on the right of the Uri
	TInt consumedWhiteSpacesRight = InetProtTextUtils::RemoveWhiteSpace(desFullUriName,InetProtTextUtils::ERemoveRight);
	_LIT(KTextAnd, " and ");
	_LIT(KTextInfo, " white spaces have been removed from the Uri");
	
	//Display the number of white spaces removed from Uri
	iConsole->Printf ( KDisplayAll,consumedWhiteSpacesLeft,&KTextAnd,consumedWhiteSpacesRight, &KTextInfo);
	
	iConsole->Getch();
	}
	
//Encode and Decode the Uri
void CExampleInetProtUtil::EscapeEncodeDecodeL()
	{
	//Take an eg file to encode it and then decode it....
	_LIT(KFullUriName,"K:\\ws\\direct\\direct.mmp");
	TBuf<40> desFullUriName(KFullUriName);
	
	//UTF-8 defines a mapping from sequences of octets to sequences of chars
	HBufC8* convert = EscapeUtils::ConvertFromUnicodeToUtf8L(desFullUriName);

	//Encode the eg Uri and display it
	_LIT(KTextEncode, "\n\n\nThe Encoded Uri is....\n");
	iConsole->Printf ( KTextEncode );
	HBufC16* encode = EscapeUtils::EscapeEncodeL(desFullUriName,EscapeUtils::EEscapeNormal);
	TPtr uriEncoded = encode->Des();
	TBuf16<100> desEncodedUri;
	desEncodedUri.Copy (uriEncoded);
	iConsole->Printf ( _L("%S"), &desEncodedUri );

	//Decode the eg Uri and display it
	_LIT(KTextDecode, "\nThe Decoded Uri is....\n");
	iConsole->Printf ( KTextDecode );
	
	HBufC16* decode = EscapeUtils::EscapeDecodeL(desFullUriName);
	TPtr uriDecoded = decode->Des();
	TBuf16<100> desDecodedUri;
	desDecodedUri.Copy (uriDecoded);
	iConsole->Printf ( _L("%S"), &desDecodedUri );

	delete decode;
	delete encode;
	delete convert;

	iConsole->Getch();
	iConsole->Printf ( KLeaveALine );
	}
	
//Resolve the Uri w.r.t a Base and a refernce Uri
void CExampleInetProtUtil::ResolveUriL()
	{
	_LIT(KTextResolve1, "\n\n\nThe Base and reference Uris are\n");
	iConsole->Printf ( KTextResolve1 );
	
	TUriParser8 parserResolve1;
	CUri8* aUriBase = CUri8::NewL(parserResolve1);

	//Adding Scheme
	_LIT8(KScheme, "http");
	aUriBase->SetComponentL(KScheme,EUriScheme);
	
	//Adding Host
	_LIT8(KHost, "symbian.com");
	aUriBase->SetComponentL(KHost,EUriHost);
	
	//Adding Port
	_LIT8(KPort, "90");
	aUriBase->SetComponentL(KPort,EUriPort);
	
	//Adding Path
	_LIT8(KPath, "/resolve.aspx");
	aUriBase->SetComponentL(KPath,EUriPath);
	
	const TDesC8& desBaseUri =aUriBase->Uri().UriDes();
	TBuf16<100> desBase;
	desBase.Copy (desBaseUri);
	iConsole->Printf ( desBase );
	
	iConsole->Printf( KLeaveALine );
	
	CUri8* aUriRef = CUri8::NewL(parserResolve1);
	
	//Adding Path
	_LIT8(KPath1, "/uris/base/reference/resolve.aspx");
	aUriRef->SetComponentL(KPath1,EUriPath);
	
	//Adding Query
	_LIT8(KQuery1, "bar=2&x=3");
	aUriRef->SetComponentL(KQuery1,EUriQuery);
	
	const TDesC8& desRefUri =aUriRef->Uri().UriDes();
	TBuf16<100> desRef;
	desRef.Copy (desRefUri);
	iConsole->Printf ( desRef );
	
	_LIT(KTextResolve, "\nThe Resolved Uri is....\n");
	iConsole->Printf ( KTextResolve );
	
	//Resolve the 2 Uri's to get a resultant uri
	CUri8* Uri8 = CUri8::ResolveL(aUriBase->Uri(),aUriRef->Uri());
	
	// Display the resultant Uri
	const TDesC8& desDisplayReslovedUri = Uri8->Uri().UriDes();
	TBuf16<100> desResolve;
	desResolve.Copy (desDisplayReslovedUri);
	iConsole->Printf ( desResolve );
	
	delete Uri8;
	delete aUriRef;
	delete aUriBase; 
	
	iConsole->Getch ();
	}

	
////////////////////////////////////////////
//				Main				      //
////////////////////////////////////////////

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* tc = CTrapCleanup::New();
	TRAPD(err, CExampleInetProtUtil::NewL());
	if (err != KErrNone)
		User::Panic(KInetProtUtilExamplePanic,err);
	delete tc;
	__UHEAP_MARKEND;
	return KErrNone;
	}

	
