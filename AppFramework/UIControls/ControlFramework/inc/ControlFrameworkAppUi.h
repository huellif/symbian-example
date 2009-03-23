// ControlFrameworkAppUi.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef  __CONTROLFRAMEWORKAPPUI_H
#define  __CONTROLFRAMEWORKAPPUI_H

#include <eikappui.h>

class CControlFrameworkView;

//Application UI class
class CControlFrameworkAppUi : public CEikAppUi
    {
public:
	CControlFrameworkAppUi();
	~CControlFrameworkAppUi();
	void AddViewL(CControlFrameworkView* aView);
	enum TViews
		{
		EExampleView
		};
		
	// from CEikAppUi
	void ConstructL();
			
private:
	void HandleCommandL(TInt);

private:
	RPointerArray<CControlFrameworkView> iAppViews;
	};


#endif // __CONTROLFRAMEWORKAPPUI_H

