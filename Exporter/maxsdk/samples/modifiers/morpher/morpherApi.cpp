//**************************************************************************/
// Copyright (c) 1998-2010 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
#include <object.h>
#include <iparamb.h>
#include ".\include\morpherAPI.h"
#include "wm3.h"

bool MaxMorphModifier::IsValidMorphModifier(Modifier* modifier)
{
	return (modifier && 
					modifier->SuperClassID() == OSM_CLASS_ID && 
					modifier->ClassID() == MR3_CLASS_ID);
}

MaxMorphModifier::MaxMorphModifier() : mMorpher(NULL)
{
}

MaxMorphModifier::MaxMorphModifier(Modifier* pMorpher) : mMorpher(NULL)
{
	Init(pMorpher);
}

bool MaxMorphModifier::Init(Modifier* pMorpher)
{
	if (IsValidMorphModifier(pMorpher))
	{
		mMorpher = static_cast<MorphR3*>(pMorpher);
		if (IsCacheEmpty())
		{
			// May fail if the Morpher we are wrapping is not applied to an object,
			// but that should not prevent this wrapper from being considered initialized.
			RebuildCache();
		}
		return true;
	}
	return false;
}

MaxMorphModifier::~MaxMorphModifier()
{
	mMorpher = NULL;
}

bool MaxMorphModifier::IsValid() const
{
	return (IsValidMorphModifier(mMorpher));
}

Modifier* MaxMorphModifier::GetModifier() const
{
	return mMorpher;
}

int MaxMorphModifier::NumMorphChannels() const
{
	return (!IsValid()) ? 0 : (int)mMorpher->chanBank.size();
}

MaxMorphChannel MaxMorphModifier::GetMorphChannel(int channelId) const
{
	if (0 <= channelId && channelId < NumMorphChannels())
	{
		morphChannel& lChannel = mMorpher->chanBank[channelId];
		return MaxMorphChannel(&lChannel, channelId);
	}
	return MaxMorphChannel(NULL, -1);
}

bool MaxMorphModifier::SwapMorphChannels(int channelId1, int channelId2)
{
	if (IsValid() && 
		channelId1 != channelId2 &&
		(0 <= channelId1 && channelId1 < NumMorphChannels()) &&
		(0 <= channelId2 && channelId2 < NumMorphChannels()))
	{
#pragma warning (push)
#pragma warning (disable: 4996)
		IMorphClass morphClass;
		morphClass.SwapMorphs(mMorpher, channelId1, channelId2, TRUE);
#pragma warning (pop)
		return true;
	}
	return false;
}

bool MaxMorphModifier::MoveMorphChannel(int fromChannelId, int toChannelId)
{
	if (IsValid() && 
		fromChannelId != toChannelId &&
		(0 <= fromChannelId && fromChannelId < NumMorphChannels()) &&
		(0 <= toChannelId && toChannelId < NumMorphChannels()))
	{
#pragma warning (push)
#pragma warning (disable: 4996)
		IMorphClass morphClass;
		morphClass.SwapMorphs(mMorpher, fromChannelId, toChannelId, FALSE);
#pragma warning (pop)
		return true;
	}
	return false;
}

bool MaxMorphModifier::IsValidMorphTarget(INode* targetNode, TimeValue t) const
{
	return GetMorphNode::IsValidMorphTargetType(mMorpher, targetNode, t);
}

void MaxMorphModifier::RefreshChannelsUI() const
{
	if (IsValid())
	{
		mMorpher->Update_channelFULL();
	}
}

#pragma region CountModContextProc
namespace {
class CountModContextProc : public ModContextEnumProc
{
public:
	virtual BOOL proc(ModContext *mc)
	{
		DbgAssert(mc != NULL);
		mModContexts.Append(1, &mc);
		return TRUE;
	}
	int GetModContextCount() const
	{
		return mModContexts.Count();
	}
	ModContext* GetModContext(int i) const
	{
		if (0 <= i && i < mModContexts.Count())
		{
			return mModContexts[i];
		}
		return NULL;
	}
private:
	Tab<ModContext*> mModContexts;
};
}; // end namespace
#pragma endregion

bool MaxMorphModifier::RebuildCache()
{
	// Force building up the morpher's cache.
	// Morph targets cannot be added unless the cache has been built.
	if (mMorpher != NULL)
	{
			DeleteCache();

		// In order to update the cache we need the object the morpher is applied to.
		// To get the object, we need to evaluate the derived object the morpher is part of.
		// If the morpher is instanced (has more then 1 ModContext) we bail.

		// Get all ModContextsof the morpher.
		CountModContextProc modContextEnumProc;
		mMorpher->EnumModContexts(&modContextEnumProc);
		DbgAssert(modContextEnumProc.GetModContextCount() <= 1);
		if (modContextEnumProc.GetModContextCount() == 1)
		{
			ModContext* modContext = modContextEnumProc.GetModContext(0);
			DbgAssert(modContext != NULL);
			IDerivedObject* derivedObj = NULL;
			int modIndex = 0;
			// Get the derived object corresponding to the ModContext
			mMorpher->GetIDerivedObject(modContext, derivedObj, modIndex);
			DbgAssert(derivedObj != NULL);
			if (derivedObj != NULL)
			{
				// Evaluate the derived object. This effectively updates its cache.
				ObjectState os = derivedObj->Eval(GetCOREInterface()->GetTime());
				return (TRUE == mMorpher->MC_Local.AreWeCached());
			}
		}
	}
	return false;
}

bool MaxMorphModifier::RebuildCacheFromObject(Object* object)
{
	if (NULL == mMorpher || NULL == object)
	{
		return false;
	}

	bool doRebuildCache = false;
	// If the morpher is already applied to an object different than the one
	// passed in as parameter, reject rebuilding the cache
	CountModContextProc modContextEnumProc;
	mMorpher->EnumModContexts(&modContextEnumProc);
	int numModContexts = modContextEnumProc.GetModContextCount();
	if (0 == numModContexts)
	{
		doRebuildCache = true;
	}
	else 
	{
		if (1 == numModContexts)
		{
			ModContext* modContext = modContextEnumProc.GetModContext(0);
			DbgAssert(modContext != NULL);
			IDerivedObject* derivedObj = NULL;
			int modIndex = 0;
			// Get the derived object corresponding to the ModContext
			mMorpher->GetIDerivedObject(modContext, derivedObj, modIndex);
			DbgAssert(derivedObj != NULL);
			if (derivedObj != NULL)
			{
				Object* baseObject = derivedObj->GetObjRef();
				baseObject = baseObject->FindBaseObject();
				doRebuildCache = (baseObject == object);
			}
		}
	}

	if (doRebuildCache)
	{
		DeleteCache();
		// Evaluate the object. This effectively updates its cache.
		mMorpher->MC_Local.MakeCache(object);
		return (TRUE == mMorpher->MC_Local.AreWeCached());
	}
	return false;
}

bool MaxMorphModifier::IsCacheEmpty() const
{
	return (NULL != mMorpher && !mMorpher->MC_Local.AreWeCached());
}

bool MaxMorphModifier::DeleteCache()
{
	if (mMorpher != NULL)
	{
		mMorpher->MC_Local.NukeCache();
		return (!mMorpher->MC_Local.AreWeCached());
	}
	return false;
}

// ************** MaxMorphChannel ************
MaxMorphChannel::MaxMorphChannel() 
	: mChannel(NULL), mChannelId(-1)
{
}

MaxMorphChannel::MaxMorphChannel(morphChannel* pImpl, int channelId) 
	: mChannel(NULL), mChannelId(-1)
{
	Init(pImpl, channelId);
}

void MaxMorphChannel::Init(morphChannel* pChannel, int channelId)
{
	DbgAssert(NULL != pChannel);
	DbgAssert(0 <= channelId && channelId < MR3_NUM_CHANNELS);
	mChannel = pChannel;
	mChannelId = channelId;
}

void MaxMorphChannel::Reset(bool active, bool modded, int numPoints)
{
	if (mChannel)
	{
		DeleteChannel();
		mChannel->mActive = active;
		mChannel->mModded = modded;
		mChannel->mNumPoints = numPoints;
		mChannel->mPoints.resize(numPoints);
		mChannel->mDeltas.resize(numPoints);
		mChannel->mWeights.resize(numPoints);
	}
}

void MaxMorphChannel::BuildFromNode( INode *pNode , bool resetTime, TimeValue t, bool picked)
{
    if (mChannel)
    {
        mChannel->buildFromNode(pNode, resetTime, t, picked);
    }
}

bool MaxMorphChannel::IsActive() const
{
	return ((mChannel) ? (TRUE == mChannel->mActiveOverride) : false);
}

void MaxMorphChannel::SetActive(bool bActive)
{
	if (NULL != mChannel)
	{
		// MorpherR3 does not support undoing changes to a channel's active state
		mChannel->mActiveOverride = bActive;
		mChannel->mp->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}
}

bool MaxMorphChannel::HasData() const
{
	return ((mChannel) ? (TRUE == mChannel->mActive) : false);
}

bool MaxMorphChannel::IsValid() const
{
	return ((mChannel) ? (FALSE == mChannel->mInvalid) : false);
}

const MCHAR* MaxMorphChannel::GetName() const
{
	return (mChannel) ? mChannel->mName.data() : NULL;
}

void MaxMorphChannel::SetName(const MCHAR* newName)
{
	if (mChannel)
	{
		// MorpherR3 does not support undoing changes to the channel's name
		mChannel->mName = newName;
	}
}

bool MaxMorphChannel::DeleteChannel()
{
	if (NULL != mChannel && NULL != mChannel->mp)
	{
		MorphR3* morpher = mChannel->mp;
		if (theHold.Holding())
		{
			theHold.Put(new Restore_FullChannel(morpher, mChannelId));
		}
		morpher->DeleteChannel(mChannelId);
		DbgAssert(GetMorphTarget() == NULL);
		DbgAssert(NumProgressiveMorphTargets() == 0);
		morpher->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
		morpher->NotifyDependents(FOREVER, PART_ALL, REFMSG_SUBANIM_STRUCTURE_CHANGED);
		return true;
	}
	return false;
}

INode* MaxMorphChannel::GetMorphTarget() const
{
	return (mChannel) ? mChannel->mConnection : NULL;
}

MorphExport bool MaxMorphChannel::SetMorphTarget(INode* newTarget, TimeValue t)
{
	if (NULL == mChannel || NULL == mChannel->mp || 
			NULL == newTarget || NULL != GetMorphTarget())
	{
		return false;
	}
	if (GetMorphNode::IsValidMorphTargetType(mChannel->mp, newTarget, t))
	{
		mChannel->ResetMe();

		if (theHold.Holding()) 
		{
			theHold.Put(new Restore_FullChannel(mChannel->mp, mChannelId));
		}

		// Set a new target for the channel
		mChannel->mp->ReplaceReference(101 + mChannelId, newTarget);
		mChannel->buildFromNode(newTarget, FALSE, t, TRUE);	// updates UI and dependents
		mChannel->mNumProgressiveTargs = 0;
		mChannel->ReNormalize();
	}
	return true;
}

bool MaxMorphChannel::SwapMorphTargets(int morphIndex1, int morphIndex2)
{
	MorphExport void SwapPTargets(MorphR3 *mp,const int morphIndex, const int from, const int to, const bool isundo);
	if (NULL != mChannel && NULL != mChannel->mp &&
		morphIndex1 != morphIndex2 &&
		IsValidProgressiveMorphTargetIndex(morphIndex1) &&
		IsValidProgressiveMorphTargetIndex(morphIndex2))
	{
#pragma warning (push)
#pragma warning (disable: 4996)
		IMorphClass morphClass;
		morphClass.SwapPTargets(mChannel->mp, mChannelId, morphIndex1, morphIndex2, TRUE /* unused */);
#pragma warning (pop)
		return true;
	}
	return false;
}

float MaxMorphChannel::GetMorphWeight(TimeValue t) const
{
	float result = 0.0f;
	if (NULL != mChannel)
	{
		Interval validity(FOREVER);
		mChannel->cblock->GetValue(0, t, result, validity);
	}
	return result;
}

bool MaxMorphChannel::SetMorphWeight(TimeValue t, float newValue) const
{
	if (NULL == mChannel || NULL == mChannel->mp || NULL == mChannel->mp->pblock)
	{
		return false;
	}
	// Clamp new value to either global or channel specific limits
	Interval validity(FOREVER);
	BOOL globalUseLimits = TRUE; 
	float globalMaxLimit = 0.0f;
	float globalMinLimit = 0.0f;
	IParamBlock* morpherPBlock = mChannel->mp->pblock;
	morpherPBlock->GetValue(PB_OV_USELIMITS, t, globalUseLimits, validity);
	morpherPBlock->GetValue(PB_OV_SPINMAX, t, globalMaxLimit, validity);
	morpherPBlock->GetValue(PB_OV_SPINMIN, t, globalMinLimit, validity);

	float minLimit = globalMinLimit;
	float maxLimit = globalMaxLimit;
	if (mChannel->mUseLimit)
	{
		maxLimit= mChannel->mSpinmax;
		minLimit = mChannel->mSpinmin;
	}
	bool useLimits = (mChannel->mUseLimit || globalUseLimits);
	if (useLimits)
	{
		if (newValue > maxLimit) newValue= maxLimit;
		if (newValue < minLimit) newValue = minLimit;
	}

	bool res = (TRUE == mChannel->cblock->SetValue(0, t, newValue)); 
	mChannel->mp->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	return res;
}

Control* MaxMorphChannel::GetMorphWeightController() const
{
	return (mChannel && mChannel->cblock) ? mChannel->cblock->GetController(0) : NULL;
}

int MaxMorphChannel::NumMorphPoints() const
{
	return (mChannel) ? mChannel->mNumPoints : 0;
}

int MaxMorphChannel::IsValidMorphPointIndex(int pointIndex) const
{
	return (mChannel) ? (0 <= pointIndex && pointIndex < NumMorphPoints()) : false;
}

Point3 MaxMorphChannel::GetMorphPoint(int pointIndex) const
{
	if (NULL == mChannel || !IsValidMorphPointIndex(pointIndex))
	{
		return Point3(0.0f,0.0f,0.0f);
	}
	return mChannel->mPoints[pointIndex];
}

void MaxMorphChannel::SetMorphPoint(int pointIndex, const Point3& newPoint)
{
	if (NULL != mChannel && NULL != mChannel->mp && IsValidMorphPointIndex(pointIndex))
	{
		// MorpherR3 does not support undoing changes to morph point values
		mChannel->mPoints[pointIndex] = newPoint;
		mChannel->mp->NotifyDependents(FOREVER, PART_GEOM, REFMSG_CHANGE);
	}
}

Point3 MaxMorphChannel::GetMorphPointDelta(int pointIndex) const
{
	if (NULL == mChannel || !IsValidMorphPointIndex(pointIndex))
	{
		return Point3(0.0f,0.0f,0.0f);
	}

	return mChannel->mDeltas[pointIndex];
}

void MaxMorphChannel::SetMorphPointDelta(int pointIndex, const Point3& newDelta)
{
	if (NULL != mChannel && NULL != mChannel->mp && IsValidMorphPointIndex(pointIndex))
	{
		// MorpherR3 does not support undoing changes to morph point's delta
		mChannel->mDeltas[pointIndex] = newDelta;
		mChannel->mp->NotifyDependents(FOREVER, PART_GEOM, REFMSG_CHANGE);
	}
}

double MaxMorphChannel::GetMorphPointWeight(int pointIndex) const
{
	if (NULL == mChannel || !IsValidMorphPointIndex(pointIndex))
	{
		return 0.0;
	}
	return mChannel->mWeights[pointIndex];
}

void MaxMorphChannel::SetMorphPointWeight(int pointIndex, double newWeight)
{
	if (NULL != mChannel && NULL != mChannel->mp && IsValidMorphPointIndex(pointIndex))
	{
		// MorpherR3 does not support undoing changes to morph point weights
		mChannel->mWeights[pointIndex] = newWeight;
		mChannel->mp->NotifyDependents(FOREVER, PART_GEOM, REFMSG_CHANGE);
	}
}

int MaxMorphChannel::NumMaxProgressiveTargets()
{
	return MAX_PROGRESSIVE_TARGETS + 1;
}

int MaxMorphChannel::NumProgressiveMorphTargets() const
{
	int numProgMorphTargets = 0;
	if (NULL != mChannel)
	{
		if (NumMorphPoints() > 0)
		{
			numProgMorphTargets = mChannel->NumProgressiveTargets() + 1; 
		}
	}
	return numProgMorphTargets;
}


bool MaxMorphChannel::IsValidProgressiveMorphTargetIndex(int targetIndex) const
{
	return (0 <= targetIndex && targetIndex < NumProgressiveMorphTargets());
}

INode* MaxMorphChannel::GetProgressiveMorphTarget(int targetIndex) const
{
	INode* morphTarget = NULL;
	if (NULL == mChannel || !IsValidProgressiveMorphTargetIndex(targetIndex))
	{
		morphTarget = NULL;
	}
	else if (0 == targetIndex)
	{
		morphTarget = GetMorphTarget();
		DbgAssert(mChannel->mp->GetReference(101 + mChannelId) == morphTarget);
	}
	else 
	{
		morphTarget = mChannel->mTargetCache[targetIndex-1].mTargetINode;
		DbgAssert(mChannel->mp->GetReference(200 + targetIndex + mChannelId*MAX_PROGRESSIVE_TARGETS) == morphTarget);
	}
	return morphTarget;
}

bool MaxMorphChannel::AddProgressiveMorphTarget(INode* newTarget)
{
	if (NULL != mChannel && NULL != mChannel->mp)
	{
	#pragma warning (push)
	#pragma warning (disable: 4996)
		IMorphClass morphClass;
		bool res = (TRUE == morphClass.AddProgessiveMorph(mChannel->mp, mChannelId, newTarget));
	#pragma warning (pop)
		return res;
	}
	return false;
}

bool MaxMorphChannel::DeleteProgressiveMorphTarget(int targetIndex)
{
	if (NULL == mChannel || NULL == mChannel->mp || !IsValidProgressiveMorphTargetIndex(targetIndex))
	{
		return false;
	}
#pragma warning (push)
#pragma warning (disable: 4996)
	IMorphClass morphClass;
	bool res = (TRUE == morphClass.DeleteProgessiveMorph(mChannel->mp, mChannelId, targetIndex));
#pragma warning (pop)
	return res;
}

bool MaxMorphChannel::SortProgressiveMorphTargets(int morphIndex)
{
	if (NULL != mChannel && NULL != mChannel->mp && IsValidProgressiveMorphTargetIndex(morphIndex))
	{
#pragma warning (push)
#pragma warning (disable: 4996)
	IMorphClass morphClass;
	morphClass.SortProgressiveTarget(mChannel->mp, mChannelId, morphIndex);
#pragma warning (pop)
	mChannel->mp->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	return true;
	}
	return false;
}

float MaxMorphChannel::GetProgressiveMorphWeight(int targetIndex) const
{
	float weight = 0.0f;
	if (NULL != mChannel && IsValidProgressiveMorphTargetIndex(targetIndex))
	{
		if (targetIndex == 0)
		{
			weight = mChannel->mTargetPercent;
		}
		else if (targetIndex >= 1)
		{
			weight = mChannel->mTargetCache[targetIndex-1].mTargetPercent;
		}
	}
	return weight;
}

MorphExport bool MaxMorphChannel::SetProgressiveMorphWeight( int morphIndex, float newValue )
{
	if (NULL != mChannel && NULL != mChannel->mp && IsValidProgressiveMorphTargetIndex(morphIndex))
	{
		if (theHold.Holding()) 
		{
			theHold.Put(new Restore_FullChannel(mChannel->mp, mChannelId) );
		}

		if (newValue < MorphR3::kProgressiveTargetWeigthMin) newValue = MorphR3::kProgressiveTargetWeigthMin;
		if (newValue > MorphR3::kProgressiveTargetWeigthMax) newValue = MorphR3::kProgressiveTargetWeigthMax;

		if (0 == morphIndex)
		{
			mChannel->mTargetPercent = newValue;
		}
		else if (morphIndex >= 1)
		{
			mChannel->mTargetCache[morphIndex-1].mTargetPercent = newValue;
		}
		return SortProgressiveMorphTargets();
	}
	return false;
}

float MaxMorphChannel::GetProgressiveMorphTension() const
{
	float tension = 0.0f;
	if (NULL != mChannel)
	{
		tension = mChannel->mCurvature;
	}
	return tension;
}

MorphExport bool MaxMorphChannel::SetProgressiveMorphTension( float newTensionValue )
{
	if (NULL != mChannel && NULL != mChannel->mp)
	{
		if (theHold.Holding())
		{
			theHold.Put( new Restore_FullChannel(mChannel->mp, mChannelId));
		}

		if (newTensionValue < MorphR3::kTensionMin) newTensionValue = MorphR3::kTensionMin;
		if (newTensionValue > MorphR3::kTensionMax) newTensionValue = MorphR3::kTensionMax;
		mChannel->mCurvature = newTensionValue;

		mChannel->mp->NotifyDependents(FOREVER, PART_GEOM, REFMSG_CHANGE);
		return true;
	}
	return false;
}

Point3 MaxMorphChannel::GetProgressiveMorphPoint(int morphIndex, int pointIndex) const
{
	Point3 point(0.0f, 0.0f, 0.0f);
	if (NULL != mChannel && IsValidProgressiveMorphTargetIndex(morphIndex) && IsValidMorphPointIndex(pointIndex))
	{
		if (0 == morphIndex)
		{
			point = mChannel->mPoints[pointIndex];
		}
		else if (morphIndex >= 1)
		{
			point = mChannel->mTargetCache[morphIndex-1].GetPoint(pointIndex);
		}
	}
	return point;
}




