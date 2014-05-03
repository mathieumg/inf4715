#pragma once

//**************************************************************************/
// Copyright (c) 2011 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Parameter IDs For EggShape Plug In
// AUTHOR: 
//***************************************************************************/

enum ParamBlocks
{ 
	egg_shape_params,
	// Indicate the number of references and instance parameter block
	// Any new parameter blocks that are binded to the instances themselves should be added before this.
	egg_shape_num_param_blocks,

	// Class parameters
	egg_shape_keyboard
};

enum EggShapeParamsElements
{ 
	egg_shape_make_donut,	// Whether the egg has 2 layer, like a donut.
	egg_shape_length,		// The length of the egg, defined to be 3 * R where R is the redius of the bottom circle
	egg_shape_width,		// The width of the egg, degined to be 2 * R where R is the redius of the bottom circle
	egg_shape_thickness,	// The distance between the inner layer and the outer layer
	egg_shape_rotation,		// The tilting of the egg with respect to the egg's vertical axis (axis going through the center of the upper circle and the bottom circle)

	egg_shape_num_params	// Keep track of the number of parameters
};

enum EggShapeKeyboardElements
{
	egg_shape_keyboard_length,
	egg_shape_keyboard_width,
	egg_shape_keyboard_thickness,
	egg_shape_keyboard_rotation,
	egg_shape_keyboard_position,

	egg_shape_num_keyboard_param
};
