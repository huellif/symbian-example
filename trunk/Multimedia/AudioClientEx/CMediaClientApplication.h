// CMediaClientApplication.h
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
//

#ifndef CMEDIACLIENTAPPLICATION
#define CMEDIACLIENTAPPLICATION

#include <eikapp.h>


/**

Application class derived from Uikon framework.

*/
class CMediaClientApplication : public CEikApplication 
    {
private: // from CApaApplication
/**

Creates the application's document.

@return	A new document		
*/
    CApaDocument* CreateDocumentL();

/**

Gets the application's UID

@return	The application's UID
*/
    TUid AppDllUid() const;
    };

#endif // CMEDIACLIENTAPPLICATION
