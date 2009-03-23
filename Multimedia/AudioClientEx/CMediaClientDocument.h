// CMediaClientDocument.h
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
//

#ifndef CMEDIACLIENTDOCUMENT
#define CMEDIACLIENTDOCUMENT

#include <eikdoc.h>

#include "CMediaClientEngine.h"

/** Simple document class to own the engine */
class CMediaClientDocument : public CEikDocument
    {
public:
    // construct/destruct
/**
Allocates and constructs a new document.
@cat Construction and destruction
@return New document object
@param aApp Application
*/
	static CMediaClientDocument* NewL(CEikApplication& aApp);
/**
Destructor.
@cat Construction and destruction
*/
~CMediaClientDocument();


/**
Gets the application engine.

@return The application engine
*/
	CMediaClientEngine& Engine();

private:
/**
Constructor.
@cat Construction and destruction
@param aApp Application
*/
	CMediaClientDocument(CEikApplication& aApp);
/**
Second phase constructor.
@cat Construction and destruction
*/	void ConstructL();


/**
Creates an app UI.
From <code>CEikDocument</code>.
@return A new <code>CMediaClientAppUI</code> object
*/
	CEikAppUi*  CreateAppUiL();

private: 
	/** The application engine */
	CMediaClientEngine* iEngine; 
    };

#endif // CMEDIACLIENTDOCUMENT
