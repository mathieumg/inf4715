/*===========================================================================*\
 | 
 |  FILE:	MorpherMXS.h
 |			A new MAX Script Plugin that adds Morpher modifier access
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 5-4-99
 | 
\*===========================================================================*/

#include <maxscript/maxscript.h>

// Various MAX and MXS includes
#include <maxscript/foundation/numbers.h>
#include <maxscript/maxwrapper/maxclasses.h>
#include <maxscript/foundation/streams.h>
#include <maxscript/foundation/mxstime.h>
#include <maxscript/maxwrapper/mxsobjects.h>
#include <maxscript/compiler/parser.h>
#include <maxscript/foundation/3dmath.h>
#include <maxscript/foundation/numbers.h>

#include "max.h"
#include "stdmat.h"

// Morpher header
#include "wm3.h"

// define the new primitives using macros from SDK
#include <maxscript\macros\define_instantiation_functions.h>



BOOL check_ValidMorpher(ReferenceTarget* obj,Value** arg_list);
