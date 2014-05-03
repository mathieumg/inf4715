///
/// Copyright (C) 2014 - All Rights Reserved
/// All rights reserved. Mathieu M-Gosselin
/// 
/// @brief Declares the Lua engine manager interface.
///

#ifndef VCNLUACORE_H 
#define VCNLUACORE_H

#pragma once

#include "VCNScripting/ScriptingCore.h"



// VCN forward declarations
class VCNLuaCoreImpl;

class VCNLuaCore : public VCNScriptingCore
{
	VCN_CLASS;

public:

	/// Default constructor.
	VCNLuaCore();

	/// Default constructor.
	virtual ~VCNLuaCore();

	// VCNCore Interface

	/// Called at startup
	virtual VCNBool Initialize() override;

	/// Called when the core gets destroyed.
	virtual VCNBool Uninitialize() override;

	/// Called once every frame
	virtual VCNBool Process(const float elapsedTime) override;

	// VCNScriptingCore Interface

	virtual VCNBool loadScript(std::string scriptName)  override;

    virtual std::string trigger(std::string triggerName, std::string triggerType) override;

	virtual void trigger(std::string triggerName, std::string triggerType, VCNInt arg0) override;

	virtual void zoneTriggerEnter(std::string triggerName) override;

	virtual void zoneTriggerLeave(std::string triggerName) override;

	virtual void zoneTriggerMove(std::string triggerName) override;

	virtual void switchTriggerOn(std::string triggerName) override;

	virtual void switchTriggerOff(std::string triggerName) override;

	virtual void* getState() override;

private:

	VCNLuaCoreImpl* mImpl;

};

#endif // VCNLUACORE_H
