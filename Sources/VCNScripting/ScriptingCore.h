///
/// Copyright (C) 2014 - All Rights Reserved
/// All rights reserved. Mathieu M-Gosselin
///
/// Defines the scripting interface
/// 

#pragma once

#include <string.h>

#include "VCNCore/Core.h"
#include "VCNUtils/Vector.h"
#include "VCNUtils/Matrix.h"

class VCNScriptingCore : public VCNCore<VCNScriptingCore>
{
public:
  
  // Boom!
  virtual ~VCNScriptingCore() = 0;

  virtual VCNBool VCNScriptingCore::loadScript(std::string scriptName) = 0;

  virtual std::string VCNScriptingCore::trigger(std::string triggerName, std::string triggerType) = 0;

  virtual void VCNScriptingCore::trigger(std::string triggerName, std::string triggerType, VCNInt arg0) = 0;

  virtual void VCNScriptingCore::zoneTriggerEnter(std::string triggerName) = 0;

  virtual void VCNScriptingCore::zoneTriggerLeave(std::string triggerName) = 0;

  virtual void VCNScriptingCore::zoneTriggerMove(std::string triggerName) = 0;

  virtual void VCNScriptingCore::switchTriggerOn(std::string triggerName) = 0;

  virtual void VCNScriptingCore::switchTriggerOff(std::string triggerName) = 0;

  virtual void* VCNScriptingCore::getState() = 0;

protected:

  VCNScriptingCore();
};
