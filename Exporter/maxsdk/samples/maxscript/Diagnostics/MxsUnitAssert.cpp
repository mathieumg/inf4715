
#include "MxsUnitReporter.h"
#include "MxsUnitAssert.h"
#include <maxscript/maxscript.h>
#include <maxscript/kernel/value.h>
#include <maxscript/foundation/numbers.h>
#include <maxscript/foundation/functions.h>

void InitMxsAssert()
{
	
}

// Declare C++ function and register it with MAXScript
#include <maxscript\macros\define_instantiation_functions.h>
	def_visible_primitive(assert_true     , "assert_true"     );
	def_visible_primitive(assert_false    , "assert_false"    );
	def_visible_primitive(assert_equal    , "assert_equal"    );
	def_visible_primitive(assert_not_equal, "assert_not_equal");
	def_visible_primitive(assert_defined  , "assert_defined"  );
	def_visible_primitive(assert_undefined, "assert_undefined");
	def_visible_primitive(assert_float, "assert_float");
	def_visible_primitive(assert_string_equal, "assert_string_equal");
	def_visible_primitive(assert_point3_equal, "assert_point3_equal");
	def_visible_primitive(assert_matrix_equal, "assert_matrix_equal");

static double DEFAULT_FLOAT_TOLERANCE = 0.001;
void LogFailure( Value* message, Value* expected, Value* actual )
{
	const MCHAR*        messageS    = NULL;
	if (message)
		 messageS = message->to_string();
	
	Value*        file_name   = thread_local(source_file);
	const MCHAR*        filename = _M("No File");
	if (file_name)
		filename = file_name->to_string();
	    

	StringStream* expected_stream = new StringStream(_T(""));
	expected->sprin1(expected_stream);
	const MCHAR* expectedString = expected_stream->to_string();

	StringStream* got_stream = new StringStream(_T(""));
	actual->sprin1(got_stream);
	const MCHAR* gotString = got_stream->to_string();

	MSTR function_name = _M("<No Stack frame>");
	Value** frame = thread_local(current_frame);
	if ( (frame != NULL) && (frame[2]->_is_function()) )
	{
		MAXScriptFunction* cur_fn = (MAXScriptFunction*)frame[2];
		function_name = cur_fn->name;
	}
	else if ( (frame != NULL) && (is_codeblock(frame[2])) )
	{
		CodeBlock* block = (CodeBlock*)frame[2];
		if (block->name)
		{
			function_name = block->name->to_string();
		}
	}

	unsigned int  line_number = (unsigned int)thread_local(source_line);
	MSTR failure = MxsUnitReporter::CreateMessage(function_name.data(), expectedString, gotString, messageS, filename , line_number );
	MxsUnitReporter::GetInstance()->LogAssertFailure(failure);
	// Also report to the maxscript listener, which is considered "standard out" in maxscript parlance.
	thread_local(current_stdout)->printf(_T("%s\n"), failure.data());
}

// common code for boolean assert methods
Value* BooleanAssert( Value** arg_list, int count, bool bExpected)
{
	Value* actual = arg_list[0];
	bool bActual = (actual->to_bool() == TRUE);

	Value* expected = bExpected ? &true_value : &false_value;

	Value* message = key_arg_or_default(message, NULL);
	Value* result = &true_value;
	if (bExpected != bActual)
	{
		// report error here
		result = &false_value;
		LogFailure(message, expected, actual);
	}
	return result;
}

//! common code for assert equal maxscript methods
Value* EqualAssert(bool bExpected, Value** arg_list,int count)
{
	Value* expected = arg_list[0];
	Value* actual   = arg_list[1];

	Value* message = key_arg_or_default(message, NULL);
	Value* result = &true_value;

	// All this is equivalent to the maxscript form of "value == value"
	BOOL comparible = expected->comparable(actual);
	Value* temp[1]  = {actual};
	bool equal      = expected->eq_vf(temp,0) == &true_value;
	bool same       = (comparible && equal);
	
	if (same != bExpected)
	{
		// report error here
		result = &false_value;
		LogFailure(message, expected, actual);
	}
	return result;
}

Value* FloatEqualAssert(bool bExpected, Value** arg_list,int count, double tolerance)
{
	Value* expected = arg_list[0];
	Value* actual   = arg_list[1];

	Value* message = key_arg_or_default(message, NULL);
	Value* result = &true_value;

	double d_expected = expected->to_double();
	double d_actual   = actual->to_double();
	double diff = d_expected - d_actual;
	if (diff < 0) // poor man's absolute value method. :)
	{
		diff = -diff;
	}

	bool good_enough = diff < tolerance;

	if (good_enough != bExpected)
	{
		// report error here
		result = &false_value;
		LogFailure(message, expected, actual);
	}
	return result;
}

Value* StringEqualAssert(bool bExpected, Value** arg_list,int count)
{
	Value* expected = arg_list[0];
	Value* actual   = arg_list[1];

	const MCHAR* s_expected = expected->to_string();
	const MCHAR* s_actual   = actual->to_string();

	// The default is to take case into account when comparing strings
	Value* ignoreCase = key_arg_or_default(ignoreCase, &true_value);
	BOOL ignore_case = ignoreCase->to_bool();
	Value* message = key_arg_or_default(message, NULL);
	
	bool same = false;
	if (ignore_case)
	{
		// case insensitive comparison
		same = _tcsicmp(s_expected,s_actual) == 0;
	} 
	else
	{
		same = _tcscmp(s_expected,s_actual) == 0;
	}

	Value* result = &true_value;
	if (same != bExpected)
	{
		// report error here
		result = &false_value;
		LogFailure(message, expected, actual);
	}
	return result;
}


bool ComparePoint3( const Point3& expected, const Point3& actual, double tolerance )
{
	int failures = 0;

	float diff_x = abs(expected.x - actual.x);
	if (diff_x > tolerance)
		failures++;

	float diff_y = abs(expected.y - actual.y);
	if (diff_y > tolerance)
		failures++;

	float diff_z = abs(expected.z - actual.z);
	if (diff_z > tolerance)
		failures++;

	bool result = true;
	if (failures > 0)
	{
		result = false;
	}
	return result;
}
static const bool expect_true = true;
static const bool expect_false = false;
Value* assert_true_cf(Value** arg_list, int count)
{
	//--------------------------------------------------------
	// Maxscript usage:
	// assert_true <bool> [message:<string>]
	//--------------------------------------------------------
	check_arg_count_with_keys(assert_true, 1, count);
	Value* result = BooleanAssert(arg_list, count, expect_true);
	return result;
}

Value* assert_false_cf(Value** arg_list, int count)
{
	//--------------------------------------------------------
	// Maxscript usage:
	// assert_false <bool> [message:<string>]
	//--------------------------------------------------------
	check_arg_count_with_keys(assert_false, 1, count);
	Value* result = BooleanAssert(arg_list, count, expect_false);
	return result;
}



Value* assert_equal_cf(Value** arg_list, int count)
{
	//--------------------------------------------------------
	// Maxscript usage:
	// assert_equal expected actual [message:<string>]
	//--------------------------------------------------------
	check_arg_count_with_keys(assert_equal, 2, count);
	Value* result = EqualAssert(expect_true, arg_list,count);
	return result;
}

Value* assert_not_equal_cf(Value** arg_list, int count)
{
	//--------------------------------------------------------
	// Maxscript usage:
	// assert_not_equal expected actual [message:<string>]
	//--------------------------------------------------------
	check_arg_count_with_keys(assert_not_equal, 2, count);
	Value* result = EqualAssert(expect_false, arg_list, count);
	return result;
}

Value* assert_float_cf(Value** arg_list, int count)
{
	//--------------------------------------------------------
	// Maxscript usage:
	// assert_float expected actual [tolerance: delta] [message:<string>]
	//--------------------------------------------------------
	check_arg_count_with_keys(assert_float, 2, count);
	Value* arg = NULL;
	double tolerance  = float_key_arg(tolerance, arg, DEFAULT_FLOAT_TOLERANCE);
	Value* result = FloatEqualAssert(expect_true, arg_list,count, tolerance);
	return result;
}


Value* assert_point3_equal_cf(Value** arg_list, int count)
{
	//--------------------------------------------------------
	// Maxscript usage:
	// assert_point3_equal expected actual [tolerance: delta] [message:<string>]
	//--------------------------------------------------------
	check_arg_count_with_keys(assert_point3_equal, 2, count);

	Point3 expected = arg_list[0]->to_point3();
	Point3 actual   = arg_list[1]->to_point3();

	Value* arg = NULL;
	double tolerance  = float_key_arg(tolerance, arg, DEFAULT_FLOAT_TOLERANCE);

	bool same = ComparePoint3(expected, actual, tolerance);

	Value* result = &true_value;
	if (!same)
	{
		Value* message = key_arg_or_default(message, NULL);
		LogFailure(message, arg_list[0], arg_list[1]);
		result = &false_value;
	}
	return result;
}

Value* assert_matrix_equal_cf(Value** arg_list, int count)
{
	//--------------------------------------------------------
	// Maxscript usage:
	// assert_matrix_equal expected actual [tolerance: delta] [message:<string>]
	//--------------------------------------------------------
	check_arg_count_with_keys(assert_matrix_equal, 2, count);

	Matrix3 expected = arg_list[0]->to_matrix3();
	Matrix3 actual   = arg_list[1]->to_matrix3();

	Value* arg = NULL;
	double tolerance  = float_key_arg(tolerance, arg, DEFAULT_FLOAT_TOLERANCE);

	int failures = 0;

	bool same = ComparePoint3(expected.GetRow(0), actual.GetRow(0), tolerance);
	if (!same)
		failures++;
	same = ComparePoint3(expected.GetRow(1), actual.GetRow(1), tolerance);
	if (!same)
		failures++;
	same = ComparePoint3(expected.GetRow(2), actual.GetRow(2), tolerance);
	if (!same)
		failures++;
	same = ComparePoint3(expected.GetRow(3), actual.GetRow(3), tolerance);
	if (!same)
		failures++;

	Value* result = &true_value;
	if (failures > 0)
	{
		Value* message = key_arg_or_default(message, NULL);
		LogFailure(message, arg_list[0], arg_list[1]);
		result = &false_value;
	}
	return result;
}

Value* assert_string_equal_cf(Value** arg_list, int count)
{
	//--------------------------------------------------------
	// Maxscript usage:
	// assert_string_equal expected actual [ignorecase: val] [message:<string>]
	// NOTE: The default for ignore case is true, since maxscript itself ignores the case for
	// strings. So if you want a case sensitive comparison pass in false for val.
	//--------------------------------------------------------
	check_arg_count_with_keys(assert_equal, 2, count);
	Value* result = StringEqualAssert(expect_true, arg_list,count);
	return result;
}

Value* assert_defined_cf(Value** arg_list, int count)
{
	//--------------------------------------------------------
	// Maxscript usage:
	// assert_defined <val> [message:<string>]
	//--------------------------------------------------------
	check_arg_count_with_keys(assert_defined, 1, count);
	Value* actual   = arg_list[0]->eval();
	Value* message = key_arg_or_default(message, NULL);
	Value* result = &true_value;
	if (actual == &undefined)
	{
		// There is no global 'Defined' Value class, so I have to
		// use a string to stand in for the expected Value.
		Value* expected = new String(_T("defined"));
		LogFailure(message, expected, actual);
		result = &false_value;
	}
	
	return result;
}

Value* assert_undefined_cf(Value** arg_list, int count)
{
	//--------------------------------------------------------
	// Maxscript usage:
	// assert_undefined <val> [message:<string>]
	//--------------------------------------------------------
	check_arg_count_with_keys(assert_undefined, 1, count);
	Value* actual   = arg_list[0]->eval();
	Value* message = key_arg_or_default(message, NULL);
	Value* result = &true_value;
	if (actual != &undefined)
	{
		Value* expected = &undefined;
		LogFailure(message, expected, actual);
		result = &false_value;
	}
	return result;
}
