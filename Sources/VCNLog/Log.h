///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#ifndef VCNLOG_H
#define VCNLOG_H

#pragma once

#include "LogManager.h"
#include "LogProxy.h"

extern ILogManager& VCNLog;
extern ILogManager& vcnlog;

// Standard init of various log procs
void VCNLogInit(std::string const& filename);

#endif // VCNLOG_H
