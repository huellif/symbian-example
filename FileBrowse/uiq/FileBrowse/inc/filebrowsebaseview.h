// FileBrowseBaseView.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef __FILEBROWSEBASEVIEW_H__
#define __FILEBROWSEBASEVIEW_H__

#include <QikViewBase.h>
#include <MQikListBoxObserver.h>

class CRFsEngine;

class CFileBrowseBaseView : public CQikViewBase, public MQikListBoxObserver
	{
public:
	static CFileBrowseBaseView* NewL(CQikAppUi& aAppUi,CRFsEngine& aEngine);
	static CFileBrowseBaseView* NewLC(CQikAppUi& aAppUi,CRFsEngine& aEngine);
	~CFileBrowseBaseView();

	TVwsViewId ViewId() const;
public: // from MQikListBoxObserver 
	void HandleListBoxEventL(CQikListBox* aListBox, TQikListBoxEvent aEventType,
                           TInt aItemIndex, TInt aSlotId);  
protected: // From CQikViewBase
	void ViewConstructL();
	void ViewActivatedL(const TVwsViewId& aPrevViewId,
                      const TUid aCustomMessageId,
                      const TDesC8& aCustomMessage);
private:
	void ShowRootL();
	void NavigateL(TInt aIndex);
	void OpenFolderL();
private:
	CFileBrowseBaseView(CQikAppUi& aAppUi,CRFsEngine& aEngine);
	void ConstructL();

private:
	CRFsEngine& iEngine;
	CQikListBox* iListBox;  // Not owned by the class
    RArray<TInt> iFocusPos; // position of focus in listbox for the folders passed, used whem going back
private:	
	TFileName iPath;
	TBool iRoot;			// if this is ETrue, the root directories 'C' and 'D' are shown
	TInt iFilePosition;		// marks where files are displayed in the directory
	TInt iCurrPos;          // index in current folder/listbox 
							// 0 - iFilePosition -> a directory is currently selected 
							// > iFilePosition  - a file is currently selected
	};


#endif // __FILEBROWSEBASEVIEW_H__


