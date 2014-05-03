///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Vicuna exception implementation
///

#include "Precompiled.h"
#include "Exception.h"

#include "VCNUtils/StringUtils.h"
#include "VCNUtils/Assert.h"

///////////////////////////////////////////////////////////////////////
///
/// Construct an exception.
/// 
/// @param message  [IN] message describing the exception.
///
VCNException::VCNException(const std::string& message /*= ""*/) 
  : std::exception( message.c_str() )
  , mMessage(message)
{
}

///////////////////////////////////////////////////////////////////////
VCNException::VCNException(const std::wstring& message)
  : std::exception( VCN_W2A(message) )
  , mMessage( VCN_W2A(message) )
{

}

///////////////////////////////////////////////////////////////////////
///
/// Destructs the exception.
///
VCNException::~VCNException() throw()
{
}

///////////////////////////////////////////////////////////////////////
///
/// Returns the message describing the exception.
///
/// @return raw string message.
///
const char* VCNException::what() const throw() 
{
  return mMessage.c_str();
}

///////////////////////////////////////////////////////////////////////
///
/// Constructs an exception and asserts to the user.
/// It is prefered to use the VNC_ASSERT_EXCEPTION macro to build
/// this type of exception.
/// 
/// @param file     [IN] source filename where the exception occurred
/// @param line     [IN] source line where the exception occurred
/// @param message  [IN] description of the assert/exception.
///
VCNAssertException::VCNAssertException(const std::string& file, int line, const std::string& message) 
  : VCNException( message )
{
  bool alwaysAssert = true;
  std::wstring wmessage = VCN_A2W( message );
  if( CustomAssert( 0, _T(__FUNCTION__), wmessage.c_str(), line, file.c_str(), alwaysAssert) ) 
  {                                                                                                   
    _asm { int 3 }
  }
}
