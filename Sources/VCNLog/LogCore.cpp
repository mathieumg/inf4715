///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "LogCore.h"
#include "LogProxy.h"
#include "VCNUtils/StringUtils.h"

//boost::mutex CLogManager::m_mutex;

VCNLogCore::VCNLogCore() 
  : m_defaultProc(kLogProcAll)
{
}

VCNLogCore::~VCNLogCore()
{
  for(std::vector< VCNLogProc* >::iterator it = m_LogProcs.begin(); it != m_LogProcs.end(); ++it)
  {
    delete (*it);
  }
}

bool VCNLogCore::SetDefaultProc(VCNLogProcType in_logProcTypes)
{
  m_defaultProc = in_logProcTypes;
  return true;
}

void VCNLogCore::GetMessage(VCNString& out_str, const VCNTChar* in_pMessage, va_list ap)
{
  int n, size=2048;
  while (1) 
  {
    out_str.resize(size);

    n = _vsntprintf_s(&out_str[0], size,_TRUNCATE, in_pMessage, ap);

    if (n > -1 && n < size)
      break;
    if (n > -1)
      size=n+1;
    else
      size*=2;
  }
}

bool VCNLogCore::LogMessage(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage)
{
  return DispatchMessage( in_MsgType, in_pMessage, m_defaultProc );
}

bool VCNLogCore::LogMessage(const VCNLogProcType& in_logProcs, const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage)
{
  return DispatchMessage( in_MsgType, in_pMessage, in_logProcs );
}

bool VCNLogCore::LogMessageF(const VCNLogProcType& in_logProcs, const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage, ...)
{
  va_list l_ArgList;
  va_start(l_ArgList, in_pMessage);

  VCNString str;
  GetMessage(str, in_pMessage, l_ArgList);
  va_end(l_ArgList);

  return DispatchMessage( in_MsgType, str.c_str(), in_logProcs );
}

bool VCNLogCore::LogMessageF(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage, ...)
{
  va_list l_ArgList;
  va_start(l_ArgList, in_pMessage);
  VCNString str;
  GetMessage(str, in_pMessage, l_ArgList);
  va_end(l_ArgList);

  return DispatchMessage( in_MsgType, str.c_str(), m_defaultProc );	
}


bool VCNLogCore::DispatchMessage(VCNMsgType in_MsgType, const VCNTChar* in_pMessage, const VCNLogProcType& in_logProcs)
{
  bool ret = true;
  if (m_LogProcs.empty())
  {
    _ftprintf_s(stderr, in_pMessage);
  }
  else
  {
    std::vector< VCNLogProc* >::iterator it;

    for(it = m_LogProcs.begin(); it != m_LogProcs.end(); ++it)
    {
      if ((*it)->Type() & in_logProcs)
      {
        if( !(**it).LogMessage(in_MsgType, in_pMessage) )
        {
          ret = false;
        }
      }
    }
  }
  return ret;
}

bool VCNLogCore::AddLogProc(VCNLogProc* in_pLogProc)
{
  if (in_pLogProc == NULL || in_pLogProc->GetName() == NULL)
    return false;

  // see if we want to allow duplicate according to the name of the logproc or we just filter with the address (which is weak)
  if( !ContainsLogProc(in_pLogProc) )
  {
    //        boost::unique_lock<boost::mutex> lock( m_mutex );
    m_LogProcs.push_back( in_pLogProc );
    return true;
  }
  return false;
}

bool VCNLogCore::RemoveLogProc(const VCNLogProc* in_pLogProc)
{
  std::vector< VCNLogProc* >::iterator it;
  //    boost::unique_lock<boost::mutex> lock( m_mutex );

  for(it = m_LogProcs.begin(); it != m_LogProcs.end(); ++it)
  {
    if( (*it) == in_pLogProc )
    {
      delete *it;
      m_LogProcs.erase( it );
      return true;
    }
  }
  return false;
}

bool VCNLogCore::ContainsLogProc(VCNLogProcType in_logProcTypes)
{
  //	boost::unique_lock<boost::mutex> lock( m_mutex );

  for(std::vector< VCNLogProc* >::iterator it = m_LogProcs.begin(); it != m_LogProcs.end(); ++it)
  {
    if ( ((*it)->Type() & in_logProcTypes) ==  (*it)->Type())
    {
      return true;
    }
  }

  return false;
}

bool VCNLogCore::ContainsLogProc(const VCNLogProc* in_pLogProc)
{
  if (in_pLogProc == NULL)
    return false;

  std::vector< VCNLogProc* >::iterator it;
  //    boost::unique_lock<boost::mutex> lock( m_mutex );

  for(it = m_LogProcs.begin(); it != m_LogProcs.end(); ++it)
  {
    if( in_pLogProc->GetName() != NULL )
    {
      if( std::string( (*it)->GetName() ) == std::string( in_pLogProc->GetName() ) )
      {
        return true;
      }
    }
  }

  return false;
}

std::shared_ptr<IStreamOperator> VCNLogCore::streamer(const VCNString& v) 
{
  return std::shared_ptr<VCNLogProxy>(new VCNLogProxy(*this, kNone, v, m_defaultProc));
}

std::shared_ptr<IStreamOperator> VCNLogCore::streamer(VCNMsgType const& v)
{
  return std::shared_ptr<VCNLogProxy>(new VCNLogProxy(*this, v, VCNTXT(""), m_defaultProc));
}

std::shared_ptr<IStreamOperator> VCNLogCore::streamer(VCNLogProcType const& v)
{
  return std::shared_ptr<VCNLogProxy>(new VCNLogProxy(*this, kNone, VCNTXT(""), v));
}
