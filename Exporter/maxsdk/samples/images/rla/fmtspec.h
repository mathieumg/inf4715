//-----------------------------------------------------------------------------
// --------------------
// File ....: fmtspec.h
// --------------------
// Author...: Gus J Grubba
// Date ....: March 1997
// Descr....: RLA File Format Specific Parameters
//
// History .: Mar, 27 1997 - Started
//
//-----------------------------------------------------------------------------
        
#ifndef _RLAFMTSPEC_
#define _RLAFMTSPEC_

//-----------------------------------------------------------------------------
//-- RLA data Structure -------------------------------------------------------
//
//	Channels are defined in gbuf.h:
//
//	BMM_CHAN_Z
//	BMM_CHAN_MTL_ID
//	BMM_CHAN_NODE_ID
//	BMM_CHAN_UV
//	BMM_CHAN_NORMAL
//	BMM_CHAN_REALPIX
//    etc..

#define RLAVERSION 301

struct RLAUSERDATA_old {
	DWORD	version;		//-- Reserved
	DWORD	channels;		//-- Bitmap with channels to save
	BOOL	usealpha;		//-- Save Alpha (if present)
	BYTE 	rgb;			//-- 0=8b, 1=16b, 2=32b
	BOOL 	defaultcfg;		//-- Reserved
	char 	desc[128];		//-- Description (char)
	char 	user[32];		//-- User Name (char)
	BOOL	premultAlpha;   //-- Premultiply Alpha
}; 

struct RLAUSERDATA {
	DWORD	version;		//-- Reserved
	DWORD	channels;		//-- Bitmap with channels to save
	BOOL	usealpha;		//-- Save Alpha (if present)
	BYTE 	rgb;			//-- 0=8b, 1=16b, 2=32b
	BOOL 	defaultcfg;		//-- Reserved
	char 	descA[128];		//-- Description (char)
	char 	userA[32];		//-- User Name (char)
	BOOL	premultAlpha;   //-- Premultiply Alpha
	wchar_t descW[128];		//-- Description (wchar_t)
	wchar_t userW[32];		//-- User Name (wchar_t)

	void	setDescription(const TCHAR* description)
	{
		WStr descriptionW = WStr::FromMCHAR(description);
		wcsncpy(descW,descriptionW.data(),_countof(descW)-1);
		Interface14 *iface = GetCOREInterface14();
		UINT codePage = iface->CodePageForLanguage(iface->LanguageToUseForFileIO());
		strncpy(descA, descriptionW.ToCP(codePage), _countof(descA)-1);
	}
	const TCHAR* getDescription() const 
	{
#ifdef _UNICODE
		return descW;
#else
		return descA;
#endif
	}

	void	setUserName(const TCHAR* userName)
	{
		WStr userNameW = WStr::FromMCHAR(userName);
		wcsncpy(userW,userNameW.data(),_countof(userW)-1);
		Interface14 *iface = GetCOREInterface14();
		UINT codePage = iface->CodePageForLanguage(iface->LanguageToUseForFileIO());
		strncpy(userA, userNameW.ToCP(codePage), _countof(userA)-1);
	}
	const TCHAR* getUserName() const 
	{
#ifdef _UNICODE
		return userW;
#else
		return userA;
#endif
	}
}; 

#endif

//-- EOF: fmtspec.h -----------------------------------------------------------
