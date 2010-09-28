/*
 * buf_format.cpp
 *
 *  Created on: 2010-9-28
 *      Author: thomas
 */

#include <e32cons.h>

#include "buf_format.h"

GLREF_C CConsoleBase* console;

void prompt_case(TDesC& aDesC)
    {
    console->Printf(_L("\n------------------\ncase : %S\n"), &aDesC);
    console->Getch();
    }
