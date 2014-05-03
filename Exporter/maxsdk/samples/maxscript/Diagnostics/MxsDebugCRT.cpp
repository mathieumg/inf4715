/**********************************************************************
*<
FILE: mxsDebugCRT.cpp

DESCRIPTION:	This maxscript plugin wraps a few important functions in the Debug CRT.

3DSMAX USAGE:	

CREATED BY:		Chris Johnson

HISTORY:		Started in Feb 2009

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <utilexp.h>
#include <dbgprint.h>
#include <maxscript\maxscript.h>
#include "mxsDebugCRT.h"


void InitMxsDebugCRT()
{
	// empty for now
}

// Declare C++ function and register it with MAXScript

#include "maxscript\macros\define_instantiation_functions.h"
	def_visible_primitive(heapcheck     , "CRTheapcheck"  );
	def_visible_primitive(CRTCheckMemory, "CRTCheckMemory");
	def_visible_primitive(CRTCorruptHeap, "CRTCorruptHeap");
	def_visible_primitive(CRTCheckAssert, "CRTCheckAssert");


// This method will corrupt the heap! Dangerous! Used for testing only.
Value* CRTCorruptHeap_cf(Value** arg_list, int count)
{
	//--------------------------------------------------------
	//Maxscript usage:
	// CRTCorruptHeap <int>offset
	//--------------------------------------------------------
	check_arg_count(CRTCorruptHeap, 1, count);
	int offset = arg_list[0]->to_int();
	
	int* iparray = new int[500]; // leaks memory
	// calculate an index that is past the bounds of the array
	int badIndex = 500 + offset;
	DebugPrint(_T("Corrupting Heap on purpose\n"));
	DebugPrint(_T("iparray[0] = %#x\n"), iparray);
	DebugPrint(_T("iparray[499] = %#x\n"), iparray + 499);
	DebugPrint(_T("iparray[badIndex] = %#x\n"), iparray + badIndex);
	// heap corruption
	iparray[badIndex] = 0xdeadbeaf;

	// clean up
	//delete[] iparray;
	//iparray = NULL;

	return &ok;
}

//! Runs consistency checks on the heap. 
//! This is enabled in release builds, unlike _CrtCheckMemory.
Value* heapcheck_cf(Value**, int count)
{
	check_arg_count(heapcheck, 0, count);
	int heap_status = _heapchk();
	/* I'll use these later
	switch (heap_status)
	{
	case _HEAPOK:
		break;
	case _HEAPEMPTY:
		break;
	case _HEAPBADPTR:
		break;
	case _HEAPBADNODE:
		break;
	case _HEAPBADBEGIN:
		break;
	}
	*/
	return (heap_status == _HEAPOK) ? &true_value : &false_value;
}

/*	\brief A Maxscript wrapper around the CRT Debug function _CrtCheckMemory
	This only works for debug builds. For release builds, use the maxscript function CRTheapcheck 
	defined immediately above this function.
	From MSDN: 
	Confirms the integrity of the memory blocks allocated in the debug heap (debug version only).
	The _CrtCheckMemory function validates memory allocated by the debug heap manager by verifying 
	the underlying base heap and inspecting every memory block. If an error or memory inconsistency 
	is encountered in the underlying base heap, the debug header information, or the overwrite buffers, 
	_CrtCheckMemory generates a debug report with information describing the error condition. 
	When _DEBUG is not defined, calls to _CrtCheckMemory are removed during preprocessing. */
Value* CRTCheckMemory_cf(Value** /*arg_list*/, int count)
{
	//--------------------------------------------------------
	//Maxscript usage:
	// CRTCheckMemory()
	//--------------------------------------------------------

	check_arg_count(CRTCheckMemory, 0, count);
	BOOL CRTresult = TRUE;
#ifdef _DEBUG
	CRTresult = _CrtCheckMemory();
#endif
	Value* result = &false_value;

	if (CRTresult)
		result = &true_value;
	
	return result;
}

Value* CRTCheckAssert_cf(Value** arg_list, int count)
{
	check_arg_count(CRTCheckAssert, 1, count);
	int choice = arg_list[0]->to_int();
	bool stop = false;
	switch (choice)
	{
	case 1: 
			DbgAssert(stop);
			break;
	case 2:
			assert(stop);
			break;
	}

	stop = true;

	return &ok;
}