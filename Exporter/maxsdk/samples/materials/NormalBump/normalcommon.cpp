/**********************************************************************
 *<
	FILE: common.cpp

	DESCRIPTION:	Common items for Normal Bump and Vector Displacement

	CREATED BY: 

	HISTORY:

 *>	Copyright (c) 2010, All Rights Reserved.
 **********************************************************************/

// Self
#include "normalcommon.h"

// System
#include <intrin.h>



// Atomic operation:
// Sets (*p)+=a, and returns the previous value of (*p)
DWORD LockXAdd( DWORD* p, DWORD a ) {
   return _InterlockedExchangeAdd(reinterpret_cast<LONG *>(p), a);
}

// Atomic operation:
// If (*p)==a, then ... sets (*p)=b and returns true
// Else           ... sets (*q)=(*p) and returns false
DWORD LockCmpXChg( DWORD* p, DWORD Exchange, DWORD Comperand ) {
   return _InterlockedCompareExchange(reinterpret_cast<LONG *>(p), Exchange, Comperand);
}

// Atomic operation: (*p)++
void LockInc( DWORD* p ) {
   _InterlockedIncrement(reinterpret_cast<LONG*>(p));
}

// Atomic operation: (*p)--
void LockDec( DWORD* p ) {
   _InterlockedDecrement(reinterpret_cast<LONG*>(p));
}

// Atomic operation: (*p) &= a
void LockAnd( DWORD* p, DWORD a ) {
   _InterlockedAnd(reinterpret_cast<LONG*>(p), a);
}

//===========================================================================
//
// Class ReaderWriterLock
//
//===========================================================================

#define READER_BITS 0x7FFFFFFF
#define WRITER_BITS 0x80000000

void ReaderWriterLock::EnterRead() {
   // Increments the count, and get the previous count (atomic operation)
   int checkBits = LockXAdd( &bits, 1 );
   while( checkBits & WRITER_BITS ) {
      Sleep(0); //SwitchToThread();
      checkBits = bits;
   }
}

void ReaderWriterLock::ExitRead() {
   DbgAssert( (bits & READER_BITS)!=0 );
   LockDec( &bits ); //bits-- (atomic operation)
}

void ReaderWriterLock::EnterWrite() {
   DWORD prevBits;
   while( TRUE ) {
      // If bits==0, sets bits=WRITER_BITS. Returns original value of bits (atomic operation)
      prevBits = LockCmpXChg( &bits, WRITER_BITS, 0 );
      if( prevBits==0  ) break; // success
      else
         Sleep(0); //SwitchToThread();
   }
}

void ReaderWriterLock::ExitWrite() {
   DbgAssert( (bits & WRITER_BITS)!=0 );
   LockAnd( &bits, (~WRITER_BITS) ); //bits &= ~WRITER_BITS (atomic operation)
}
