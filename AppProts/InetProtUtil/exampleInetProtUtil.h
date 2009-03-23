
// ExampleInetProtUtil.h
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
//
#include <Uri8.h>
#include <DelimitedParser8.h>
#include <inetprottextutils.h>
#include <EscapeUtils.h>
#include <e32cons.h>


////////////////////////////////////////////////////////
//				CExampleInetProtUtil			      //
////////////////////////////////////////////////////////

class CExampleInetProtUtil : public CBase
{	
public :

	CExampleInetProtUtil();
	
	virtual ~CExampleInetProtUtil();
	
	void ConstructL();
	
	static void NewL();
	
	//Create an URI
	void CreateUriL();
	
	//Modify URI components
	void ModifyUriComponentsL();
	
	//Resolve
	void ResolveUriL();
	
	//Parse the URI
	void ParseUriL();
	
	//Validate URI components
	void ValidateUriComponentsL();
	
	//Extract URI components
	void ExtractUriComponentsL();
	
	//Retrieve Filename from a given Uri
	void RetrieveFileNameL();
	
	//Add and Trim delimiters
	void ModifyDelimiterL();
	
	//Text Utilities
	void WhiteSpaceRemover();
	
	//Escape encode and decode
	void EscapeEncodeDecodeL();
	
private:
	CConsoleBase* iConsole;
	
};
	
	
	
////////////////////////////////////////////////////////
//			 CExampleDelimiterModifier	     	      //
////////////////////////////////////////////////////////
	
	
class CExampleDelimiterModifier : public TDelimitedParserBase8 //, public CDelimitedDataBase8
{
public:	// Methods

	inline TBool CheckFrontDelimiter() const;

	inline TBool CheckBackDelimiter() const;

    CExampleDelimiterModifier();

	inline void Parse(const TDesC8& aData);

	inline void SetDelimiter(TChar );
	
};

void CExampleDelimiterModifier::SetDelimiter(TChar aChar)
{
	TDelimitedParserBase8::SetDelimiter(aChar);
}
	
void CExampleDelimiterModifier::Parse(const TDesC8& aData)
{
	TDelimitedParserBase8::Parse(aData);
}
	
CExampleDelimiterModifier::CExampleDelimiterModifier()
{
	
}
	
TBool CExampleDelimiterModifier::CheckFrontDelimiter() const
{
		TBool frontDelimiter = FrontDelimiter();
		return frontDelimiter;
}

TBool CExampleDelimiterModifier::CheckBackDelimiter() const
{
		TBool backDelimiter = BackDelimiter();
		return backDelimiter;
}
