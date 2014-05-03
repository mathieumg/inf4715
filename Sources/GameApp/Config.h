///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#pragma once

// Engine includes
#include "VCNUtils/Singleton.h"
#include "VCNUtils/StringUtils.h"
#include "VCNUtils/Types.h"
#include "VCNUtils/Vector.h"

class Config : public Singleton<Config>
{

public:

  Config(void);
  virtual ~Config(void);

  // Config file
  static const VCNTChar* kFile;

  /// Adds a new constant to the database
  void PushConstant(const VCNString& name, const VCNString& type, const VCNString& value);

  /// Returns a constant boolean
  const bool GetBool(const std::string& name) { return GetBool(VCN_A2W(name)); }
  const bool GetBool(const std::wstring& name);
  const bool GetBool(const std::string& obj, const std::string& attr) { return GetBool(VCN_A2W(obj), VCN_A2W(attr)); }
  const bool GetBool(const std::wstring& obj, const std::wstring& attr);

  /// Returns a constant float
  const float GetFloat(const std::string& name) { return GetFloat(VCN_A2W(name)); }
  const float GetFloat(const std::wstring& name);
  const float GetFloat(const std::string& obj, const std::string& attr) { return GetFloat(VCN_A2W(obj), VCN_A2W(attr)); }
  const float GetFloat(const std::wstring& obj, const std::wstring& attr);

  // Returns a constant float
  const int GetInt(const std::string& name) { return GetInt(VCN_A2W(name)); }
  const int GetInt(const std::wstring& name);
  const int GetInt(const std::string& obj, const std::string& attr) { return GetInt(VCN_A2W(obj), VCN_A2W(attr)); }
  const int GetInt(const std::wstring& obj, const std::wstring& attr);

  /// Returns a constant vector of dimension 3
  const Vector3 GetVector3(const std::string& name) { return GetVector3(VCN_A2W(name)); }
  const Vector3 GetVector3(const std::wstring& name);
  const Vector3 GetVector3(const std::string& obj, const std::string& attr) { return GetVector3(VCN_A2W(obj), VCN_A2W(attr)); }
  const Vector3 GetVector3(const std::wstring& obj, const std::wstring& attr);

  // Returns a constant float
  const VCNString GetString(const std::string& name) { return GetString(VCN_A2W(name)); }
  const VCNString GetString(const std::wstring& name);
  const VCNString GetString(const std::string& obj, const std::string& attr) { return GetString(VCN_A2W(obj), VCN_A2W(attr)); }
  const VCNString GetString(const std::wstring& obj, const std::wstring& attr);

private:

  struct Constant 
  {
    enum Type {
      TYPE_BOOLEAN,
      TYPE_INTEGER,
      TYPE_FLOAT,
      TYPE_VECTOR3,
      TYPE_STRING
    };

    union Generic {
      bool    b;
      int     i;
      float   f;
      float   v[3];
    };

    VCNString name;
    Type      type;
    Generic   value;
    VCNString text;   // only for TYPE_STRING
  };

  typedef std::vector<Constant*> ConstantList;
  typedef std::map<VCNString, Constant*> ConstantMap;
  typedef std::map<VCNString, ConstantList> KeywordMap;

  ConstantList  Constants;
  ConstantMap   CstMap;
  KeywordMap    CstKeywords;
};

#define CST                     (&Config::GetInstance())
#define CST_BOOL( x )           Config::GetInstance().GetBool(x)
#define CST_FLOAT( x )          Config::GetInstance().GetFloat(x)
#define CST_INT( x )            Config::GetInstance().GetInt(x)
#define CST_V3( x )             Config::GetInstance().GetVector3(x)
#define CST_STR( x )            Config::GetInstance().GetString(x)
