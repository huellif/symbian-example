// MediaClientView.cpp
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
//

// The view is very simple: it simply writes a string
// describing the current state of the application
#include <AudioClientEx.rsg>
#include "CMediaClientView.h"
#include "CMediaClientDocument.h"
#include "CMediaClientEngine.h"

#include <eikenv.h>


CMediaClientView* CMediaClientView::NewL(const TRect& aRect, CMediaClientEngine& aEngine)
    {
    CMediaClientView* new_object = new (ELeave) CMediaClientView(aEngine);
    CleanupStack::PushL(new_object);
    new_object->ConstructL(aRect);
    CleanupStack::Pop();

    return new_object;
    }

// Set up
void CMediaClientView::ConstructL(const TRect& aRect) 
    {    
    CreateWindowL();
    Window().SetShadowDisabled(ETrue);
    SetRect(aRect);

    ActivateL();
    }

CMediaClientView::CMediaClientView(CMediaClientEngine& aEngine)
:iEngine(aEngine)
	{
	}

CMediaClientView::~CMediaClientView()
    {
    }

void CMediaClientView::SetDescription(const TDesC& aDescription)
	{
	iDescription = aDescription;
	}

// Update the view
void CMediaClientView::Draw(const TRect& /*aRect*/) const
    {
    CWindowGc& gc = SystemGc();

    TInt boxHeight=Rect().Height(); // get height of text box
    const CFont* font=iEikonEnv->TitleFont();
    TInt textHeight(font->HeightInPixels()); // system font height
    TInt offset=(textHeight+boxHeight)/2; // 1/2 font ht below halfway down box
    TInt margin=0; // margin is zero
    
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);    
    gc.SetBrushColor(KRgbWhite);
    gc.SetPenColor(KRgbBlack);    
    gc.UseFont(font);

	gc.DrawRect(Rect());
    gc.DrawText(iDescription, Rect(), offset, CGraphicsContext::ECenter, margin);
    }

