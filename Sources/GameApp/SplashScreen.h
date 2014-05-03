///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#if !defined(AFX_SPLASHSCREEN_H__44FBD1FF_4158_40A5_BB5C_8BB71EDD2EE7__INCLUDED_)
#define AFX_SPLASHSCREEN_H__44FBD1FF_4158_40A5_BB5C_8BB71EDD2EE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Config.h"

#include "VCNUtils/Types.h"

class SplashScreen  
{
public:
  SplashScreen( HWND parentWnd );
  virtual ~SplashScreen();

  static void ShowSplashScreen( HWND pParentWnd = NULL,  int IMAGE_ID = NULL, LPCTSTR statusMessage = NULL, int millisecondsToDisplay=0 );
  static void HideSplashScreen();

protected:
  BOOL Create( HWND pParentWnd=NULL, int IMAGE_ID=NULL);
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  void GetVersionStrings();
  void ReportError( LPCTSTR format, ...  );
  BOOL RegisterClass( LPCTSTR szWindowClassName );
  void ClearMessageQueue();

  HFONT CreatePointFont( int pointSize, LPCTSTR fontName, HDC dc);
  SIZE FindFontPointSize( HDC paintDC, LPCTSTR fontName, TCHAR **stringsToCheck, int numberOfStringsToCheck, SIZE maximumSize );
  void OnPaint( HWND hWnd );
  void DisplayProductName( HDC paintDC, int windowWidth, int windowHeight );
  void DisplayBody( HDC paintDC, int windowWidth, int windowHeight );
  void DisplayStatusLine( HDC paintDC, int windowWidth, int windowHeight );

private:
  HWND m_hDlg;
  HWND m_hParentWnd;
  HWND m_hWnd;
  HINSTANCE m_instance;
  static SplashScreen* m_pSplashWnd;
  static ATOM m_szWindowClass ;
  static BOOL m_useStderr;

  HBITMAP m_bitmap;

  static LPTSTR m_productNameString;
  static LPTSTR m_companyNameString;
  static LPTSTR m_versionNumberString;
  static LPTSTR m_versionString;
  static LPTSTR m_copyrightString;
  static LPTSTR m_commentsString;
  static LPTSTR m_statusMessage;

  static int m_millisecondsToDisplay;        // 0 ==> until mouse click or keystroke
  
  // create rectangle that product name has to fit in
  static const int m_productNameVerticalOffset;    // empty space between top 3-D border and product name
  static const int m_productNameVerticalHeight;    // empty space between bottom 3-D border and bottom of product name
  static const int m_productNameLeftMargin;    // distance from left side to place name, company, copyright and version
  static const int m_productNameRightMargin;    // distance from right side to place name, company, copyright and version
  static const LPCTSTR m_productNameFontName;    // name of font for application name
  static SIZE m_productNamePointSize;        // point size used for the application name  
  static COLORREF m_productNameTextColor;      // color used for text

  static const BOOL m_displayCompanyName;      // true if displaying companyName
  static const BOOL m_displayVersion;        // true if displaying version
  static const BOOL m_displayCopyright;      // true if displaying copyright
  static const BOOL m_displayComments;      // true if displaying comments

  // create rectangle that strings in body have to fit in
  static const int m_bodyVerticalOffset;        // empty space between top 3-D border and top of body
  static const int m_bodyVerticalHeight;      // empty space between bottom 3-D border and bottom of body
  static const int m_bodyLeftMargin;        // distance from left side to place company name, copyright, version and comment
  static const int m_bodyRightMargin;        // distance from right side to place company name, copyright, version and comment
  static const LPCTSTR m_bodyFontName;      // name of font for company name, copyright, version and comment  
  static SIZE m_bodyPointSize;          // point size used for company name, copyright, version and comment  
  static COLORREF m_bodyTextColor;        // color used for company name, copyright, version and comment

  static const int m_statusVerticalOffset;        // empty space between top 3-D border and top of status line
  static const int m_statusVerticalHeight;      // empty space between bottom 3-D border and bottom of status line
  static const int m_statusLeftMargin;      // distance from left side to status line
  static const int m_statusRightMargin;      // distance from right side to place status line
  static const LPCTSTR m_statusMessageFontName;  // name of font for status message while starting  
  static SIZE m_statusMessagePointSize;      // point size used for status message while starting  
  static COLORREF m_statusMessageTextColor;    // color used for status message while starting

};

/// Sets the label of the loading screen.
inline void ShowLoading(const VCNString& label)
{
  if ( CST_BOOL("WindowSettings.ShowSplashScreen") )
  {
    extern HWND GLOBAL_WINDOW_HANDLE;
    SplashScreen::ShowSplashScreen( GLOBAL_WINDOW_HANDLE, IDB_SPLASH, label.c_str(), 1000 );
  }
}

inline void HideLoading()
{
  SplashScreen::HideSplashScreen();
}

#endif // !defined(AFX_SPLASHSCREEN_H__44FBD1FF_4158_40A5_BB5C_8BB71EDD2EE7__INCLUDED_)
