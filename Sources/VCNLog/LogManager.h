///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#ifndef ILOGMANAGER_H
#define ILOGMANAGER_H

#pragma once

#include "LogProc.h"
#include "StreamOperator.h"

class ILogManager : public IStreamOperator
{
public:

  /// Log a message, calls the message processors.
  virtual bool LogMessage(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage) = 0;

  /// Log a message, calls the message processors.
  virtual bool LogMessage(const VCNLogProcType& in_logProcs, const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage) = 0;

  /// Log a message, calls the message processors. String can have printf format.
  virtual bool LogMessageF(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage, ...) = 0;

  /// Log a message, calls the message processors. String can have printf format.
  virtual bool LogMessageF(const VCNLogProcType& in_logProcs, const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage, ...) = 0;

  /// Add a message processor. -- The message processor will be deleted when log manager gets destroyed
  virtual bool AddLogProc(VCNLogProc* in_pLogProc) = 0;

  /// Remove a message processor.
  virtual bool RemoveLogProc(const VCNLogProc* in_pLogProc) = 0;

  /// Does the manager contains the specified message processor.
  virtual bool ContainsLogProc(const VCNLogProc* in_pLogProc) = 0;

  /// Does the manager contains the specified message processor.
  virtual bool ContainsLogProc(VCNLogProcType in_logProcTypes) = 0;

  /// Set which proc are used when procs are not specified to log a message. (ie: prevents output to both stdout and stderr)
  virtual bool SetDefaultProc(VCNLogProcType in_logProcTypes) = 0;
};

#endif // ILOGMANAGER_H
