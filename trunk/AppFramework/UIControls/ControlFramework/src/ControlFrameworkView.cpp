// ControlFrameworkAppView.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include <eikenv.h>
#include <ControlFramework.rsg>

#include "ControlFrameworkView.h"
#include "ControlFrameworkAppUi.h"
#include "ControlFrameworkGlobals.h"

// Maximum number of lines that the text can be split into 
const TInt KMaxLines = 2;
// Maximum number of characters that the text object can hold
const TInt KMaxStringLength = 64;
_LIT(KPointerEventMessage, "Pointer event %d at (%d,%d)");
_LIT(KKeyEventMessage, "Key 0x%x, modifier 0x%x");

CControlFrameworkView::CControlFrameworkView(CControlFrameworkAppUi& aAppUi) : iAppUi(aAppUi), iFont(TCoeFont::ELarge, TCoeFont::EPlain)
	{
	}

CControlFrameworkView* CControlFrameworkView::NewLC(CControlFrameworkAppUi& aAppUi)
	{
	CControlFrameworkView* view = new(ELeave) CControlFrameworkView(aAppUi);
	CleanupStack::PushL(view);
	return view;
	}

CControlFrameworkView::~CControlFrameworkView()
	{
	iRunInfoArray.Close();
	delete iBidiText;
	}

// Called by the UI framework the first time the view is activated.
// As much as possible of the code normally found in ConstructL() has 
// been moved here to improve application startup time.
void CControlFrameworkView::ViewConstructL()
	{
	// The control is window-owning.
	CreateWindowL();

	iBidiText = TBidiText::NewL(KMaxStringLength, KMaxLines);
	iRunInfoArray.OpenL();
	HBufC* message = iEikonEnv->AllocReadResourceLC(R_WELCOME_TEXT);
	iBidiText->SetText(*message, iRunInfoArray); // TDirectionality determined by resource
	CleanupStack::PopAndDestroy(message);
	
	// Set extent of the view. ClientRect calculates to ScreenSize minus Toolbar, Menubar, Statusbar etc.
	SetRect(iAppUi.ClientRect());
	
	// The control is ready to draw, so notify the UI framework.
	ActivateL();
	}	


// Uniquely identifies the view 
TVwsViewId CControlFrameworkView::ViewId() const
	{
	return TVwsViewId(KUidControlFrameworkAppUid, KUidControlFrameworkView);
	}


void CControlFrameworkView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
	{
	}

void CControlFrameworkView::ViewDeactivated()
	{
	}

// Receives pointer events and prepares to write a description of 
// the pointer event to the screen.
void CControlFrameworkView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	TBuf<KMaxStringLength> text;
	text.Format(KPointerEventMessage, aPointerEvent.iType,aPointerEvent.iPosition.iX,aPointerEvent.iPosition.iY);
	iBidiText->SetText(text, TBidiText::ELeftToRight, iRunInfoArray);
	Window().Invalidate();
	}

// Draws the border and the message text
void CControlFrameworkView::Draw(const TRect& aRect) const
	{
	TRect rect = aRect;
	DrawBorder(rect);
	DrawMessage(rect);
	}

// Draws a 5 pixel wide border around the window 
void CControlFrameworkView::DrawBorder(TRect& aRect) const
	{
	CWindowGc& gc=SystemGc();
	gc.SetPenSize(TSize(5,5));
	gc.DrawRect(aRect);
	aRect.Shrink(5,5);
	}

// Displays the message stored in iBidiText centered vertically and horizontally.
void CControlFrameworkView::DrawMessage(const TRect& aRect) const
	{
	CWindowGc& gc=SystemGc();

	if (IsStrikethrough())
		gc.SetStrikethroughStyle(EStrikethroughOn);
	
	if (IsUnderline())
		gc.SetUnderlineStyle(EUnderlineOn);

	XCoeTextDrawer textDrawer(TextDrawer());
	textDrawer->SetAlignment(EHCenterVCenter);
	const CFont& font = ScreenFont(iFont);
	iBidiText->WrapText((aRect.iBr.iX - aRect.iTl.iX) , font, NULL, KMaxLines);
	textDrawer.DrawText(gc, *iBidiText, aRect, font);
	}

/*
Moves the window and prints key event information to the screen.
The following key combinations cause the window to move: 
1. Shift + KeyLeft arrow
2. Shift + KeyRight arrow
3. Shift + KeyDown arrow
4. Shift + KeyUp arrow
*/
TKeyResponse CControlFrameworkView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (aType == EEventKey)
		{
		TInt modifiers=aKeyEvent.iModifiers;
		TInt code=aKeyEvent.iCode;

		// Write a description of the key event to the screen
		TBuf<KMaxStringLength> text;
		text.Format(KKeyEventMessage, code,modifiers);
		iBidiText->SetText(text, TBidiText::ELeftToRight, iRunInfoArray);
		Window().Invalidate();
		
		if (modifiers&EModifierShift)
			{
			TPoint pos=Position();
			switch (code)
				{
				case EKeyLeftArrow:
					pos.iX--;
					break;
				case EKeyRightArrow:
					pos.iX++;
					break;
				case EKeyUpArrow:
					pos.iY--;
					break;
				case EKeyDownArrow:
					pos.iY++;
					break;
				default:
					break;
				}
			if (pos != Position())
				{
				SetPosition(pos);
				return(EKeyWasConsumed);
				}
			}
		}
	return(EKeyWasNotConsumed);
	}

TBool CControlFrameworkView::IsStrikethrough() const
	{
	return iFontFlags.IsSet(EStrikethrough);
	}

void CControlFrameworkView::ToggleStrikethrough()
	{
	if (IsStrikethrough())
		iFontFlags.Clear(EStrikethrough);
	else 
		iFontFlags.Set(EStrikethrough);
	
	Window().Invalidate();
	}     
	
TBool CControlFrameworkView::IsUnderline() const
	{ 
	return iFontFlags.IsSet(EUnderline);
	}

void CControlFrameworkView::ToggleUnderline()
	{
	if (IsUnderline())
		iFontFlags.Clear(EUnderline);
	else 
		iFontFlags.Set(EUnderline);
	
	Window().Invalidate();
	} 
	
TBool CControlFrameworkView::IsBold() const
	{
	return iFontFlags.IsSet(EBold);
	}

void CControlFrameworkView::ToggleBold()
	{
	if (IsBold())
		{
		iFontFlags.Clear(EBold);
		iFontFlags.Clear(EItalic);
		iFont = TCoeFont(iFont.LogicalSize(), TCoeFont::EPlain);
		}
	else 
		{
		iFontFlags.Set(EBold);
		iFontFlags.Clear(EItalic);
		iFont = TCoeFont(iFont.LogicalSize(), TCoeFont::EBold);
		}
	
	Window().Invalidate();
	}     
	
TBool CControlFrameworkView::IsItalic() const
	{
 	return iFontFlags.IsSet(EItalic);
	}
    

void CControlFrameworkView::ToggleItalic()
	{
	if (IsItalic())
		{
		iFontFlags.Clear(EBold);
		iFontFlags.Clear(EItalic);
		iFont = TCoeFont(iFont.LogicalSize(), TCoeFont::EPlain);
		}
	else 
		{	
		iFontFlags.Set(EItalic);
		iFontFlags.Clear(EBold);
		iFont = TCoeFont(iFont.LogicalSize(), TCoeFont::EItalic);
		}
		
	Window().Invalidate();
	} 	
