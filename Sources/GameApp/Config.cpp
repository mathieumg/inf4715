///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// Implements the configuration class.
///

#include "Precompiled.h"
#include "Config.h"

// Engine includes
#include "VCNUtils/StringUtils.h"
#include "VCNUtils/Utilities.h"
#include "VCNUtils/Assert.h"
#include "VCNUtils/Macros.h"

const VCNTChar* Config::kFile = VCNTXT("config.xml");

//////////////////////////////////////////////////////////////////////////////
///  Default constructor.
//////////////////////////////////////////////////////////////////////////////
Config::Config()
{
}


//////////////////////////////////////////////////////////////////////////////
///
///  Destructor.
//
Config::~Config( void )
{
  for_each(Constants.begin(), Constants.end(), DeletePointer());
}


//////////////////////////////////////////////////////////////////////////////
///
/// Pushes a new constant.
///
/// @param  name  The name.
/// @param  type  The type.
/// @param  value The value.
///               
void Config::PushConstant(const VCNString& name, const VCNString& type, const VCNString& value)
{
  Constant* cst = new Constant();

  Constant::Type    cstType;
  Constant::Generic cstValue;
  VCNString         cstText;

  if (type == TEXT("bool")) { cstType = Constant::TYPE_BOOLEAN; }
  else if (type == TEXT("integer")) { cstType = Constant::TYPE_INTEGER; }
  else if (type == TEXT("float")) { cstType = Constant::TYPE_FLOAT; }
  else if (type == TEXT("vector3")) { cstType = Constant::TYPE_VECTOR3; }
  else if (type == TEXT("string")) { cstType = Constant::TYPE_STRING; }
  else VCN_ASSERT_FAIL(TEXT("constant [%s] type is invalid with [%s]"), name.c_str(), type.c_str());

  VCNString copyValue = StringUtils::Trim(value);
  switch (cstType)
  {
  case Constant::TYPE_BOOLEAN:
    {
      cstValue.b = (copyValue == TEXT("true") || copyValue == TEXT("1"));
    }
    break;

  case Constant::TYPE_INTEGER:
    {
      // check if numeric
      int digit;
      VCNString tempValue = copyValue;
      StringUtils::FindAndReplace(tempValue, TEXT("-"), TEXT(""));

      for(unsigned i=0; i<tempValue.length(); i++)
      {
        digit = isdigit(tempValue[i]);
        if(digit==0)
        {
          VCN_ASSERT_FAIL(TEXT("[%s] must be a numeric value. Current value: [%s]"), tempValue.c_str(), value.c_str());
        }
      }
      cstValue.i = _tstoi(copyValue.c_str());

    }
    break;

  case Constant::TYPE_FLOAT:
    {
      //check if numeric
      int digit;
      VCNString tempValue = copyValue;
      StringUtils::FindAndReplace(tempValue, TEXT("."), TEXT(""));
      StringUtils::FindAndReplace(tempValue, TEXT("f"), TEXT(""));
      StringUtils::FindAndReplace(tempValue, TEXT(" "), TEXT(""));
      StringUtils::FindAndReplace(tempValue, TEXT("-"), TEXT(""));

      for(unsigned i=0; i<tempValue.length(); i++)
      {
        digit = isdigit(tempValue[i]);
        if(digit == 0)
        {
          VCN_ASSERT_FAIL(TEXT("[%s] must be a numeric value. Current value: [%s]"), tempValue.c_str(), value.c_str());
        }
      }

      cstValue.f = (float)_tstof(copyValue.c_str());
    }
    break;

  case Constant::TYPE_VECTOR3:
    {
      VCNString tempValue;
      int digit;

      StringUtils::FindAndReplace(copyValue, TEXT(","), TEXT(" "));
      StringUtils::FindAndReplace(copyValue, TEXT("f"), TEXT(""));

      std::vector<VCNString> tokens = StringUtils::Split(copyValue, TEXT(" "));
      VCN_ASSERT_MSG(tokens.size() == 3, TEXT("[%s] vector3 type must have 3 floats (we have[%s])"), name.c_str(), copyValue.c_str());

      //check if numeric
      tempValue = copyValue;
      StringUtils::FindAndReplace(tempValue, TEXT("."), TEXT(""));
      StringUtils::FindAndReplace(tempValue, TEXT(" "), TEXT(""));
      StringUtils::FindAndReplace(tempValue, TEXT("-"), TEXT(""));

      for(unsigned i=0; i<tempValue.length(); i++)
      {
        digit = isdigit(tempValue[i]);
        if(digit==0)
        {
          VCN_ASSERT_FAIL(TEXT("[%s] must be a numeric value. Current value: [%s]"), tempValue.c_str(), value.c_str());
        }
      }

      cstValue.v[0] = (float)_tstof(tokens[0].c_str());
      cstValue.v[1] = (float)_tstof(tokens[1].c_str());
      cstValue.v[2] = (float)_tstof(tokens[2].c_str());
    }
    break;

  case Constant::TYPE_STRING:
    {
      cstText = value;
    }
    break;

  default:
    {
      VCN_ASSERT_FAIL(TEXT("constant type invalid"));
    }
    break;
  }

  cst->name = name;
  cst->type = cstType;
  cst->value = cstValue;
  cst->text = cstText;

  Constants.push_back(cst);
  CstMap[name] = cst;

  // Parse keywords
  std::vector<VCNString> tokens = StringUtils::Split(name, TEXT("."));
  for (size_t i = 0; i < tokens.size(); ++i)
  {
    CstKeywords[tokens[i]].push_back(cst);
  }
}



//////////////////////////////////////////////////////////////////////////////
///
/// Gets a bool.
///
/// @param  name The name.
///
/// @return true if it succeeds, false if it fails.
///
const bool Config::GetBool( const std::wstring& name )
{
  Constant* cst = CstMap[name];
  VCN_ASSERT_MSG(cst, _T("Config Bool not found [%s]"), name.c_str() ); 
  VCN_ASSERT(cst->type == Constant::TYPE_BOOLEAN); 
  return cst->value.b;
}



//////////////////////////////////////////////////////////////////////////////
/// 
/// Gets a bool.
///
/// @param  obj  The object.
/// @param  attr The attribute.
///
/// @return true if it succeeds, false if it fails.
///         
const bool Config::GetBool(const std::wstring& obj, const std::wstring& attr)
{
  const ConstantList& l = CstKeywords[obj];

  for (size_t i = 0; i < l.size(); ++i)
  {
    std::wstring fullname = obj + TEXT(".") + attr;
    if (fullname == l[i]->name)
      return l[i]->value.b;
  }

  VCN_ASSERT_FAIL(TEXT("value for obj:%s attr:%s not found"), obj.c_str(), attr.c_str());
  return false;
}



////////////////////////////////////////////////////////////////////////
///
/// Returns the float value associated with the constant name.
/// 
/// @param name   [IN]    constant name
///
/// @return constant value
///
const float Config::GetFloat(const std::wstring& name)
{
  Constant* cst = CstMap[name];
  VCN_ASSERT(cst->type == Constant::TYPE_FLOAT); 
  return cst->value.f;
}



//////////////////////////////////////////////////////////////////////////
const float Config::GetFloat(const std::wstring& obj, const std::wstring& attr)
{
  const ConstantList& l = CstKeywords[obj];

  for (size_t i = 0; i < l.size(); ++i)
  {
    std::wstring fullname = obj + TEXT(".") + attr;
    if (fullname == l[i]->name)
      return l[i]->value.f;
  }

  VCN_ASSERT_FAIL(TEXT("value for obj:%s attr:%s not found"), obj.c_str(), attr.c_str());
  return 0;
}



//////////////////////////////////////////////////////////////////////////
const Vector3 Config::GetVector3( const std::wstring& name )
{
  Constant* cst = CstMap[name];
  VCN_ASSERT(cst->type == Constant::TYPE_VECTOR3); 
  return Vector3(cst->value.v);
}



//////////////////////////////////////////////////////////////////////////
const Vector3 Config::GetVector3( const std::wstring& obj, const std::wstring& attr )
{
  const ConstantList& l = CstKeywords[obj];

  for (size_t i = 0; i < l.size(); ++i)
  {
    std::wstring fullname = obj + TEXT(".") + attr;
    if (fullname == l[i]->name)
      return Vector3(l[i]->value.v);
  }

  VCN_ASSERT_FAIL(TEXT("value for obj:%s attr:%s not found"), obj.c_str(), attr.c_str());
  return Vector3();
}



//////////////////////////////////////////////////////////////////////////
const int Config::GetInt( const std::wstring& name )
{
  Constant* cst = CstMap[name];
  VCN_ASSERT(cst->type == Constant::TYPE_INTEGER); 
  return cst->value.i;
}



//////////////////////////////////////////////////////////////////////////
const int Config::GetInt( const std::wstring& obj, const std::wstring& attr )
{
  const ConstantList& l = CstKeywords[obj];

  for (size_t i = 0; i < l.size(); ++i)
  {
    std::wstring fullname = obj + TEXT(".") + attr;
    if (fullname == l[i]->name)
      return l[i]->value.i;
  }

  VCN_ASSERT_FAIL(TEXT("value for obj:%s attr:%s not found"), obj.c_str(), attr.c_str());
  return 0;
}



//////////////////////////////////////////////////////////////////////////
const VCNString Config::GetString( const std::wstring& name )
{
  Constant* cst = CstMap[name];
  VCN_ASSERT(cst->type == Constant::TYPE_INTEGER); 
  return cst->text;
}



//////////////////////////////////////////////////////////////////////////
const VCNString Config::GetString( const std::wstring& obj, const std::wstring& attr )
{
  const ConstantList& l = CstKeywords[obj];

  for (size_t i = 0; i < l.size(); ++i)
  {
    std::wstring fullname = obj + TEXT(".") + attr;
    if (fullname == l[i]->name)
      return l[i]->text;
  }

  VCN_ASSERT_FAIL(TEXT("value for obj:%s attr:%s not found"), obj.c_str(), attr.c_str());
  return obj;
}
