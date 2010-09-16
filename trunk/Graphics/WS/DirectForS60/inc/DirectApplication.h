/*
 ============================================================================
 Name		: DirectApplication.h
 Author	  : 
 Copyright   : Your copyright notice
 Description : Declares main application class.
 ============================================================================
 */

#ifndef __DIRECTAPPLICATION_H__
#define __DIRECTAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "Direct.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidDirectApp =
	{
	_UID3
	};

// CLASS DECLARATION

/**
 * CDirectApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CDirectApplication is the application part of the
 * AVKON application framework for the Direct example application.
 */
class CDirectApplication : public CAknApplication
	{
public:
	// Functions from base classes

	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidDirectApp).
	 */
	TUid AppDllUid() const;

protected:
	// Functions from base classes

	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates CDirectDocument document object. The returned
	 * pointer in not owned by the CDirectApplication object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL();
	};

#endif // __DIRECTAPPLICATION_H__
// End of File
