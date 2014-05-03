///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Defines error constants
///

#ifndef VCNERROR_H
#define VCNERROR_H

#pragma once

// ***** ERROR CODES *****

typedef VCNInt VCNRESULT;

enum VCNResult 
{
  VCN_OK              = 0x0,
  VCN_CANCELED        = 0x82000000,
  VCN_FAIL,
  VCN_NOT_IMPLEMENTED,

// Specific error messages

  VCN_CREATEAPI       ,
  VCN_CREATEDEVICE    ,
  VCN_CREATEBUFFER    ,
  VCN_INVALIDPARAM    ,
  VCN_INVALIDID       ,
  VCN_BUFFERSIZE      ,
  VCN_BUFFERLOCK      ,
  VCN_NOTCOMPATIBLE   ,
  VCN_OUTOFMEMORY     ,
  VCN_FILENOTFOUND    ,
  VCN_INVALIDFILE     ,
  VCN_NOSHADERSUPPORT 
};

#define VCN_FAILED(b)    (b != VCN_OK)
#define VCN_SUCCESSED(b) (b == VCN_OK)

#endif // VCNERROR_H
