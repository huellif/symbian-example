// epoc.h
//
// Copyright (c) 1998-1999 Symbian Ltd.  All rights reserved.
//
#ifndef __EPOC_H__
#define __EPOC_H__

//
// Useful EPOCisms
// for use on HOST
//

#define _FOFF(c,f) ((TInt)(&((c *)0)->f))

typedef void TAny;
typedef signed char TInt8;
typedef unsigned char TUint8;
typedef short int TInt16;
typedef unsigned short int TUint16;
typedef long int TInt32;
typedef unsigned long int TUint32;
typedef signed int TInt;
typedef unsigned int TUint;
typedef float TReal32;
typedef double TReal64;
typedef double TReal;
typedef unsigned char TText8;
typedef unsigned short int TText16;
typedef int TBool;
typedef TUint32 TLinAddr;
typedef TUint32 TPhysAddr;

const int KErrNone=0;
const int KErrNotFound=-1;

const int KMaxName=0x80;
const int KMaxNameSize=KMaxName<<1;

// const TLinAddr KDebuggerAddress = 0x40000354;
// const TLinAddr KDebuggerAddress = 0x80000378;
const TLinAddr KDebuggerAddress = 0x40000408;
const TLinAddr KCp15TableAddress = 0x40000c00;

const int KMaxBreakPoints = 16;
const int KMaxShadowPages=16;
const int KNullBreakPoint=-1;

const TLinAddr KPageDirectory=0x41000000;
const TLinAddr KPageTableInfo=0x41080000;
const TLinAddr KPageTableBase=0x42000000;
const TUint32 KPageMask=0xfff;
const TUint32 KPageTableInfoIdShift=16;

const TInt KPageSizeInBytes=4096;
const TInt KPageSizeMinus1=4095;
const TInt KPageSizeShift=12;
const TInt KChunkSizeInBytes=1048576;
const TInt KChunkSizeMinus1=1048575;
const TInt KChunkSizeShift=20;
const TInt KPageTableSizeInBytes=1024;
const TInt KPageTableSizeMinus1=1023;
const TInt KPageTableSizeShift=10;
const TInt KPageTablesInPage=4;
const TInt KPageTablesInPageMinus1=3;
const TInt KPageTablesInPageShift=2;
const TInt KPagesInPDE=256;
const TInt KPagesInPDEMinus1=(KPagesInPDE-1);
const TInt KPagesInPDEShift=8;
const TInt KMaxPageTables=4096;
const TInt KMaxPageTablePages=1024;
const TUint KSectionDescriptorPhysAddrMask=0xFFF00000u;

#endif
