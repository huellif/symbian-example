/*
 ============================================================================
 Name		: hellosisApplication.h
 Author	  : 
 Copyright   : Your copyright notice
 Description : Declares main application class.
 ============================================================================
 */

#ifndef __HELLOSISAPPLICATION_H__
#define __HELLOSISAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "hellosis.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidhellosisApp =
	{
	_UID3
	};

// CLASS DECLARATION

/**
 * ChellosisApplication application class.
 * Provides factory to create concrete document object.
 * An instance of ChellosisApplication is the application part of the
 * AVKON application framework for the hellosis example application.
 */
class ChellosisApplication : public CAknApplication
	{
public:
	// Functions from base classes

	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidhellosisApp).
	 */
	TUid AppDllUid() const;

protected:
	// Functions from base classes

	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates ChellosisDocument document object. The returned
	 * pointer in not owned by the ChellosisApplication object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL();
	};

#endif // __HELLOSISAPPLICATION_H__
// End of File
