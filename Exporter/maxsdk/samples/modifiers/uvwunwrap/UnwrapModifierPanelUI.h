
#pragma once

#include <windowsx.h>
#include "custcont.h"

class UnwrapMod;

class ModifierPanelUI
{
public:
	ModifierPanelUI();
	~ModifierPanelUI();

	void Init(UnwrapMod *mod);

	void SetDefaults(int index, HWND parentWindow);

	void Setup(HINSTANCE hInstance, HWND rollupHWND, const MCHAR *iniFile);
	void TearDown();
	void CreateDefaultToolBars();
	void LoadInActions(int index);
protected:
	UnwrapMod *mMod;
	HWND mRollupHwnd;

};