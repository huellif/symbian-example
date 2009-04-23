/*
 ============================================================================
 Name		: hellosisDocument.h
 Author	  : 
 Copyright   : Your copyright notice
 Description : Declares document class for application.
 ============================================================================
 */

#ifndef __HELLOSISDOCUMENT_h__
#define __HELLOSISDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class ChellosisAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * ChellosisDocument application class.
 * An instance of class ChellosisDocument is the Document part of the
 * AVKON application framework for the hellosis example application.
 */
class ChellosisDocument : public CAknDocument
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a ChellosisDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of ChellosisDocument.
	 */
	static ChellosisDocument* NewL(CEikApplication& aApp);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a ChellosisDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of ChellosisDocument.
	 */
	static ChellosisDocument* NewLC(CEikApplication& aApp);

	/**
	 * ~ChellosisDocument
	 * Virtual Destructor.
	 */
	virtual ~ChellosisDocument();

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a ChellosisAppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * ChellosisDocument.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
	ChellosisDocument(CEikApplication& aApp);

	};

#endif // __HELLOSISDOCUMENT_h__
// End of File
