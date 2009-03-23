//
// CImageDocument
//

#include "CImageDocument.h"
#include "CImageAppUi.h"

CImageDocument::CImageDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{
	}

CEikAppUi* CImageDocument::CreateAppUiL()
	{
    return new(ELeave) CImageAppUi;
	}


