///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Defines smart pointer types
///
/// @remark Changing this file might recompile everything
///

#ifndef VCNSMARTTYPES_H
#define VCNSMARTTYPES_H

#pragma once

#include <memory>

// Forward declarations
class VCNParticleEmitter;
class VCNShader;

// Smart pointer predefined types
typedef std::shared_ptr<VCNParticleEmitter>   VCNParticleEmitterPointer;
typedef std::shared_ptr<VCNShader>            VCNShaderPointer;

// Handles predefined types (Handles doesn't imply the life of the pointed object)
typedef std::weak_ptr<VCNShader>              VCNShaderHandle;

// Smart pointer list predefined types
typedef std::list<VCNParticleEmitterPointer>  VCNParticleEmitterList;

#endif // VCNSMARTTYPES_H
