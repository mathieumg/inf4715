///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "LogProc.h"

#include <ctime>

using namespace std;

#if defined( _UNICODE )
#define COUT std::wcout
#define CERR std::wcerr
#else
#define COUT std::cout
#define CERR std::cerr
#endif

const VCNTChar* MsgTypeString(const VCNMsgType& in_MsgType)
{
  if      (in_MsgType == kInfo)    {return VCNTXT("[Info] ");}
  else if (in_MsgType == kWarning) {return VCNTXT("[Warning] ");}
  else if (in_MsgType == kError)   {return VCNTXT("[Error] ");}
  else if (in_MsgType == kAssert)  {return VCNTXT("[Assert] ");}

  return L"";
}

const char* VCNLogProcStdOut::GetName() const { return "StdOut"; }

bool VCNLogProcStdOut::LogMessage(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage)
{
  COUT << MsgTypeString(in_MsgType) << in_pMessage;
  COUT.flush();
  return true;
}

const char* VCNLogProcStdErr::GetName() const { return "StdErr"; }

bool VCNLogProcStdErr::LogMessage(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage)
{
  CERR << MsgTypeString(in_MsgType) << in_pMessage;
  CERR.flush();
  return true;
}

const char* VCNLogProcFile::GetName() const { return m_fileName.c_str(); }

VCNLogProcFile::VCNLogProcFile() :
m_outfile()
{
}

VCNLogProcFile::VCNLogProcFile(const char* in_pFile) :
m_outfile()
{
  Openfile(in_pFile);
}


bool VCNLogProcFile::Openfile(const char* in_pFile)
{
  if( in_pFile == NULL )
    return false;

  m_fileName = in_pFile;
  m_outfile.open( in_pFile, std::ios::app | std::ios::out | std::ios::ate );
  if( m_outfile.is_open() )
  {
    // Log session header
    //
    // Use strftime to build a customized time string. 
    VCNTChar datebuf[128];
    time_t ltime;
    struct tm now;
    time( &ltime );
    _localtime64_s( &now, &ltime );
    _tcsftime( datebuf, sizeof(datebuf) / sizeof(VCNTChar), VCNTXT("Vicuna Session on %Y-%m-%d at %H:%M"), &now );

    m_outfile << VCNTXT("\n+=======================================+");
    m_outfile << VCNTXT("\n| ") << datebuf << VCNTXT(" |");
    m_outfile << VCNTXT("\n+=======================================+\n");
    return true;
  }
  return false;
}

VCNLogProcFile::~VCNLogProcFile()
{
  if( m_outfile.is_open() )
  {
    m_outfile.close();
  }
}

bool VCNLogProcFile::LogMessage(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage)
{
  if( m_outfile.is_open() )
  {
    m_outfile << MsgTypeString(in_MsgType) << in_pMessage;
    m_outfile.flush();
  }
  return true;
}

const char* VCNLogProcVisualStudio::GetName() const { return "VisualStudio"; }

bool VCNLogProcVisualStudio::LogMessage(const VCNMsgType& in_MsgType, const VCNTChar* in_pMessage)
{
  VCNStringStream str;
  str << MsgTypeString(in_MsgType) << in_pMessage;
  OutputDebugString( str.str().c_str() );

  return true;
}
