// epoc_mmu_utils.cpp
//
// Copyright (c) 1998-1999 Symbian Ltd.  All rights reserved.
//

const TUint KPtePermissionUserRw=0x00000ff0;
const TUint KPtePermissionMask=0x00000ff0;

TUint32 PageTableId(TInt pdeIndex)
//
// Extract the identifer of the page table from the PageTableInfo array on the target
//
	{

	TLinAddr ptinfoptr=KPageTableInfo + (pdeIndex << 2);
	TUint32 ptinfo;
	bool r=gEpocDebugger->ReadWord(ptinfoptr, &ptinfo);
	if (!r)
		return NULL;
	return ptinfo >> KPageTableInfoIdShift;
	}

TLinAddr PageTableLinearAddress(TInt aPageTableId)
//
// Construct the Linear address of the page table, given its page table id.
//
	{
	return (KPageTableBase+(aPageTableId<<KPageTableSizeShift));
	}

TLinAddr PageTableEntryAddress(TLinAddr aLinAddr)
//
// Return the Address of the PTE for the given Linear Address
//
	{
	TInt pdeIndex=aLinAddr>>KChunkSizeShift;
	TInt pageTableId=PageTableId(pdeIndex);
	TLinAddr pageTableLin=PageTableLinearAddress(pageTableId);
	return pageTableLin+(((aLinAddr&KChunkSizeMinus1)>>KPageSizeShift)<<2);
	}
