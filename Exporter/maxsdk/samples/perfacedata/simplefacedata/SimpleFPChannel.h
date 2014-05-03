//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
/**********************************************************************
	FILE: SimpleFPChannel.h

	DESCRIPTION: A templated implementation of a FPValue-exposed 
					SimpleFaceChannel wrapper.
				- class declaration
				- class implementation, templated

	AUTHOR: ktong - created 01.30.2006
***************************************************************************/
#ifndef _SIMPLEFPCHANNEL_H_
#define _SIMPLEFPCHANNEL_H_

#include "ISimpleFPChannel.h"
#include "SimpleFaceChannel.h"

template <class T>
class SimpleFPChannel : public ISimpleFPChannel
{
protected:
	//! \brief mpChan - Storage for a SimpleFaceChannel. DO NOT free this channel from
	//! within this wrapper, this channel is managed by a FaceDataMgr.
	SimpleFaceChannel<T>* mpChan;

public:
	SimpleFPChannel(IFaceDataChannel* pChan);

	//! \brief GetChannel - Get the wrapped channel
	IFaceDataChannel* GetChannel();

	//! \brief GetChannelName - Get the channel name.
	const TCHAR* GetChannelName();

	//! \brief SetChannelName - Set the channel name
	void SetChannelName(const TCHAR* pName);

	//! \brief GetChannelID - Get the channel ID.
	Class_ID GetChannelID();

	//! \brief SetChannelID - Set the channel ID.
	void SetChannelID(const Class_ID &classID);

	//! \brief GetType - Get the data type of the channel.
	int GetChannelType();

	//! \brief GetNumFaces - Number of faces in this channel
	ULONG GetNumFaces();

	//! \brief SetNumFaces - Set the number of faces in this channel
	void SetNumFaces(ULONG numFaces);

	//! \brief Compare - Compare two FPValues as types of this channel.
	BOOL Compare(const FPValue &val1, const FPValue &val2);

	//! \brief GetValue - Get the value of the specified face from the specified channel.
	BOOL GetValue(ULONG face, FPValue &val);

	//! \brief SetValue - Set the value of the specified face from the specified channel.
	BOOL	SetValue(ULONG face, const FPValue* pVal);

	//! \brief GetValues - Returns a copy of the channel list. Empty list on failure.
	BOOL	GetValues(FPValue &val);

	//! \brief SetValues - Set the channel with a list of values. The new list and the channel must match in size.
	BOOL SetValues(const FPValue* pVal);

	IOResult	Save(ISave* isave);
	IOResult	Load(ILoad* iload);
};	

//-----------------------------------------------------------------------------
template <class T>
SimpleFPChannel<T>::SimpleFPChannel(IFaceDataChannel* pChan)
//-----------------------------------------------------------------------------
{
	// can be null if wrapping a non-channel
	mpChan = static_cast<SimpleFaceChannel<T>*>(pChan);
}

//-----------------------------------------------------------------------------
template <class T>
IFaceDataChannel* SimpleFPChannel<T>::GetChannel()
//-----------------------------------------------------------------------------
{
	// can be null if wrapping a non-channel
	return mpChan;
}


//-----------------------------------------------------------------------------
template <class T>
const TCHAR* SimpleFPChannel<T>::GetChannelName()
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	return mpChan->GetChannelName();
}

//-----------------------------------------------------------------------------
template <class T>
void SimpleFPChannel<T>::SetChannelName(const TCHAR* pName)
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	mpChan->SetChannelName(pName);
}

//-----------------------------------------------------------------------------
template <class T>
Class_ID SimpleFPChannel<T>::GetChannelID()
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	return mpChan->GetChannelID();
}

//-----------------------------------------------------------------------------
template <class T>
void SimpleFPChannel<T>::SetChannelID(const Class_ID &classID)
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	return mpChan->SetChannelID(classID);
}

//-----------------------------------------------------------------------------
template <class T>
int SimpleFPChannel<T>::GetChannelType()
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	return mpChan->GetChannelType();
}

//-----------------------------------------------------------------------------
template <class T>
ULONG SimpleFPChannel<T>::GetNumFaces()
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	return mpChan->Count();
}

//-----------------------------------------------------------------------------
template <class T>
void SimpleFPChannel<T>::SetNumFaces(ULONG numFaces)
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	ULONG oldNum = mpChan->Count();
	int needMore = numFaces - oldNum;
	if (needMore < 0) {
		mpChan->FacesDeleted(numFaces, -needMore);
	} else if (needMore > 0) {
		mpChan->FacesCreated(oldNum, needMore);
	}
}

//-----------------------------------------------------------------------------
template <class T>
BOOL SimpleFPChannel<T>::Compare(const FPValue &val1, const FPValue &val2)
//-----------------------------------------------------------------------------
{
	DbgAssert(val1.type == val2.type);
	// same type, and non null
	if ((val1.type == val2.type) && (val1.ptr && val2.ptr)) {
		T v1 = static_cast<T>(val1.i);
		T v2 = static_cast<T>(val2.i);
		return (v1 == v2);
	} else {
		// same type, both have null data, means they're both "undefined"
		return ((val1.type == val2.type) && (!val1.ptr && !val2.ptr));
	}
}

//-----------------------------------------------------------------------------
template <class T>
BOOL	SimpleFPChannel<T>::GetValue(ULONG face, FPValue &val)
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	T data;
	if (mpChan->GetValue(face, data)) {
		val.Load(mpChan->GetChannelType(), data);
		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
template <class T>
BOOL	SimpleFPChannel<T>::SetValue(ULONG face, const FPValue* pVal)
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	if (pVal->type == mpChan->GetChannelType()) {
		const T* data = reinterpret_cast<const T*>(&(pVal->ptr));
		return mpChan->SetValue(face, *data);
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
template <class T>
BOOL SimpleFPChannel<T>::GetValues(FPValue &val)
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	val.Load(mpChan->GetChannelType() + TYPE_TAB, new Tab<T>);
	if ( !mpChan->GetValues(*(static_cast<Tab<T>*>(val.ptr))) ) {
		val.Load(TYPE_VALUE, NULL);
		return FALSE;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
template <class T>
BOOL	SimpleFPChannel<T>::SetValues(const FPValue* pVal)
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	if (pVal->type == (mpChan->GetChannelType() + TYPE_TAB)) {
		Tab<T>* pValues = static_cast<Tab<T>*>(pVal->ptr);
		return mpChan->SetValues(*pValues);
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
template <class T>
IOResult	SimpleFPChannel<T>::Save(ISave* isave)
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	return mpChan->Save(isave);
}
//-----------------------------------------------------------------------------
template <class T>
IOResult	SimpleFPChannel<T>::Load(ILoad* iload)
//-----------------------------------------------------------------------------
{
	DbgAssert(mpChan);
	return mpChan->Load(iload);
}



#endif