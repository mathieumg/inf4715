//-----------------------------------------------------------------------------
// --------------------
// File ....: fmtspec.h
// --------------------
// Author...: Gus J Grubba
// Date ....: March 1997
// Descr....: Targa File Format Specific Parameters
//
// History .: Mar, 27 1997 - Started
//
//-----------------------------------------------------------------------------
        
#ifndef _TGAFMTSPEC_
#define _TGAFMTSPEC_

//-- Targa write types

#define TGA_16_BITS 0			//-- Alpha bit set if alpha > 0
#define TGA_24_BITS 1
#define TGA_32_BITS 2

//-----------------------------------------------------------------------------
//-- TGA data Structure -------------------------------------------------------
//

#define TGAVERSION 103

struct TGAUSERDATA_old {
	DWORD	version;			//-- Reserved
	BOOL	saved;				//-- Reserved
	BYTE	writeType;			//-- Write type define above
	BYTE	compressed;			//-- Compression (TRUE or FALSE)
	BOOL	alphaSplit;			//-- Write separate Alpha File (TRUE or FALSE)
	BOOL	preMultAlpha;			//-- Write premultiplied alpha (TRUE or FALSE)
	char	author[41];			//-- Author name
	char	jobname[41];		//-- Job Name
	char	comments1[81];		//-- Comments
	char	comments2[81];
	char	comments3[81];
	char	comments4[81];
};

struct TGAUSERDATA {
	DWORD	version;			//-- Reserved
	BOOL	saved;				//-- Reserved
	BYTE	writeType;			//-- Write type define above
	BYTE	compressed;			//-- Compression (TRUE or FALSE)
	BOOL	alphaSplit;			//-- Write separate Alpha File (TRUE or FALSE)
	BOOL	preMultAlpha;			//-- Write premultiplied alpha (TRUE or FALSE)
	char	authorA[41];			//-- Author name
	char	jobnameA[41];		//-- Job Name
	char	comments1A[81];		//-- Comments
	char	comments2A[81];
	char	comments3A[81];
	char	comments4A[81];
	wchar_t	authorW[41];			//-- Author name
	wchar_t	jobnameW[41];		//-- Job Name
	wchar_t	comments1W[81];		//-- Comments
	wchar_t	comments2W[81];
	wchar_t	comments3W[81];
	wchar_t	comments4W[81];

	const TCHAR* getAuthor() const 
	{
#ifdef _UNICODE
		return authorW;
#else
		return authorA;
#endif
	}
	void	setAuthor(const TCHAR* pAuthor)
	{
		WStr author = WStr::FromMCHAR(pAuthor);
		wcsncpy(authorW,author.data(),_countof(authorW)-1);
		Interface14 *iface = GetCOREInterface14();
		UINT codePage = iface->CodePageForLanguage(iface->LanguageToUseForFileIO());
		strncpy(authorA, author.ToCP(codePage), _countof(authorA)-1);
	}
	const TCHAR* getJobName() const 
	{
#ifdef _UNICODE
		return jobnameW;
#else
		return jobnameA;
#endif
	}
	void	setJobName(const TCHAR* pJobName)
	{
		WStr jobname = WStr::FromMCHAR(pJobName);
		wcsncpy(jobnameW,jobname.data(),_countof(jobnameW)-1);
		Interface14 *iface = GetCOREInterface14();
		UINT codePage = iface->CodePageForLanguage(iface->LanguageToUseForFileIO());
		strncpy(jobnameA, jobname.ToCP(codePage), _countof(jobnameA)-1);
	}
	const TCHAR* getComments1() const 
	{
#ifdef _UNICODE
		return comments1W;
#else
		return comments1A;
#endif
	}
	void	setComments1(const TCHAR* pComments1)
	{
		WStr comments1 = WStr::FromMCHAR(pComments1);
		wcsncpy(comments1W,comments1.data(),_countof(comments1W)-1);
		Interface14 *iface = GetCOREInterface14();
		UINT codePage = iface->CodePageForLanguage(iface->LanguageToUseForFileIO());
		strncpy(comments1A, comments1.ToCP(codePage), _countof(comments1A)-1);
	}
	const TCHAR* getComments2() const 
	{
#ifdef _UNICODE
		return comments2W;
#else
		return comments2A;
#endif
	}
	void	setComments2(const TCHAR* pComments2)
	{
		WStr comments2 = WStr::FromMCHAR(pComments2);
		wcsncpy(comments2W,comments2.data(),_countof(comments2W)-1);
		Interface14 *iface = GetCOREInterface14();
		UINT codePage = iface->CodePageForLanguage(iface->LanguageToUseForFileIO());
		strncpy(comments2A, comments2.ToCP(codePage), _countof(comments2A)-1);
	}
	const TCHAR* getComments3() const 
	{
#ifdef _UNICODE
		return comments3W;
#else
		return comments3A;
#endif
	}
	void	setComments3(const TCHAR* pComments3)
	{
		WStr comments3 = WStr::FromMCHAR(pComments3);
		wcsncpy(comments3W,comments3.data(),_countof(comments3W)-1);
		Interface14 *iface = GetCOREInterface14();
		UINT codePage = iface->CodePageForLanguage(iface->LanguageToUseForFileIO());
		strncpy(comments3A, comments3.ToCP(codePage), _countof(comments3A)-1);
	}
	const TCHAR* getComments4() const 
	{
#ifdef _UNICODE
		return comments4W;
#else
		return comments4A;
#endif
	}
	void	setComments4(const TCHAR* pComments4)
	{
		WStr comments4 = WStr::FromMCHAR(pComments4);
		wcsncpy(comments4W,comments4.data(),_countof(comments4W)-1);
		Interface14 *iface = GetCOREInterface14();
		UINT codePage = iface->CodePageForLanguage(iface->LanguageToUseForFileIO());
		strncpy(comments4A, comments4.ToCP(codePage), _countof(comments4A)-1);
	}
};
#endif

//-- EOF: fmtspec.h -----------------------------------------------------------
