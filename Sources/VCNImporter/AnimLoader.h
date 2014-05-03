///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// This class contains all the facilities to read animations
/// from files (XML or BIN) and to write animations to files
/// (BIN).
///

#ifndef VICUNA_ANIMATION_LOADER
#define VICUNA_ANIMATION_LOADER

#pragma once

#include "XMLLoader.h"
#include "VCNUtils/Types.h"

class VCNAnim;
class VCNAnimJoint;

class VCNAnimLoader : public VCNXMLLoader
{

public:

	static VCNAnim* LoadAnim( const VCNString& filename );

protected:

	// Functions relating to XML loading...
	static VCNAnim* LoadAnimXML( const VCNString& filename );
	static VCNAnimJoint* LoadJointXML( XMLNodePtr jointNode );

protected:

	// This utility class should not be instanced.
	VCNAnimLoader();
	virtual ~VCNAnimLoader();

};

#endif
