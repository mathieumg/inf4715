#include "MxsUnitReporter.h"
#include <maxscript/foundation/numbers.h>
#include <maxscript/foundation/strings.h>

MxsUnitReporter * MxsUnitReporter::mInstance = NULL;

MxsUnitReporter::MxsUnitReporter()
{
	mMessages.setLengthReserved(10);
}

MxsUnitReporter::~MxsUnitReporter()
{
	
}

MSTR MxsUnitReporter::CreateMessage(const MCHAR* frame_name, const MCHAR* expected, const MCHAR* actual, const MCHAR* message, const MCHAR* fileName, unsigned int lineNumber)
{
	MSTR result;

	if (NULL == message)
	{
		result.printf(_M("Assert - Expected %s, got %s - (%s) in frame %s, at line %i"), expected, actual, fileName, frame_name, lineNumber);
	}
	else
	{
		result.printf(_M("Assert - %s - Expected %s, got %s (%s) in frame %s, at line %i"), message, expected, actual, fileName, frame_name, lineNumber );
	}
	
	return result;
}

void MxsUnitReporter::LogAssertFailure( MSTR message)
{
	MxsUnitMessage entry;
	entry.message = message;
	entry.type = AssertFailure;
	mMessages.append(entry);
}

void MxsUnitReporter::LogAssertFailure( const MCHAR* message)
{
	MxsUnitMessage entry;
	entry.message = MSTR(message);
	entry.type = AssertFailure;
	mMessages.append(entry);
}

void MxsUnitReporter::LogExceptionFailure( MSTR message)
{
	MxsUnitMessage entry;
	entry.message = message;
	entry.type = ExceptionFailure;
	mMessages.append(entry);
}

void MxsUnitReporter::LogExceptionFailure( const MCHAR* message)
{
	MxsUnitMessage entry;
	entry.message = MSTR(message);
	entry.type = ExceptionFailure;
	mMessages.append(entry);
}

void MxsUnitReporter::LogMessage( const MCHAR* message)
{
	MSTR temp(message);
	LogMessage(temp);
}

void MxsUnitReporter::LogMessage( MSTR message )
{
	MxsUnitMessage entry;
	entry.message = message;
	entry.type = LoggedMessage;
	mMessages.append(entry);
}
MaxSDK::Array<MSTR> MxsUnitReporter::GetFailure(ReportType type)
{
	MaxSDK::Array<MSTR> result;
	for (size_t i = 0; i < mMessages.length(); i++)
	{
		if (mMessages[i].type == type)
		{
			MSTR entry = mMessages[i].message;
			result.append(entry);
		}
	}
	return result;
}

MaxSDK::Array<MSTR> MxsUnitReporter::GetAssertFailures()
{
	return GetFailure(AssertFailure);
}

MaxSDK::Array<MSTR> MxsUnitReporter::GetExceptionFailures()
{
	return GetFailure(ExceptionFailure);
}

MaxSDK::Array<MSTR> MxsUnitReporter::GetMessages()
{
	MaxSDK::Array<MSTR> result;
	for (size_t i = 0; i < mMessages.length(); i++)
	{
		MSTR entry = mMessages[i].message;
		result.append(entry);
	}
	return result;
}

void MxsUnitReporter::Clear()
{
	mMessages.removeAll();
	mUserData = _M("");
}

MxsUnitReporter* MxsUnitReporter::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new MxsUnitReporter();
	}
	return mInstance;
}

void MxsUnitReporter::DestroyInstance()
{
	if (mInstance != NULL)
	{
		delete mInstance;
		mInstance = NULL;
	}
}

MSTR MxsUnitReporter::GetUserData()
{
	return mUserData;
}

void MxsUnitReporter::SetUserData(MSTR& val)
{
	mUserData = val;
}

// ====================================================================
#include <maxscript/maxscript.h>
#include <maxscript/kernel/value.h>
#include <maxscript/foundation/arrays.h>

//! Utility method to convert a MaxSDK::Array to a maxscript string array
Array* GetArrayItems(MaxSDK::Array<MSTR>& theArray)
{
	size_t size = theArray.lengthUsed();
	Array* ar = new Array((int)size);
	for (size_t i = 0; i < size ; i++)
	{
		MSTR aString = theArray[i];
		String* gcString = new String(aString.data());
		ar->append(gcString);
	}
	return ar;
}

// Declare C++ function and register it with MAXScript
// This will be visible with maxscript as a global struct called "AssertReporter"
// The global struct will have the following member methods:
#include <maxscript\macros\define_instantiation_functions.h>
	def_struct_primitive( Clear       , AssertReporter, "Clear" );
	def_struct_primitive( GetAssertFailures , AssertReporter, "GetAssertFailures" );
	def_struct_primitive( GetAssertFailuresCount , AssertReporter, "GetAssertFailuresCount" );
	def_struct_primitive( GetExceptionFailures , AssertReporter, "GetExceptionFailures" );
	def_struct_primitive( GetExceptionFailuresCount , AssertReporter, "GetExceptionFailuresCount" );
	def_struct_primitive( GetMessages , AssertReporter, "GetMessages" );
	def_struct_primitive( LogMessage  , AssertReporter, "LogMessage" );
	def_struct_primitive( LogException  , AssertReporter, "LogException" );
	def_struct_primitive( LogAssertFailure  , AssertReporter, "LogAssertFailure" );
	def_struct_primitive( GetUserData, AssertReporter, "GetUserData" );
	def_struct_primitive( SetUserData, AssertReporter, "SetUserData" );


Value* Clear_cf(Value** , int count)
{
	check_arg_count(Clear, 0, count);
	MxsUnitReporter::GetInstance()->Clear();
	return &ok;
}

Value* GetAssertFailures_cf(Value** , int count)
{
	check_arg_count(GetAssertFailures, 0, count);
	MaxSDK::Array<MSTR> failures = MxsUnitReporter::GetInstance()->GetAssertFailures();
	one_typed_value_local(Array* result);
	vl.result = GetArrayItems(failures);
	return_value(vl.result);
}

Value* GetAssertFailuresCount_cf(Value** , int count)
{
	check_arg_count(GetAssertFailuresCount, 0, count);
	MaxSDK::Array<MSTR> failures = MxsUnitReporter::GetInstance()->GetAssertFailures();
	one_value_local(result);
	vl.result = Integer::intern((int)failures.lengthUsed());
	return_value(vl.result);
}

Value* GetExceptionFailures_cf(Value** , int count)
{
	check_arg_count(GetExceptionFailures, 0, count);
	MaxSDK::Array<MSTR> failures = MxsUnitReporter::GetInstance()->GetExceptionFailures();
	one_typed_value_local(Array* result);
	vl.result = GetArrayItems(failures);
	return_value(vl.result);
}

Value* GetExceptionFailuresCount_cf(Value** , int count)
{
	check_arg_count(GetAssertFailuresCount, 0, count);
	MaxSDK::Array<MSTR> failures = MxsUnitReporter::GetInstance()->GetExceptionFailures();
	one_value_local(result);
	vl.result = Integer::intern((int)failures.lengthUsed());
	return_value(vl.result);
}

Value* GetMessages_cf(Value** , int count)
{
	check_arg_count(GetMessages, 0, count);
	MaxSDK::Array<MSTR> messages = MxsUnitReporter::GetInstance()->GetMessages();
	one_typed_value_local(Array* result);
	vl.result = GetArrayItems(messages);
	return_value(vl.result);
}

Value* LogMessage_cf(Value** arg_list, int count)
{
	check_arg_count(LogMessage, 1, count);
	const MCHAR* message = arg_list[0]->to_string();
	MxsUnitReporter::GetInstance()->LogMessage(message);
	return &ok;
}

Value* LogException_cf(Value** arg_list, int count)
{
	check_arg_count(LogException, 1, count);
	const MCHAR* message = arg_list[0]->to_string();
	MxsUnitReporter::GetInstance()->LogExceptionFailure(message);
	return &ok;
}

Value* LogAssertFailure_cf(Value** arg_list, int count)
{
	check_arg_count(LogAssertFailure, 1, count);
	const MCHAR* message = arg_list[0]->to_string();
	MxsUnitReporter::GetInstance()->LogAssertFailure(message);
	return &ok;
}
Value* SetUserData_cf(Value** arg_list, int count)
{
	check_arg_count(SetUserData, 1, count);
	const MCHAR* message = arg_list[0]->to_string();
	MSTR msg(message);
	MxsUnitReporter::GetInstance()->SetUserData( msg );
	return &ok;
}
Value* GetUserData_cf(Value** /*arg_list*/, int count)
{
	check_arg_count(GetUserData, 1, count);
	MSTR data = MxsUnitReporter::GetInstance()->GetUserData();
	
	return new String( data.data());
}