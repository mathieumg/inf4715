///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief String utilities inline implementations
///

///////////////////////////////////////////////////////////////////////
///
/// Splits the string into tokens.
/// 
/// @param str          [IN]  string to parse
/// @param tokens       [OUT] output tokens
/// @param delimiters   [IN]  delimiters to watch
///
inline void StringUtils::Split(const VCNString& str, std::vector<VCNString>& tokens, const VCNString& delimiters)
{
  // Make sure token vector is empty.
  tokens.clear();

  // Parse input string and extract tokens one by one.
  for (VCNString::size_type p1 = 0, p2 = 0; p1 != VCNString::npos; )
  {
    p1 = str.find_first_not_of(delimiters, p1);
    if (p1 != VCNString::npos)
    {
      p2 = str.find_first_of(delimiters , p1);
      tokens.push_back(str.substr(p1, p2 - p1));
      p1 = p2;
    }
  }
}

////////////////////////////////////////////////////////////
inline const std::vector<VCNString> StringUtils::Split(const VCNString& str, const VCNString& delimiters)
{
  std::vector<VCNString> tokens;
  Split(str, tokens, delimiters);
  return tokens;
}

////////////////////////////////////////////////////////////
inline const VCNString StringUtils::ReadFile(const VCNString& filename)
{
  // Open the file to be read.
  std::ifstream File(filename.c_str());
  if ( File.is_open() )
    return _T("");

  // Put the file in the mem buffer.
  std::wostringstream oss;
  oss << File.rdbuf();

  return oss.str();
}

////////////////////////////////////////////////////////////
inline const unsigned long StringUtils::StringHash(const VCNString& str)
{
  unsigned long result = 0;
  for (VCNString::const_iterator i = str.begin(); i != str.end(); ++i)
    result = 5 * result + *i;

  return result;
}

////////////////////////////////////////////////////////////
inline const VCNString StringUtils::ToLower(const VCNString& str)
{
  VCNString result(str.size(), ' ');

  // Le cast est nécessaire pour faire fonctionner ce code sous gcc
  std::transform(str.begin(), str.end(), result.begin(), static_cast<int (*)(int)>(std::tolower));

  return result;
}

////////////////////////////////////////////////////////////
inline const VCNString StringUtils::ToUpper(const VCNString& str)
{
  VCNString result(str.size(), ' ');

  // Le cast est nécessaire pour faire fonctionner ce code sous gcc
  std::transform(str.begin(), str.end(), result.begin(), static_cast<int (*)(int)>(std::toupper));

  return result;
}

///////////////////////////////////////////////////////////////////////
inline const std::wstring StringUtils::StringToWString(const std::string& s)
{
  std::wstring wstr;
  wstr.resize( s.size() );
  mbstowcs( &wstr[0], s.c_str(), s.size() );
  return wstr;
}

///////////////////////////////////////////////////////////////////////
inline const std::string StringUtils::WStringToString(const std::wstring& s)
{
  std::string mbstr;
  mbstr.resize( s.size() );
  wcstombs( &mbstr[0], s.c_str(), s.size() );
  return mbstr;
}

////////////////////////////////////////////////////////////
inline StringBuilder::StringBuilder()
{
}

////////////////////////////////////////////////////////////
template <typename T> inline StringBuilder::StringBuilder(const T& value)
{
  mOutStream << value;
}

////////////////////////////////////////////////////////////
template <typename T> inline StringBuilder& StringBuilder::operator ()(const T& value)
{
  mOutStream << value;
  return *this;
}

////////////////////////////////////////////////////////////
template <typename T> inline StringBuilder& StringBuilder::operator <<(const T& value)
{
  mOutStream << value;
  return *this;
}

////////////////////////////////////////////////////////////
template <> inline StringBuilder& StringBuilder::operator()<std::string> (const std::string& Value)
{
  mOutStream << StringUtils::StringToWString(Value);
  return *this;
}

////////////////////////////////////////////////////////////
template <typename T> inline StringBuilder& StringBuilder::operator+(const T& Value)
{
  mOutStream << Value;
  return *this;
}

////////////////////////////////////////////////////////////
template <typename T> inline StringBuilder& StringBuilder::operator+=(const T& Value)
{
  mOutStream << Value;
  return *this;
}

////////////////////////////////////////////////////////////
inline StringBuilder::operator const VCNString()
{
  return mOutStream.str();
}

////////////////////////////////////////////////////////////
inline StringExtractor::StringExtractor(const VCNString& str) 
  : mInStream(str)
{
}

////////////////////////////////////////////////////////////
template <typename T> inline StringExtractor& StringExtractor::operator ()(T& value)
{
  if (!(mInStream >> value))
  {
    if (mInStream.eof())
      throw VCNException(StringBuilder("Failed to extract \"")(typeid(T).name())("\" at EOF."));
    else
      throw VCNException(StringBuilder("Failed to extract \"")(typeid(T).name())("\""));
  }

  return *this;
}
