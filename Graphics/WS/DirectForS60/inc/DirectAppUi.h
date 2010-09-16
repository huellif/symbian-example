/*
 ============================================================================
 Name		: DirectAppUi.h
 Author	  : 
 Copyright   : Your copyright notice
 Description : Declares UI class for application.
 ============================================================================
 */

#ifndef __DIRECTAPPUI_h__
#define __DIRECTAPPUI_h__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CDirectAppView;

// CLASS DECLARATION
/**
 * CDirectAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CDirectAppUi : public CAknAppUi
	{
public:
	// Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CDirectAppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	CDirectAppUi();

	/**
	 * ~CDirectAppUi.
	 * Virtual Destructor.
	 */
	virtual ~CDirectAppUi();

private:
	// Functions from base classes

	/**
	 * From CEikAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	void HandleCommandL(TInt aCommand);

	/**
	 *  HandleStatusPaneSizeChange.
	 *  Called by the framework when the application status pane
	 *  size is changed.
	 */
	void HandleStatusPaneSizeChange();

	/**
	 *  From CCoeAppUi, HelpContextL.
	 *  Provides help context for the application.
	 *  size is changed.
	 */
	CArrayFix<TCoeHelpContext>* HelpContextL() const;

    class COverlayDialog : public CActive
        {
    public:
        COverlayDialog();
        ~COverlayDialog();
        void ShowDialog();

    private:
        void RunL();
        void DoCancel();

    private:
        RNotifier iNotifier;
        TInt iR;
        };
    
private:
	// Data

	/**
	 * The application view
	 * Owned by CDirectAppUi
	 */
	CDirectAppView* iAppView;
    COverlayDialog* iOverlayDialog;
	};

#endif // __DIRECTAPPUI_h__
// End of File
