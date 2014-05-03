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
#pragma once

// 3ds Max SDK includes
#include <MaxHeap.h>
#include <maxtypes.h>
#include <strbasic.h>
// Morpher API includes
#include "MorpherExport.h"
#include "MorpherClassID.h"

// 3ds Max SDK forward declarations
class Modifier;
class MorphR3;
class MaxMorphChannel;
class INode;
class Control;
class Point3;

/**
	Classes MaxMorphModifier and MaxMorphChannel represent the API 3rd party plug-ins
	can use to work with 3ds Max's Morpher modifier.
	Although these classes are not guaranteed to stay binary backward compatible with
	their previous versions, using them over class MorphR3 and class morphChannel 
	is recommended.
*/

/** 
	A wrapper around a 3ds Max morpher modifier instance.
	A MaxMorphModifier object has morph channels corresponding to morph target objects. 
	For more information on morph channels, their targets and other parameters see 
	class MaxMorphChannel.
	Client code can create instances of class MaxMorphModifier using a given 
	Morpher modifier instance and extract information about it or change its 
	state via the MaxMorphModifier methods.
	To find an instance of the 3ds Max Morpher modifier, one can use one of the 
	EnumGeomPipeline() methods. For an example, see MorphR3::TestMorphReferenceDependencies()
	Since MaxMorphModifier stores a pointer to a Morpher modifier instance, 
	using it from a different thread or a non-blocking method may allow for the 
	possibility of the Morpher modifier instance to be deleted, which will invalidate 
	the pointer held by the MaxMorpherModifier instance.
*/
class MaxMorphModifier : public MaxHeapOperators
{
public:
	/** 
		One step construction. Constructs a MaxMorpherModifier based on a given Morpher instance. 
		\param modifier - Instance of a Morpher modifier. If the supplied modifier is
		not a Morpher (it's classid is not MR3_CLASS_ID), MaxMorphModifier::IsValid() 
		return false and the MaxMorphModifier won't be usable.
	*/
	MorphExport MaxMorphModifier(Modifier* modifier);

	/** 
		Two step construction. Call Init() before using this MaxMorphModifier instance
	*/
	MorphExport MaxMorphModifier();
	/**
	Sets this MaxMorphModifier as a wrapper of the supplied Morpher modifier.
	If the supplied modifier is not a Morpher (it's classid is not MR3_CLASS_ID), 
	MaxMorphModifier::IsValid() return false and the MaxMorphModifier won't be usable.
	The function attaches itself to the received morpher modifier.
	\return True if this MaxMorphModifier instance has been properly initialized
	*/
	MorphExport bool Init(Modifier* pMorpher);

	/**
		Destructor. The wrapped Morpher modifier instance is not destroyed. 
	*/
	MorphExport ~MaxMorphModifier();

	/** 
		Returns true if this MaxMorphModifier instance has been correctly initialized
		with a Morpher modifier. Otherwise returns false.
	*/
	MorphExport bool IsValid() const;

	/** 
		Returns the morpher modifier this MaxMorphModifier instance wraps
	*/
	MorphExport Modifier* GetModifier() const;

	/**
		Returns the number of morph channels.
		If this MaxMorphModifier instance hasn't been initialized, returns 0.
		Otherwise it always returns the same value whether the channels are used or not.
	*/
	MorphExport int NumMorphChannels() const;

	/** 
		Gets the specified morpher channel.
		\param channelId - The index of the morph channel to retrieve. It's range is 
		[0, NumMorphChannels()-1]
		\return A MaxMorphChannel instance that wraps the Morpher modifier's channel
		Returns NULL if the channel index is invalid or this MaxMorpherModifier 
		instance is not initialized.
	*/
	MorphExport MaxMorphChannel GetMorphChannel(int channelId) const;

	/** 
		Swaps two morph channels. 
		\param channelId1 The index of the 1st morph channel
		\param channelId2 The index of the 2nd morph channel
		\return true if the operation succeeded.
	*/
	MorphExport bool SwapMorphChannels(int channelId1, int channelId2);

	/** 
		Moves a channel to a specified channel. The "to" or destination channel data 
		is replaced by the "from" or source channel data.
		\param fromChannelId The id of the channel to move (source channel id)
		\param toChannelId The id of the channel to move to (destination channel id)
		\return True if the operation succeeded
	*/
	MorphExport bool MoveMorphChannel(int fromChannelId, int toChannelId);

	/** Morpher cache related methods */
	//@{
	/**
		Returns true if the morpher's cache is empty, otherwise false.
		A morpher modifier instance that hasn't been applied to an object would have
		an empty cache.
	*/
	MorphExport bool IsCacheEmpty() const;

	/**
		Builds the morpher's cache by first deleting it and then building it based 
		on the object the morpher is applied to. The operation will not succeed if the
		morpher modifier is instanced across several objects.
		\return True if it succeeded. 
	*/
	MorphExport bool RebuildCache();

	/**
		Builds the morpher's cache from the specified object. If the morpher is
		applied to an object already, this operation will fail. Applying the morpher
		to an object other than the once the cache is built from will result in the 
		cache being rebuilt.
		\param object The base object to be use to create the morpher's cache
		\return True if it succeeded. 
	*/
	MorphExport bool RebuildCacheFromObject(Object* object);

	/**
		Deletes the morpher's cache.
		\return True if it succeeded.
	*/
	MorphExport bool DeleteCache();
	//@}

	/**
		Refreshes the Morpher's user interface.
		Note that the individual operations on the morpher or its channels will not
		necessarily result in a refresh of the morpher user interface. This allows
		for executing several operations in a row without incurring the cost of 
		refreshing the user interface for each operation.
	*/
	MorphExport void RefreshChannelsUI() const;

	/**
		Returns true if the given node meets the requirements of a morph target.
		See MaxMorphChannel::SetMorphTarget() for a description of these requirements.
		This method allows for testing if a given node can be successfully set as
		either a morph or progressive morph target.
		\param targetNode The node to check for validity
		\param t The time at which the target's state is checked for validity
		\returns True if the node is a valid target for this morph modifier, false otherwise.
	*/
	MorphExport bool IsValidMorphTarget(INode* targetNode, TimeValue t) const;
	
	/**
		Returns true if the specified modifier is a morph modifier that can be used
		with class MaxMorphModifier
	 */
	MorphExport static bool IsValidMorphModifier(Modifier* modifier);

private:
	// Builds the morpher's cache if its empty
	bool UpdateCache();

	// The 3ds Max morpher modifier this class wraps
	MorphR3* mMorpher;
};

class morphChannel;

/** 
	A wrapper around a 3ds Max Morpher modifier's channel.
	Client code gets access to a MaxMorphChannel via MaxMorphModifier::GetMorphChannel().
	The life-time of the instances of this class is managed by class MaxMorpherModifier.
	A morph channel can have associated a main target object and a weight representing 
	the target's contribution to the overall morph solution.
	In addition to the target object, a morph channel can have so called progressive
	(intermediary) targets which used to fine tune how the object being morphed reaches 
	the state represented by the morph channel's target. Each progressive target has 
	associated a weight and a tension value. The weight of a progressive 
	target represents how much it contributes to reaching the morph channel's target 
	state, while the tension is used to affect the interpolation of the points between 
	the intermediary targets.
	Here's an example of how client code would get the name of morph 0 channel:
	\code
	Modifier* morphModifier; 
	// initialize morphModifier with a pointer to a MorphR3 instance
	MaxMorphModifier maxMorphModifier(morphModifier);
	const MCHAR* morphChannelName = maxMorphModifier.GetMorphChannel(0).GetName();
	\endcode
*/
class MaxMorphChannel : public MaxHeapOperators
{
public:
	/** 
		Returns true is the channel is turned on and used in computing the morph 
		solution. Otherwise returns false. Note that an active channel may not 
		necessarily contain morph data.
	*/
	MorphExport bool IsActive() const;

	/** 
		Sets the active state of the morph channel.
		\param bActive The active state of the morph channel.
	*/
	MorphExport void SetActive(bool bActive);

	/**
		Returns true if the morph channel has morph data. Morph channels with data
		may or may not have a target object connected to it.
	*/
	MorphExport bool HasData() const;

	/**
		Returns true if the morph channel has valid morph data. Invalid morph channels
		can be active and have data. A morph channel can become invalid if it's target
		object does not satisfy one of the eligibility criteria. 
		See MaxMorphChannel::SetMorphTarget() for information on these eligibility criteria.
	*/
	MorphExport bool IsValid() const;

	/** 
		Initializes the morph channel.
		This method can be used when a morph channel needs to be set up but the actual
		morph target object is not available. In this case this method allows to active
		the channel and set up the number of morph point. Then MaxMorphChannel::SetMorphPoint() 
		can be used to set the values of the morph points. 
		Note that calling Reset() causes all data stored by the morph channel to be lost.
		\param active - if true, the morph channel is activated
		\param modded - if true, the channel is marked as modified
		\param numPoints - The number of morph points to allocate
	*/
	MorphExport void Reset(bool active, bool modded, int numPoints);

	/** 
		Retrieves the morph channel's name. 
		\return The name of the morph channel or NULL if this MaxMorphChannel 
		instance hasn't been properly initialized.
	*/
	MorphExport const MCHAR* GetName() const;

	/**
		Renames the morph channel. 
		\param newName - the name to be set
	*/
	MorphExport void SetName(const MCHAR* newName);

	/**
	Deletes the channel's targets and re-initializes all its data
	Use DeleteTarget to delete the channel's target only.
	Use DeleteProgressiveMorphTarget to delete a progressive target only.
	\return True if operation was successfully, false otherwise
	*/
	MorphExport bool DeleteChannel();

	/** \name Morph Target methods	*/
	//@{
	/**
		Retrieves the morph channel's target object. 
		\return The target object, or NULL if this MaxMorphChannel instance 
		hasn't been properly initialized, or the channel does not have a target.
	*/
	MorphExport INode* GetMorphTarget() const;
	
	/**
		Sets the morph channel's target object.
		The target object has to be a deformable object that has the same number of 
		points as the object being morphed. Also, setting the object as a target
		should not create a loop in the reference graph of the morpher modifier. 
		For example, the object being morphed cannot be set as a target.
		A morph target cannot be set on the morpher, unless the morpher's cache has 
		been initialized (built). If the morpher modifier has been applied to an object
		and the morpher has been evaluated at least once, its cache has been built.
		For more information on the working with the morpher's cache see methods: 
		MaxMorphModifier::IsCacheEmpty(), MaxMorphModifier::RebuildCache(), 
		MaxMorpherModifier::DeleteCache().
		\param newTarget - the target object to be set. Must be non-NULL.
		\param t The time at which the target's state is to be evaluated and used 
		as a morph target
		\return True, if setting the target was successful, false otherwise - see above
		for the conditions that need to be satisfied by the target object.
	*/
	MorphExport bool SetMorphTarget(INode* newTarget, TimeValue t);

	/**
		Create the morph channel from the INode.
		\param node - the node to be used at the morph channel's target
		\param t - the time at which the node should be evaluated in order to compute the morph data
		\param resetTime - if false, the node is evaluated at the time specified by the t parameter,
		otherwise the current time is used
		\param picked - if true, the channel's name is set to that of the node
	*/
	MorphExport void BuildFromNode(INode* node, bool resetTime = true, TimeValue t = 0, bool picked = false);

	/** 
		Swaps two morph targets of tis morph channel.
		\param morphIndex1 One of the morph target indexes involved in the swap
		\param morphIndex2 The other of the morph target indexes involved in the swap
		\return True is succeeded.
	*/
	MorphExport bool SwapMorphTargets(int morphIndex1, int morphIndex2);

	/**
		Retrieves the target object's contribution or weight to the overall morph 
		solution at the specified time frame. The weight can vary over time.
		\param t The time at which to retrieve the weight value
		\return The weight of this target's contribution to the overall morph 
		solution in the [0.0-100.0] range
	*/
	MorphExport float GetMorphWeight(TimeValue t) const;

	/**
		Sets the target object's contribution or weight to the overall morph solution
		at the specified time frame.
		\param t - The time at which to set the weight value
		\param newValue - The new contribution of the target to the overall morph solution
	*/
	MorphExport bool SetMorphWeight(TimeValue t, float newValue) const;

	/**
		Retrieves the controller corresponding to this morph channel's weight values.
		If the weight is not keyframed, returns NULL.
	*/
	MorphExport Control* GetMorphWeightController() const;
	//@}

	/** \name Morph Target Point methods */
	//@{
	/** 
		Retrieves the number of points that correspond to the morph channel's target object.
		See Object::NumPoints() for more information on the interpretation of the term "points".
		\return The number of points stored in this morph channel or 0 if this 
		MaxMorphChannel instance hasn't been properly initialized.
	*/
	MorphExport int NumMorphPoints() const;

	/** 
		Returns true if the specified target point index is valid, false otherwise
	*/
	MorphExport int IsValidMorphPointIndex(int pointIndex) const;

	/** 
		Retrieves the specified morph point. 
		\param pointIndex - The index of the point 
		\return The value of the specified point, or (0.0f, 0.0f, 0.0f) if this 
		MaxMorphChannel instance hasn't been properly initialized, or the specified
		index is invalid.
	*/
	MorphExport Point3 GetMorphPoint(int pointIndex) const;

	/** 
		Sets the value of the specified target point. 
		\param pointIndex - The index of the point 
		\param newPoint - The new point value
	*/
	MorphExport void SetMorphPoint(int pointIndex, const Point3& newPoint);

	/** 
		Retrieves the delta value for the specified target point. 
		\todo What is the "delta" value?
		\param pointIndex - The index of the point 
		\return The delta value of the specified point, or (0.0f, 0.0f, 0.0f) if this 
		MaxMorphChannel instance hasn't been properly initialized, or the specified
		index is invalid.
	*/
	MorphExport Point3 GetMorphPointDelta(int pointIndex) const;

	/**
		Sets the delta value for the specified target point. 
		\param pointIndex - The index of the point 
		\param newDelta - The delta value to be set
	*/
	MorphExport void SetMorphPointDelta(int pointIndex, const Point3& newDelta);

	/** 
		Retrieves the weight value for the specified target point
		\param pointIndex - The index of the point 
		\return The weight value of the specified point, or 0.0 if this 
		MaxMorphChannel instance hasn't been properly initialized, or the specified
		index is invalid.
	*/
	MorphExport double GetMorphPointWeight(int pointIndex) const;

	/**
		Sets the weight value for the specified target point. 
		\param pointIndex - The index of the point 
		\param newWeight - The weight value to be set
	*/
	MorphExport void SetMorphPointWeight(int pointIndex, double newWeight);
	//@}

	/** \name Progressive Target methods */
	//@{
	/**
		Retrieves the maximum number of supported progressive morph targets.
		It includes the main morph target too.
	*/
	MorphExport static int NumMaxProgressiveTargets();

	/**
		Retrieves the current number of progressive morph targets for this morph channel. 
		The morph channel may or may not have progressive targets.
		This number includes the channel's morph target too.
		Deleting a morph target object from the scene does not immediately changes the
		number of morph targets reported by the channel.
		\return The number of morph targets, or 0 if this MaxMorphChannel instance 
		hasn't been properly initialized or has progressive targets.
	*/
	MorphExport int NumProgressiveMorphTargets() const;

	/**
		Returns true if the specified progressive target index is valid, false otherwise.
	*/
	MorphExport bool IsValidProgressiveMorphTargetIndex(int morphIndex) const;

	/**
		Retrieves the specified progressive target object. 
		The target at index zero represents the channel's target. See MaxMorphChannel::GetMorphTarget().
		\param morphIndex - the index of the progressive target object.
		\return The specified target, or NULL if either the target object has been 
		deleted from the scene or this MaxMorphChannel instance hasn't 
		been properly initialized, or the specified index is invalid.
	*/
	MorphExport INode* GetProgressiveMorphTarget(int morphIndex) const;

	/**
		Appends a progressive morph target to the morpher
		See MaxMorphChannel::SetMorphTarget() for a description of the requirements the
		target object needs to meet. 
		\param morphTarget - the morph target to be appended
		\return True if the morph target was added, false otherwise.
	*/
	MorphExport bool AddProgressiveMorphTarget(INode* morphTarget);
	
	/** 
		Deletes the specified progressive target
		Deleting the target at index zero, will delete the channel's target and 
		replace it with the next progressive target.
		\param morphIndex - the index of the target object
		\return True if the operation succeeded, false otherwise.
	*/
	MorphExport bool DeleteProgressiveMorphTarget(int morphIndex);

	/** 
		Sorts the progressive targets by their weight, starting with the specified 
		morph target.
		\param morphIndex The index of the morph target the sort should start from. 
		Pass 0 to sort all morph targets.
		\return True if succeeded.
	*/
	MorphExport bool SortProgressiveMorphTargets(int morphIndex = 0);

	/**
		Retrieves the contribution of the specified progressive target to the solution 
		of this morph channel. The progressive target weight does not vary over time.
		\param morphIndex - the index of the target object
		\return The progressive target's weight, or 0.0 if this MaxMorphChannel instance hasn't 
		been properly initialized, or the target index is invalid.
	*/
	MorphExport float GetProgressiveMorphWeight(int morphIndex) const;

	/**
		Sets the contribution of the specified progressive target to the solution 
		of this morph channel.
		\param morphIndex - the index of the target object
		\param newValue - The weight for the progressive target 
	*/
	MorphExport bool SetProgressiveMorphWeight(int morphIndex, float newValue);

	/**
		Retrieves the tension used to interpolate between the progressive targets.
		The value is in the range [0.0f, 1.0f].
		A value of 1.0f means loose transition between progressive targets, i.e. 
		progressive target are "overshot" slightly.
		A value of 0.0f means a linear transition between progressive targets.
	*/
	MorphExport float GetProgressiveMorphTension() const;

	/**
		Sets the tension used to interpolate between progressive targets
		\param newTensionValue - The tension value to be set 
	*/
	MorphExport bool SetProgressiveMorphTension(float newTensionValue);

	/**
		Retrieves the specified morph point within the specified progressive target.
		\param morphIndex - the index of the target object
		\param pointIndex - The index of the point 
		\return The value of the specified point, or (0.0f, 0.0f, 0.0f) if this 
		MaxMorphChannel instance hasn't been properly initialized, or one of the 
		specified indexes is invalid.
	*/
	MorphExport Point3 GetProgressiveMorphPoint(int morphIndex, int pointIndex) const;
	//@}

private:
	// Default constructor.
	MaxMorphChannel();
	// Constructor
	MaxMorphChannel(morphChannel* channelImp, int channelId);
	// Initializes this MaxMorphChannel instance with the Morpher channel it wraps
	void Init(morphChannel* pImp, int channelId);

private:    
	morphChannel* mChannel;
	int mChannelId;
	friend class MaxMorphModifier;
};
