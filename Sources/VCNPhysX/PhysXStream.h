///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
///

#ifndef VCNPHYSXSTREAM_H
#define VCNPHYSXSTREAM_H

#pragma once

class VCNPhysxMemoryOutputStream: public PxOutputStream
{
public:
  VCNPhysxMemoryOutputStream();
  virtual				~VCNPhysxMemoryOutputStream();

  PxU32		write(const void* src, PxU32 count);

  PxU32		getSize()	const	{	return mSize; }
  PxU8*		getData()	const	{	return mData; }
private:
  PxU8*		mData;
  PxU32		mSize;
  PxU32		mCapacity;
};

class VCNPhysxFileOutputStream: public PxOutputStream
{
public:
  VCNPhysxFileOutputStream(const char* name);
  virtual				~VCNPhysxFileOutputStream();

  PxU32		write(const void* src, PxU32 count);

  bool		isValid();
private:
  FILE*		mFile;
};

class VCNPhysxMemoryInputData: public PxInputData
{
public:
  VCNPhysxMemoryInputData(PxU8* data, PxU32 length);

  PxU32		read(void* dest, PxU32 count);
  PxU32		getLength() const;
  void		seek(PxU32 pos);
  PxU32		tell() const;

private:
  PxU32		mSize;
  const PxU8*	mData;
  PxU32		mPos;
};

class VCNPhysxFileInputData: public PxInputData
{
public:
  VCNPhysxFileInputData(const char* name);
  virtual				~VCNPhysxFileInputData();

  PxU32		read(void* dest, PxU32 count);
  void		seek(PxU32 pos);
  PxU32		tell() const;
  PxU32		getLength() const;

  bool		isValid() const;
private:
  FILE*		mFile;
  PxU32		mLength;
};


#endif // VCNPHYSXSTREAM_H

