// xmlexample.cpp
//
// Copyright (c) Symbian Software Ltd 2007. All rights reserved.
//
/** 
@file
This example program demonstrates the use of XML Framework APIs. 
The code implements all the functions of MContentHandler class.

The example parses the XML file and WBXML file present in the specified path. To know how to 
write an XML file you can refer to this URL http://www.w3schools.com/xml/.
*/
#include "xmlexample.h"
#include <Xml/DocumentParameters.h>
#include <Xml/ParserFeature.h>
#include <Xml/MatchData.h>
#include <e32cons.h>

using namespace Xml;

_LIT(KTitle, "XML example");
_LIT(KStartKey, "\nThe example parses the given XML and WBXML files. Press any key to start\n");
_LIT(KPressAKey,"\nPress any key to continue \n");
_LIT(KExit,"\n Press any key to exit the application \n");
_LIT(KParsingXml,"\nParsing the XML file");
_LIT(KParsingWbXml,"\nParsing the WBXML file");
_LIT(KParsingMatch,"\nParsing the XML file using match data criteria");
_LIT(KError,"\n Could not open the XML file. Please check if the file exists.");

_LIT(KOnStartDoc,"\n<!-- CXmlExample::OnStartDocumentL -->\n");
_LIT(KOnEndDoc,"\n<!-- CXmlExample::OnEndDocumentL -->\n");
_LIT(KOnContent,"<!-- CXmlExample::OnContentL -->");
_LIT(KOnProcInstrn,"\n<!-- CXmlExample::OnProcessingInstructionL -->\n");
_LIT(KOnError,"CXmlExample::OnError - ERROR: code=%d\n");

_LIT(KVersion,"<?xml version=\"1.0\" encoding=\"%S\"?>\n");
_LIT(KStartElement,"\nStart of element: <%S>\n");
_LIT(KEndElement,"\nEnd of element: </%S>\n");
_LIT(KContent,"%S\n");
_LIT(KAttrib,"The attribute of element <%S>=\"%S\"");
_LIT(KProcInstrn,"<?%S %S?>\n");
_LIT(KOnSkipped, "Found skipped entity %S");
_LIT(KNameSpaceRep,"\nNamespace mapping reporting enabled\n ");
_LIT(KPrefixMap, "\nThe prefix is %S and URI is %S\n");
_LIT(KPrefixEnd, "End of prefix URI mapping %S\n");

_LIT(KSimpleXmlFile,    "z:\\private\\E80000AE\\XmlExample.xml");
_LIT(KSimpleWbXmlFile,    "z:\\private\\E80000AE\\XmlExample.wbxml");
_LIT8(KXmlMimeType, "text/xml");
_LIT8(KWbxmlMimeType, "text/wbxml");

/**
Allocates and constructs a CXmlExample object.
Initialises all member data to their default values.
*/	
CXmlExample* CXmlExample::NewL()
	{
	CXmlExample* self = new (ELeave) CXmlExample();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CXmlExample::CXmlExample()
	:iError(KErrNone)
	{
	}

void CXmlExample::ConstructL()
	{
	iConsole = Console::NewL(KTitle,TSize(KConsFullScreen,KConsFullScreen));
	iConsole->Printf ( KStartKey);
	iConsole->Getch ();
	}

/**	
Destructor
*/
CXmlExample::~CXmlExample()
	{
	iConsole->Printf(KExit);
	iConsole->Getch();
	delete iConsole;
	}

/**
A callback to indicate the start of the document.
It retrieves the XML version and encoding details and prints it to the console.

Implementation of the pure virtual function of MContentHandler.

@param aDocParam 	Specifies the various parameters of the document.
@param aErrorCode 	The error code. 
@leave				If aErrorCode is not KErrNone, the function leaves.
 */
void CXmlExample::OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	iConsole->Printf(KOnStartDoc);

	const TDesC8& encoding8 = aDocParam.CharacterSetName().DesC();
	HBufC* encoding16= Copy8To16LC(encoding8);

	iConsole->Printf(KVersion, encoding16);
	CleanupStack::PopAndDestroy(encoding16);

	}
	
/**
A callback to indicate the end of the document.

Implementation of the pure virtual function of MContentHandler.

@param aErrorCode 	The error code.
@leave				If aErrorCode is not KErrNone, the function leaves.
*/
void CXmlExample::OnEndDocumentL(TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	iConsole->Printf(KOnEndDoc);
	User::After(700000); // 0.7 seconds
	}

/**
A callback to indicate an element has been parsed.
It prints element details on the console.

Implementation of the pure virtual function of MContentHandler.

@param aElement 	A handle to the element's details.
@param aAttributes 	Attributes of the element.
@param aErrorCode 	The error code.
@leave 				If aErrorCode is not KErrNone, the function leaves.
*/
void CXmlExample::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);

	const TDesC8& localName8 = aElement.LocalName().DesC();
	
	HBufC* localName16 = Copy8To16LC(localName8);
	
	iConsole->Printf(KStartElement,localName16);

	CleanupStack::PopAndDestroy();  // Delete localName16

	TInt nAttributes = aAttributes.Count();
	for(TInt i=0; i<nAttributes; i++)
		{
		const RAttribute& attribute = aAttributes[i];
		const RTagInfo& nameInfo = attribute.Attribute();

		const TDesC8& localName8 = nameInfo.LocalName().DesC();
		const TDesC8& value8 = attribute.Value().DesC();

		HBufC* localName16 = Copy8To16LC(localName8);
		HBufC* value16 = Copy8To16LC(value8);

		iConsole->Printf(KAttrib, localName16, value16);
		CleanupStack::PopAndDestroy(2,localName16);
		}
	User::After(700000); // 0.7 sec

	}

/**
A callback to indicate the end of the element has been reached.
Prints element end tag to the console.

Implementation of the pure virtual function of MContentHandler.

@param aElement 	The handle to the element's details.
@param aErrorCode 	The error code.
@leave 				If aErrorCode is not KErrNone, the function leaves.
 */
void CXmlExample::OnEndElementL(const RTagInfo& aElement, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	
	const TDesC8& localName8 = aElement.LocalName().DesC();
	HBufC* endBuf16 = Copy8To16LC(localName8);
	
	iConsole->Printf(KEndElement,endBuf16);
	
	 CleanupStack::PopAndDestroy(endBuf16);
	}

/**
Copies 8 bit descriptor data to 16 bit descriptor
@leave 	KErrNoMemory, User 11
*/
HBufC* CXmlExample::Copy8To16LC(const TDesC8& aDes)
	{
 	HBufC* buf16 = HBufC::NewLC(aDes.Length());
 	buf16->Des().Copy(aDes);
 	return buf16; 
	}


/**
A callback that sends the content of the element and prints element content data to the console.
Not all the content may be returned in one go. The data may be sent in chunks.
When an OnEndElementL is received this means there is no more content to be sent.

Implementation of the pure virtual function of MContentHandler.

@param aData8		The raw content data for the element.
					The client is responsible for converting the data to the
					required character set if necessary.
					In some instances the content may be binary and 
					must not be converted.
@param aErrorCode	The error code.
@leave 				If aErrorCode is not KErrNone, the function leaves.
 */
void CXmlExample::OnContentL(const TDesC8& aData8, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	iConsole->Printf(KOnContent);
	
	HBufC* data16 = Copy8To16LC(aData8);
	
	iConsole->Printf(KContent,data16);	
	
	CleanupStack::PopAndDestroy(data16);
	
	}

/**
A receive notification of a processing instruction.
It prints the target and data to the console.

Implementation of the pure virtual function of MContentHandler.

@param aTarget		The processing instruction target.
@param aData 		The processing instruction data. If empty, none was supplied.
@param aErrorCode 	The error code.
@leave 				If aErrorCode is not KErrNone, the function leaves.
 */
 void CXmlExample::OnProcessingInstructionL(const TDesC8& aTarget8, const TDesC8& aData8, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	iConsole->Printf(KOnProcInstrn);


	HBufC* target16 = Copy8To16LC(aTarget8);
	HBufC* data16 = Copy8To16LC(aData8);

	iConsole->Printf(KProcInstrn,target16,data16);
	CleanupStack::PopAndDestroy(2,target16);
	User::After(700000); // 0.7 sec
	}

/**
Implementation of the pure virtual function of MContentHandler.
Indicates that an error has occurred.

@param aError 	The error code
*/
void CXmlExample::OnError(TInt aError)
	{
	iError = aError;
	iConsole->Printf(KOnError,aError);
	iConsole->Printf(KPressAKey);
	iConsole->Getch();
	}

/** 
Obtain the interface matching the specified uid.
*/
TAny* CXmlExample::GetExtendedInterface(const TInt32 /*aUid*/)
	{
	// do nothing.
	return NULL;
	}
	
/**
A notification of the beginning of the scope of a prefix-URI Namespace mapping.
Always called before the corresponding OnStartElementL method.

Implementation of the pure virtual function of MContentHandler.

@param aPrefix 		The Namespace prefix being declared.
@param aUri 		The Namespace URI the prefix is mapped to.
@param aErrorCode	The error code.
@leave 				KErrNotSupported
 */
void CXmlExample::OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	const TDesC8& localPrefix8 = aPrefix.DesC();
	HBufC* temp1 = Copy8To16LC(localPrefix8);
	const TDesC8& localUri8 = aUri.DesC();
	HBufC* temp2 = Copy8To16LC(localUri8);
	
	iConsole->Printf(KPrefixMap, temp1, temp2);
	CleanupStack::PopAndDestroy(2, temp1);
	
	iConsole->Printf(KPressAKey);
	iConsole->Getch();
		
	}
	
/**
Notification of the end of the scope of a prefix-URI mapping.
*/
void CXmlExample::OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	const TDesC8& localPrefix8 = aPrefix.DesC();
	HBufC* temp = Copy8To16LC(localPrefix8);
	iConsole->Printf(KPrefixEnd, temp);
	CleanupStack::PopAndDestroy(temp);
	}

/**
A notification of ignorable whitespace in element content. 
*/
void CXmlExample::OnIgnorableWhiteSpaceL(const TDesC8&, TInt)
	{
	
	}

/**
A notification of a skipped entity. If the parser encounters an
external entity, it does not need to expand it - it can return the entity as aName
for the client to deal with.

Implementation of the pure virtual function of MContentHandler.

@param aName 		Name of the skipped entity.
@param aErrorCode 	The error code.
@leave 				KErrNotSupported
*/
void CXmlExample::OnSkippedEntityL(const RString& aName, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	
	const TDesC8& localName8 = aName.DesC();
	HBufC* temp = Copy8To16LC(localName8);
	
	iConsole->Printf(KOnSkipped, temp);
	CleanupStack::PopAndDestroy(temp);
	User::After(700000); // 0.7 sec delay		
	}

/**
Passes an XML and a WBXML file to the framework for parsing.
*/
void CXmlExample::ParseExampleDocumentsL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile handle;
	TInt err;
	// Open an example XML file for parsing
	TRAP(err,handle.Open(fs, KSimpleXmlFile, EFileShareReadersOnly));
	if (err != KErrNone)
		{
		iConsole->Printf(KError);
		User::Leave(err);
		}
	CleanupClosePushL(handle);

	// Open parser session for text/xml mime type
	CParser* parser = CParser::NewLC(KXmlMimeType, *this);
	
	iConsole->Printf(KParsingXml);
	iConsole->Printf(KPressAKey);
	iConsole->Getch();
	
	err = parser->EnableFeature(EReportNamespaceMapping);
	 
	if(err==KErrNone) 
	{
	iConsole->Printf(KNameSpaceRep);
	} 

	// Starts parser session
	ParseL(*parser, handle); // this will result in callbacks to handler
	
	// Closes the parser session.
	CleanupStack::PopAndDestroy(parser);
	CleanupStack::Pop(&handle);

	// Open an example WBXML file for parsing
	TRAP(err,handle.Open(fs, KSimpleWbXmlFile, EFileShareReadersOnly));
	if (err != KErrNone)
		{
		iConsole->Printf(KError);
		User::Leave(err);
		}
	CleanupClosePushL(handle);

	parser = CParser::NewLC(KWbxmlMimeType, *this);
	
	iConsole->Printf(KParsingWbXml);
	iConsole->Printf(KPressAKey);
	iConsole->Getch();
	
	// Starts parser session and active scheduler.
	ParseL(*parser, handle); // this will result in callbacks to handler

	CleanupStack::PopAndDestroy(parser);
	CleanupStack::Pop(&handle);

	// Open an example XML file for parsing using a parser specified with 
	// MatchData. In this case, the default Symbian OS parser is requested.
	
	TRAP(err,handle.Open(fs, KSimpleXmlFile, EFileShareReadersOnly));
	if (err != KErrNone)
		{
		iConsole->Printf(KError);
		User::Leave(err);
		}	
	CleanupClosePushL(handle);

	CMatchData *matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlMimeType);
	matchData->SetVariantL(_L8("Symbian"));
	
	iConsole->Printf(KParsingMatch);
	iConsole->Printf(KPressAKey);
	iConsole->Getch();
	
	parser = CParser::NewLC(*matchData, *this);
	ParseL(*parser, handle);
	
	CleanupStack::PopAndDestroy(2,matchData);
	CleanupStack::PopAndDestroy(&handle);
	CleanupStack::PopAndDestroy(&fs);
	
	}

LOCAL_C void MainL()
	{
	CXmlExample* app = CXmlExample::NewL();
	CleanupStack::PushL(app);
	
	app->ParseExampleDocumentsL();
	CleanupStack::PopAndDestroy(app);

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



