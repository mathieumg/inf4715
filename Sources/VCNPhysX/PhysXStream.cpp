///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief PhysX Stream classes implementation
///

#include "Precompiled.h"
#include "PhysxStream.h"

VCNPhysxMemoryOutputStream::VCNPhysxMemoryOutputStream() :
mData		(NULL),
  mSize		(0),
  mCapacity	(0)
{
}

VCNPhysxMemoryOutputStream::~VCNPhysxMemoryOutputStream()
{
  if(mData)
    delete[] mData;
}

PxU32 VCNPhysxMemoryOutputStream::write(const void* src, PxU32 size)
{
  PxU32 expectedSize = mSize + size;
  if(expectedSize > mCapacity)
  {
    mCapacity = expectedSize + 4096;

    PxU8* newData = new PxU8[mCapacity];
    PX_ASSERT(newData!=NULL);

    if(newData)
    {
      memcpy(newData, mData, mSize);
      delete[] mData;
    }
    mData = newData;
  }
  memcpy(mData+mSize, src, size);
  mSize += size;
  return size;
}

///////////////////////////////////////////////////////////////////////////////

VCNPhysxMemoryInputData::VCNPhysxMemoryInputData(PxU8* data, PxU32 length) :
mSize	(length),
  mData	(data),
  mPos	(0)
{
}

PxU32 VCNPhysxMemoryInputData::read(void* dest, PxU32 count)
{
  PxU32 length = PxMin<PxU32>(count, mSize-mPos);
  memcpy(dest, mData+mPos, length);
  mPos += length;
  return length;
}

PxU32 VCNPhysxMemoryInputData::getLength() const
{
  return mSize;
}

void VCNPhysxMemoryInputData::seek(PxU32 offset)
{
  mPos = PxMin<PxU32>(mSize, offset);
}

PxU32 VCNPhysxMemoryInputData::tell() const
{
  return mPos;
}

///////////////////////////////////////////////////////////////////////////////

VCNPhysxFileOutputStream::VCNPhysxFileOutputStream(const char* filename)
{
  mFile = NULL;
  fopen_s(&mFile, filename, "wb");
}

VCNPhysxFileOutputStream::~VCNPhysxFileOutputStream()
{
  if(mFile)
    fclose(mFile);
}

PxU32 VCNPhysxFileOutputStream::write(const void* src, PxU32 count)
{
  return mFile ? (PxU32)fwrite(src, 1, count, mFile) : 0;
}

bool VCNPhysxFileOutputStream::isValid()
{
  return mFile != NULL;
}

///////////////////////////////////////////////////////////////////////////////

VCNPhysxFileInputData::VCNPhysxFileInputData(const char* filename)
{
  mFile = NULL;
  fopen_s(&mFile, filename, "rb");

  if(mFile)
  {
    fseek(mFile, 0, SEEK_END);
    mLength = ftell(mFile);
    fseek(mFile, 0, SEEK_SET);
  }
  else
  {
    mLength = 0;
  }
}

VCNPhysxFileInputData::~VCNPhysxFileInputData()
{
  if(mFile)
    fclose(mFile);
}

PxU32 VCNPhysxFileInputData::read(void* dest, PxU32 count)
{
  PX_ASSERT(mFile);
  const size_t size = fread(dest, 1, count, mFile);
  PX_ASSERT(PxU32(size)==count);
  return PxU32(size);
}

PxU32 VCNPhysxFileInputData::getLength() const
{
  return mLength;
}

void VCNPhysxFileInputData::seek(PxU32 pos)
{
  fseek(mFile, pos, SEEK_SET);
}

PxU32 VCNPhysxFileInputData::tell() const
{
  return ftell(mFile);
}

bool VCNPhysxFileInputData::isValid() const
{
  return mFile != NULL;
}
