#include <maxscript/maxscript.h>
#include <maxscript/kernel/value.h>
#include <maxscript/foundation/numbers.h>
#include <maxscript/foundation/functions.h>

#ifdef _WIN64
	static const TCHAR* VLDDLL = _T("vld_x64.dll");
#else
	static const TCHAR* VLDDLL = _T("vld_x86.dll");
#endif

HMODULE ghvld = NULL;
extern void GetFormattedMessage(DWORD last_error);

void InitMxsVLD()
{
	if (ghvld == NULL)
	{
		ghvld = GetModuleHandle(VLDDLL);
		if (ghvld == NULL)
		{
			GetFormattedMessage(GetLastError());
		}
	}
}

// Declare C++ function and register it with MAXScript
// This will be visible with maxscript as a global struct called "VLD"
// The global struct will have the following member methods:
#include <maxscript\macros\define_instantiation_functions.h>
	def_struct_primitive( Disable       , VLD, "Disable" );
	def_struct_primitive( Enable , VLD, "Enable" );
	def_struct_primitive( Restore , VLD, "Restore" );
	def_struct_primitive( ReportLeaks , VLD, "ReportLeaks" );
	def_struct_primitive( RefreshModules , VLD, "RefreshModules" );
	def_struct_primitive( ResolveCallstacks , VLD, "ResolveCallstacks" );
	def_struct_primitive( EnableModule , VLD, "EnableModule" );
	def_struct_primitive( DisableModule  , VLD, "DisableModule" );
	def_struct_primitive( TestLeakMemory , VLD, "TestLeakMemory" );


typedef void (*VLDAPI_func)();

Value* Disable_cf(Value** , int count)
{
	check_arg_count(MxsVLD_Disable, 0, count);
	if (ghvld == NULL)
	{
		return &undefined;
	}
	VLDAPI_func func = (VLDAPI_func)GetProcAddress(ghvld, "VLDDisable");
	if (func != NULL)
	{
		func();
		return &ok;
	}
	return &undefined;
}

Value* Enable_cf(Value** , int count)
{
	check_arg_count(Enable, 0, count);
	if (ghvld == NULL)
	{
		return &undefined;
	}
	VLDAPI_func func = (VLDAPI_func)GetProcAddress(ghvld, "VLDEnable");
	if (func != NULL)
	{
		func();
		return &ok;
	}
	return &undefined;
}

Value* Restore_cf(Value** , int count)
{
	check_arg_count(Restore, 0, count);
	if (ghvld == NULL)
	{
		return &undefined;
	}
	VLDAPI_func func = (VLDAPI_func)GetProcAddress(ghvld, "VLDRestore");
	if (func != NULL)
	{
		func();
		return &ok;
	}
	return &undefined;
}

Value* ReportLeaks_cf(Value** , int count)
{
	check_arg_count(ReportLeaks, 0, count);
	if (ghvld == NULL)
	{
		return &undefined;
	}
	VLDAPI_func func = (VLDAPI_func)GetProcAddress(ghvld, "VLDReportLeaks");
	if (func != NULL)
	{
		func();
		return &ok;
	}
	return &undefined;
}

Value* RefreshModules_cf(Value** , int count)
{
	check_arg_count(RefreshModules, 0, count);
	if (ghvld == NULL)
	{
		return &undefined;
	}
	VLDAPI_func func = (VLDAPI_func)GetProcAddress(ghvld, "VLDRefreshModules");
	if (func != NULL)
	{
		func();
		return &ok;
	}
	return &undefined;
}

Value* ResolveCallstacks_cf(Value** , int count)
{
	check_arg_count(ResolveCallstacks, 0, count);
	if (ghvld == NULL)
	{
		return &undefined;
	}
	VLDAPI_func func = (VLDAPI_func)GetProcAddress(ghvld, "VLDResolveCallstacks");
	if (func != NULL)
	{
		func();
		return &ok;
	}
	return &undefined;
}

Value* EnableModule_cf(Value** /*arg_list*/, int count)
{
	check_arg_count(EnableModule, 1, count);

	return &ok;
}

Value* DisableModule_cf(Value** , int count)
{
	check_arg_count(DisableModule, 1, count);

	return &ok;
}


void MxsVLDLeakMemory(int amount)
{
	void* foo = malloc(amount);
	UNUSED_PARAM(foo);
	int* bar = new int[amount];
	UNUSED_PARAM(bar);
}

Value* TestLeakMemory_cf(Value** arg_list, int count)
{
	check_arg_count(TestLeakMemory, 1, count);

	int amount = arg_list[0]->to_int();

	MxsVLDLeakMemory(amount);

	return &ok;
}
