///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "D3D9Animator.h"

#include "D3D9Types.h"

#include "VCNUtils/Assert.h"
#include "VCNUtils/Matrix.h"

void UpdateFrameMatrices( LPD3DXFRAME pFrameBase, const D3DXMATRIX* pParentMatrix );
ID3DXAnimationController* CloneAnimationController(ID3DXAnimationController* other);

class VCND3DAnimatorCallbackHandler : public ID3DXAnimationCallbackHandler
{
public:
    VCND3DAnimatorCallbackHandler(VCND3DAnimator* instance)
        : m_Instance(instance)
    {}

    HRESULT CALLBACK HandleCallback( THIS_ UINT Track, LPVOID pCallbackData )
    {
        KeyFrameHit* keyFrame = ( KeyFrameHit* )pCallbackData;
        if (keyFrame == nullptr)
            return S_OK; //just in case

        m_Instance->HandleAnimationCallback( *keyFrame );
        return S_OK;
    }

    VCND3DAnimator* m_Instance;
};


void ValidateResult(HRESULT result)
{
    VCN_ASSERT( SUCCEEDED(result) );
}

VCND3DAnimator::VCND3DAnimator()
    :   m_meshID(-1),
        m_animator(nullptr),
        m_frameRoot(nullptr),
        m_Playing(false),
        m_Looped(false),
        m_Time(0),
        m_TimeToLoop(0),
        m_TransitionTime(0),
        m_CurrentTrack(0)
{}

VCND3DAnimator::VCND3DAnimator(VCNResID meshID, ID3DXAnimationController* animator, D3DXFRAME* root, const std::vector<D3DXMATRIX*>& matrixPointers)
    :   m_meshID(meshID),
        m_animator(CloneAnimationController(animator)),
        m_frameRoot(root),
        m_transformPtrs(matrixPointers),
        m_Playing(false),
        m_Looped(false),
        m_Time(0),
        m_TimeToLoop(0),
        m_TransitionTime(0),
        m_CurrentTrack(0)
{
    Matrix4 id;
    id.SetIdentity();
    m_instanceTransforms.resize(m_transformPtrs.size(), id);

    InitialiseAnimator();
}

VCND3DAnimator::~VCND3DAnimator()
{
    m_animator->Release();
}

void VCND3DAnimator::InitialiseAnimator()
{
    m_AnimationIndexes.clear();

    for(std::size_t i = 0; i < m_animator->GetNumAnimationSets(); ++i)
    {
        LPD3DXANIMATIONSET set;
        HRESULT result = m_animator->GetAnimationSet(i, &set);
        if (FAILED(result))
            continue;

        m_AnimationIndexes[ std::string( set->GetName() ) ] = i;
        set->Release();
    }

    // Start with all tracks disabled
    
    std::size_t trackCount = m_animator->GetMaxNumTracks();
    for(std::size_t i = 0; i < trackCount; ++i)
        m_animator->SetTrackEnable( i, FALSE );

    m_animator->ResetTime();
}

VCNIComponent::Ptr VCND3DAnimator::Copy() const
{
    VCND3DAnimator* copy = new VCND3DAnimator(m_meshID, m_animator, m_frameRoot, m_transformPtrs);
    return VCNIComponent::Ptr(copy);
}

void VCND3DAnimator::Initialise(const Parameters& params)
{
    //VCN_ASSERT_MSG(false, "THIS COMPONENT DOESN'T SUPPORT PERSONA INSTANTIATION");
}

void VCND3DAnimator::SetKeyFrames(const std::string& animName, const std::vector<KeyFrame>& frames)
{
    m_KeyEvents.clear();

    LPD3DXKEYFRAMEDANIMATIONSET animationSet;
    HRESULT hr = m_animator->GetAnimationSet( m_AnimationIndexes[ animName ], (LPD3DXANIMATIONSET*)&animationSet );
    VCN_ASSERT( SUCCEEDED(hr) );

    //Create d3d keys from our keys
    std::vector<D3DXKEY_CALLBACK> d3dKeys;
    d3dKeys.resize( frames.size() );

    

    std::transform(frames.begin(), frames.end(), d3dKeys.begin(), [&](const KeyFrame& frame) -> D3DXKEY_CALLBACK
    {
        m_KeyEvents.push_back( std::unique_ptr<KeyFrameHit>(new KeyFrameHit( frame.m_Name )) );

        D3DXKEY_CALLBACK callback;

        callback.Time = frame.m_Ratio * (float)animationSet->GetPeriod() * (float)animationSet->GetSourceTicksPerSecond();
        callback.pCallbackData = m_KeyEvents[ m_KeyEvents.size() - 1 ].get();

        return callback;
    });

    //compress the new animation set with the new keys baked in
    LPD3DXBUFFER compressionBuffer = nullptr;

    hr = animationSet->Compress( D3DXCOMPRESS_DEFAULT, 0.5f /*TODO check compression ratio*/, nullptr, &compressionBuffer );
    VCN_ASSERT( SUCCEEDED(hr) );

    LPD3DXCOMPRESSEDANIMATIONSET newAnimationSet = nullptr;
    hr = D3DXCreateCompressedAnimationSet(
                    animationSet->GetName(),
                    animationSet->GetSourceTicksPerSecond(),
                    animationSet->GetPlaybackType(),
                    compressionBuffer,
                    d3dKeys.size(),
                    &d3dKeys[0],
                    &newAnimationSet);

    VCN_ASSERT( SUCCEEDED(hr) );

    compressionBuffer->Release();

    m_animator->UnregisterAnimationSet( animationSet );
    animationSet->Release();

    hr = m_animator->RegisterAnimationSet( newAnimationSet );
    VCN_ASSERT( SUCCEEDED(hr) );

    newAnimationSet->Release();

    InitialiseAnimator();
}

void VCND3DAnimator::HandleAnimationCallback(const KeyFrameHit& frame)
{
    GetOwner()->Fire(frame);
}

void VCND3DAnimator::Update(VCNFloat dt)
{
    VCND3DAnimatorCallbackHandler callbackHandler = VCND3DAnimatorCallbackHandler(this);

    m_Time += dt;
    HRESULT result = m_animator->AdvanceTime( dt, &callbackHandler );
    VCN_ASSERT( SUCCEEDED(result) );

    UpdateFrameMatrices( m_frameRoot, nullptr );

    std::transform(std::begin(m_transformPtrs), std::end(m_transformPtrs), std::begin(m_instanceTransforms), [](D3DXMATRIX* matrix)
    {
        return Matrix4((float*)matrix->m);
    });

    if (!m_Playing)
        return;
 
    LPD3DXANIMATIONSET currentAnim = nullptr;
    m_animator->GetTrackAnimationSet( m_CurrentTrack, &currentAnim);
    VCN_ASSERT( currentAnim != nullptr );

    D3DXTRACK_DESC td;
    m_animator->GetTrackDesc(m_CurrentTrack, &td);

    if (td.Position >= currentAnim->GetPeriod() - m_TimeToLoop - VCN::EPSILON_VCN)
    {
        if (m_Looped)
        {
            Loop(std::string(m_Current), m_TimeToLoop, m_TimeToLoop);
        }
        else if (!m_Fallback.empty())
        {
            Loop(std::string(m_Fallback), m_TransitionTime, m_TimeToLoop);
        }
        else
        {
            Pause();
        }
    }

    currentAnim->Release();
}

void VCND3DAnimator::OnOwnerSet(const VCNIComponent::OwnerData& data)
{
    if (!data.New)
        return;

    m_StartAnimHandle = data.New->GetEvents().RegisterListener(this, &VCND3DAnimator::OnStartAnimation);
    m_LoopAnimHandle = data.New->GetEvents().RegisterListener(this, &VCND3DAnimator::OnLoopAnimation);
}

void VCND3DAnimator::Start(const std::string& name, float transitionLen)
{
    m_Fallback.clear();

    Stop(transitionLen);

    int newTrack = (m_CurrentTrack + 1) % 2;

    LPD3DXANIMATIONSET animationset = nullptr;

    m_animator->GetAnimationSet(m_AnimationIndexes[name], &animationset);
    VCN_ASSERT( animationset != nullptr );

    ValidateResult(m_animator->SetTrackAnimationSet( newTrack, animationset));

    animationset->Release();

    ValidateResult(m_animator->UnkeyAllTrackEvents( newTrack ));

    ValidateResult(m_animator->SetTrackEnable( newTrack, true));
    ValidateResult(m_animator->SetTrackWeight( newTrack, 0.0f));
    ValidateResult(m_animator->SetTrackPosition( newTrack, 0.0));
    ValidateResult(m_animator->SetTrackSpeed( newTrack, 1.0f));
    ValidateResult(m_animator->KeyTrackWeight( newTrack, 1.0f, m_Time, transitionLen, D3DXTRANSITION_LINEAR ));

    m_CurrentTrack = newTrack;
    m_TransitionTime = transitionLen;
    m_TimeToLoop = 0.05f;
    m_Current = name;
    m_Playing = true;
}

void VCND3DAnimator::Start(const std::string& name, const std::string& fallbackLoop, float transitionLen, float transitionToFallback)
{
    Start(name, transitionLen);
    
    m_Fallback = fallbackLoop;
    m_TimeToLoop = transitionToFallback;
}

void VCND3DAnimator::Pause()
{
    m_animator->SetTrackSpeed(m_CurrentTrack, 0.0f);

    LPD3DXANIMATIONSET currentAnim = nullptr;
    m_animator->GetTrackAnimationSet( m_CurrentTrack, &currentAnim);
    VCN_ASSERT( currentAnim != nullptr );

    D3DXTRACK_DESC td;
    m_animator->GetTrackDesc(m_CurrentTrack, &td);

    m_animator->SetTrackPosition( m_CurrentTrack, currentAnim->GetPeriod() - 0.01f );

    currentAnim->Release();

    m_Playing = false;
}

void VCND3DAnimator::Stop(float transitionLen)
{
    ValidateResult(m_animator->UnkeyAllTrackEvents( m_CurrentTrack ));
    ValidateResult(m_animator->KeyTrackEnable( m_CurrentTrack, false, m_Time + transitionLen ));
    ValidateResult(m_animator->KeyTrackWeight( m_CurrentTrack, 0.0f, m_Time, transitionLen, D3DXTRANSITION_LINEAR ));

    m_Current.clear();
    m_Fallback.clear();
    m_Playing = false;
    m_Looped = false;
}

void VCND3DAnimator::Loop(const std::string& name, float transitionLen, float timeBeforeRestart)
{
    Start( name, transitionLen );
    m_Looped = true;
    m_TimeToLoop = timeBeforeRestart;
}

void VCND3DAnimator::OnStartAnimation(const StartAnimation& start)
{
    if (m_AnimationIndexes.find( start.Name ) == m_AnimationIndexes.end())
        return;

    if (start.Fallback.empty())
    {
        Start(start.Name, start.TransitionTime);
    }
    else
    {
        Start(start.Name, start.Fallback, start.TransitionTime, start.RestartTime);
    }
}

void VCND3DAnimator::OnLoopAnimation(const LoopAnimation& loop)
{
    if (m_AnimationIndexes.find( loop.Name ) == m_AnimationIndexes.end())
        return;

    Loop( loop.Name, loop.TransitionTime, loop.RestartTime );
}

ID3DXAnimationController* CloneAnimationController(ID3DXAnimationController* source)
{
    ID3DXAnimationController* clone = nullptr;
    source->CloneAnimationController(source->GetMaxNumAnimationOutputs(),
                                    source->GetMaxNumAnimationSets(),
                                    source->GetMaxNumTracks(),
                                    source->GetMaxNumEvents(),
                                    &clone);

    return clone;
}

void UpdateFrameMatrices( LPD3DXFRAME pFrameBase, const D3DXMATRIX* pParentMatrix )
{
    MultiAnimFrame* pFrame = ( MultiAnimFrame* )pFrameBase;

    if( pParentMatrix != nullptr )
        D3DXMatrixMultiply( &pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix );
    else
        pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

    if( pFrame->pFrameSibling != nullptr )
    {
        UpdateFrameMatrices( pFrame->pFrameSibling, pParentMatrix );
    }

    if( pFrame->pFrameFirstChild != nullptr )
    {
        UpdateFrameMatrices( pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix );
    }
}