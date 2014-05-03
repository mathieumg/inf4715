//**************************************************************************/
// Copyright (c) 1998-2010 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Unwrap UI classes
// AUTHOR: Peter Watje
// DATE: 2010/08/31 
//***************************************************************************/

#pragma once

#include <windowsx.h>
#include "custcont.h"
#include "UnwrapCustomToolBars.h"

//This is just a class that Unwrap uses for it buttons outside the toolbar
 class UnwrapButton
 {
 public:
	 UnwrapButton(int id, HWND hwnd);
	 ~UnwrapButton();
	 ICustButton* ButtonPtr();
	 int ID();
 protected:
	 UnwrapButton();
	 ICustButton *mButton;  //the actual button interface
	 int mID;				//the id
 };

/*

This class manages all the Toolbar UI for the Unwrap modifer which includes UI in the dialog, side panel and modifier panel.
Popups are not handled.
*/

class UnwrapCustomUI
{
public:
	UnwrapCustomUI();
	~UnwrapCustomUI();

	void SetMod(UnwrapMod *mod, HINSTANCE instance);

	//any UI elements with has the parent equal to parentWindow will be freed.
	void Free( HWND parentWindow);

	//this will update the position of all the toolbars
	void UpdatePositions();

	//updates all our check boxes UIs
	void UpdateCheckButtons();

	//fets the flyout of an id
	int GetFlyOut(int buttonID);
	//sets the fly of an ID
	void SetFlyOut(int buttonID, int flyOut, BOOL notify);
	//enables a button
	void Enable(int buttonID, BOOL enable);

	//sets a control as indeterminate
	void SetIndeterminate(int buttonID,BOOL indet);

	//gets set a floating spinner
	void SetSpinFValue(int buttonID, float v);
	float GetSpinFValue(int buttonID);

	//gets set an integer spinner
	void SetSpinIValue(int buttonID, int v);
	int GetSpinIValue(int buttonID);


	//used only for chekbox UI items
	BOOL IsChecked(int buttonID);

	//this repaints all the toolbars that have the parent, if NULL all are repainted
	void InvalidateToolbar(HWND parent);

	//loads and saves the tool bars
	int Load(const MCHAR *iniFileName, int owner, HWND parentWindow);
	int LoadSingle(const MCHAR *iniFileName, int owner, HWND parentWindow, HWND toolbarHWND);
	void Save(const MCHAR *iniFileName);

	//returns the number of tool bars
	int NumberToolBars();
	//retrieves a a specific toolbar by index
	const MCHAR* Name(int index);
	//retrieves a a specific toolbar by name
	ToolBarFrame* GetToolBarFrame(const MCHAR* name);
	//adds a new toolbar
	void	AppendToolBar(int owner, HWND parent, const MCHAR* name, int pos, int x, int y, int width, int height, bool popup);

	void AppendToolBar(HWND hwnd,int owner, const MCHAR* name);

	void AddButton(int id, HWND hWnd, BOOL checkButton);
	void FreeButtons();

	//this is the message proc that should be inserted into the message loop
	//it returns true if the customUI handled the message
	int MessageProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	int GetCornerHeight(int corner);

	void DisplayFloaters(BOOL show);

protected:
	UnwrapMod*			mMod;
	HINSTANCE			mhInstance;
	Tab<ToolBarFrame*>  mToolbars;
	Tab<UnwrapButton*>   mButtons;
	int					mCornerHeight[4];

};