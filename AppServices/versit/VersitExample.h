// VersitExample.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//
//

#if (!defined __VERSITEXAMPLE_H__)
#define __VERSITEXAMPLE_H__

#include <vcard.h>
#include <VERSIT.H>

class CExampleVersit : public CBase
	{
public:
	static CExampleVersit* NewL();
	void EgVersitL();
	~CExampleVersit();	
	
private:
	void ConstructL();
	void ExternalizeToFileL(const TDesC& aFile);
	void InternalizeFromFileL(RFile& aFile);
	void CreateAndExternalizeVCardL();
	void InternalizeVCardL();	
	
private:
	CVersitParser* iParser;
	RFs iFsSession;
	};
#endif /* __VERSITEXAMPLE_H__ */
