///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Base particle emitter implementation
///

#include "Precompiled.h"
#include "ParticleEmitter.h"

#include "VCNUtils/Utilities.h"

///////////////////////////////////////////////////////////////////////
VCNParticleEmitter::VCNParticleEmitter(ParticleEmitterParams& params)
: mSettings(VCNParticleCore::GetInstance()->GetSettings(params.name))
, mPosistion(params.pos)
, mDirection(params.dir)
, mLife(mSettings.emitterLife)
, mAlive(true)
, mNumToAdd(0)
, mActive(0)
, mLastUpdate(0.0f)
{
  mDirection.Normalize();

  mParticles.reserve(mSettings.rate * static_cast<int>(mSettings.particleLife) + 1);
}


///////////////////////////////////////////////////////////////////////
VCNParticleEmitter::~VCNParticleEmitter(void)
{
}


///////////////////////////////////////////////////////////////////////
const bool VCNParticleEmitter::Update(float dt)
{
  if(mAlive)
  {
    //determine # of particles_ to be added
    mNumToAdd = (int)(mSettings.rate * dt + mLastUpdate);
    if(mNumToAdd <= 0)
      mLastUpdate += float(mSettings.rate) * dt;
    if(mLastUpdate >= 1.0f)
      mLastUpdate -= 1.0f;

    //check if over the particle limit
    //int sub = numToAdd_ + active_ - (int)(settings_.rate * settings_.particleLife);
    //if(sub > 0)
    //numToAdd_ -= sub;

    //decrease life_ of emitter and check if its dead
    if(mLife != -1)
    {
      mLife -= dt;
      if(mLife <= 0)
        mAlive = false;
    }
  }
  else
    mNumToAdd = 0;

  UpdateParticles(dt);

  for(;mNumToAdd > 0; mNumToAdd--)
  {
    VCNParticle p;
    InitParticle(p, dt);
    mParticles.push_back(p);
    mActive++;
  }

  if(mActive <= 0 && !mAlive)
  {
    // Request to be deleted from particle manager
    return false;
  }

  return true;
}


///////////////////////////////////////////////////////////////////////
void VCNParticleEmitter::UpdateParticle(VCNParticle& p, float dt)
{
  if(p.life > 0.0f)
    p.life -= dt;

  //if its dead, then use it as an "add"
  if(p.life <= 0.0f || !p.alive)
  {
    if(mNumToAdd > 0)
    {
      if(!p.alive) 
        mActive++; // if it wasn't alive then add to active_
      InitParticle(p, dt);
      mNumToAdd--;
    }
    else
    {
      if(p.alive)
        mActive--; //if it WAS alive then subtract from active_
      p.alive = false;
    }
  }
  else if (p.active) //it's alive so perform rotation calculations
  {
    p.rotation += p.rotRate * dt;
    if(p.rotation >= 4*VCN::PI_VCN || p.rotation <= -4*VCN::PI_VCN) p.rotation /= 2;
  }
}


/******************************************************************/
/*                  VCNParticleDefaultEmitter                     */
/******************************************************************/


VCNParticleDefaultEmitter::VCNParticleDefaultEmitter(ParticleEmitterParams& params)
  : VCNParticleEmitter(params)
{
}


void VCNParticleDefaultEmitter::InitParticle(VCNParticle& p, float dt)
{
  float variance = mSettings.variance < 1.0f ? 1.0f : mSettings.variance; // just to make sure a variance of 0.0 is used

  //get a random direction...
  Vector3 direction = mDirection;
  float angle       = VCN::Random(-VCN::PI_VCN, VCN::PI_VCN);
  direction.x       += cos(angle);
  direction.y       += VCN::Random(-1.0f, 1.0f);
  direction.z       += sin(angle);
  direction.Normalize();

  p.vel       = direction * VCN::Random(mSettings.vel / variance, mSettings.vel);
  p.life      = VCN::Random(mSettings.particleLife / variance, mSettings.particleLife);
  p.initlife  = p.life;
  p.size      = VCN::Random(mSettings.size / variance, mSettings.size);
  p.pos       = mPosistion + direction * VCN::Random(0.01f,1.0f) * dt;
  p.rotation  = VCN::Random(-VCN::PI_VCN, VCN::PI_VCN);
  p.rotRate   = VCN::Random(mSettings.rotationRate / variance, mSettings.rotationRate) * (p.rotation > 0 ? 1 : -1);
  p.alive     = true;
  p.active    = true;
}

// Perform your own operations on particles then call updateParticle(p,dt)
void VCNParticleDefaultEmitter::UpdateParticles(float dt)
{
  std::vector<VCNParticle>::iterator itor = mParticles.begin();
  for(; itor != mParticles.end(); ++itor )
  {
    itor->vel += mSettings.accel * dt;
    itor->pos += itor->vel * dt;

    UpdateParticle(*itor, dt);
  }
}

