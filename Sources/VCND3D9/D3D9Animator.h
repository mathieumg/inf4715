///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#pragma once

#include "VCNNodes/ComponentBase.h"
#include "VCNNodes/Event.h"
#include "VCNRenderer/AnimationEvents.h"
#include "VCNUtils/Types.h"

#include <vector>

struct ID3DXAnimationController;
struct _D3DXFRAME;
typedef _D3DXFRAME D3DXFRAME;
struct D3DXMATRIX;

class VCND3DAnimator :  public VCNComponentBase<VCND3DAnimator>,
                        public VCNIUpdatable,
                        public VCNIListener
{
public:
    struct KeyFrame
    {
        float m_Ratio;
        std::string m_Name;

        KeyFrame(const std::string& name, float ratio)
            : m_Ratio(ratio), m_Name(name)
        {}
    };

    VCND3DAnimator();
    VCND3DAnimator(VCNResID meshID, ID3DXAnimationController* animator, D3DXFRAME* root, const std::vector<D3DXMATRIX*>& matrixPointers);

    virtual ~VCND3DAnimator();

    virtual void Initialise(const Parameters& params) override;
    virtual void Update(VCNFloat dt) override;
    virtual VCNIComponent::Ptr Copy() const override;

    virtual void OnOwnerSet(const OwnerData& data) override;

    const std::vector<Matrix4>* GetTransformPtr() const
    { return &m_instanceTransforms; }

    void Start(const std::string& name, const std::string& fallbackLoop, float transitionLen, float timeBeforeFallback);
    void Start(const std::string& name, float transitionLen);

    void Stop(float transitionLen);
    void Pause();

    void Loop(const std::string& name, float transitionLen, float timeBeforeRestart);

    void SetKeyFrames(const std::string& animName, const std::vector<KeyFrame>& frames);

    bool IsPlaying()
    { return m_Playing; }

    bool IsLooped()
    { return m_Looped; }

    const std::string& GetCurrentAnimation() const
    { return m_Current; }

private:
    friend class VCND3DAnimatorCallbackHandler;
    void HandleAnimationCallback(const KeyFrameHit& frame);

    void InitialiseAnimator();

    void OnStartAnimation(const StartAnimation& start);
    void OnLoopAnimation(const LoopAnimation& loop);

    VCNResID m_meshID;
    ID3DXAnimationController* m_animator;
    D3DXFRAME* m_frameRoot;
    std::vector<D3DXMATRIX*> m_transformPtrs;

    std::vector<Matrix4> m_instanceTransforms;

    VCNIEvent::Handle m_StartAnimHandle;
    VCNIEvent::Handle m_LoopAnimHandle;

    std::map<std::string, std::size_t> m_AnimationIndexes;

    bool m_Playing;
    bool m_Looped;

    float m_Time;
    float m_TransitionTime;
    float m_TimeToLoop;

    std::vector< std::unique_ptr<KeyFrameHit> > m_KeyEvents;

    std::size_t m_CurrentTrack;

    std::string m_Current;
    std::string m_Fallback;
};

template class VCNComponentBase<VCND3DAnimator>;

