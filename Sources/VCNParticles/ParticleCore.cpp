///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Particle module entry point implementation
///

#include "Precompiled.h"
#include "ParticleCore.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"

#include "VCNRenderer/GPUProfiling.h"
#include "VCNRenderer/MaterialCore.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNRenderer/XformCore.h"
#include "VCNResources/Cache.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Chrono.h"
#include "VCNUtils/Constants.h"
#include "VCNUtils/Error.h"
#include "VCNUtils/Sphere.h"

VCN_TYPE( VCNParticleCore, VCNCore<VCNParticleCore> );

///////////////////////////////////////////////////////////////////////
VCNParticleCore::VCNParticleCore()
  : mVB(0)
  , mIB(0)
  , mEffect(0)
  , mBase(0)
  , mFlush(512)
  , mDiscard(2048)
{
}

///////////////////////////////////////////////////////////////////////
VCNParticleCore::~VCNParticleCore()
{
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNParticleCore::Initialize()
{
  if ( !BaseCore::Initialize() )
    return false;

  // Create the particle effect
  mEffect = new VCNParticleEffect();

  // Create particle vertex buffer
  mVB = VCNResourceCore::GetInstance()->GetResource<VCNCache>( 
    VCNRenderCore::GetInstance()->CreateCache(  
      0, 
      mDiscard * 4 * sizeof(VCNParticleVertex),
      sizeof(VCNParticleVertex),
      CF_POOL_DEFAULT | CF_DYNAMIC | CF_WRITEONLY ) );
  VCN_ASSERT( mVB );

  // Create particle index buffer
  mIB = VCNResourceCore::GetInstance()->GetResource<VCNCache>( 
    VCNRenderCore::GetInstance()->CreateCache( VT_INDEX, nullptr, mFlush * 6 * kCacheStrides[VT_INDEX], CF_WRITEONLY ) );
  VCN_ASSERT( mIB );
    
  // First fill the index buffer
  FillIndexBuffer();

  mDirectory = _T("./Particles");

  // Load all of the files in directory
  TCHAR file[MAX_PATH];
  _stprintf(file, _T("%s/*.txt"), mDirectory.c_str());

  WIN32_FIND_DATA data;
  HANDLE hp = FindFirstFile(file, &data);
  do
  {
    _stprintf(file, _T("%s/%s"), mDirectory.c_str(), data.cFileName);
    mEmitterSettings.insert(LoadSettingsFromFile(file));
  } while(FindNextFile(hp, &data)); 
  // Terminate the find
  FindClose(hp);

  return true;
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNParticleCore::Uninitialize()
{
  mVB->Release();
  mVB = 0;

  mIB->Release();
  mIB = 0;

  delete mEffect;
  mEffect = 0;

  return BaseCore::Uninitialize();
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNParticleCore::Process(const float dt)
{
  VCNParticleEmitterList::iterator it = mEmitters.begin();
  while(it != mEmitters.end())
  {
    if( !(*it)->Update(dt) )
      it = mEmitters.erase(it);
    else
      ++it;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////
void VCNParticleCore::Render()
{
  GPU_PROFILE_BLOCK_NAME( L"Particles" );

  if ( mEmitters.empty() || GetParticleCount() == 0 )
    return;

  VCNRenderCore* renderer = VCNRenderCore::GetInstance();

  mEffect->Begin( mIB, mVB );

  // Lock and fill the vertex buffer in chunks
  VCNParticleVertex* vertices;
  mVB->Lock( mBase * sizeof(VCNParticleVertex) * 4, mFlush * sizeof(VCNParticleVertex) * 4, 
    (VCNByte**)&vertices, mBase ? CF_LOCK_NOOVERWRITE : CF_LOCK_DISCARD );

  int numParticlesToRender = 0;

  VCNSphere emitterBoundingSphere;
  const Frustum& viewFrustum = VCNXformCore::GetInstance()->GetViewFrustum();

  VCNParticleEmitterList::const_iterator eItor = mEmitters.begin();
  VCNParticleEmitterList::const_iterator eItorEnd = mEmitters.end();
  for(; eItor != eItorEnd; ++eItor)
  {
    const VCNParticleEmitterPointer emitter = (*eItor);
    const ParticleEmitterSettings& emitterSettings = emitter->GetSettings();

    // Discard the emitter if it is not in the view frustum.
    emitterBoundingSphere.Set( (emitterSettings.size+emitterSettings.vel) * 2.0f, emitter->GetPosition() );
    if ( !viewFrustum.SphereInFrustum(emitterBoundingSphere.GetCenter(), emitterBoundingSphere.GetRadius()))
    {
      //TRACE( L"Skip\n" );
      continue;
    }

    // Set the particle texture if any.
    if ( emitterSettings.texID != kInvalidResID )
    {
      mEffect->SetTexture( emitterSettings.texID );
    }

    //check blend mode
    switch( emitterSettings.blend )
    {
    case ALPHA:
      renderer->SetBlendMode( RS_BLEND_ALPHA );
      break;
    case ADDITIVE:
      renderer->SetBlendMode( RS_BLEND_ADDITIVE );
      break;
    case SUBTRACTIVE:
      renderer->SetBlendMode( RS_BLEND_SUBTRACTIVE );
      break;
    }

    const std::vector<VCNParticle>& particles = emitter->GetParticles();
    std::vector<VCNParticle>::const_iterator pItor = particles.begin();
    std::vector<VCNParticle>::const_iterator pItorEnd = particles.end();
    for(; pItor != pItorEnd; ++pItor)
    {
      const VCNParticle& particle = *pItor;
      if(!particle.alive) continue; //if NOT alive continue

      // Compute vertices
      const float percent = (particle.initlife - particle.life) / particle.initlife;
      const VCNColor xcolor = emitterSettings.startColor + (emitterSettings.endColor - emitterSettings.startColor) * percent;
      const unsigned long color = xcolor.ToARGB();

      float size = particle.size;
      float rotate = particle.rotation;
      vertices->pos   = particle.pos;
      vertices->color = color;
      vertices->tex.Set(-size, -size, rotate);
      vertices++;
      vertices->pos   = particle.pos;
      vertices->color = color;
      vertices->tex.Set(-size, size, rotate);
      vertices++;
      vertices->pos   = particle.pos;
      vertices->color = color;
      vertices->tex.Set(size, size, rotate);
      vertices++;
      vertices->pos   = particle.pos;
      vertices->color = color;
      vertices->tex.Set(size, -size, rotate);
      vertices++;

      // If you reach the end of this chunk send those particles
      if (++numParticlesToRender >= mFlush) 
      {
        FlushBuffer(numParticlesToRender);
        
        // Lock next chunk in buffers
        mVB->Lock( mBase * sizeof(VCNParticleVertex) * 4, mFlush * sizeof(VCNParticleVertex) * 4,
          (VCNByte**)&vertices, mBase ? CF_LOCK_NOOVERWRITE : CF_LOCK_DISCARD );
      }
    }

    // If there are any left over at the end of this emitter render those
    if( numParticlesToRender ) 
    {
      FlushBuffer(numParticlesToRender);

      // Lock next chunk in buffers
      mVB->Lock( mBase * sizeof(VCNParticleVertex) * 4, mFlush * sizeof(VCNParticleVertex) * 4,
        (VCNByte**)&vertices, mBase ? CF_LOCK_NOOVERWRITE : CF_LOCK_DISCARD );
    }
  }

  mVB->Unlock();
  mBase = 0;

  mEffect->End();
}


///////////////////////////////////////////////////////////////////////
ParticleEmitterSettings& VCNParticleCore::GetSettings(const VCNString& name)
{
  SettingMap::iterator i = mEmitterSettings.find(name);
  if (i == mEmitterSettings.end())
    throw VCNException(L"Cannot find EmitterSettings: " + name);
  return i->second;
}

///////////////////////////////////////////////////////////////////////
void VCNParticleCore::FillIndexBuffer()
{
  if ( mIB )
  {
    VCNUInt16* pIndices;
    if ( mIB->Lock( 0, kCacheStrides[VT_INDEX] * 6 * mFlush, (VCNByte**)&pIndices, 0 ))
    {
      int index = 0;
      for(int i=0; i < (int)mFlush * 6;)
      {
        pIndices[i++] = static_cast<WORD>(index);
        pIndices[i++] = static_cast<WORD>(index + 1);
        pIndices[i++] = static_cast<WORD>(index + 2);
        pIndices[i++] = static_cast<WORD>(index + 3);
        pIndices[i++] = static_cast<WORD>(index);
        pIndices[i++] = static_cast<WORD>(index + 2);
        index += 4;
      } 
      mIB->Unlock();
    }
  }
}

///////////////////////////////////////////////////////////////////////
int VCNParticleCore::GetParticleCount() const
{
  int count = 0;
  VCNParticleEmitterList::const_iterator itor = mEmitters.begin();
  for(; itor != mEmitters.end(); ++itor)
    count += (*itor)->GetParticleCount();
  return count;
}


///////////////////////////////////////////////////////////////////////
VCNParticleCore::SettingMap::value_type VCNParticleCore::LoadSettingsFromFile(const VCNString& emitterFileName)
{
  ParticleEmitterSettings es;
  VCNString name = L"";

  // load it from file
  std::wifstream f_in(emitterFileName.c_str());
  if(!f_in) {
    throw new VCNException( L"Could NOT load " + emitterFileName + L" in VCNParticleCore::loadSettingsFromFile");
  }
  else {
    VCNString dummy; //used to store the dummy strings before actual data on each line
    f_in >> dummy; f_in >> name;
    
    VCNString texturePath;
    f_in >> dummy; f_in >> texturePath;
    texturePath = L"./Textures/" + texturePath;

    es.texID = VCNMaterialCore::GetInstance()->CreateTexture( texturePath );
    
    f_in >> dummy; f_in >> es.accel.x >> es.accel.y >> es.accel.z;
    f_in >> dummy; f_in >> es.startColor.r >> es.startColor.g >> es.startColor.b >> es.startColor.a;
    f_in >> dummy; f_in >> es.endColor.r >> es.endColor.g >> es.endColor.b >> es.endColor.a;
    
    VCNString blend;
    f_in >> dummy; f_in >> blend;
    
    if(blend == L"alpha")
      es.blend = ALPHA;
    else if(blend == L"additive")
      es.blend = ADDITIVE;
    else
      es.blend = SUBTRACTIVE;
    
    f_in >> dummy; f_in >> es.emitterLife;
    f_in >> dummy; f_in >> es.particleLife;
    f_in >> dummy; f_in >> es.rate;
    f_in >> dummy; f_in >> es.vel;
    f_in >> dummy; f_in >> es.size;
    f_in >> dummy; f_in >> es.variance;
    f_in >> dummy; f_in >> es.rotationRate;
    
    f_in.close();
  }
  return std::map<VCNString, ParticleEmitterSettings>::value_type(name,es);
}


///////////////////////////////////////////////////////////////////////
void VCNParticleCore::FlushBuffer(int& numParticlesToRender)
{
  mVB->Unlock();

  // Render particles
  mEffect->RenderIndexedPrimitive( PT_TRIANGLELIST, mBase * 4, 0, numParticlesToRender * 4, 0, numParticlesToRender * 2 );

  mBase += mFlush;
  if(mBase >= mDiscard)
    mBase = 0;

  numParticlesToRender = 0;
}

///////////////////////////////////////////////////////////////////////
void VCNParticleCore::AddEmitter(VCNParticleEmitterPointer em)
{
  mEmitters.push_back(em);
}

///////////////////////////////////////////////////////////////////////
void VCNParticleCore::RemoveEmitter(VCNParticleEmitterPointer emitter)
{
  mEmitters.erase( std::remove(mEmitters.begin(), mEmitters.end(), emitter), mEmitters.end()); 
}
