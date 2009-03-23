// MMFDes.h
//
// Copyright (c) 2001-2002 Symbian Ltd.  All rights reserved.
//

#ifndef __MMFDES_H_
#define __MMFDES_H_


#include <mmf\server\mmfclip.h>
#include <mmf\common\mmfutilities.h>

//Panic codes for MMFDescriptor
enum TMMFExDescriptorPanicCode
	{
	EMMFDescriptorPanicBufferEmptiedLNotSupported = 1,
	EMMFDescriptorPanicBufferFilledLNotSupported
	};
	
class TMMFExDescriptorParams
	{
public:
	TAny* iDes ; // Address of TPtr8 describing source Descriptor
	TThreadId iDesThreadId ;
	} ;

typedef TPckgBuf<TMMFExDescriptorParams>  TMMFDescriptorConfig ;

// Defines a MMF sink and source plug-in that can write and read media data
// to a descriptor
// Implements CMMFClip, which itself derives from MDataSource and MDataSink
class CMMFExDescriptor : public CMMFClip
	{
public:
	// Data source factory
	static MDataSource* NewSourceL() ; 
	// Data sink factory
	static MDataSink* NewSinkL() ; 
	~CMMFExDescriptor() ;
	
private:
	// From MDataSource
	TFourCC SourceDataTypeCode( TMediaId /*aMediaId*/) ;
	void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer,TMediaId /*aMediaId*/) ;//called by a MDataSink to request buffer fill
	void BufferEmptiedL( CMMFBuffer* aBuffer ) ;
	TBool CanCreateSourceBuffer() ;
	CMMFBuffer* CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &aReference) ;
	void ConstructSourceL(  const TDesC8& aInitData ) ;

	// From MDataSink
	TFourCC SinkDataTypeCode(TMediaId /*aMediaId*/) ; //used by data path MDataSource/Sink for codec matching
	void EmptyBufferL( CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId /*aMediaId*/ ) ; 
	void BufferFilledL( CMMFBuffer* aBuffer ) ;
	TBool CanCreateSinkBuffer() ;
	CMMFBuffer* CreateSinkBufferL( TMediaId /*aMediaId*/ , TBool &aReference) ;
	void ConstructSinkL( const TDesC8& aInitData ) ;

	// From CMMFClip
	void ReadBufferL( TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer);
	void WriteBufferL( TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier);
	void ReadBufferL( CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer) ;
	void WriteBufferL( CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier) ;
	void ReadBufferL( CMMFBuffer* aBuffer, TInt aPosition) ;
	void WriteBufferL( CMMFBuffer* aBuffer, TInt aPosition) ;
	TInt64 BytesFree() ;  // amount of space available for the clip
	TInt Size() ;  // length of clip

	// Construction
	void ConstructL( const TDesC8& aInitData ) ;
	CMMFExDescriptor();
	// Helpers
	void Reset() { iOffset = 0 ; };

private:
	// Need to know about the thread that the descriptor is in
	RThread iDesThread;
	TDes8* iDes ;
	TInt iOffset;
	TFourCC iSinkFourCC;
	TFourCC iSourceFourCC;
	TBool iUseTransferBuffer;
	} ;


#endif
