#include "MxsStopWatch.h"

// These declarations here expose the methods or operations on stopwatch
// to maxscript
#undef def_property_getter
#define def_property_getter(a)
#undef def_local_generic
#define def_local_generic(fn, name)	\
	MxsStopWatchGeneric fn##_gf (_M(name), &MxsStopWatch::fn##_vf)
#include "MxsStopWatch.inl"
// It looks like this when its done:
/*
MxsStopWatchGeneric Start_gf  ("Start",  &MxsStopWatch::Start_vf);
MxsStopWatchGeneric Pause_gf  ("Pause",  &MxsStopWatch::Pause_vf);
MxsStopWatchGeneric Resume_gf ("Resume", &MxsStopWatch::Resume_vf);
MxsStopWatchGeneric Stop_gf   ("Stop",   &MxsStopWatch::Stop_vf);
MxsStopWatchGeneric Reset_gf  ("Reset",  &MxsStopWatch::Reset_vf);
*/

// Declare the Property value Names
#undef def_local_generic
#define def_local_generic(a,b)
#undef def_property_getter
#define def_property_getter(name) Value* n_##name;
#include "MxsStopWatch.inl"
// It looks like this when its done:
/*
Value* n_IsRunning;
Value* n_IsPaused;
Value* n_IsStopped;
Value* n_TimerCurrentState;
Value* n_ElapsedTime;
*/

// Instantiate the property value Names.
#undef def_property_getter
#define def_property_getter(name) n_##name = Name::intern(_M(#name));
void InitMxsStopWatch()
{
	// code commented out below
	#include "MxsStopWatch.inl"
	// It looks like th is when its done:
	/*
	n_IsRunning = Name::intern("IsRunning");
	n_IsPaused = Name::intern("IsPaused");
	n_IsStopped = Name::intern("IsStopped");
	n_TimerCurrentState = Name::intern("TimerCurrentState");
	n_ElapsedTime = Name::intern("ElapsedTime");
	*/
}

DEFINE_LOCAL_GENERIC_CLASS_DEBUG_OK(MxsStopWatch,MxsStopWatchGeneric);
local_visible_class_instance(MxsStopWatch,_T("StopWatch"))

// ===========================================================================
// MxsStopWatchClass
// Meta class responsible for instantiating the actual maxscript stopwatch
// ===========================================================================

// declared through the macro local_applyable_class
Value* MxsStopWatchClass::apply(Value**, int count, CallContext* )
{
	check_arg_count(StopWatch, 0, count);
	Value* watch = new MxsStopWatch();
	return watch;
}

// ===========================================================================
// MxsStopWatch
// ===========================================================================

MxsStopWatch::MxsStopWatch()
{
	tag = class_tag(MxsStopWatch);
}

MxsStopWatch::~MxsStopWatch()
{ }

void  MxsStopWatch::collect()
{
	delete this;
}

void  MxsStopWatch::sprin1(CharStream* s)
{
	s->printf( _M("Stop Watch Class %f"), mWatch.GetElapsedTime() );
}

// Maxscript visible Methods
Value* MxsStopWatch::Start_vf(Value** /*arg_list*/, int count)
{
	check_arg_count(Start, 0, count);
	mWatch.Start();
	return &ok;
}

Value* MxsStopWatch::Pause_vf(Value** /*arg_list*/, int count)
{
	check_arg_count(Pause, 0, count);
	mWatch.Pause();
	return &ok;
}

Value* MxsStopWatch::Resume_vf(Value** /*arg_list*/, int count)
{
	check_arg_count(Resume, 0, count);
	mWatch.Resume();
	return &ok;
}

Value* MxsStopWatch::Stop_vf(Value** /*arg_list*/, int count)
{
	check_arg_count(Stop, 0, count);
	mWatch.Stop();
	return &ok;
}

Value* MxsStopWatch::Reset_vf(Value** /*arg_list*/, int count)
{
	check_arg_count(Reset, 0, count);
	mWatch.Reset();
	return &ok;
}

// Maxscript visible Properties
Value* MxsStopWatch::get_IsRunning(Value** , int )
{
	return NULL;
}

Value* MxsStopWatch::get_IsPaused(Value** , int )
{
	return mWatch.IsPaused() ? &true_value : &false_value;
}

Value* MxsStopWatch::get_IsStopped(Value** , int )
{
	return mWatch.IsStopped() ? &true_value : &false_value;
}

Value* MxsStopWatch::get_TimerCurrentState(Value** , int )
{
	int state = (int)mWatch.GetTimerCurrentState();
	return Integer::intern(state);
}

Value* MxsStopWatch::get_ElapsedTime(Value** , int )
{
	double elapsed = mWatch.GetElapsedTime();
	return Double::intern(elapsed);
}

Value*
MxsStopWatch::get_property(Value** arg_list, int count)
{
	Value* prop = arg_list[0];
	if (prop == n_IsRunning)
		return mWatch.IsRunning() ? &true_value : &false_value;
	else if (prop == n_IsPaused)
		return mWatch.IsPaused() ? &true_value : &false_value;
	else if (prop == n_IsStopped)
		return mWatch.IsStopped() ? &true_value : &false_value;
	else if (prop == n_TimerCurrentState)
		return Integer::intern( mWatch.GetTimerCurrentState() );
	else if (prop == n_ElapsedTime)
		return Double::intern( mWatch.GetElapsedTime() );
	else
		return Value::get_property(arg_list, count);
}

Value*
MxsStopWatch::set_property(Value** arg_list, int count)
{
	return Value::set_property(arg_list, count);
}