// ContactViews.h
//
// Copyright (c) 2005 Symbian Software Ltd.  All rights reserved.
//
//

#if (!defined __CONTACTVIEWS_H__)
#define __CONTACTVIEWS_H__

#include <cntdb.h>
#include <cntview.h>
#include <cntviewbase.h>


// CExampleViews is an active object and a contact view observer.
class CExampleViews : public CActive , public MContactViewObserver
	{
public:
	static CExampleViews* NewL(CContactDatabase& aDb);
	~CExampleViews();
	void Start();

private:
	CExampleViews(CContactDatabase& aDb);
	void ConstructL();

	// CActive implementation
	void RunL();
	void DoCancel();

	// MContactViewObserver implementation
	virtual void HandleContactViewEvent(const CContactViewBase& aView, const TContactViewEvent& aEvent);

	CContactFindView* CreateFindViewL();

private:
    // Pointer to the contact database
	CContactDatabase& iDb;
	// Fields to use in the views
	RContactViewSortOrder iSortOrder;
	// Pointer to the local view
	CContactLocalView* iLocalView;
	// Pointer to the find view 
	CContactFindView* iFindView;
	// Pointer to the filter view
	CContactFilteredView* iFilterView;
	// Used to test the number of views created
	TInt iNumViewsCreated;
	};


#endif // __CONTACTVIEWS_H__
