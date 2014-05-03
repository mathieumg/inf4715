#include <StopWatch.h>
#include <maxscript/maxscript.h>
#include <maxscript/foundation/numbers.h>
#include <maxscript/macros/generic_class.h>
#include <maxscript/macros/local_class.h>

//! Called when this Library is loaded.
void InitMxsStopWatch();

class MxsStopWatch;
/* this defines two classes:
	class MxsStopWatchGenericClass
	class MxsStopWatchGeneric    */
DECLARE_LOCAL_GENERIC_CLASS(MxsStopWatch,MxsStopWatchGeneric);

/*	Macro to create a factory class which is actually responsible for 
	instantiating the stopwatch for defining class MxsStopWatchClass */
local_applyable_class(MxsStopWatch);

class MxsStopWatch : public Value
{
public:
	MxsStopWatch();
	~MxsStopWatch();
	classof_methods (MxsStopWatch, Value);
	void  collect();
	void  sprin1(CharStream* s);
	ValueMetaClass* local_base_class() { return class_tag(MxsStopWatch); } // local base class in this class's plug-in
#undef def_local_generic
#undef def_property
#define def_local_generic(fn, name)	\
	Value* fn##_vf(Value** arglist, int arg_count)
// All properties on the stop watch class are read only
#define def_property_getter(prop)										\
	Value*	get_##prop(Value** arg_list, int count);

#include "MxsStopWatch.inl"

	Value*	get_property(Value** arg_list, int count);
	Value*	set_property(Value** arg_list, int count);

private:
	MaxSDK::Util::StopWatch mWatch;
};


