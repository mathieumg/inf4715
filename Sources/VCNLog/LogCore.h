///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Defines the main log manager.
///

#ifndef VCNLOGCORE_H
#define VCNLOGCORE_H

#pragma once

#include "LogManager.h"

class VCNLogCore : public ILogManager
{
public:
  VCNLogCore();
  ~VCNLogCore();

  virtual bool LogMessage(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage) override;
  virtual bool LogMessage(const VCNLogProcType& in_logProcs, const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage) override;
  virtual bool LogMessageF(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage, ...) override;
  virtual bool LogMessageF(const VCNLogProcType& in_logProcs, const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage, ...) override;
  virtual bool AddLogProc(VCNLogProc* in_pLogProc) override;
  virtual bool RemoveLogProc(const VCNLogProc* in_pLogProc) override;
  virtual bool ContainsLogProc(const VCNLogProc* in_pLogProc) override;
  virtual bool ContainsLogProc(VCNLogProcType in_logProcTypes) override;
  virtual bool SetDefaultProc(VCNLogProcType in_logProcTypes) override;

  virtual std::shared_ptr<IStreamOperator> streamer(const VCNString& v) override;
  virtual std::shared_ptr<IStreamOperator> streamer(VCNMsgType const& v) override;
  virtual std::shared_ptr<IStreamOperator> streamer(VCNLogProcType const& v) override;

private:

  void GetMessage(VCNString& out_str, const VCNTChar* in_pMessage, va_list ap);
  bool DispatchMessage( VCNMsgType in_MsgType, const VCNTChar* in_pMessage, const VCNLogProcType& in_logProcs );

  //static boost::mutex m_mutex;

  std::vector< VCNLogProc* >  m_LogProcs;
  VCNLogProcType              m_defaultProc;
};

#endif // VCNLOGCORE_H
