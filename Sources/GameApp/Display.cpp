///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Display monitor implementation
///

#include "Precompiled.h"
#include "Display.h"

//////////////////////////////////////////////////////////////////////////
///
///  Default constructor
///
///  This function doesn't return a value
///
//////////////////////////////////////////////////////////////////////////
Display::Display(void)
: mIsFullscreen(false)
{
}



//////////////////////////////////////////////////////////////////////////
///
///  Default destructor. If we are in fullscreen we reset the resolution to default.
///
///  This function doesn't return a value
///
//////////////////////////////////////////////////////////////////////////
Display::~Display(void)
{
  // If we were in Fullscreen mode lets reset the last resolution (desktop res.).
  if (mIsFullscreen)
  {
    Reset();
  }
}



////////////////////////////////////////////////////////////////////////
///
///  Switch the display to Fullscreen mode.
///
///  @param [in]       width : The width in pixel to take.
///  @param [in]       height : The height in pixel to take.
///  @param [in]       bitsPerPixel : The nb. of bits per pixel to take.
///
///  @return  @li TRUE => Success
///           @li FALSE => Failure
///
////////////////////////////////////////////////////////////////////////
BOOL Display::SwitchFullscreen(DWORD width, DWORD height, DWORD bitsPerPixel)
{
  if (mIsFullscreen==false)  // Attempt Fullscreen Mode?
  {
    DEVMODE dmScreenSettings;  // Device Mode
    // Makes Sure Memory's Cleared
    memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
    // Size Of The Devmode Structure
    dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth  = width;                              // Selected Screen Width
    dmScreenSettings.dmPelsHeight  = height;                              // Selected Screen Height
    dmScreenSettings.dmBitsPerPel  = bitsPerPixel;                       // Selected Bits Per Pixel
    dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // Try To Set Selected Mode And Get Results.  
    // NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
    if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
    {
      // If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
      if (MessageBoxA(NULL,
        "The Requested Fullscreen Mode Is Not Supported By" \
        "\nYour Video Card. Use Windowed Mode Instead?", "Fullscreen mode",
        MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
      {
        mIsFullscreen=false;  // Windowed Mode Selected
      }
      else
      {
        // Pop Up A Message Box Letting User Know The Program Is Closing.
        MessageBoxA(NULL,"Program Will Now Close.", "ERROR",MB_OK|MB_ICONSTOP);
        return FALSE;
      }
    }
    else
    {
      mIsFullscreen = true;
    }
  }

  return TRUE;
}



////////////////////////////////////////////////////////////////////////
///
///  Reset the screen resolution to default.
///
///  @return nothing
///
////////////////////////////////////////////////////////////////////////
void Display::Reset()
{
  ChangeDisplaySettings(NULL,0);
  mIsFullscreen = false;
}
