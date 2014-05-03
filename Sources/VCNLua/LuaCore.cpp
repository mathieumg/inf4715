///
/// Copyright (C) 2014 - All Rights Reserved
/// All rights reserved. Mathieu M-Gosselin
/// 
/// @brief Dispatch calls to the Lua implementation VCNLuaCoreImpl
///

#include "Precompiled.h"
#include "LuaCore.h"

#include "VCNUtils/Assert.h"

#include <Windows.h>
#include <iostream>
#include <sstream>

/// Used temporarily to output to the VS debug console.
#define DBOUT( s )            \
{                             \
	std::wostringstream os_;    \
	os_ << s;                   \
	OutputDebugStringW(os_.str().c_str());  \
}


extern "C" {
	/* TO BE REMOVED: Example of a C++ call from within the loaded Lua script, for example
					  to start a cut scene, play a sound, etc.
	static int l_cppfunction(lua_State *L) {
		double arg = luaL_checknumber(L, 1);
		lua_pushnumber(L, arg * 0.5);
		return 1;
	}
	*/
}


VCN_TYPE(VCNLuaCore, VCNScriptingCore);

class VCNLuaCoreImpl
{
public:

	/// Called at startup
	VCNBool Initialize()
	{
		DBOUT("INITALIZING LUA" << std::endl);
		
		L = luaL_newstate();
		luaL_openlibs(L);

		return true;
	}

	/// Called when the core gets destroyed.
	VCNBool Uninitialize()
	{
		lua_close(L);
		
		return true;
	}

	/// Called once every frame
	///TODO: Use in some fashion to warn the current Lua script about time progression,
	/// has implications for time-sensitive puzzles.
	VCNBool Process(const float elapsedTime)
	{
		return true;
	}

	/// Loads a .lua script to memory, keeping track of state.
	VCNBool loadScript(std::string scriptName)
	{
		std::string scriptPath = "Scripts/" + scriptName + ".lua";

		/// Loads the Lua chunk.
		if (luaL_loadfile(L, scriptPath.c_str())) {
			std::stringstream error;
			error << "Something went wrong loading the script: " << lua_tostring(L, -1);
			VCN_ASSERT_FAIL(error.str().c_str());
			lua_pop(L, 1);
			return false;
		}

		/// Executes the Lua chunk.
		if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
			std::stringstream error;
			error << "Something went wrong executing the script: " << lua_tostring(L, -1);
			VCN_ASSERT_FAIL(error.str().c_str());
			lua_pop(L, 1);
			return false;
		}
		
		return true;
	}

	// Calls a Lua function (it doesn't have to exist in the loaded script) with the specified name.
	std::string executeLuaFunction(std::string functionName) {
		lua_getglobal(L, functionName.c_str());
		lua_pcall(L, 0, 1, 0); // Expecting a return for now, might not need that...
		std::string ret(lua_tostring(L, -1));
		DBOUT("The return value of the function was " << lua_tostring(L, -1) << std::endl);
		lua_pop(L, 1);
		return ret;
	}

	// Calls a Lua function (it doesn't have to exist in the loaded script) with the specified name.
	void executeLuaFunction(std::string functionName, VCNInt arg0) {
		lua_getglobal(L, functionName.c_str());
		lua_pushnumber(L, arg0);
		lua_pcall(L, 1, 0, 0);
	}

	/// Calls a Lua function (it doesn't have to exist in the loaded script) with a specially formatted name.
	std::string trigger(std::string triggerName, std::string triggerType) {
		std::transform(triggerName.begin(), triggerName.end(), triggerName.begin(), ::toupper);
		return executeLuaFunction("on" + triggerName + "_" + triggerType);
	}

	void trigger(std::string triggerName, std::string triggerType, VCNInt arg0)
	{
		std::transform(triggerName.begin(), triggerName.end(), triggerName.begin(), ::toupper);
		executeLuaFunction("on" + triggerName + "_" + triggerType, arg0);
	}

	/// Helper methods.
	void zoneTriggerEnter(std::string triggerName) {
		trigger(triggerName, "enter");
	}

	void zoneTriggerLeave(std::string triggerName) {
		trigger(triggerName, "leave");
	}

	void zoneTriggerMove(std::string triggerName) {
		trigger(triggerName, "move");
	}

	void switchTriggerOn(std::string triggerName) {
		trigger(triggerName, "on");
		trigger(triggerName, "toggle");
	}

	void switchTriggerOff(std::string triggerName) {
		trigger(triggerName, "off");
		trigger(triggerName, "toggle");
	}

	void* getState() {
		return L;
	}

	/* TO BE REMOVED, ULTIMATELY.
	void tempTest() {
		DBOUT("** Make a insert a global var into Lua from C++" << std::endl);
		lua_pushnumber(L, 1.1);
		lua_setglobal(L, "cppvar");

		DBOUT("** Read a global var from Lua into C++" << std::endl);
		lua_getglobal(L, "luavar");
		double luavar = lua_tonumber(L, -1);
		lua_pop(L, 1);
		DBOUT("C++ can read the value set from Lua luavar = " << luavar << std::endl);

		DBOUT("** Execute a Lua function from C++" << std::endl);
		lua_getglobal(L, "myluafunction");
		lua_pushnumber(L, 5);
		lua_pcall(L, 1, 1, 0);
		DBOUT("The return value of the function was " << lua_tostring(L, -1) << std::endl);
		lua_pop(L, 1);

		DBOUT("** Execute a C++ function from Lua" << std::endl);
		DBOUT("**** First register the function in Lua" << std::endl);
		lua_pushcfunction(L, l_cppfunction);
		lua_setglobal(L, "cppfunction");

		DBOUT("**** Call a Lua function that uses the C++ function" << std::endl);
		lua_getglobal(L, "myfunction");
		lua_pushnumber(L, 5);
		lua_pcall(L, 1, 1, 0);
		DBOUT("The return value of the function was " << lua_tonumber(L, -1) << std::endl);
		lua_pop(L, 1);
	}
	*/

protected:


private:

	lua_State *L;
};

//////////////////////////////////////////////////////////////////////////
VCNLuaCore::VCNLuaCore()
: mImpl(new VCNLuaCoreImpl())
{
}

//////////////////////////////////////////////////////////////////////////
VCNLuaCore::~VCNLuaCore()
{
}

//////////////////////////////////////////////////////////////////////////
VCNBool VCNLuaCore::Initialize()
{
  return mImpl->Initialize();
}

//////////////////////////////////////////////////////////////////////////
VCNBool VCNLuaCore::Uninitialize()
{
  // Shutdown the implementation
  return mImpl->Uninitialize();
}

//////////////////////////////////////////////////////////////////////////
VCNBool VCNLuaCore::Process(const float elapsedTime)
{
  return mImpl->Process( elapsedTime );
}

VCNBool VCNLuaCore::loadScript(std::string scriptName)
{
	return mImpl->loadScript(scriptName);
}

std::string VCNLuaCore::trigger(std::string triggerName, std::string triggerType)
{
    return mImpl->trigger(triggerName, triggerType);
}

void VCNLuaCore::trigger(std::string triggerName, std::string triggerType, VCNInt arg0)
{
    return mImpl->trigger(triggerName, triggerType, arg0);
}

void VCNLuaCore::zoneTriggerEnter(std::string triggerName)
{
	mImpl->zoneTriggerEnter(triggerName);
}

void VCNLuaCore::zoneTriggerLeave(std::string triggerName) {
	mImpl->zoneTriggerLeave(triggerName);
}

void VCNLuaCore::zoneTriggerMove(std::string triggerName) {
	mImpl->zoneTriggerMove(triggerName);
}

void VCNLuaCore::switchTriggerOn(std::string triggerName) {
	mImpl->switchTriggerOn(triggerName);
}

void VCNLuaCore::switchTriggerOff(std::string triggerName) {
	mImpl->switchTriggerOff(triggerName);
}

//FIXME: Only exposing LUA state to facilitate debugging... I think!
void* VCNLuaCore::getState() {
	return mImpl->getState();
}