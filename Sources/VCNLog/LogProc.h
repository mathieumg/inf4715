///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Defines the LogProc class.
///

#ifndef VCNLOGPROC_H
#define VCNLOGPROC_H

#pragma once

#include "VCNUtils/Types.h"

enum VCNMsgType
{
  kNone,
  kInfo,
  kWarning,
  kError,
  kAssert
};

// [Flag]
enum VCNLogProcType
{
  kLogProcAll          = 0xFFFFFFFF,
  kLogProcStdOut       = 0x1,
  kLogProcStdErr       = 0x2,
  kLogProcFile         = 0x4,
  kLogProcVisualStudio = 0x8,

  kLogProcUserDefined  = 0x10000
};

inline VCNLogProcType operator|(VCNLogProcType a, VCNLogProcType b)
{return static_cast<VCNLogProcType>(static_cast<int>(a) | static_cast<int>(b));}
inline VCNLogProcType operator&(VCNLogProcType a, VCNLogProcType b)
{return static_cast<VCNLogProcType>(static_cast<int>(a) & static_cast<int>(b));}

class VCNLogProc
{
public:

  /// Returns the log processor type
  virtual VCNLogProcType Type() = 0;

  /// Returns the log processor name
  virtual const char* GetName() const = 0;

  /// Log a message, calls the message processors.
  virtual bool LogMessage(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage) = 0;
};

class VCNLogProcStdOut : public VCNLogProc
{
public:
  virtual VCNLogProcType Type() {return kLogProcStdOut;}
  virtual const char* GetName() const;
  virtual bool LogMessage(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage);
};

class VCNLogProcStdErr : public VCNLogProc
{
public:
  virtual VCNLogProcType Type() {return kLogProcStdErr;}

  virtual const char* GetName() const;
  virtual bool LogMessage(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage);
};

class VCNLogProcFile : public VCNLogProc
{
public:
  virtual VCNLogProcType Type() {return kLogProcFile;}

  VCNLogProcFile();
  VCNLogProcFile(const char* in_pFile);
  virtual bool Openfile(const char* in_pFile);
  ~VCNLogProcFile();

  virtual const char* GetName() const;
  virtual bool LogMessage(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage);

protected:
  VCNFileStream m_outfile;
  std::string   m_fileName;
};

class VCNLogProcVisualStudio : public VCNLogProc
{
public:
  virtual VCNLogProcType Type() {return kLogProcVisualStudio;}

  virtual const char* GetName() const;
  virtual bool LogMessage(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage);
};

#endif // VCNLOGPROC_H
