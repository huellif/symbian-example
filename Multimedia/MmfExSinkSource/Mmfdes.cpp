// mmfdes.cpp
//
// Copyright 1997-2005 Symbian Software Ltd.  All rights reserved.
//


#include <f32file.h>
#include <E32Std.h>
#include <mmfdatabuffer.h>

#include "mmfexdes.h"
#include "MmfExSinkSourceUIDs.hrh"

// Panic function
void Panic(TMMFExDescriptorPanicCode aPanicCode)
	{
	_LIT(KMMFExDescriptorPanicCategory, "MMFExDescriptor");
	User::Panic(KMMFExDescriptorPanicCategory, aPanicCode);
	}

//
// CMMFExDescriptor
//

// Plugin implementation UIDs
const TUid KDescriptorSourceUIDObject	= {KDescriptorSourceUID};
const TUid KDescriptorSinkUIDObject	= {KDescriptorSinkUID};

// Constructor
CMMFExDescriptor::CMMFExDescriptor( ) : CMMFClip( TUid(KDescriptorSourceUIDObject), TUid(KDescriptorSinkUIDObject )) 
	{
	iOffset = 0 ;
	}

// Destructor
CMMFExDescriptor::~CMMFExDescriptor()
	{
	iDesThread.Close() ;
	}

// Factory function for source plug-in
MDataSource* CMMFExDescriptor::NewSourceL( )
	{
	CMMFExDescriptor* self = new (ELeave) CMMFExDescriptor( ) ;
	return STATIC_CAST( MDataSource*, self ) ;
	}
	
// Factory function for sink plug-in
MDataSink* CMMFExDescriptor::NewSinkL( )
	{
	CMMFExDescriptor* self = new (ELeave) CMMFExDescriptor( ) ;
	return STATIC_CAST( MDataSink*, self ) ;
	}

// MDataSource factory
// aInitData is a packaged TMMFExDescriptorParams
void CMMFExDescriptor::ConstructSourceL( const TDesC8& aInitData )
	{
	ConstructL( aInitData ) ;
	}

// MDataSink factory
// aInitData is a packaged TMMFExDescriptorParams
void CMMFExDescriptor::ConstructSinkL( const TDesC8& aInitData )
	{
	ConstructL( aInitData ) ;
	}

// Construction helper
// aInitData is a packaged TMMFExDescriptorParams
void CMMFExDescriptor::ConstructL( const TDesC8& aInitData ) 
	{
	// get descriptor to which to read/write, and the thread 
	// to which it belongs
	TMMFExDescriptorParams params;
	TPckgC<TMMFExDescriptorParams> config(params);
	config.Set(aInitData);
	iDes = STATIC_CAST( TDes8*, config().iDes);
	User::LeaveIfError( iDesThread.Open( config().iDesThreadId ) );
	}


// From MDataSource

TFourCC CMMFExDescriptor::SourceDataTypeCode(TMediaId /*aMediaId*/) 
	{
	return iSourceFourCC ;
	}

// Fills a buffer from the source descriptor
void CMMFExDescriptor::FillBufferL( CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId /*aMediaId*/  ) 
	{
	// Current position in Descriptor is iOffset.

	// Read from iDes in iDesThread into Des in aBuffer.

	// Assume that the amount to be read is the size of the buffer descriptor
	// Should check that there is sufficient data in the source buffer
	// If there is not enough to fill the target then copy what there is

	// Use of a single iOffset will preclude use by more than one client (use ReadBufferL())
	if ( aBuffer->Type() == KUidMmfDataBuffer  ) 
		{
		TDes8& bufferDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();
		TInt targetMaxLength = bufferDes.MaxLength();
		TInt sourceLength = iDes->Length();
		
		if ( ( sourceLength - iOffset - targetMaxLength ) > 0 )
			{
			bufferDes = iDes->Mid(iOffset,targetMaxLength);
			iOffset += targetMaxLength;
			}
		else
			bufferDes.SetLength(0);

		// Check if the buffer is the last buffer and if so set the last buffer flag on the CMMFDataBuffer
		TInt requestSize = aBuffer->RequestSize();
		if (requestSize)
			{ // The buffer has a request size  - so assume last buffer if length is less than the request size
			if (bufferDes.Length() < requestSize) 	
				aBuffer->SetLastBuffer(ETrue);
			}
		else
			{ // There is no request size so assume last buffer if length is less than the max length
			if (bufferDes.Length() < bufferDes.MaxLength()) 
				aBuffer->SetLastBuffer(ETrue);
			}

		aConsumer->BufferFilledL( aBuffer ) ;
		}
	else
		User::Leave(KErrNotSupported);
	}

// called by MDataSink to pass back emptied buffer to the source
void CMMFExDescriptor::BufferEmptiedL( CMMFBuffer* /*aBuffer*/ )
	{
	// not supported in this plug-in
	__ASSERT_DEBUG(EFalse, Panic(EMMFDescriptorPanicBufferEmptiedLNotSupported));
	}

// tests if the plug-in can create a source buffer
TBool CMMFExDescriptor::CanCreateSourceBuffer()
	{
	// this can't: it needs a descriptor from another thread
	return EFalse ;
	}

// creates a source buffer
CMMFBuffer* CMMFExDescriptor::CreateSourceBufferL(  TMediaId /*aMediaId*/, TBool& /*aReference*/ )
	{
	// is not supported in this plug-in
	User::Leave(KErrNotSupported);
	return NULL;
	}


// from MDataSink

// gets sink codec type
TFourCC CMMFExDescriptor::SinkDataTypeCode(TMediaId /*aMediaId*/) 
	{
	return iSinkFourCC ;
	}

// Empties supplied buffer into the sink descriptor 
void CMMFExDescriptor::EmptyBufferL( CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId /*aMediaId*/ )
	{
	// Current position in Descriptor is iOffset.

	// Assume that the amount to be read is the size of the buffer descriptor
	// Should check that there is sufficient data in the source buffer
	// If there is not enough to fill the target then copy what there is

	// Use of a single iOffset will preclude use by more than one client (use ReadBufferL())

	if ( aBuffer->Type() == KUidMmfDataBuffer  ) 
		{
		TDes8& bufferDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();

		TInt sourceLength = bufferDes.Length() ;	
		TInt targetLength = iDes->MaxLength() - iOffset;
		if ( targetLength>0 )
			{
			if (sourceLength>targetLength)
				{
				sourceLength = targetLength;
				bufferDes.SetLength(targetLength);
				}

			iDes->Append(bufferDes) ;

			iOffset += sourceLength ;
			}
		else
			bufferDes.SetLength(0);

		aSupplier->BufferEmptiedL( aBuffer ) ;
		}
	}

// called by MDataSource to pass back full buffer to the sink
void CMMFExDescriptor::BufferFilledL( CMMFBuffer* /*aBuffer*/ )
	{
	// not supported in this plug-in
	__ASSERT_DEBUG(EFalse, Panic(EMMFDescriptorPanicBufferFilledLNotSupported));
	}

// tests if the plug-in can create a sink buffer
TBool CMMFExDescriptor::CanCreateSinkBuffer()
	{
	// this can't: it needs a descriptor from another thread	
	return EFalse ;
	}

// creates a sink buffer
CMMFBuffer* CMMFExDescriptor::CreateSinkBufferL( TMediaId /*aMediaId*/ , TBool& /*aReference*/)
	{
	// not supported in this plug-in
	User::Leave(KErrNotSupported);
	return NULL;
	}

// from CMMFClip

// Loads aBuffer with specified amount of data from a specified point in the source descriptor 
void CMMFExDescriptor::ReadBufferL(TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer)
	{
	if (aBuffer->Type() == KUidMmfDataBuffer) 
		{
		TDes8& bufferDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();

		if (aLength>bufferDes.MaxLength())
			User::Leave(KErrOverflow);

		if ((aLength<0) || (aPosition<0))
			User::Leave(KErrArgument);

		ReadBufferL(aBuffer, aPosition, aConsumer);
		}
	else
		User::Leave(KErrNotSupported);
	}

// Loads aBuffer from a specified point in the source descriptor  
void CMMFExDescriptor::ReadBufferL(CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer)
	{
	if (aBuffer->Type() == KUidMmfDataBuffer) 
		{
		TDes8& bufferDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();

		TInt sourceLength = iDes->Length() ;
		TInt destinationMaxLength = bufferDes.MaxLength();
		TInt len = sourceLength - aPosition;
		if (len > destinationMaxLength)
			len = destinationMaxLength;

		if (len>0)
			{
			TPtrC8 srcPtr(iDes->Mid(aPosition,len));
			bufferDes.Copy(srcPtr);
			}
		else
			bufferDes.SetLength(0);

		// Check if the buffer is the last buffer and if so set the last buffer flag on the CMMFDataBuffer
		TInt requestSize = aBuffer->RequestSize();
		if (requestSize)
			{ // The buffer has a request size  - so assume last buffer if length is less than the request size
			if (bufferDes.Length() < requestSize) 	
				aBuffer->SetLastBuffer(ETrue);
			}
		else
			{ // There is no request size so assume last buffer if length is less than the max length
			if (bufferDes.Length() < bufferDes.MaxLength()) 
				aBuffer->SetLastBuffer(ETrue);
			}

		if (aConsumer)
			aConsumer->BufferFilledL(aBuffer);
		}
	else
		User::Leave(KErrNotSupported);
	}

// Loads aBuffer from a specified point in the source descriptor 
// This is intended for synchronous usage
void CMMFExDescriptor::ReadBufferL(CMMFBuffer* aBuffer, TInt aPosition)
	{
	ReadBufferL(aBuffer, aPosition, NULL);
	}

// Writes aBuffer at specified location into the sink descriptor
void CMMFExDescriptor::WriteBufferL(CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier) 
	{
	if (aBuffer->Type() == KUidMmfDataBuffer)
		{
		TDes8& bufferDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();

		WriteBufferL(bufferDes.Length(), aBuffer, aPosition, aSupplier);
		}
	else
		User::Leave(KErrNotSupported);
	}

// Writes specified length of aBuffer at specified location into the sink descriptor
void CMMFExDescriptor::WriteBufferL(TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier)
	{
	if (aBuffer->Type() == KUidMmfDataBuffer) 
		{
		TDes8& bufferDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();

		if (aLength>bufferDes.Length() || (aLength<0) || (aPosition<0))
			User::Leave(KErrArgument);

		TInt sourceLength = aLength;
		TPtr8 bufferPtr(((sourceLength) ? &bufferDes[0] : NULL), sourceLength, sourceLength);
		TInt targetLength = iDes->MaxLength() - aPosition;
		if (targetLength>0 && sourceLength > 0)
			{
			if (sourceLength>targetLength)
				User::Leave(KErrOverflow);

			if ((iDes->Length() - aPosition) > 0)
				{
				TInt bytesToReplace = iDes->Length() - aPosition;
				if (sourceLength > bytesToReplace) 
					{
					TPtrC8 replaceBuf = bufferPtr.Left(bytesToReplace);
					TPtrC8 appendBuf = bufferPtr.Right(sourceLength-bytesToReplace);
					iDes->Replace(aPosition, bytesToReplace, replaceBuf);
					iDes->Append(appendBuf);
					} 
				else
					iDes->Replace(aPosition, sourceLength, bufferPtr);

				} 
			else if (aPosition == iDes->Length())
				iDes->Append(bufferPtr.Ptr(),sourceLength);
			else
				{
				iDes->AppendFill(0,aPosition - iDes->Length());
				iDes->Append(bufferPtr.Ptr(),sourceLength);
				}
			}
		else if (targetLength<0)
			User::Leave(KErrArgument);
		else if (aLength != 0)
			User::Leave(KErrOverflow);

		if (aSupplier)
			aSupplier->BufferEmptiedL(aBuffer);
		}
	else
		User::Leave(KErrNotSupported);
	}

// Writes aBuffer at specified location into the sink descriptor
void CMMFExDescriptor::WriteBufferL( CMMFBuffer* aBuffer, TInt aPosition)
	{
	WriteBufferL( aBuffer, aPosition, NULL );
	}

// Gets the space available in the clip
TInt64 CMMFExDescriptor::BytesFree() 
	{
	// get difference between length and maxlength
	TInt64 length = iDes->Length() ;
	TInt64 maxLength =  iDes->MaxLength() ;
	return( maxLength - length ) ;
	}

// Gets the size of the clip
TInt CMMFExDescriptor::Size() 
	{
	// Length (not max length) of descriptor
	TInt length = iDes->Length();
	return(length);
	}
