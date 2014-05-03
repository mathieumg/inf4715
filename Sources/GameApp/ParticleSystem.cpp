///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Game particle system implementation
///

#include "Precompiled.h"
#include "ParticleSystem.h"

// Project includes

// Engine includes
#include "VCNCore/CoreTimer.h"
#include "VCNParticles/ParticleCore.h"
#include "VCNParticles/ParticleEmitter.h"
#include "VCNRenderer/EffectCore.h"
#include "VCNRenderer/GPUProfiling.h"
#include "VCNRenderer/XformCore.h"

//////////////////////////////////////////////////////////////////////////
ParticleSystem::ParticleSystem( VCNNodeID nodeID )
: VCNNode( nodeID )
{
  SetTag( VCNTXT("Particle system") );
}

//////////////////////////////////////////////////////////////////////////
ParticleSystem::~ParticleSystem()
{
}

//////////////////////////////////////////////////////////////////////////
void ParticleSystem::Process(const float elapsedTime)
{
  VCNNode::Process( elapsedTime );
}

//////////////////////////////////////////////////////////////////////////
void ParticleSystem::Render() const
{
  if ( VCNEffectCore::GetInstance()->IsSceneEffect() )
    return;

  VCNParticleCore::GetInstance()->Render();
}

///////////////////////////////////////////////////////////////////////
void ParticleSystem::CreateFireEmitter(const Vector3& position)
{
  //set up parameters for creation
  ParticleEmitterParams params;
  params.name = L"lantern";
  params.pos = position;
  params.dir = Vector3( 0, 1.0f, 0 );
  params.radius = 0.2f;

  VCNParticleCore::GetInstance()->AddEmitter( VCNParticleEmitterPointer(new VCNParticleDefaultEmitter(params)) );
}