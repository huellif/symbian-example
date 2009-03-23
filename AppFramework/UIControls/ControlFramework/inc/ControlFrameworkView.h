// ControlFramework_View.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef __CONTROLFRAMEWORKVIEW_H
#define __CONTROLFRAMEWORKVIEW_H

#include <coecntrl.h>
#include <coeview.h>
#include <Babitflags.h> 

class CControlFrameworkAppUi;

//View class
class CControlFrameworkView : public CCoeControl, public MCoeView
	{
public:
	static CControlFrameworkView* NewLC(CControlFrameworkAppUi& iAppUi);
	~CControlFrameworkView();
	
	TBool IsBold() const;
	TBool IsItalic() const;
	TBool IsUnderline() const;
	TBool IsStrikethrough() const;
	void ToggleBold();
	void ToggleItalic();
	void ToggleUnderline();
	void ToggleStrikethrough();

	// from MCoeView
	TVwsViewId ViewId() const;
    
protected:
	CControlFrameworkView(CControlFrameworkAppUi& iAppUi);
	void DrawBorder(TRect& aRect) const;
	void DrawMessage(const TRect& aRect) const;

	// from MCoeView
	void ViewConstructL();
	void ViewActivatedL(const TVwsViewId &aPrevViewId, TUid aCustomMessageId, const TDesC8 &aCustomMessage);
	void ViewDeactivated();
    
	// from CCoeControl
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void Draw( const TRect& /*aRect*/ ) const;
    
private:
	CControlFrameworkAppUi& iAppUi;
	TBidiText* iBidiText;

	RRunInfoArray iRunInfoArray;
	TCoeFont iFont;
    
	enum TCommandBools
		{
		EBold,
		EItalic,
		EUnderline,
		EStrikethrough
		};
	TBitFlags iFontFlags;
	};



#endif // __CONTROLFRAMEWORKVIEW_H

