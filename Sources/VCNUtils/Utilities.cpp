///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Various utilities
///

#include "Precompiled.h"
#include "Utilities.h"

#include "VCNUtils/Macros.h"

////////////////////////////////////////////////////////////////////////
const std::string VCN::RemoveUnwantedCharacters(const std::string& str, const std::string& chars)
{
  using namespace std;

  std::string ret = str;
  std::string::const_iterator charItr = chars.begin();
  std::string::const_iterator charEndItr = chars.end();
  for( ; charItr != charEndItr; ++charItr)
  {
    size_t pos = ret.find(*charItr);
    while ( pos != string::npos )
    {
      ret.erase(pos, 0);
      pos = ret.find(*charItr);
    }
  }

  return ret;
}


////////////////////////////////////////////////////////////////////////
///
/// Print an error to the user.
///
/// @param[in] message : error message to be displayed.
///
void VCN::PrintError(const std::string& message)
{
#ifdef WIN32
  MessageBoxA(0, message.c_str(), "Error", MB_OK | MB_ICONERROR);
#else
  std::cerr << message << std::endl;
#endif
}


//////////////////////////////////////////////////////////////////////////
///
/// Prints floating-point numbers using engineering notation and SI prefixes 
/// Source: http://www.cs.tut.fi/~jkorpela/c/eng.html
///
/// @usage printf("%sW",eng(eff,4,0))
/// 
/// @param value    [IN/OUT]    floating value to be printed
/// @param digits   [IN/OUT]    number of digit to display
/// @param numeric  [IN/OUT]    SI or not to SI
///
/// @return string base representation
///
char* VCN::Eng(double value, int digits, int numeric)
{
  #define PREFIX_START (-24)
  /* Smallest power of then for which there is a prefix defined.
  If the set of prefixes will be extended, change this constant
  and update the table "prefix". */

  static char *prefix[] = {
    "y", "z", "a", "f", "p", "n", "u", "m", "",
    "k", "M", "G", "T", "P", "E", "Z", "Y"
  }; 
  #define PREFIX_END (PREFIX_START+ (int)((sizeof(prefix)/sizeof(char *)-1)*3))

  int expof10;
  static char result[100];
  char *res = result;

  if (value < 0.)
  {
    *res++ = '-';
    value = -value;
  }
  if (value == 0.)
  {
    return "0.0";
  }

  expof10 = (int) log10(value);
  if(expof10 > 0)
    expof10 = (expof10/3)*3;
  else
    expof10 = (-expof10+3)/3*(-3); 

  value *= pow(10.0f,-expof10);

  if (value >= 1000.)
  { value /= 1000.0; expof10 += 3; }
  else if(value >= 100.0)
    digits -= 2;
  else if(value >= 10.0)
    digits -= 1;

  if(numeric || (expof10 < PREFIX_START) ||    
    (expof10 > PREFIX_END))
    sprintf_s(res, STATIC_ARRAY_COUNT(result), "%.*fe%d", digits-1, value, expof10); 
  else
    sprintf_s(res, STATIC_ARRAY_COUNT(result), "%.*f %s", digits-1, value, 
    prefix[(expof10-PREFIX_START)/3]);
  return result;
}

////////////////////////////////////////////////////////////////////////
///
/// Returns true if the value is almost zero
///
/// @param[in] value : value to be checked
///
/// @return true if almost zero
///
////////////////////////////////////////////////////////////////////////
const bool VCN::IsZero(float value)
{
  return (value < EPSILON_VCN) && (-value < EPSILON_VCN);
}


////////////////////////////////////////////////////////////////////////
///
/// Converts radians to degrees
///
const float VCN::RAD_TO_DEG_VCN(const float angleRad)
{
  return angleRad * 57.2957795130823208767981548141052f;
}


////////////////////////////////////////////////////////////////////////
///
/// Returns the conversion of degrees to radians.
///
const float VCN::DEG_TO_RAD_VCN(const float angleDeg)
{
  return angleDeg * 0.0174532925199432957692369076848861f;
}


////////////////////////////////////////////////////////////////////////
///
/// Returns the conversion of kilometers per hours to meter per seconds.
///
const float VCN::KMH_TO_MS( const float kmh )
{
  return (kmh * 1000.0f / 3600.0f);
}


////////////////////////////////////////////////////////////////////////
///
/// Returns the conversion of meter per seconds to kilometers per hours.
///
const float VCN::MS_TO_KMH( const float ms )
{
  return (ms * 3.6f);
}


////////////////////////////////////////////////////////////////////////
///
/// Returns the sign of the number.
///
float VCN::Sign( float value )
{
  if (value == 0.0f) {
    return 0.0f;
  }
  else if (value > 0.0f) {
    return  1.0f;
  }
  else {
    return -1.0f;
  }
}


////////////////////////////////////////////////////////////////////////
///
/// Returns the absolute number of a value.
///
float VCN::Abs( float nombre )
{
  return (Sign(nombre) * nombre);
}


////////////////////////////////////////////////////////////////////////
///
/// Checks if the value is in interval.
///
const bool VCN::IsBetween( float valeur, float borneMin, float borneMax )
{
  return (valeur >= borneMin) && (valeur <= borneMax);
}


////////////////////////////////////////////////////////////////////////
///
///  Checks if point is in square.
///
const bool VCN::IsInSquare( float x, float xMin, float xMax, float y, float yMin, float yMax )
{
  return (IsBetween( x, xMin, xMax ) && IsBetween( y, yMin, yMax ));
}


////////////////////////////////////////////////////////////////////////
///
/// Returns biggest aspect ratio of rectangle.
///
float VCN::AspectRatio( float a, float b )
{
  float r = a / b;
  if (r < 1.0f)
    r = 1.0f / r;

  return r;
}


////////////////////////////////////////////////////////////////////////
///
/// Returns true of @a path exists.
///
const VCNBool VCN::FileExists(const VCNString& path)
{
#ifdef UNICODE
  std::ifstream file;
#else
  std::wifstream file;
#endif
  file.open( path.c_str() );
  const bool fail = file.fail();
  if ( !fail ) file.close();
  return !fail;
}


//////////////////////////////////////////////////////////////////////////
///
/// Returns the nearest power of two.
/// 
/// @param value   [IN/OUT]    next power of two to be evaluated.
///
/// @return next power of two.
///
const unsigned int VCN::NearestPowerOfTwo(unsigned int value)
{
  unsigned int Temp = value;
  unsigned int PowerOfTwo = 0;

  while (Temp > 1)
  {
    Temp >>= 1;
    ++PowerOfTwo;
  }

  unsigned int Retval = 1 << PowerOfTwo;

  return Retval == value ? Retval : Retval << 1;
}

///////////////////////////////////////////////////////////////////////
std::string VCN::StripClassAndStruct(const std::string& tostrip)
{
	VCNString ret = StringUtils::StringToWString(tostrip);
	StringUtils::FindAndReplace(ret, L"class ", L"");
	StringUtils::FindAndReplace(ret, L"struct ", L"");
	return StringUtils::WStringToString(ret);
}
