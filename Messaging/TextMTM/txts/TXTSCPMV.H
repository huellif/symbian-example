// TXTSCPMV.H
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#ifndef _TXTSCPMV_H_
#define _TXTSCPMV_H_

#if !defined(__MSVSTD_H__)
#include <msvstd.h> 
#endif
#if !defined(__MSVENTRY_H__)
#include <msventry.h>
#endif

#if !defined(__TXUT_H__)
#include "txut.h" //for TTxtProgress
#endif

#define ETxtRetrievingMessage  1
#define ETxtFinished		   0


const TInt KMsgTxtRefreshMailboxPriority = 1;

class CParaFormatLayer;
class CCharFormatLayer;
class CMsvServerEntry;
class CTxtCopyMoveBase;

// 
// CTxtActiveOper: performs an operation on exactly one Msv Entry.
// Derive from this for the several move/copy/delete operations
//
// The methods have a RFs at their disposal, for access to the current
// folder for the operation in the file system.
//

class CTxtActiveOper : public CActive
	{
public:
	CTxtActiveOper(RFs& aFs, CMsvServerEntry& aServerEntry);
	virtual ~CTxtActiveOper();
	void Start(TMsvId& aSourceId, TMsvId& aDestId, TRequestStatus& aStatus);
	void SetCurrentCaller(CTxtCopyMoveBase* aCurrentCaller);
public:
	virtual CTxtActiveOper* CopyConstructL()=0;
	// CopiedHeader reports whether messages are copied and entries need to be created.
	// the destination entry id will then be passed on in the Start method
	virtual TBool CopiedHeader() const = 0;
	// DeleteSourceAfterwards specifies whether the source entry needs to be deleted
	// afterwards. This will then be done automatically also.
	virtual TBool DeleteSourceAfterwards() const = 0;
	// MoveIsToService specifies whether the messages that are moved are moved to the service.
	// Some additional operations need to be done to make the entry a valid entry, 
	// the flag iServiceId has to be set, and the correctness of the iDetails has
	// to be assured. Also the file name has to be altered if it already exists.
	virtual TBool MoveIsToService() const = 0;
protected:
	// RunL will then do the actual transfer. It should call ReportFinished when the operation
	// has completed.
	void RunL();
	virtual void DoRunL()=0;
	virtual void DoCancel();
protected:
	TRequestStatus*		iReportStatus;
	CMsvServerEntry&	iServerEntry;
	RFs					&iFs;
	TMsvId				iSourceId;
	TMsvId				iDestId;
	CTxtCopyMoveBase*	iCurrentCaller;   // Currently being called from. 
	TFileName iFileName;				  // Current source entry info
	};

//
// CTxtCopyMoveBase: defines some base functionality for transferring
// between the server and the local file system. It needs a CTxtActiveOper
// class at construction, that will actually copy the body text over asynchronously.
//

class CTxtCopyMoveBase : public  CActive
	{
public:
	static CTxtCopyMoveBase* NewL(CTxtActiveOper* aActiveOperation, const CMsvEntrySelection& aSource, 
		CMsvServerEntry& aDestination, TMsvId& aServiceEntryId, TParse& aParse);
	CTxtCopyMoveBase* CopyConstructL(const CMsvEntrySelection& aSource);
	~CTxtCopyMoveBase();
	void Start(TRequestStatus& aStatus);
	TTxtProgress& Progress();
protected:
	CTxtCopyMoveBase(CTxtActiveOper* aActiveOperation, const CMsvEntrySelection& aSource, 
		CMsvServerEntry& aDestination, TMsvId& aServiceEntryId, TParse& aParse);
	void ConstructL();
	void DoCancel();
	void RunL();
	void DoRunL();
private:
	void CheckDeleteSource();
	void SetupNewEntryL(TMsvId& SourceMsgId, TMsvId& newDestMsvId);
private: 
	TInt iTxtCopyMoveState;
	CTxtActiveOper*				iActiveOperation;
	const CMsvEntrySelection&	iSource;
	CMsvServerEntry&			iDestination;
	TMsvId						iServiceEntryId;
	TMsvId						iDestId;
	TInt						iMsgCounter;
	TTxtProgress				iProgress;
	TRequestStatus*				iReportStatus;
public: 
	// iServiceDestinationpath is used as a current folder, when transferring to the RFs
	TParse& iServiceDestinationpath;
	};

//
// CTxtActiveOper-derived classes, defining the actual moving around of 
// body texts.
//

//
// CTxtCopyToLocalOp, CTxtCopyFromLocalOp, CTxtCopyWithinServiceOp:
// The copy operations
//

class CTxtCopyToLocalOp : public CTxtActiveOper
	{
public:
	CTxtActiveOper* CopyConstructL();
	CTxtCopyToLocalOp(RFs& aFs, CMsvServerEntry& aServerEntry) 
		: CTxtActiveOper(aFs, aServerEntry) {};
	TBool CopiedHeader() const;
	virtual TBool DeleteSourceAfterwards() const;
	TBool MoveIsToService() const;
protected:
	void DoRunL();
private:
	void DoMessageCopyToLocalL();
	};

class CTxtCopyFromLocalOp : public CTxtActiveOper
	{
public:
	CTxtActiveOper* CopyConstructL();
	CTxtCopyFromLocalOp(RFs& aFs, CMsvServerEntry& aServerEntry) 
		: CTxtActiveOper(aFs, aServerEntry) {};
	TBool CopiedHeader() const;
	virtual TBool DeleteSourceAfterwards() const;
	TBool MoveIsToService() const;
protected:
	void DoRunL();
private:
	void DoMessageCopyFromLocalL(const TMsvEntry& aEntry);
	};

class CTxtCopyWithinServiceOp : public CTxtActiveOper
	{
public:
	CTxtActiveOper* CopyConstructL();
	CTxtCopyWithinServiceOp(RFs& aFs, CMsvServerEntry& aServerEntry) 
		: CTxtActiveOper(aFs, aServerEntry) {};
	TBool CopiedHeader() const;
	virtual TBool DeleteSourceAfterwards() const;
	TBool MoveIsToService() const ;
protected:
	void DoRunL();
private:
	void DoMessageCopyWithinServiceL();
	};

//
// CTxtMoveToLocalOp, CTxtMoveFromLocalOp, CTxtMoveWithinServiceOp:
// The move operations: these are only a special case of copying, 
// where the original needs to be deleted.
//

class CTxtMoveToLocalOp : public CTxtCopyToLocalOp
	{
public:
	CTxtActiveOper* CopyConstructL();
	CTxtMoveToLocalOp(RFs& aFs, CMsvServerEntry& aServerEntry) 
		: CTxtCopyToLocalOp(aFs, aServerEntry) {};
	TBool DeleteSourceAfterwards() const;
	};

class CTxtMoveFromLocalOp : public CTxtCopyFromLocalOp
	{
public:
	CTxtActiveOper* CopyConstructL();
	CTxtMoveFromLocalOp(RFs& aFs, CMsvServerEntry& aServerEntry) 
		: CTxtCopyFromLocalOp(aFs, aServerEntry) {};
	TBool DeleteSourceAfterwards() const;
	};

class CTxtMoveWithinServiceOp : public CTxtCopyWithinServiceOp
	{
public:
	CTxtActiveOper* CopyConstructL();
	CTxtMoveWithinServiceOp(RFs& aFs, CMsvServerEntry& aServerEntry) 
		: CTxtCopyWithinServiceOp(aFs, aServerEntry) {};
	TBool DeleteSourceAfterwards() const;
	};

//
// CTxtDeleteOp: Delete operation
//

class CTxtDeleteOp : public CTxtActiveOper
	{
public:
	CTxtActiveOper* CopyConstructL();
	CTxtDeleteOp(RFs& aFs, CMsvServerEntry& aServerEntry) 
		: CTxtActiveOper(aFs, aServerEntry) {};
	TBool CopiedHeader() const;
	TBool DeleteSourceAfterwards() const;
	TBool MoveIsToService() const;
protected:
	void DoRunL();
	};


inline void CTxtActiveOper::SetCurrentCaller(CTxtCopyMoveBase* aCurrentCaller)
	{
	iCurrentCaller = aCurrentCaller;
	}


#endif

