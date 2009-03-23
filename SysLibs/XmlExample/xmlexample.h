// xmlexample.h
// Copyright (c) Symbian Software Ltd 2007. All rights reserved.
//

#ifndef XMLEXAMPLE_H
#define XMLEXAMPLE_H

#include <Xml/ContentHandler.h>
#include <Xml/Parser.h>

class RFile;

/**
CXmlExample demonstrates the usage of the XML Framework to parse an XML and WBXML file.

CXmlExample implements the XML Framework's MContentHandler interface,  
which the parser calls to notify the application of parsing events, such
as the start of elements.
*/
class CXmlExample: public CBase, public Xml::MContentHandler
	{
public:
	static CXmlExample* NewL();
	~CXmlExample();

	// From MContentHandler
	void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode);
	void OnEndDocumentL(TInt aErrorCode);
	void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
	void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
	void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
	void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
	void OnSkippedEntityL(const RString& aName, TInt aErrorCode) ;
	void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
	void OnError(TInt aErrorCode);
	TAny* GetExtendedInterface(const TInt32 aUid);
	
	void ParseExampleDocumentsL();
	HBufC* Copy8To16LC(const TDesC8& aDes);

public:
	TBool iLeaveOnStartElement;
	TInt iNumElements;
	TInt iNumSkippedEntities;
	TInt iNumPrefixMappings;
	TInt iNumPrefixUnmappings;
	TInt iError;
	
private:
	CXmlExample();
	void ConstructL();
	/** Pointer to the console interface*/
	CConsoleBase*  iConsole;

	};
	
#endif // XMLEXAMPLE_H
