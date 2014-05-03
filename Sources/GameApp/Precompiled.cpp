///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"

/// This is done because we do not link against the standard library and
/// the VCNLua.lib library needs the definition of HUGE_VAL in it.
double _HUGE = HUGE_VAL;

// Uncomment to detect leaks
// http://vld.codeplex.com/
//#include <vld.h>
