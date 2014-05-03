///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "Log.h"

#include "LogCore.h"

// Create the main instance logger, just like "cout"
VCNLogCore VCNLogInstance;
ILogManager& VCNLog = VCNLogInstance;
ILogManager& vcnlog = VCNLogInstance;

// Standard init of log procs
void VCNLogInit(std::string const& filename)
{
  if (!VCNLog.ContainsLogProc(kLogProcStdOut))       VCNLog.AddLogProc( new VCNLogProcStdOut() );
  if (!VCNLog.ContainsLogProc(kLogProcStdErr))       VCNLog.AddLogProc( new VCNLogProcStdErr() );
  if (!VCNLog.ContainsLogProc(kLogProcVisualStudio)) VCNLog.AddLogProc( new VCNLogProcVisualStudio() );

  if ( !filename.empty() ) 
  {
    if (!VCNLog.ContainsLogProc(kLogProcFile)) VCNLog.AddLogProc( new VCNLogProcFile(filename.c_str()) );
  }

  // Don't output to stderr except when specified (prevents double output)
  VCNLog.SetDefaultProc(static_cast<VCNLogProcType>(~static_cast<int>( (kLogProcAll & kLogProcStdErr) )));	
}
