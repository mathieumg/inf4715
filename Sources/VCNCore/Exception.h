///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Exception interface and implementation
///

#ifndef VCN_EXCEPTION_H
#define VCN_EXCEPTION_H

// System includes
#include <exception>
#include <string>

///
/// Base class for exception.
///
class VCNException : public std::exception
{
public :

  /// Build exception
  explicit VCNException(const std::string& message = "");

  /// Builds the exception using a unicode string that will get converted to ascii
  explicit VCNException(const std::wstring& message);

  /// Delete exception
  virtual ~VCNException() throw();

  /// Returns the exception message if any.
  virtual const char* what() const throw() override;

private :

  /// Message describing the exception
  std::string mMessage;
};

///
/// Defines an exception that raises an assert as well
///
class VCNAssertException : public VCNException
{
public:

  /// Constructs the exception and asserts.
  VCNAssertException(const std::string& file, int line, const std::string& message);

private:

};

/// Use the macro to construct and VCNAssertException easily.
#define VNC_ASSERT_EXCEPTION(message) VCNAssertException(__FILE__, __LINE__, message)

#endif // VCN_EXCEPTION_H
