///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#pragma once

#include "VCNUtils/Types.h"
#include "VCNUtils/StringUtils.h"

// Convenience class to use existing ostream << operator
class IStreamOperator
{
public:
  /*
  Returns
  */
  virtual std::shared_ptr<IStreamOperator> streamer(const VCNString& v) = 0;
  virtual std::shared_ptr<IStreamOperator> streamer(VCNMsgType const& v) = 0;
  virtual std::shared_ptr<IStreamOperator> streamer(VCNLogProcType const& v) = 0;

  /*
  Stream operator << compatibility for ease of use
  */
  template<typename T>
  std::shared_ptr<IStreamOperator> operator<<(const T& v) 
  {
    VCNStringStream stream;
    stream << v;
    return streamer(stream.str());
  }

#if defined( _UNICODE )
  std::shared_ptr<IStreamOperator> operator<<(const std::string& v) 
  {
    return streamer( StringUtils::StringToWString(v) );
  }

  std::shared_ptr<IStreamOperator> operator<<(const char* v) 
  {
    return streamer( StringUtils::StringToWString(v) );
  }
#else
  std::shared_ptr<IStreamOperator> operator<<(const std::wstring& v) 
  {
    return streamer( StringUtils::WStringToString(v) );
  }

  std::shared_ptr<IStreamOperator> operator<<(const wchar_t* v) 
  {
    return streamer( StringUtils::WStringToString(v) );
  }
#endif

  std::shared_ptr<IStreamOperator> operator<<(const VCNString& v) 
  {
    return streamer(v);
  }

  std::shared_ptr<IStreamOperator> operator<<(const wchar_t* v) 
  {
    return streamer(v);
  }

  // Support Log << Warning << "My warning message";  -- Note: This is an overload, not a template specialization
  std::shared_ptr<IStreamOperator> operator<< (const VCNMsgType& v) 
  {
    return streamer(v);
  }

  // Support Log << Warning << "My warning message";  -- Note: This is an overload, not a template specialization
  std::shared_ptr<IStreamOperator> operator<< (const VCNLogProcType& v) 
  {
    return streamer(v);
  }

  // Support Log << endl;
  std::shared_ptr<IStreamOperator> operator<<( std::basic_ostream<VCNTChar>& (*pf)(std::basic_ostream<VCNTChar>& _Ostr) )
  {
    return streamer(VCNTXT("\n"));
  }
};

// Allow shared_ptr to be used globally
template<typename T>
std::shared_ptr<IStreamOperator> operator<<(std::shared_ptr<IStreamOperator> const& l, const T& r) 
{
  return *l << r;
}

extern std::shared_ptr<IStreamOperator> operator<<( std::shared_ptr<IStreamOperator> const& l, std::basic_ostream<VCNTChar>& (*pf)(std::basic_ostream<VCNTChar>& _Ostr) );
