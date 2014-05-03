///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "LogCore.h"
#include "LogProxy.h"

using namespace std;

VCNLogProxy::VCNLogProxy(ILogManager& in_manager, VCNMsgType const& in_msgtype, const VCNString& in_str, VCNLogProcType in_logprocs) 
  : m_manager(in_manager)
  , m_msgtype(in_msgtype)
  , m_logprocs(in_logprocs)
  , m_str(in_str)
{
}

VCNLogProxy::~VCNLogProxy()
{
  if ( !m_str.empty() )
  {
    m_manager.LogMessage(m_logprocs, m_msgtype, m_str.c_str());
  }
}

// For Operator <<
std::shared_ptr<IStreamOperator> VCNLogProxy::streamer(const VCNString& v) 
{
  m_str += v;
  std::shared_ptr<VCNLogProxy> ret(new VCNLogProxy(m_manager, m_msgtype, m_str, m_logprocs));
  m_str = VCNTXT("");
  return ret;
}

std::shared_ptr<IStreamOperator> VCNLogProxy::streamer(VCNMsgType const& v)
{
  m_msgtype = v;
  return streamer(VCNString());
}

std::shared_ptr<IStreamOperator> VCNLogProxy::streamer(VCNLogProcType const& v)
{
  m_logprocs = m_logprocs | v;
  return streamer(VCNString());
}

std::shared_ptr<IStreamOperator> operator<<( std::shared_ptr<IStreamOperator> const& l, std::basic_ostream<VCNTChar>& (*pf)(std::basic_ostream<VCNTChar>& _Ostr) )
{
  return *l << VCNTXT("\n");
}
