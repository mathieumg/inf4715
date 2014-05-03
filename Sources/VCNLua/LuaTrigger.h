

#pragma once

#ifndef LUA_TRIGGER_H


#include "VCNUtils\Types.h"
#include <sstream>
#include <iostream>


// Name and type should not contain any spaces
class LuaTrigger
{
public:
	VCNString triggerName;
	VCNString triggerType;

	LuaTrigger()
	{
		triggerName = L"";
		triggerType = L"";
	}

	LuaTrigger(const VCNString& name, const VCNString& type)
	{
		triggerType = name;
		triggerType = type;
	}

	bool Trigger(); // Trigger without params
	void Trigger(VCNInt arg0); // Send an int to trigger.
	void Trigger(const std::string& params); // Trigger with params

	bool IsEmpty() const;

	bool operator==(const LuaTrigger& other) const;
	bool operator==(const LuaTrigger& other);
	bool operator==(LuaTrigger& other);

	//LuaTrigger& operator =(const LuaTrigger& b);

	void SetTriggerName(const std::string& val);
	void SetTriggerType(const std::string& val);

	friend std::ostream& operator<< (std::ostream &out, const LuaTrigger &trigger);
	friend std::istream& operator>> (std::istream &in, LuaTrigger &trigger);

};






#define LUA_TRIGGER_H
#endif
