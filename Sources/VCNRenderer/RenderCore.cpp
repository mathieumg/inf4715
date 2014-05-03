///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Abstract renderer implementation
///

#include "Precompiled.h"
#include "RenderCore.h"

#include <cstdarg>

VCN_TYPE( VCNRenderCore, VCNCore<VCNRenderCore> ) ;

//-------------------------------------------------------------
///
//-------------------------------------------------------------
VCNRenderCore::VCNRenderCore()
  : mScreenWidth(1)
  , mScreenHeight(1)
{
}

//-------------------------------------------------------------
///
//-------------------------------------------------------------
VCNRenderCore::~VCNRenderCore()
{
}

//-------------------------------------------------------------
///
//-------------------------------------------------------------
VCNBool VCNRenderCore::Initialize()
{
  return BaseCore::Initialize();
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNRenderCore::Uninitialize()
{
  return BaseCore::Uninitialize();
}

///////////////////////////////////////////////////////////////////////
VCNRESULT VCNRenderCore::RenderText(VCNUInt nID, 
                                  VCNInt x, VCNInt y, 
                                  VCNByte r, VCNByte g, VCNByte b, 
                                  const VCNTChar* format, ...)
{
  va_list args;
  va_start(args, format);

  const VCNRESULT result = RenderText( nID, x, y, r, g, b, 0, format, args);

  va_end(args);
  return result;
}

///////////////////////////////////////////////////////////////////////
VCNRESULT VCNRenderCore::RenderText(VCNUInt nID, 
                                  VCNInt x, VCNInt y, 
                                  const VCNColor& c, 
                                  VCNRect* rect, const VCNTChar* format, ...)
{
  va_list args;
  va_start(args, format);

  const VCNRESULT result = RenderText( nID, x, y, c.Red(), c.Green(), c.Blue(), rect, format, args );

  va_end(args);
  return result;
}

///////////////////////////////////////////////////////////////////////
VCNRESULT VCNRenderCore::RenderText(VCNUInt nID, VCNInt x, VCNInt y, const VCNColor& c, const VCNTChar* format, ...)
{
  va_list args;
  va_start(args, format);

  const VCNRESULT result = RenderText( nID, x, y, c.Red(), c.Green(), c.Blue(), 0, format, args );

  va_end(args);
  return result;
}

///////////////////////////////////////////////////////////////////////
VCNRESULT VCNRenderCore::RenderText(VCNUInt nID, 
                                  VCNFloat x, VCNFloat y, 
                                  VCNByte r, VCNByte g, VCNByte b, 
                                  VCNRect* rect, const VCNTChar* format, ...)
{
  va_list args;
  va_start(args, format);

  const VCNRESULT result = RenderText( nID, static_cast<VCNInt>(x), static_cast<VCNInt>(y), r, g, b, rect, format, args);

  va_end(args);
  return result;
}

///////////////////////////////////////////////////////////////////////
VCNRESULT VCNRenderCore::RenderText(VCNUInt nID, 
                                  VCNFloat x, VCNFloat y, 
                                  VCNByte r, VCNByte g, VCNByte b, 
                                  const VCNTChar* format, ...)
{
  va_list args;
  va_start(args, format);

  const VCNRESULT result = RenderText( nID, static_cast<VCNInt>(x), static_cast<VCNInt>(y), r, g, b, 0, format, args);

  va_end(args);
  return result;
}

///////////////////////////////////////////////////////////////////////
VCNRESULT VCNRenderCore::RenderText(VCNUInt nID, 
                                  VCNFloat x, VCNFloat y, 
                                  const VCNColor& c, 
                                  VCNRect* rect, const VCNTChar* format, ...)
{
  va_list args;
  va_start(args, format);

  const VCNRESULT result = RenderText( nID, 
    static_cast<VCNInt>(x), static_cast<VCNInt>(y), c.Red(), c.Green(), c.Blue(), rect, format, args);

  va_end(args);
  return result;
}

///////////////////////////////////////////////////////////////////////
VCNRESULT VCNRenderCore::RenderText(VCNUInt nID, VCNFloat x, VCNFloat y, const VCNColor& c, const VCNTChar* format, ...)
{
  va_list args;
  va_start(args, format);

  const VCNRESULT result = RenderText( nID, 
    static_cast<VCNInt>(x), static_cast<VCNInt>(y), c.Red(), c.Green(), c.Blue(), 0, format, args);

  va_end(args);
  return result;
}

///////////////////////////////////////////////////////////////////////
VCNRESULT VCNRenderCore::RenderText(VCNUInt nID, 
                                  VCNInt x, VCNInt y, 
                                  VCNByte r, VCNByte g, VCNByte b, 
                                  VCNRect* rect, const VCNTChar* format, ...)
{
  va_list args;
  va_start(args, format);

  const VCNRESULT result = RenderText( nID, x, y, r, g, b, rect, format, args );

  va_end(args);

  return result;
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNRenderCore::Process(const float dt)
{
  return BaseCore::Process( dt );
}

///////////////////////////////////////////////////////////////////////
void VCNRenderCore::SetTextureFiltering(VCNRenderState mag, VCNRenderState min, VCNRenderState max)
{
  throw std::exception("The method or operation is not implemented.");
}
