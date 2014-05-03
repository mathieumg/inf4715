///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Display monitor interface
///

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

//////////////////////////////////////////////////////////////////////////
///
///  @class Display
///
///  @brief This class provide a display driver to change screen resolution.
///
//////////////////////////////////////////////////////////////////////////
class Display 
{

public:

// Constructor(s) / Destructor

    Display(void);
    ~Display(void);

// Attributes

    /// Checks if the screen is in full screen mode.
    bool IsFullscreen() const { return mIsFullscreen; }

// Operations

    /// Switch to fullscreen.
    BOOL SwitchFullscreen(DWORD width, DWORD height, DWORD bitsPerPixel);

    /// Reset screen resolution.
    void Reset();

private:

// Data members

    bool mIsFullscreen; ///< Is the display in Fullscreen mode
};

#endif // __DISPLAY_H__
