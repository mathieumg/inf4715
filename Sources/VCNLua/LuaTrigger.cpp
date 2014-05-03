
#include "Precompiled.h"
#include "LuaTrigger.h"
#include <iterator>
#include <boost/algorithm/string.hpp>
#include "VCNScripting/ScriptingCore.h"



std::ostream& operator<<(std::ostream& s, const LuaTrigger& v)
{
	const wchar_t* triggerName = v.triggerName.c_str();
	const wchar_t* triggerType = v.triggerType.c_str();
	std::string str(v.triggerName.begin(), v.triggerName.end());
	std::string str2(v.triggerType.begin(), v.triggerType.end());
	const char* bla = str.c_str();
	const char* bla2 = str2.c_str();
	s << ";" << str << ";" << str2 << ";";
	return s;
}

std::istream &operator>>(std::istream &s, LuaTrigger& v)
{
	std::vector<std::string> tokens;
	std::string r;
	for(int i=0; i<3; i++)
	{
		std::getline(s,r,';');
		tokens.push_back(r);
	}

	if (tokens.size() > 2)
	{
		std::string nameToken = tokens[1];
		std::string typeToken = tokens[2];
		const char* nameToketTest = nameToken.c_str();
		const char* typeToketTest = typeToken.c_str();
		v.triggerName = std::wstring(nameToken.begin(), nameToken.end());
		v.triggerType = std::wstring(typeToken.begin(), typeToken.end());
	}
	else
	{
		v.triggerName = L"";
		v.triggerType = L"";
	}

	return s;
}


bool LuaTrigger::operator==( const LuaTrigger& other ) const
{
	return (triggerName == other.triggerName) && (triggerType == other.triggerType);
}

bool LuaTrigger::operator==( const LuaTrigger& other )
{
	return (triggerName == other.triggerName) && (triggerType == other.triggerType);
}

bool LuaTrigger::operator==( LuaTrigger& other )
{
	return (triggerName == other.triggerName) && (triggerType == other.triggerType);
}

void LuaTrigger::SetTriggerName( const std::string& val )
{
	triggerName = std::wstring(val.begin(), val.end());
}

void LuaTrigger::SetTriggerType( const std::string& val )
{
	triggerType = std::wstring(val.begin(), val.end());
}

bool LuaTrigger::Trigger()
{
	if (IsEmpty())
	{
		return false;
	}
	else
	{
		// If the lua functions returns a value, check if it's '0' == false. If it doesn't, everything went fine
		std::string ret = VCNScriptingCore::GetInstance()->trigger(std::string(triggerName.begin(), triggerName.end()), std::string(triggerType.begin(), triggerType.end()));
		return ret.length() == 0 ? (true) : (ret[0] != '0');
	}
}


void LuaTrigger::Trigger( VCNInt arg0 )
{
	if (IsEmpty())
	{
		return;
	}
	else
	{
		// If the lua functions returns a value, check if it's '0' == false. If it doesn't, everything went fine
		VCNScriptingCore::GetInstance()->trigger(std::string(triggerName.begin(), triggerName.end()), std::string(triggerType.begin(), triggerType.end()), arg0);
	}
}

void LuaTrigger::Trigger( const std::string& params )
{
	Trigger(); // TODO: Handle parameters
}

bool LuaTrigger::IsEmpty() const
{
	return (triggerName.length() == 0) && (triggerType .length() == 0);
}

