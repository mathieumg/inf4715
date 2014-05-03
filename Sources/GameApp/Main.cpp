///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// Game main entry point
///

#include "Precompiled.h"

// Project includes
#include "Config.h"
#include "ConfigLoader.h"
#include "Display.h"
#include "Game.h"
#include "InputManager.h"
#include "SplashScreen.h"
#include "Timer.h"

// Engine includes
#include "VCNCore/CoreTimer.h"
#include "VCNUtils/Chrono.h"
#include "VCNUtils/Utilities.h"
#include "VCNUtils/Macros.h"

// Init the static variable
Display   gFullscrenDisplay;
HINSTANCE GLOBAL_APPLICATION_INSTANCE = NULL;
HWND      GLOBAL_WINDOW_HANDLE = NULL;
TCHAR     gWindowAppClass[] = TEXT("VICUNA_APP_CLASS");

/// Messages procedure callbacks.
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////////
/// 
/// Search for the best directory to be the current directory for content 
/// loading. We mostly search for Config.xml. We look in the current directory 
/// and in ../GameContentFolders/
///
/// @return return true if the current directory was found and set, 
///         otherwise false is returned.
///         
//////////////////////////////////////////////////////////////////////////////
const VCNBool SetupCurrentDirectory()
{
	Config& cfg = Config::GetInstance();
	const VCNString configFile = Config::kFile;

	// Check in the working directory.
	if ( VCN::FileExists( configFile ) )
	{
		// The current directory is already correct.
		return true;
	}

	// Paths to try.
	std::vector<const VCNString> kPaths;
	kPaths.push_back( VCNTXT("..\\Data\\") );
	kPaths.push_back( VCNTXT("..\\..\\Data\\") );
	kPaths.push_back( VCNTXT("..\\..\\Game\\") );
	kPaths.push_back( VCNTXT("..\\..\\Game\\Data\\") );

	// Scan all specified paths for the right one.
	bool found = false;
	std::for_each(kPaths.begin(), kPaths.end(), [configFile, &found](const VCNString& path) 
	{
		// Look for other possibilities
		if ( VCN::FileExists(path + configFile) )
		{
			if ( ::SetCurrentDirectory( path.c_str() ) )
			{
				found = true;
			}
		}
	});

	if ( found )
		return true;

	VCN_ASSERT_FAIL( "Current directory not found." );

	return false;
}

////////////////////////////////////////////////////////////////////////
///
/// Called by windows to start application
/// 
/// @param hInstance  [IN] application main instance
/// @param cmdLine    [IN] command line arguments
///
/// @return application return code.
///
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR cmdLine, int) 
{
	// The main thread to always on the same core
	DWORD_PTR kCPUMask = 1;
	::SetThreadAffinityMask( ::GetCurrentThread(), kCPUMask );

	// Setup the working directory
	SetupCurrentDirectory();

	// Setup logger
	VCNLogInit( "vicuna.log" );

	// Load application configurations
	VCNBool configLoaded = ConfigLoader::LoadConfig( Config::kFile );
	VCN_ASSERT_MSG(configLoaded, _T("Can't load configuration"));

	if ( CST_BOOL("Random.Enabled") )
	{
		// Let this session be unique!
		// We don't want this in debug, so we can track performance in a 
		// more deterministic way.
		srand((unsigned int)time(0));
	}
	else
	{
		srand( CST_INT("Random.Seed") );
	}

	GLOBAL_APPLICATION_INSTANCE = hInstance;

	WNDCLASSEX  wndclass;
	MSG         msg = { 0, 0, 0 };

	HICON appIcon = (HICON)LoadImage(hInstance, // small class icon 
		MAKEINTRESOURCE(IDI_GAMEAPP_ICON),
		IMAGE_ICON, 
		GetSystemMetrics(SM_CXSMICON), 
		GetSystemMetrics(SM_CYSMICON), 
		LR_DEFAULTCOLOR); 

	// Set up window attributes
	wndclass.cbSize         = sizeof(wndclass);
	wndclass.style          = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc    = MsgProc;
	wndclass.cbClsExtra     = 0;
	wndclass.cbWndExtra     = 0;
	wndclass.hInstance      = hInstance;
	wndclass.hIcon          = appIcon;
	wndclass.hCursor        = LoadCursor( NULL, IDC_ARROW );
	wndclass.hbrBackground  = NULL;
	wndclass.lpszMenuName   = NULL;
	wndclass.lpszClassName  = gWindowAppClass;
	wndclass.hIconSm        = NULL; 

	if(RegisterClassEx(&wndclass) == 0)
		return 0;

	RECT desktopRect;
	HWND desktopWnd = ::GetDesktopWindow();
	::GetWindowRect(desktopWnd, &desktopRect);

	unsigned int windowWidth = CST_INT( "WindowSettings.Res.Width" );
	unsigned int windowHeight = CST_INT( "WindowSettings.Res.Height" );
	const bool isFullscreen = CST_BOOL( "WindowSettings.Res.Fullscreen" );
	const bool useDesktopResolution = CST_BOOL("WindowSettings.Res.UseDesktop");
	const bool windowCentered = CST_BOOL("WindowSettings.Res.Centered");

	if ( useDesktopResolution )
	{
		windowWidth = desktopRect.right;
		windowHeight = desktopRect.bottom;
	}

	// Switch to fullscreen
	if ( isFullscreen )
	{
		// Switch into fullscreen if requested.
		if (!gFullscrenDisplay.SwitchFullscreen(windowWidth, windowHeight, 32))
		{
			VCN_ASSERT_FAIL( "Failed to switch to fullscreen mode" );
			return -1;
		}
	}
	else if (gFullscrenDisplay.IsFullscreen())
	{
		gFullscrenDisplay.Reset();
	}

	ShowLoading( _T("Booting...") );

	RECT windowRect;
	windowRect.left    = (long)0;             
	windowRect.right   = (long)windowWidth;  
	windowRect.top     = (long)0;       
	windowRect.bottom  = (long)windowHeight;

	// Determine window style
	//
	DWORD WindowStyle = WS_OVERLAPPEDWINDOW;
	DWORD WindowExStyle = WS_EX_APPWINDOW;

	if ( isFullscreen )
	{
		WindowStyle = WS_POPUP;
#ifdef FINAL
		// Careful with breakpoints (move VS to another windows for debugging)
		WindowExStyle |= WS_EX_TOPMOST;
#endif
	}
	else
	{ // Not a Full screen display
		WindowStyle = WS_OVERLAPPEDWINDOW;
		WindowStyle &= ~WS_THICKFRAME; // Do not allow to resize window
		WindowExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	}
	WindowStyle |= (WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	WindowStyle &= ~WS_MAXIMIZEBOX;

	// Don't show everything yet
	WindowStyle &= ~WS_VISIBLE;

	// Adjust Window To True Requested Size 
	AdjustWindowRectEx(&windowRect, WindowStyle, FALSE, WindowExStyle);

	// Fix width and height
	windowWidth = (windowRect.right-windowRect.left);
	windowHeight = (windowRect.bottom-windowRect.top);

	// Get where to set the position of the window
	int windowX = isFullscreen ? 0 : GetSystemMetrics(SM_CXSCREEN)/2 - windowWidth/2;
	int windowY = isFullscreen ? 0 : GetSystemMetrics(SM_CYSCREEN)/2 - windowHeight/2;
	if ( !windowCentered && !isFullscreen )
	{
		windowX = CST_INT( "WindowSettings.Res.X" );
		windowY = CST_INT( "WindowSettings.Res.Y" );
	}

	// Create the main window centered on desktop.
	if (!(GLOBAL_WINDOW_HANDLE = CreateWindowEx(
		WindowExStyle, 
		gWindowAppClass,
		TEXT("Prison Escape"),
		WindowStyle,
		windowX, windowY,
		windowWidth, windowHeight,
		NULL, NULL, hInstance, NULL)))
	{
		return -1;
	}

	// Create our game and initialize it
	Game* game = &Game::CreateInstance();
	if( !game->Initialize() )
	{
		// TODO: Put message box support into VCNWindows module.
		MessageBox( GLOBAL_WINDOW_HANDLE, TEXT("Sorry, Game can't start."), TEXT("VICUNA MSG"), MB_OK | MB_ICONERROR );
		return -1;
	}

	// Show the game window
	//
	ShowWindow( GLOBAL_WINDOW_HANDLE, SW_SHOW );
	SetForegroundWindow( GLOBAL_WINDOW_HANDLE );

	// Define some timer used to track time
	//
	Timer loopTimer, appTimer;
	appTimer.Start();
	loopTimer.Start();

	// Main loop
	//
	bool running = true;
	while ( running )
	{
		// Process system and user inputs
		if ( ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD) )
		{
			CHRONO_AVERAGE( "IPT" );

			if ( msg.message == WM_QUIT )
			{
				running = false;
				break;
			}

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			CHRONO_AVERAGE( "FRM" );

			float frameTime = static_cast<float>(loopTimer.GetTime());
			loopTimer.Restart();

			// Fix frame time in case we were 
			// debugging or blocking the main thread
			if ( frameTime > 1.0f )
				frameTime = 1 / 30.0f;

			// Update frame timer info.
			VCNCoreTimer::GetInstance()->Tick( frameTime );

			// Update
			{
				CHRONO_AVERAGE( "CPU" );
				running &= game->Process( frameTime );
			}

			// Render
			{
				CHRONO_AVERAGE( "GPU" );
				running &= game->Render();
			}
		}
	}

	TRACE( VCNTXT("Total Time: %.1f s\n"), appTimer.GetTime() );

	// Release the modules
	game->Uninitialize();

	// cleanup stuff     
	UnregisterClass( gWindowAppClass, hInstance );

	return 0;
}

////////////////////////////////////////////////////////////////////////
///
/// Function to treat windows messages
/// 
/// @param hWnd   [IN] window handle
/// @param msg    [IN] window message id
/// @param wParam [IN] message parameters
/// @param lParam [IN] message parameters
///
/// @return message process code
///
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Send event message to AntTweakBar
	if( TwEventWin(hWnd, msg, wParam, lParam) )
		return 0;   // Event has been handled by AntTweakBar

	InputManager& inputMgr = InputManager::GetInstance();
	inputMgr.HandleEvent(hWnd, msg, wParam, lParam);

	switch(msg)
	{
	case WM_CREATE:
		inputMgr.SetInputWindow( hWnd );
		break;

	case WM_QUIT:
		{
			return 1;
		}
		break;

	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 1;
		}
		break;

	case WM_ACTIVATE:
		{
			// Release the mouse if the user changes the focus.
			if (wParam == WA_INACTIVE)
			{
                inputMgr.SetMouseCaptured(false);
				return 0;
            }
		}
		break;

		// Intercept System Commands
	case WM_SYSCOMMAND:
		{
			// Check System Calls
			switch (wParam)
			{
			case SC_SCREENSAVE:     // Screen saver Trying To Start?
			case SC_MONITORPOWER:   // Monitor Trying To Enter power save?
				return 0;             // Prevent From Happening
			}
			break;   // Exit
		}

	default: 
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}
