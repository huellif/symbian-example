/*
 ============================================================================
 Name		: DirectDocument.h
 Author	  : 
 Copyright   : Your copyright notice
 Description : Declares document class for application.
 ============================================================================
 */

#ifndef __DIRECTDOCUMENT_h__
#define __DIRECTDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CDirectAppUi;
class CEikApplication;

// CLASS DECLARATION
class CLifeEngine;

/**
 * CDirectDocument application class.
 * An instance of class CDirectDocument is the Document part of the
 * AVKON application framework for the Direct example application.
 */
class CDirectDocument : public CAknDocument
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a CDirectDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CDirectDocument.
	 */
	static CDirectDocument* NewL(CEikApplication& aApp);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a CDirectDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CDirectDocument.
	 */
	static CDirectDocument* NewLC(CEikApplication& aApp);

	/**
	 * ~CDirectDocument
	 * Virtual Destructor.
	 */
	virtual ~CDirectDocument();

    // Gets the engine
    CLifeEngine& LifeEngine() const;

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a CDirectAppUi object and return a pointer to it.
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
	 * CDirectDocument.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
	CDirectDocument(CEikApplication& aApp);

private:
    // The engine
    CLifeEngine* iLifeEngine;
	};

#endif // __DIRECTDOCUMENT_h__
// End of File
