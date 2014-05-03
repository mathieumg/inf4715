///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Custom Assertion implementation
///

#include "Precompiled.h"
#include "Assert.h"

#include "VCNUtils/Macros.h"
#include "VCNUtils/StackWalker.h"
#include "VCNUtils/StringUtils.h"
#include "VCNUtils/Utilities.h"

#include "../3rdparty/MessageBox/PPMessageBox.h"

#include <string>

extern HWND GLOBAL_WINDOW_HANDLE;

static bool sExiting = false;

class AssertStackWalker : public StackWalker
{
public:
  AssertStackWalker() : StackWalker(), skipCounter(0) {}

  const std::string& GetTrace() const { return mStackTrace; }
  
  const std::string& GetModuleInfo() const { return mModulesInfo; }

protected:
  virtual void OnOutput(LPCSTR szText)
  { 
    // Do not print nothing
    mModulesInfo += szText;
  }

  virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry)
  {
    // Skip the first two functions on the stack cause they are part of the stack print methods
    if (skipCounter++ < 2)
      return;

    // This flag indicates if some info was pushed to the stack trace 
    // string. if not, no need to add a line return for this entry.
    bool hasInfo = false;

    if ( strlen(entry.moduleName) > 0 )
    {
      mStackTrace += "[";
      mStackTrace += entry.moduleName;
      mStackTrace += "!";
      mStackTrace += entry.undFullName;
      mStackTrace += "]";
      hasInfo = true;
    }
    if ( strlen(entry.lineFileName) > 0 )
    {
      mStackTrace += "\n";
      mStackTrace += entry.lineFileName;
      mStackTrace += std::string("(") + VCN::ToString(entry.lineNumber) + std::string(") ");
      hasInfo = true;
    }
    if ( hasInfo )
    {
      mStackTrace += "\r\n";
    }
  }

  int skipCounter;
  std::string mStackTrace;
  std::string mModulesInfo;
};

//////////////////////////////////////////////////////////////////////////
///
///  Custom assert function that pops a windows that demand the user what to do.
///  The windows print the expression being asserted and the user message. Then the
///  user can break, ignore or exit the application. The assertion is also copied to
///  the clipboard.
///
///  @param [in]       value : evaluation of the assertion, 0 or 1
///  @param [in]       expression : string literal of the expression being asserted
///  @param [in]       message : custom message explaining the assertion
///  @param [in]       line : line of the assertion
///  @param [in]       file : source file of the assertion
///  @param [in, out]  ignored : flag indicating to ignore the assertion now or later
///
///  @return @li True => Assert
///          @li False => Do not assert
///
///  @remarks Mostly useful when used with a macro (See @a GTL_ASSERT)
///
//////////////////////////////////////////////////////////////////////////
const bool CustomAssert(
  int value, const wchar_t* expression, const wchar_t* message, int line, const char* _file, bool& ignored)
{
  if (value || ignored || sExiting)
    return false;

  static wchar_t msg[4096];

  HWND parentWindow = ::GetDesktopWindow();
  if ( ::IsWindow(GLOBAL_WINDOW_HANDLE) )
  {
    parentWindow = GLOBAL_WINDOW_HANDLE;
  }

  AssertStackWalker sw;
  sw.ShowCallstack();

  // Make sure the cursor is visible and not captured
  ReleaseCapture();
  while( ShowCursor(TRUE) < 0 )
    ;

  VCNUnicodeString file = VCN_A2W( _file );
  VCNUnicodeString truncatedFile = file;
  truncatedFile.erase( 0, truncatedFile.find_last_of( '\\' ) + 1 );
  
  _snwprintf(msg, STATIC_ARRAY_COUNT(msg), 
    L"==============================================================\n"
    L"Assertion:\n\n"
    L"Message: %s\n\n"
    L"Expression: %s\n\n"
    L"Source:\n%s (%d)\n\n"
    L"Stack Trace:\n%s\n\n"
    L"==============================================================\n", 
      message, expression, file.c_str(), line, VCN_A2W(sw.GetTrace()));

  TRACE(msg);

  // Copy the assertion message to the clipboard
  if( OpenClipboard( NULL ) )
  {
    HGLOBAL hMem;
    char *pMem;

    hMem = GlobalAlloc( GHND|GMEM_DDESHARE, _tcslen( msg )+1 );

    if( hMem ) {
      pMem = (char*)GlobalLock( hMem );
      strcpy( pMem, VCN_W2A(msg) );
      GlobalUnlock( hMem );
      EmptyClipboard();
      SetClipboardData( CF_TEXT, hMem );
    }

    CloseClipboard();
  }

  if ( _tcslen(message) > 0 )
  {
    _snwprintf(msg, STATIC_ARRAY_COUNT(msg), 
      L"<font size='24' face='Courrier New'>"
      L"%s\n\n"
      L"</font>"
      L"<font size='18' face='Courrier New'>"
      L"Expression: <font color='red'>%s</font>\n\n"
      L"Source: <a href='file://%s'>%s</a> (%d)\n\n"
      L"Do you want to break?\n"
      L"</font>", message, expression, file.c_str(), truncatedFile.c_str(), line);
  }
  else
  {
    _snwprintf(msg, STATIC_ARRAY_COUNT(msg), 
      L"<font size='18' face='Courrier New'>"
      L"Expression: <font color='red'>%s</font>\n\n"
      L"Source: <a href='file://%s'>%s</a> (%d)\n\n"
      L"Do you want to break?\n"
      L"</font>", expression, file.c_str(), truncatedFile.c_str(), line);
  }

  VCNString moreInfo = VCN_ASCII_TO_TCHAR( sw.GetTrace() );

  PPMSGBOXPARAMS msgParams;
  msgParams.lpszModuleName = message;
  msgParams.nLine = line;
  msgParams.lpszCustomButtons = _T("Break\nIgnore\nIgnore always\nExit");
  msgParams.lpszMoreInfo = moreInfo.c_str();

  int result = PPMessageBox(parentWindow, msg, _T("Vicuna Assertion!"), 
    MB_ICONWARNING | MB_YESNOCANCEL | MB_DEFBUTTON2 | MB_NOSOUND | MB_TASKMODAL, &msgParams);

  //int result = ::MessageBox(parentWindow, assertion, "Assertion!", MB_ICONERROR | MB_YESNOCANCEL);

  if (result == IDCUSTOM1)
  {
    return true;
  }
  else if (result == IDCUSTOM2)
  {
     return false;
  }
  else if (result == IDCUSTOM3)
  {
    ignored = true;
    return false;
  }
  else if (result == IDCUSTOM4)
  {
    try
    {
      sExiting = true;

      // Hard exit! No life shall exist beyond this point!
      ::TerminateProcess( ::GetCurrentProcess(), -1 );
    }
    catch (...)
    {
      
    }
  }
  
  return true;
}

///////////////////////////////////////////////////////////////////////
const bool CustomAssert(int value, const wchar_t* expression, int line, const char* file, bool& ignored, const char* format, ...)
{
  va_list ap;

  va_start(ap, format);

  char message[512];
  _vsnprintf (message, STATIC_ARRAY_COUNT(message), format, ap);

  wchar_t messageWideChar[512];
  ::MultiByteToWideChar(CP_ACP, MB_COMPOSITE, message, sizeof(message), messageWideChar, STATIC_ARRAY_COUNT(message));
  bool ret = CustomAssert(value, expression, messageWideChar, line, file, ignored);

  va_end(ap);

  return ret;
}

///////////////////////////////////////////////////////////////////////
const bool CustomAssert(int value, const wchar_t* expression, int line, const char* file, bool& ignored, const wchar_t* format, ...)
{
  va_list ap;

  va_start(ap, format);

  wchar_t message[512];
  _vsnwprintf  (message, STATIC_ARRAY_COUNT(message), format, ap);
  bool ret = CustomAssert(value, expression, message, line, file, ignored);

  va_end(ap);

  return ret;
}

/* ASSERT UNIT TESTS

  VCN_ASSERT( true );
  VCN_ASSERT( false );
  VCN_ASSERT( false && "ANSI" );
  VCN_ASSERT( false && L"UNICODE" );
  VCN_ASSERT_MSG( true, "TRUE format ANSI (%d) ", 42 );
  VCN_ASSERT_MSG( false, "FALSE format ANSI (%d) ", 42 );
  VCN_ASSERT_MSG( true, L"TRUE format UNICODE (%d) ", 42 );
  VCN_ASSERT_MSG( false, L"FALSE format UNICODE (%d) ", 42 );
  VCN_ASSERT_MSG( false, "ANSI with ANSI (%s) ", "BAM ANSI!" );
  VCN_ASSERT_MSG( false, "ANSI with UNICODE (%s) ", L"BAM UNICODE!" );
  VCN_ASSERT_MSG( false, L"UNICODE with ANSI (%s) ", "BAM ANSI!" );
  VCN_ASSERT_MSG( false, L"UNICODE with UNICODE (%s) ", L"BAM UNICODE!" );
  VCN_ASSERT_FAIL( "FAIL AINSI" );
  VCN_ASSERT_FAIL( L"FAIL UNICODE" );

*/
