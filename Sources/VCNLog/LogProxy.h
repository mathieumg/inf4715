///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#pragma once

#include "LogManager.h"

/*
Proxy Class whose sole goal in life is provide easy use of << with a forced message type on the same line.
ie: Singleton::Type(Warning) << "my warning" << 12 << endl;
*/
class VCNLogProxy : public IStreamOperator
{
public:
  VCNLogProxy(ILogManager& in_manager, VCNMsgType const& in_msgtype, const VCNString& in_str, VCNLogProcType in_logprocs);
  ~VCNLogProxy();

  virtual std::shared_ptr<IStreamOperator> streamer(const VCNString& v) override;
  virtual std::shared_ptr<IStreamOperator> streamer(VCNMsgType const& v) override;
  virtual std::shared_ptr<IStreamOperator> streamer(VCNLogProcType const& v) override;

private:
  VCNLogProxy();

  VCNMsgType      m_msgtype;
  VCNLogProcType  m_logprocs;
  ILogManager&    m_manager;
  VCNString       m_str;
};
