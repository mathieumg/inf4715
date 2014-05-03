/**********************************************************************
*<
FILE: BrushPresets.cpp

DESCRIPTION:	Appwizard generated plugin

CREATED BY: 

HISTORY: 

*>	Copyright (c) 2004, All Rights Reserved.
**********************************************************************/

#include "BrushPresets.h"
#include <Util\IniUtil.h> // MaxSDK::Util::WritePrivateProfileString
#include <maxtextfile.h>
#include "msxml6.h"

#include "3dsmaxport.h"

#define DEFAULTFILENAME			_T("DefaultUI")		//LOC_Notes: do not localize this
#define STARTFILENAME			_T("MaxStartUI")	//LOC_Notes: do not localize this

#define MAX_NUM_PRESETS			50
#define ID_TOOL_PRESET_MIN		10001	//Tool ID of the first possible preset button
#define ID_TOOL_PRESET_MAX		10050	//Tool ID of the last possible preset button
#define ID_TOOL_ADDPRESET		10051
#define ID_TOOL_PRESETMGR		10052

#define PRESETCFG_TYPE_WORLD	_T("WORLD")
#define PRESETCFG_TYPE_FLOAT	_T("FLOAT")
#define PRESETCFG_TYPE_INT		_T("INT")
#define PRESETCFG_TYPE_BOOL		_T("BOOL")
#define PRESETCFG_TYPE_POINT3	_T("POINT3")
#define PRESETCFG_TYPE_COLOR	_T("COLOR")
#define PRESETCFG_TYPE_STRING	_T("STRING")
#define PRESETCFG_TYPE_CURVEPTS _T("CURVEPTS")
#define PRESETCFG_ENABLED		_T("ENABLED")
#define PRESETCFG_DISABLED		_T("DISABLED")
#define PRESETCFG_NUMPRESETS	_T("NumPresets")
#define PRESETCFG_NUMCONTEXTS	_T("NumContexts")
#define PRESETCFG_ICONMIN		_T("IconMin")
#define PRESETCFG_ICONMAX		_T("IconMax")
#define PRESETCFG_PRESETID		_T("PresetID")
#define PRESETCFG_CONTEXTID		_T("ContextID")
#define PRESETCFG_CONTEXTNAME	_T("ContextName")
#define PRESETCFG_PLUGINCLASSID	_T("PluginClassID")
#define PRESETCFG_PLUGINSUPERCLASSID _T("PluginSuperclassID")

#define ADSK_BPR_ROOT	_T("ADSK_BPR")
#define NEW_LINE			_T("\n")
#define NEW_LINE_TAB		_T("\n\t")
#define PRESETLIST		_T("PresetList")
#define PRESET				_T("Preset")
#define BRUSHPRESETS_CONFIG		_T("BrushPresets_Config")
#define CONTEXT			_T("Context")
#define CONTEXT_PARAM	_T("Context_Param")
#define POINT				_T("Point")
#define BRUSH_PRESETS	_T("Brush_Presets")

#define ICONMIN		_T("iconMin")
#define ICONMAX		_T("iconMax")
#define NAME			_T("name")
#define VALUE			_T("value")
#define TYPE			_T("type")

#define CONTEXTID		_T("contextID")
#define PLUGINCLASSID			_T("pluginClassID")
#define PLUGINSUPERCLASSID		_T("pluginSuperclassID")
#define X		_T("x")
#define Y		_T("y")
#define INX		_T("inX")
#define INY		_T("inY")
#define OUTX	_T("outX")
#define OUTY	_T("outY")
#define FLAGS	_T("flags")

#define RED	_T("red")
#define GREEN	_T("green")
#define BLUE	_T("blue")


#define CURVE_CLASS_ID Class_ID(0x5f2d3a0a, 0x4be75269)

//-----------------------------------------------------------------------------
// forward declarations

inline TCHAR* ReadToken( TCHAR* str, TCHAR* buffer, TCHAR delim  );


//-----------------------------------------------------------------------------
// static instances
BrushPresetMgr* BrushPresetMgr::mInstance = NULL;
BrushPresetMgr* BrushPresetMgr::GetInstance()
{
	if (NULL == mInstance) {
		mInstance = new BrushPresetMgr();
	}
	return mInstance;
}
void BrushPresetMgr::DestroyInstance()
{
	delete mInstance;
	mInstance = NULL;
}

BrushPresetMgr* GetBrushPresetMgr() 
{
	return BrushPresetMgr::GetInstance();
}

StandinBrushPresetContext* CreateStandinContext(
	Class_ID contextID, const TCHAR* contextName, Class_ID pluginClassID, SClass_ID pluginSClassID )
{
	StandinBrushPresetContext* standin =
		new StandinBrushPresetContext( contextID, contextName, pluginClassID, pluginSClassID );
	GetBrushPresetMgr()->AddContext( standin );
	return standin;
}


StdBrushPresetContext* StdBrushPresetContext::mInstance = NULL;
StdBrushPresetContext* StdBrushPresetContext::GetInstance() 
{
	if (NULL == mInstance) {
		mInstance = new StdBrushPresetContext();
	}
	return mInstance;
}
void StdBrushPresetContext::DestroyInstance()
{
	delete mInstance;
	mInstance = NULL;
}

StdBrushPresetContext* GetStdBrushPresetContext() 
{
	return StdBrushPresetContext::GetInstance();
}

static IPainterInterface_V5* thePainterInterface = NULL;
IPainterInterface_V5*	GetPainterInterface() { return thePainterInterface; }


IPainterInterface_V5*	CreatePainterInterface()
{ return (IPainterInterface_V5*)(GetCOREInterface()->CreateInstance(REF_TARGET_CLASS_ID,PAINTERINTERFACE_CLASS_ID)); }

StdBrushPresetParams* GetStdBrushPresetParams( IBrushPreset* preset )
{ return (StdBrushPresetParams*)preset->GetParams(STDBRUSHPRESETCONTEXT_ID); }


//-----------------------------------------------------------------------------
//-- Utilities

#define MAX(a,b) ((a)<(b)? (b):(a))
#define MIN(a,b) ((a)>(b)? (b):(a))

bool _bool( BOOL b ) {return (b!=FALSE);} // Typecast helper

inline float Clamp( float x, float lo, float hi ) {
	if( x>=hi ) return hi;
	if( x<=lo ) return lo;
	return x;
}

inline float Interp( float w, float lower, float upper ) {
	return (upper*w) + (lower*(1.0f-w));
}

inline RGBTRIPLE Interp( float w, RGBTRIPLE& lower, RGBTRIPLE& upper ) {
	RGBTRIPLE retval;
	retval.rgbtRed		= (upper.rgbtRed * w)	+ (lower.rgbtRed * (1.0f-w));
	retval.rgbtGreen	= (upper.rgbtGreen * w) + (lower.rgbtGreen * (1.0f-w));
	retval.rgbtBlue		= (upper.rgbtBlue * w)	+ (lower.rgbtBlue * (1.0f-w));
	return retval;
}

RGBTRIPLE COLORREFtoRGBTRIPLE( COLORREF c ) {
	RGBTRIPLE retval = { (c&0xFF0000)>>16, (c&0xFF00)>>8, c&0xFF };
	return retval;
}

RGBTRIPLE ColorToRGBTRIPLE( Color& c ) {
	RGBTRIPLE retval;
	retval.rgbtRed = c.r*255, retval.rgbtGreen = c.g*255, retval.rgbtBlue = c.b*255;
	return retval;
}

RGBTRIPLE GetStdColor( int id ) {
	IColorManager* colorman = ColorMan();
	return COLORREFtoRGBTRIPLE( colorman->GetColor(id) );
}

void SetWindowPos( HWND hWnd, int x, int y, HWND relativeTo ) {
	if( !hWnd ) return;

	RECT rect, offset;
	GetWindowRect( hWnd, &rect );
	if( relativeTo!=NULL )
		GetWindowRect( relativeTo, &offset );
	else offset.left = offset.top = 0;
	int absX = x+offset.left,		absY = y+offset.top;
	int w = rect.right-rect.left,	h = rect.bottom-rect.top;

	//IMPORTANT: this works with WM_MOVING but not WM_MOVE messages, because WM_MOVE reports
	//the coordinates for the client space, and we need the coordiantes for the whole window
	::SetWindowPos( hWnd, HWND_TOP, absX, absY, w, h, SWP_NOACTIVATE );
}

ClassDesc* LoadClass( Class_ID classID, SClass_ID superclassID )
{
	ClassDirectory& cdir = GetCOREInterface()->GetDllDir().ClassDir();
	return cdir.FindClassEntry( superclassID, classID )->FullCD();
}

// StickyFlag can be modified using standard assignment, but once modified will not reset to its initial value, except via Init()
class StickyFlag {
public:
	bool flagVal, initialVal;
	StickyFlag( bool initialVal ) { this->flagVal = this->initialVal = initialVal;}
	void Init() { flagVal = initialVal; }
	bool operator=( bool  newVal ) { if( newVal!=initialVal ) flagVal=newVal; return flagVal; }
	bool operator=( int   newVal ) { return operator=( _bool(newVal) ); }
	bool operator=( DWORD newVal ) { return operator=( _bool(newVal) );  }
	operator bool() { return flagVal; }
};


//-----------------------------------------------------------------------------
//-- Bitmap Utilitues
// FIXME: WinImageList does not work correctly with multiple images per list (only supports 1 image)   :(

LPBITMAPINFO CreateWinBitmap(int width, int height) {
	HDC hdc = GetDC(NULL);
	int biSizeImage = (3 * width * height);
	LPBITMAPINFO pbmi = (LPBITMAPINFO)malloc( sizeof(BITMAPINFOHEADER) + biSizeImage );
	if (pbmi) {
		BITMAPINFOHEADER& header = pbmi->bmiHeader;
		header.biSize			= sizeof(BITMAPINFOHEADER);
		header.biWidth			= width;
		header.biHeight			= height; //use negative height to specify top-down bitmap?
		header.biPlanes			= 1;
		header.biBitCount		= 24;
		header.biCompression	= BI_RGB;
		header.biSizeImage		= biSizeImage;
		header.biXPelsPerMeter	= GetDeviceCaps(hdc, LOGPIXELSX);
		header.biYPelsPerMeter	= GetDeviceCaps(hdc, LOGPIXELSY);
		header.biClrUsed		= 0;
		header.biClrImportant	= 0;
	}
	ReleaseDC(NULL,hdc);
	return pbmi;
}
void DeleteWinBitmap( LPBITMAPINFO bitmap ) {
	if( bitmap!=NULL ) free( bitmap );
}

void WinImageList::Init( int width, int height ) {
	if( imagelist!=NULL ) Free();
	imagelist = ImageList_Create( width, height, ILC_COLOR24 | ILC_MASK, 1, 1 );
	this->width = width;
	this->height = height;
}

void WinImageList::Free() {
	for( int i=0; i<images.Count(); i++ )	DeleteWinBitmap( images[i] );
	for( int i=0; i<masks.Count(); i++ )	DeleteWinBitmap( masks[i] );
	images.SetCount(0);
	masks.SetCount(0);
	ImageList_Destroy( imagelist );
	imagelist = NULL;
}

int WinImageList::Count() {
	if( imagelist==NULL ) return 0;
	return ImageList_GetImageCount( imagelist );
}

int WinImageList::AddImages( int n ) {
	HDC hdc = GetDC(NULL);
	HBITMAP hImage = CreateCompatibleBitmap( hdc, width, height );
	HBITMAP hMask = CreateCompatibleBitmap( hdc, width, height );
	int index = 0;

	for( int i=0; i<n; i++ ) {
		LPBITMAPINFO image	= CreateWinBitmap( width, height );
		LPBITMAPINFO mask	= CreateWinBitmap( width, height );
		images.Append( 1, &image );
		masks.Append( 1, &mask );

		index = ImageList_Add( imagelist, hImage, hMask );
	}

	DeleteObject( hImage );
	DeleteObject( hMask );
	ReleaseDC(NULL,hdc);
	return index;
}


void WinImageList::UpdateImage( int i ) {
	HDC hdc = GetDC(NULL);
	LPBITMAPINFO image = images[i];
	LPBITMAPINFO mask = masks[i];

	HBITMAP hImage = CreateCompatibleBitmap( hdc, width, height );
	HBITMAP hMask = CreateCompatibleBitmap( hdc, width, height );
	SetDIBits( hdc, hImage, 0, height, image->bmiColors, image, DIB_RGB_COLORS );
	SetDIBits( hdc, hMask, 0, height, mask->bmiColors, mask, DIB_RGB_COLORS );

	ImageList_Replace( imagelist, i, hImage, hMask );
	DeleteObject( hImage );
	DeleteObject( hMask );
	ReleaseDC(NULL,hdc);
}

RGBTRIPLE* WinImageList::GetImage( int i ) {
	if( (i<0) || (images.Count()<=i) )
		return NULL;
	return (RGBTRIPLE*)(&(images[i]->bmiColors[0]));
}

RGBTRIPLE* WinImageList::GetMask( int i ) {
	if( (i<0) || (masks.Count()<=i) )
		return NULL;
	return (RGBTRIPLE*)(&(masks[i]->bmiColors[0]));
}


//-----------------------------------------------------------------------------
//-- FPValueInterfaceType Methods

void AcquireIfInterface( FPValue& val ) {
	if( (val.type==TYPE_INTERFACE) && (val.fpi!=NULL) && (val.fpi->LifetimeControl()==BaseInterface::wantsRelease) )
		val.fpi->AcquireInterface();
}

void ReleaseIfInterface( FPValue& val ) {
	if( (val.type==TYPE_INTERFACE) && (val.fpi!=NULL) && (val.fpi->LifetimeControl()==BaseInterface::wantsRelease) )
		val.fpi->ReleaseInterface();
}

FPValueInterfaceType::FPValueInterfaceType( Interface_ID id ) {
	this->id = id;
	lifetime = BaseInterface::wantsRelease;
	refCount = 1; //Assume we are Aquired upon creation
}

BaseInterface* FPValueInterfaceType::GetInterface(Interface_ID id) {
	if( id==GetID() )
		return this;
	return NULL;
}

void FPValueInterfaceType::SetLifetime( LifetimeType lifetime ) {
	this->lifetime = lifetime;
}

BaseInterface* FPValueInterfaceType::AcquireInterface() {
	refCount++;
	return this;
}
void FPValueInterfaceType::ReleaseInterface() {
	refCount--;
	if( (refCount==0) && (lifetime==BaseInterface::wantsRelease) )  delete this;
}

CurvePts::CurvePts()
	: FPValueInterfaceType(CURVEIOBJECT_INTERFACE_ID)
{ }

class BezierSegment {
public:
	Point2 p1, p2, p3, p4;
	Point2 Val( float t );
	Point2 Deriv( float t );
	float SolveX( float x, float err=0.001f, float t=0.5f, float tMin=0, float tMax=1 );
};

Point2 BezierSegment::Val( float t ) {
	// Bezier function:  P1*_T^3  +  3*P2 * _T^2*T  +  3*P3*T^2*_T  +   P4*T^3
	float _t = (1.0f-t);
	float tSq = t*t, _tSq = _t*_t;

	Point2 retval	 =	( p1*(_t*_tSq) );	// First term:	P1*_T^3
	retval	+=	( p2*(3*_tSq*t) );	// Second term:	3*P2*_T^2*T
	retval	+=	( p3*(3*_t*tSq) );	// Third term:	3*P3*T^2*_T
	return retval	+	( p4*(tSq*t) );		// Fourth term:	P4*T^3
}

Point2 BezierSegment::Deriv( float t ) {
	Point2 retval	 =	  ( (p4-p1) + 3*(p2-p3) ) * 3*t*t;
	retval   +=	3*( (p1+p3) - (2*p2) ) * 2*t;
	return retval	+	3*(  p2-p1 );
}

// Assumes the X value of the segment is monotonically increasing.
// Uses Newton's method, combined with a binary search to narrow down an upper & lower bound
float BezierSegment::SolveX( float x, float err, float t, float tMin, float tMax ) {
	float xNext = Val( t ).x;
	float delta = x-xNext;
	if( (delta<err) && ((-delta)<err) ) return t;

	float tLo, tHi;
	if( delta>0 )	tMin = tLo = t,		tHi = 0.5f * (tMin + tMax);
	else			tMax = tHi = t,		tLo = 0.5f * (tMin + tMax);

	float deriv = Deriv( t ).x;
	// FIXME: deriv==0?

	float tNext = t + (delta / deriv);
	if( tNext<tLo ) tNext = tLo;
	if( tNext>tHi ) tNext = tHi;

	return SolveX( x, err, tNext, tMin, tMax );
}

float CurvePts::GetValue( float x ) {
	int N = GetNumPts();
	if( N==0 ) return 0;

	if( x<=curvePts[0].p.x )	return curvePts[0].p.y;
	if( x>=curvePts[N-1].p.x )	return curvePts[N-1].p.y;

	int i;
	for( i=0; i<(N-1); i++ )
		if( (curvePts[i].p.x<=x) && (curvePts[i+1].p.x>=x) ) break;

	BezierSegment seg;
	seg.p1 = curvePts[i].p;
	seg.p2 = (curvePts[i].p + curvePts[i].out);
	seg.p3 = (curvePts[i+1].p + curvePts[i+1].in);
	seg.p4 = curvePts[i+1].p;

	float span = curvePts[i+1].p.x - curvePts[i].p.x;
	float guess = (x-curvePts[i].p.x) / span;
	float t = seg.SolveX( x, 0.001f, guess );
	return seg.Val( t ).y;
}

void Assign( CurvePts* curve_output, ICurve* curve_input ) {
	int numPoints = curve_input->GetNumPts();
	curve_output->SetNumPts( numPoints );

	for( int i=0; i<numPoints; i++ )
		curve_output->SetPoint( i, curve_input->GetPoint( 0, i ) );
}

void Assign( ICurve* curve_output, CurvePts* curve_input ) {
	int numPoints = curve_input->GetNumPts();
	curve_output->SetNumPts( numPoints );

	for( int i=0; i<numPoints; i++ ) 
		curve_output->SetPoint( 0, i, &(curve_input->GetPoint(i)), FALSE );
}

void Assign( CurvePts* curve_output, CurvePts* curve_input ) {
	int numPoints = curve_input->GetNumPts();
	curve_output->SetNumPts( numPoints );

	for( int i=0; i<numPoints; i++ ) 
		curve_output->SetPoint( i, curve_input->GetPoint(i) );
}

CurvePts* GetCurvePtsInterface( FPValue* val ) {
	if( (val==NULL) || (val->type!=TYPE_INTERFACE) || (val->fpi==NULL) )
		return NULL;

	BaseInterface* iface = val->fpi->GetInterface( CURVEIOBJECT_INTERFACE_ID );
	return (CurvePts*)iface;
}


//-----------------------------------------------------------------------------
//-- String helper functions

inline int StringToInt(TCHAR *cp) { return _tcstol(cp, NULL, 10); }
inline DWORD StringToDWORD(const TCHAR *cp) { return _tcstol(cp, NULL, 10); }
inline float StringToFloat(TCHAR *cp) { return _tcstod(cp, NULL); }
inline Class_ID StringToClassID(const TCHAR *cp) {
	ULONG a,b;
	int res = _stscanf(cp, _T("(0x%lX,0x%lX)"), &a, &b);
	return (res==2?  Class_ID(a,b) : Class_ID(0,0));
}
inline void ClassIDToString( Class_ID classID, TCHAR* str ) {
	_stprintf( str, _T("(0x%lX,0x%lX)"), classID.PartA(), classID.PartB() );
}


inline bool IsWhitespace( TCHAR c )	{ return (c==_T(' ')) || (c==_T('\t')); }
inline bool IsEndline( TCHAR c )		{ return (c==_T('\n')) || (c==_T('\0')); }
inline bool IsDelimiter( TCHAR c )	{ return (c==_T('|')); }
inline bool IsQuote( TCHAR c )		{ return (c==_T('"')); }

// Given a string made of delimited tokens, read the next token.
// Returns the character after the delimiter (the start of the next token), or NULL for end of string
inline TCHAR* ReadToken( TCHAR* str, TCHAR* buffer, TCHAR delim  ) {
	while( IsWhitespace(*str) )  str++;
	if( IsQuote(*str) ) { //quoted token
		str++;
		// FIXME: Currently no support for escape sequences in quoted strings
		while( !IsQuote(*str) )	*buffer++ = *str++;
		while( (*str!=delim) && !IsEndline(*str) )	str++; //skip to the next token
	}
	else //non-quoted token
		while( (*str!=delim) && !IsEndline(*str) ) *buffer++ = *str++;

	*buffer = _T('\0');
	if( *str==delim ) return (str+1); //skip the delimiter character
	return NULL;
}

// Read a "context" entry from a Brush Preset config file
int ReadCfgContextString( TCHAR* str, Class_ID* contextID, TCHAR* contextName, int* numParams ) {
	TCHAR buf[1024] = {0}, *next=str;
	BOOL retval = TRUE;

	if( next==NULL ) retval = FALSE;
	else {
		next = ReadToken( next, buf, _T('|') );		// contextID
		*contextID = StringToClassID(buf);
	}

	if( next==NULL ) retval = FALSE;
	else {
		next = ReadToken( next, buf, _T('|') );		// contextName
		_tcscpy( contextName, buf );
	}

	if( next==NULL )  retval = FALSE;
	else {
		next = ReadToken( next, buf, _T('|') );		// numParams
		*numParams = StringToInt( buf );
	}

	return retval;
}

// Read a "param" entry from a Brush Preset config file
int ReadCfgParamString( TCHAR* str, int* paramID, TCHAR* paramName, TCHAR* paramValue ) {
	TCHAR buf[1024] = {0}, *next=str;
	BOOL retval = TRUE;

	if( next==NULL ) retval = FALSE;
	else {
		next = ReadToken( next, buf, _T('|') );			// paramID
		*paramID = StringToInt( buf );
	}

	if( next==NULL ) retval = FALSE;
	else {
		next = ReadToken( next, buf, _T('|') );			// contextName
		_tcscpy( paramName, buf );
	}

	if( next==NULL ) retval = FALSE;
	else {
		next = ReadToken( next, buf, _T('|') );			// paramValue
		_tcscpy( paramValue, buf );
	}

	return retval;
}


//-----------------------------------------------------------------------------
//-- Type conversion helper functions


void CurveToString( CurvePts* curve, TCHAR* str ) {
	TCHAR buf[256] = {0};
	str[0] = 0;

	int numPoints = curve->GetNumPts();
	_stprintf( str, _T("%d"), numPoints );

	for( int i=0; i<numPoints; i++ ) {
		CurvePoint& point = curve->GetPoint( i );
		_stprintf( buf, _T(":(%.3f,%.3f),(%.3f,%.3f),(%.3f,%.3f),%d"),
			point.p.x, point.p.y, point.in.x, point.in.y, point.out.x, point.out.y, point.flags );
		_tcscat( str, buf );
	}
}

void StringToCurve( TCHAR* str, CurvePts* curve ) {
	TCHAR* s, buf[256];
	s = ReadToken( str, buf, _T(':') );
	int numPoints = StringToInt(buf);
	curve->SetNumPts( numPoints );

	CurvePoint point;
	for( int i=0; i<numPoints; i++ ) {
		s = ReadToken( s, buf, _T(':') );
		_stscanf( buf, _T("(%f,%f),(%f,%f),(%f,%f),%d"),
			&point.p.x, &point.p.y, &point.in.x, &point.in.y, &point.out.x, &point.out.y, &point.flags );
		curve->SetPoint( i, point );
	}
}

void ParamType2ToString( FPValue* val, TCHAR* str ) {
	TCHAR* s = NULL;
	switch( val->type ) {
	case TYPE_WORLD:		s = PRESETCFG_TYPE_WORLD;	break;
	case TYPE_FLOAT:		s = PRESETCFG_TYPE_FLOAT;	break;
	case TYPE_INT:			s = PRESETCFG_TYPE_INT;		break;
	case TYPE_BOOL:			s = PRESETCFG_TYPE_BOOL;	break;
	case TYPE_POINT3_BV:
	case TYPE_POINT3:		s = PRESETCFG_TYPE_POINT3;	break;
	case TYPE_COLOR_BV:
	case TYPE_COLOR:		s = PRESETCFG_TYPE_COLOR;	break;
	case TYPE_TSTR_BV:
	case TYPE_TSTR:			s = PRESETCFG_TYPE_STRING;	break;
	case TYPE_STRING:		s = PRESETCFG_TYPE_STRING;	break;
	case TYPE_INTERFACE:
		if( GetCurvePtsInterface( val )!=NULL )
			s = PRESETCFG_TYPE_CURVEPTS;
		break;
	}
	if( s==NULL ) str[0] = _T('\0');
	else _tcscpy( str, s );
}

void FPValueToUIString( FPValue* val, TCHAR* str ) {
	switch( val->type ) {
	case TYPE_WORLD:		_stprintf( str, _T("%s"), FormatUniverseValue(val->f) ); break;
	case TYPE_FLOAT:		_stprintf( str, _T("%f"), val->f );				break;
	case TYPE_INT:			_stprintf( str, _T("%i"), val->i );				break;
	case TYPE_STRING:		_stprintf( str, _T("%s"), val->s );				break;
	case TYPE_TSTR_BV:
	case TYPE_TSTR:			_stprintf( str, _T("%s"), val->tstr->data() );	break;
	case TYPE_BOOL:
		_tcscpy( str, (val->i? GetString(IDS_ON) : GetString(IDS_OFF)) );
		break;
	default:
		str[0] = 0;
		break;
	}
}

void FPValueToString( FPValue* val, TCHAR* str ) {
	ParamType2ToString( val, str );
	str += _tcslen(str); //append to end of string

	switch( val->type ) {
	case TYPE_WORLD:
	case TYPE_FLOAT:		_stprintf( str, _T(":%f"), val->f );
		break;
	case TYPE_INT:
	case TYPE_BOOL:			_stprintf( str, _T(":%i"), val->i );
		break;
	case TYPE_POINT3_BV:
	case TYPE_POINT3:
	case TYPE_COLOR_BV:
	case TYPE_COLOR:		_stprintf( str, _T(":(%f,%f,%f)"), val->p->x, val->p->y, val->p->z );
		break;
	case TYPE_TSTR_BV:
	case TYPE_TSTR:			_stprintf( str, _T(":%s"), val->tstr->data() );
		break;
	case TYPE_STRING:		_stprintf( str, _T(":%s"), val->s );
		break;
	case TYPE_INTERFACE:
		{
			CurvePts* curve = GetCurvePtsInterface( val );
			if( curve!=NULL ) {
				TCHAR buf[1024] = {0};
				CurveToString( curve, buf );
				_stprintf( str, _T(":%s"), buf );
			}
		}
		break;
	default:
		str[0] = 0;
		break;
	}
}

void FillFPValue(const TCHAR* typeStr, const TCHAR* valStr, FPValue* val)
{	
	//FIXME: optimize this with bsearch()
	if(      !_tcsicmp( typeStr, PRESETCFG_TYPE_WORLD ) )		val->type = (ParamType2)TYPE_WORLD;
	else if( !_tcsicmp( typeStr, PRESETCFG_TYPE_FLOAT ) )		val->type = (ParamType2)TYPE_FLOAT;
	else if( !_tcsicmp( typeStr, PRESETCFG_TYPE_INT ) )			val->type = (ParamType2)TYPE_INT;
	else if( !_tcsicmp( typeStr, PRESETCFG_TYPE_BOOL ) )		val->type = (ParamType2)TYPE_BOOL;
	else if( !_tcsicmp( typeStr, PRESETCFG_TYPE_POINT3 ) )		val->type = (ParamType2)TYPE_POINT3_BV;
	else if( !_tcsicmp( typeStr, PRESETCFG_TYPE_COLOR ) )		val->type = (ParamType2)TYPE_COLOR_BV;
	else if( !_tcsicmp( typeStr, PRESETCFG_TYPE_STRING ) )		val->type = (ParamType2)TYPE_TSTR_BV;
	else if( !_tcsicmp( typeStr, PRESETCFG_TYPE_CURVEPTS ) )	{ // Special handling for "Curve Points" type
		TCHAR valStrCopy[MAX_PATH*2] = {0};
		_tcscpy_s(valStrCopy, _countof(valStrCopy), valStr);
		CurvePts* curve = new CurvePts();
		StringToCurve( valStrCopy, curve );
		val->type = TYPE_INTERFACE;
		val->fpi = curve;
	}
	else val->type = TYPE_VOID, val->ptr = NULL;

	switch( val->type ) {
	case TYPE_WORLD:
	case TYPE_FLOAT:		_stscanf( valStr, _T("%f"), &val->f );
		break;
	case TYPE_INT:			_stscanf( valStr, _T("%i"), &val->i );
		break;
	case TYPE_BOOL:			_stscanf( valStr, _T("%i"), &val->i );
		break;
	case TYPE_POINT3_BV:	val->p = new Point3;
		_stscanf( valStr, _T("(%f,%f,%f)"), &(val->p->x), &(val->p->y), &(val->p->z) );
		break;
	case TYPE_COLOR_BV:		val->clr = new Color;
		_stscanf( valStr, _T("(%f,%f,%f)"), &(val->clr->r), &(val->clr->g), &(val->clr->b) );
		break;
	case TYPE_TSTR_BV:		val->tstr = new TSTR( valStr );
		break;
	}
}

void StringToFPValue( TCHAR* str, FPValue* val ) {
	if( str==NULL || str[0]==0 ) {
		val->type = TYPE_VOID;
		return;
	}

	TCHAR *s = str;
	TCHAR typeStr[256] = {0}, valStr[1024] = {0};

	// read in the type string
	int i;
	for( i=0; (*s!=0) && (*s!=_T(':')); i++ )	typeStr[i] = *(s++);
	typeStr[i] = 0; // add end-of-string

	if( *s==_T(':') ) s++; //skip the delimiter

	// read in the value string
	for( i=0; (*s!=0); i++ )	valStr[i] = *(s++);
	valStr[i] = 0; // add end-of-string
	FillFPValue(typeStr, valStr, val);
}




//-----------------------------------------------------------------------------
//-- Brush Preset IO class
//-- And XML helper functions

// Macro to verify memory allcation.
#define CHK_ALLOC(p)        do { if (!(p)) { hr = E_OUTOFMEMORY;throw BPRFormatIO::XmlFailedException(_T("XML memory allocation failed")); } } while(0)

// Macro that calls a COM method returning HRESULT value.
#define CHK_HR(stmt)        do { hr=(stmt); if (FAILED(hr)) throw BPRFormatIO::XmlFailedException(_T("XML Operation failed")); } while(0)

// Macro that releases a COM object if not NULL.
#define SAFE_RELEASE(p)     do { if ((p)) { (p)->Release(); (p) = NULL; } } while(0)

// Helper function to create a VT_BSTR variant from a null terminated string. 
HRESULT VariantFromString(const TSTR& str, VARIANT &Variant)
{
	HRESULT hr = S_OK;
	BSTR bstr = SysAllocString(str.ToWStr().data());
	if ( ! bstr ) 
	{
		hr = E_OUTOFMEMORY;
		return hr;
	}

	V_VT(&Variant)   = VT_BSTR;
	V_BSTR(&Variant) = bstr;

	return hr;
}

class BPRFormatIO
{
public:

	class XmlFailedException
	{
	public:
		XmlFailedException(const TSTR& errorMsg = _T("")): mErrorMsg(errorMsg)
		{}
		TSTR ErrorMsg() {return mErrorMsg;}
	private:
		TSTR mErrorMsg;
	};


	BPRFormatIO(BrushPresetMgr* bpManager, const TSTR& fileName, bool isPropFormat = false):
	m_bpManager(bpManager),
		m_fileName(fileName),
		m_isPropFormat(isPropFormat),
		m_pXMLDom(NULL),
		mReadEncoding(0)
	{}
	~BPRFormatIO() {};

	float IconMin() { return mIconMin; }
	float IconMax() { return mIconMax; }
	void SetIconMin(float val) { mIconMin = val; }
	void SetIconMax(float val) { mIconMax = val; }
	TSTR IconMinTxt();
	TSTR IconMaxTxt();
	TSTR FileName() const { return m_fileName; }

	static bool GetReadFileName(TSTR& fileName);
	static bool GetWriteFileName(TSTR& fileName);

	bool ReadPresets();
	bool WritePresets();

protected:

	struct XMLReadGuard
	{
		XMLReadGuard(IXMLDOMDocument *xmlDom);
		~XMLReadGuard();

		IXMLDOMDocument *pXMLDom;
		IXMLDOMNodeList *pNodes;
		IXMLDOMNode *pNode;
		IXMLDOMNodeList *pCurvePointNodes;		
		VARIANT varFileName;
	};

	struct XMLWriteGuard
	{
		XMLWriteGuard(IXMLDOMDocument *xmlDom);
		~XMLWriteGuard();

		IXMLDOMDocument *pXMLDom;
		IXMLDOMElement *pRoot;
		IXMLDOMNodeList *pNodes;
		IXMLDOMElement *pNode;
		IXMLDOMElement *pPresetNode;
		VARIANT varFileName;
	};

	struct XMLParamGuard
	{
		XMLParamGuard();
		~XMLParamGuard();
		IXMLDOMNodeList *pContextNodes;
		IXMLDOMNodeList *pContextParamNodes;
		IXMLDOMElement *pPresetNode;
		IXMLDOMElement *pContextNode;
		IXMLDOMElement *pContextParamNode;
		IXMLDOMElement *pPointNode;
		IXMLDOMElement* pColorNode;
		IXMLDOMNamedNodeMap *pAttributes;
		FPValue fpValue;
	};

	BPRFormatIO() {};

	BrushPresetMgr* m_bpManager;
	TSTR m_fileName;
	float mIconMin;
	float mIconMax;
	bool m_isPropFormat;
	IXMLDOMDocument *m_pXMLDom;
	unsigned int mReadEncoding;

	static bool IsXmlFile(TSTR& fileName);

	bool ReadPresetsProp();
	bool WritePresetsProp();
	bool DetectEncoding();
	DWORD GetEncodedPrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, 
		LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD nSize, LPCTSTR lpFileName);

	bool ReadPresetsXML();
	bool WritePresetsXML();
	bool XMLParseCurvePoints(const TSTR& nodePath, int contexParamID, FPValue& paramVal);
	HRESULT GetXMLElement(IXMLDOMNodeList* nodeList, long num, IXMLDOMElement** element);
	HRESULT CreateXMLElement(const TSTR& name, IXMLDOMElement **ppElement);
	HRESULT AppendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent);
	HRESULT CreateAndAddTextNode(const TSTR& text, IXMLDOMNode *pParent);
	HRESULT CreateAndAddPINode(const TSTR& target, const TSTR& strData);
	HRESULT CreateAndAddAttributeNode(const TSTR& name, const TSTR& value, IXMLDOMElement *pParent);
	HRESULT CreateAndAddAttributeNode(const TSTR& name, float value, IXMLDOMElement *pParent);
	HRESULT CreateAndAddAttributeNode(const TSTR& name, int value, IXMLDOMElement *pParent);
	HRESULT CreateAndAddElementNode(const TSTR& name, const TSTR& newline, IXMLDOMNode *pParent, IXMLDOMElement **ppElement = NULL);
	HRESULT FPValueToXML( FPValue& val, IXMLDOMElement *pParent );

	bool GetReadConfigFile( TSTR& fileName);
	bool GetWriteConfigFile( TSTR& fileName);

	HRESULT CreateAndInitDOM();
	HRESULT GetAttribute(IXMLDOMNamedNodeMap *pAttributes, const TSTR& bstrAttributeName, TSTR& result);
};

BPRFormatIO::XMLWriteGuard::XMLWriteGuard(IXMLDOMDocument *xmlDom)
{
	pXMLDom = xmlDom;
	pRoot = NULL;
	pNodes = NULL;
	pNode = NULL;
	pPresetNode = NULL;
	
	VariantInit(&varFileName);
}
BPRFormatIO::XMLWriteGuard::~XMLWriteGuard()
{
	SAFE_RELEASE(pXMLDom);
	SAFE_RELEASE(pRoot);
	SAFE_RELEASE(pNodes);
	SAFE_RELEASE(pNode);
	SAFE_RELEASE(pPresetNode);
	
	VariantClear(&varFileName);
}

BPRFormatIO::XMLReadGuard::XMLReadGuard(IXMLDOMDocument *xmlDom)
{
	pXMLDom = xmlDom;
	pNodes = NULL;
	pNode = NULL;
	pCurvePointNodes = NULL;
	
	VariantInit(&varFileName);

}
BPRFormatIO::XMLReadGuard::~XMLReadGuard()
{
	SAFE_RELEASE(pXMLDom);
	SAFE_RELEASE(pNodes);
	SAFE_RELEASE(pNode);
	SAFE_RELEASE(pCurvePointNodes);
	
	VariantClear(&varFileName);
}


BPRFormatIO::XMLParamGuard::XMLParamGuard()
{
	pAttributes = NULL;
	pContextNodes = NULL;
	pContextParamNodes = NULL;
	pPresetNode = NULL;
	pContextNode = NULL;
	pContextParamNode = NULL;
	pPointNode = NULL;
	pColorNode = NULL;
}
BPRFormatIO::XMLParamGuard::~XMLParamGuard()
{
	SAFE_RELEASE(pContextNodes);
	SAFE_RELEASE(pContextParamNodes);
	SAFE_RELEASE(pPresetNode);
	SAFE_RELEASE(pAttributes);
	SAFE_RELEASE(pContextNode);
	SAFE_RELEASE(pContextParamNode);
	SAFE_RELEASE(pPointNode);
	SAFE_RELEASE(pColorNode);
	
	ReleaseIfInterface( fpValue );
}

bool BPRFormatIO::IsXmlFile(TSTR& fileName)
{
	DbgAssert(!fileName.isNull());
	return fileName.EndsWith(_T('x'), false);
}

TSTR BPRFormatIO::IconMinTxt() 
{ 
	TSTR res;
	res.printf( _T("%f"), IconMin() );
	return res;
}

TSTR BPRFormatIO::IconMaxTxt() 
{ 
	TSTR res;
	res.printf( _T("%f"), IconMax() );
	return res;
}

// The config file to load from;
// Either MaxStartUI.bpr or DefaultUI.bpr,
// or returns FALSE if neither exist
bool BPRFormatIO::GetReadFileName(TSTR& fileName) {

	TSTR unresFileName(TSTR(STARTFILENAME) + GetString(IDS_BPRX_EXTENTION));
	if( !GetCUIFrameMgr()->ResolveReadPath(unresFileName, fileName)) {
		unresFileName = TSTR(DEFAULTFILENAME) + GetString(IDS_BPRX_EXTENTION);
		if( !GetCUIFrameMgr()->ResolveReadPath(unresFileName, fileName) ) {
			return false;
		}
	}
	return true;
}

// The config file to save into; MaxStartUI.bpr
bool BPRFormatIO::GetWriteFileName(TSTR& fileName) {

	TSTR unresolvedFileName(STARTFILENAME);
	unresolvedFileName += GetString(IDS_BPR_EXTENTION);

	// Let's see what extension the read file has
	if ( GetReadFileName(fileName) && IsXmlFile(fileName) )
	{
		// the read file exists and it is XML
		unresolvedFileName = STARTFILENAME;
		unresolvedFileName += GetString(IDS_BPRX_EXTENTION);
	}

	// this really shouldn't fail
	bool result = GetCUIFrameMgr()->ResolveWritePath(unresolvedFileName, fileName);
	DbgAssert(result);
	return true;
}

bool BPRFormatIO::GetReadConfigFile( TSTR& fileName)
{
	// Find config file, if necessary
	if( fileName.isNull() || ! DoesFileExist(fileName.data()) ) {
		return GetReadFileName(fileName) != FALSE;
	}
	return true;
}

bool BPRFormatIO::GetWriteConfigFile( TSTR& fileName)
{
	if( fileName.isNull() ) {
		if( ! GetWriteFileName(fileName) )
			return false;
		fileName = m_fileName;
		return true;
	}
	return true;
}

bool BPRFormatIO::ReadPresets()
{
	if ( ! GetReadConfigFile(m_fileName) )
		return false;

	if (IsXmlFile(m_fileName))
		return ReadPresetsXML();

	return ReadPresetsProp();
}

bool BPRFormatIO::WritePresets()
{
	if ( ! GetWriteConfigFile(m_fileName) )
		return false;

	if (IsXmlFile(m_fileName))
		return WritePresetsXML();

	return WritePresetsProp();
}

HRESULT BPRFormatIO::CreateAndInitDOM()
{
	HRESULT hr = CoCreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pXMLDom));
	if ( ! SUCCEEDED(hr))
		return hr;

	// these methods should not fail so don't inspect result
	m_pXMLDom->put_async(VARIANT_FALSE);  
	m_pXMLDom->put_validateOnParse(VARIANT_FALSE);
	m_pXMLDom->put_resolveExternals(VARIANT_FALSE);
	return hr;
}

HRESULT BPRFormatIO::GetAttribute(IXMLDOMNamedNodeMap *pAttributes, const TSTR& bstrAttributeName, TSTR& result)
{
	HRESULT hr = S_OK;
	BSTR bstrValue = NULL;
	IXMLDOMNode *pNode = NULL;
	try
	{
		CHK_HR(pAttributes->getNamedItem(bstrAttributeName.ToBSTR(), &pNode));

		if (!pNode)
		{
			// Well, we do not have such attribute, so just leave
			result = _T("");
			SAFE_RELEASE(pNode);
			SysFreeString(bstrValue);
			return S_OK;
		}

		CHK_HR(pNode->get_text(&bstrValue));
		SAFE_RELEASE(pNode);

		result = TSTR::FromBSTR(bstrValue);
		SysFreeString(bstrValue);

	}
	catch(XmlFailedException)
	{
		SAFE_RELEASE(pNode);
		SysFreeString(bstrValue);
		return hr;
	}

	return hr;
}

HRESULT BPRFormatIO::GetXMLElement(IXMLDOMNodeList* nodeList, long num, IXMLDOMElement** element)
{
	IXMLDOMNode* pNode = NULL;
	*element = NULL;
	HRESULT hr = nodeList->get_item(num, &pNode);
	if (SUCCEEDED(hr))
	{
		*element = static_cast<IXMLDOMElement*>(pNode);
		if ( ! &element)
			hr = S_FALSE;
	}
	if (FAILED(hr))
	{
		DbgAssert(!_T("GetXMLElement failed!"));
	}
	return hr;
}

bool BPRFormatIO::XMLParseCurvePoints(const TSTR& nodePath, int contexParamID, FPValue& paramVal)
{
	XMLReadGuard guard(NULL);

	TSTR nodePointPath;
	nodePointPath.printf(nodePath + _T("[%d]/%ls"), contexParamID, POINT);
	HRESULT hr = m_pXMLDom->selectNodes(nodePointPath.ToBSTR(), &guard.pCurvePointNodes);
	if( ! guard.pCurvePointNodes || FAILED(hr) )
	{
		DbgAssert(!"Could not do XML query of Point nodes!");
		return false;
	}

	long curvePointNumber;
	hr = guard.pCurvePointNodes->get_length(&curvePointNumber);
	if ( FAILED(hr) || ! curvePointNumber )
	{
		DbgAssert(!"Could not do XML query of Point nodes!");
		return false;;
	}
	
	CurvePts* curve = new CurvePts();
	curve->SetNumPts( curvePointNumber );
	
	for (int i = 0; i < curvePointNumber; ++i)
	{
		XMLParamGuard paramGuard;

		if ( FAILED(GetXMLElement(guard.pCurvePointNodes, i, &paramGuard.pPointNode)) )
			continue;

		CHK_HR(paramGuard.pPointNode->get_attributes(&paramGuard.pAttributes));
		CurvePoint point;
		TSTR attrVal;

		CHK_HR(GetAttribute(paramGuard.pAttributes, X, attrVal));
		point.p.x = _tstof(attrVal.data());
		CHK_HR(GetAttribute(paramGuard.pAttributes, Y, attrVal));
		point.p.y = _tstof(attrVal.data());
		CHK_HR(GetAttribute(paramGuard.pAttributes, INX, attrVal));
		point.in.x = _tstof(attrVal.data());
		CHK_HR(GetAttribute(paramGuard.pAttributes, INY, attrVal));
		point.in.y = _tstof(attrVal.data());
		CHK_HR(GetAttribute(paramGuard.pAttributes, OUTX, attrVal));
		point.out.x = _tstof(attrVal.data());
		CHK_HR(GetAttribute(paramGuard.pAttributes, OUTY, attrVal));
		point.out.y = _tstof(attrVal.data());
		CHK_HR(GetAttribute(paramGuard.pAttributes, FLAGS, attrVal));
		point.flags = _tstoi(attrVal.data());

		curve->SetPoint( i, point);
	}

	paramVal.type = TYPE_INTERFACE;
	paramVal.fpi = curve;

	return hr == S_OK;
}

bool BPRFormatIO::ReadPresetsXML()
{
	HRESULT hr = CoInitialize(NULL);
	if ( ! SUCCEEDED(hr))
		return false;

	hr = S_OK;
	
	XMLReadGuard guard(m_pXMLDom);
	VARIANT_BOOL varStatus;
	
	m_bpManager->Reset();

	try
	{

		CHK_HR(CreateAndInitDOM());

		CHK_HR(VariantFromString(m_fileName, guard.varFileName));
		CHK_HR(m_pXMLDom->load(guard.varFileName, &varStatus));
		if (varStatus != VARIANT_TRUE)
		{
			DbgAssert(!_T("Failed to load DOM from brush preset XML file"));
			return false;
		}

		// Global settings
		TSTR query;
		query.printf(_T("//%s[1]"), BRUSHPRESETS_CONFIG);
		CHK_HR(m_pXMLDom->selectSingleNode(query.ToBSTR(), &guard.pNode));
		if ( ! guard.pNode)
		{
			DbgAssert(!"Could not do XML query of BrushPresets_Config node!");
			return false;
		}
		
		{
			XMLParamGuard paramGuard;
			CHK_HR(guard.pNode->get_attributes(&paramGuard.pAttributes));

			TSTR attrVal;
			CHK_HR(GetAttribute(paramGuard.pAttributes, ICONMAX, attrVal));
			SetIconMax( _tstoi(attrVal.data()) );

			CHK_HR(GetAttribute(paramGuard.pAttributes, ICONMIN, attrVal));
			SetIconMin( _tstoi(attrVal.data()) );
		}

		// Get all contexts

		query.printf(_T("//%s/*"), BRUSHPRESETS_CONFIG);
		CHK_HR(m_pXMLDom->selectNodes(query.ToBSTR(), &guard.pNodes));
		if( ! guard.pNodes)
		{
			DbgAssert(!"Could not do XML query of BrushPresets_Config node!");
			return false;
		}

		long length;
		CHK_HR(guard.pNodes->get_length(&length));
		for (long i = 0; i < length; ++i)
		{
			XMLParamGuard paramGuard;

			if ( FAILED(GetXMLElement(guard.pNodes, i, &paramGuard.pPresetNode)) )
				continue;
			
			CHK_HR(paramGuard.pPresetNode->get_attributes(&paramGuard.pAttributes));

			TSTR attrVal;

			CHK_HR(GetAttribute(paramGuard.pAttributes, CONTEXTID, attrVal));
			Class_ID contextID = StringToClassID( attrVal.data() );

			// bad contextID or already have info about context? don't create standin
			if( (contextID == Class_ID(0,0)) || (m_bpManager->GetContextIndex(contextID) >= 0) )
			{
				continue;
			}

			TSTR contextName;
			CHK_HR(GetAttribute(paramGuard.pAttributes, NAME, contextName));

			CHK_HR(GetAttribute(paramGuard.pAttributes, PLUGINCLASSID, attrVal));
			Class_ID pluginClassID = StringToClassID( attrVal.data() );

			CHK_HR(GetAttribute(paramGuard.pAttributes, PLUGINSUPERCLASSID, attrVal));
			SClass_ID pluginSClassID = StringToDWORD( attrVal.data() );

			CreateStandinContext( contextID, contextName, pluginClassID, pluginSClassID );
		}
		SAFE_RELEASE(guard.pNodes);
		
		// Preset List

		query.printf(_T("//%s/%s"), PRESETLIST, PRESET); // select only Preset children of the PresetList node
		CHK_HR(m_pXMLDom->selectNodes(query.ToBSTR(), &guard.pNodes));
		if( ! guard.pNodes)
		{
			DbgAssert(!"Could not do XML query of PresetList node!");
			SAFE_RELEASE(guard.pNodes);
			return false;
		}
		CHK_HR(guard.pNodes->get_length(&length));

		// For each preset, find one section in the config file
		for (long i = 0; i < length; ++i)
		{
			StdBrushPreset* preset = m_bpManager->CreateStdPreset( i+1 );

			// select all Context children of the current preset node
			TSTR queryPreset;
			queryPreset.printf(query + _T("[%d]/%s"), i+1, CONTEXT); 

			XMLParamGuard paramGuard;

			CHK_HR(m_pXMLDom->selectNodes(queryPreset.ToBSTR(), &paramGuard.pContextNodes));
			if( ! paramGuard.pContextNodes )
			{
				DbgAssert(!"Could not do XML query of Preset context node!");
				return false;
			}

			long contextNodesLength = 0;
			CHK_HR(paramGuard.pContextNodes->get_length(&contextNodesLength));

			for (long j = 0; j < contextNodesLength; ++j)
			{
				XMLParamGuard paramGuard03;
				
				if ( FAILED(GetXMLElement(paramGuard.pContextNodes, j, &paramGuard03.pContextNode)) )
					continue;

				CHK_HR(paramGuard03.pContextNode->get_attributes(&paramGuard03.pAttributes));
				
				TSTR attrVal;
				Class_ID contextID = Class_ID(0,0);
				CHK_HR(GetAttribute(paramGuard03.pAttributes, CONTEXTID, attrVal));
				contextID = StringToClassID(attrVal.data());
				
				StandinBrushPresetParams* standin = NULL;
				if (contextID != Class_ID(0, 0))
				{
					standin = preset->CreateStandinParams( contextID );
					preset->AddParams( standin );
				}
				else
				{
					DbgAssert(!_T("How come we do not have a standin!"));
				}

				// select all Context children of the current preset node
				TSTR queryParams;
				queryParams.printf(queryPreset + _T("[%d]/%s"), j+1, CONTEXT_PARAM); 

				CHK_HR(m_pXMLDom->selectNodes(queryParams.ToBSTR(), &paramGuard.pContextParamNodes));
				if( ! paramGuard.pContextParamNodes )
				{
					DbgAssert(!"Could not do XML query of Preset context param nodes!");
					return false;
				}
				long contextParamNodesLength = 0;
				CHK_HR(paramGuard.pContextParamNodes->get_length(&contextParamNodesLength));
				for (long k = 0; k < contextParamNodesLength; ++k)
				{

					XMLParamGuard paramGuard02;

					if ( FAILED(GetXMLElement(paramGuard.pContextParamNodes, k, &paramGuard02.pContextParamNode)) )
						continue;

					TSTR paramName, paramType, paramValue;
					CHK_HR(paramGuard02.pContextParamNode->get_attributes(&paramGuard02.pAttributes));
					CHK_HR(GetAttribute(paramGuard02.pAttributes, NAME, paramName));
					CHK_HR(GetAttribute(paramGuard02.pAttributes, TYPE, paramType));
					CHK_HR(GetAttribute(paramGuard02.pAttributes, VALUE, paramValue));


					// See if we have any children
					if (paramType == PRESETCFG_TYPE_CURVEPTS)
					{
						if (!XMLParseCurvePoints(queryParams, k+1, paramGuard02.fpValue))
						{
							DbgAssert(!_T("Could not get XML curve point nodes!"));
							break;
						}
					}
					else if (paramType == PRESETCFG_TYPE_COLOR)
					{
						paramGuard02.fpValue.clr = new Color;
						TSTR attrVal;
						CHK_HR(GetAttribute(paramGuard02.pAttributes, RED, attrVal));
						paramGuard02.fpValue.clr->r = _tstof(attrVal.data());
						CHK_HR(GetAttribute(paramGuard02.pAttributes, GREEN, attrVal));
						paramGuard02.fpValue.clr->g = _tstof(attrVal.data());
						CHK_HR(GetAttribute(paramGuard02.pAttributes, BLUE, attrVal));
						paramGuard02.fpValue.clr->b = _tstof(attrVal.data());
					}
					else
					{
						FillFPValue(paramType.data(), paramValue.data(), &paramGuard02.fpValue);
					}

					if (standin)
						standin->AddParam( k+1, paramGuard02.fpValue );
				}
			}

			// If there was an error loading the preset, delete it.
			if( FAILED(hr) && preset != NULL ) 
				m_bpManager->DeletePreset( preset );

		}
	}
	catch(XmlFailedException)
	{
		DbgAssert( ! _T("Something is wrong with BPR XML reading") );
		return false;
	}
	
	CoUninitialize();
	return true;
}

bool BPRFormatIO::ReadPresetsProp()
{
	if (!DetectEncoding())
		return false;

	TCHAR cfgBuf[1024] = {0};
	TCHAR key[1024] = {0};
	TCHAR name[1024] = {0};
	TCHAR contextSection[1024] = {0}, presetSection[1024] = {0}, contextName[1024] = {0};
	TCHAR* mgrSection = BRUSHPRESETS_CONFIG;

	m_bpManager->Reset();

	TCHAR* prevLocale = _tsetlocale(LC_ALL, NULL); //Get and copy the locale string
	if( prevLocale!=NULL ) prevLocale = _tcsdup(prevLocale);
	_tsetlocale(LC_ALL, _T("C")); //Set the new locale

	int numPresets = 0, numContexts = 0;
	BOOL ok = GetEncodedPrivateProfileString( mgrSection, PRESETCFG_NUMPRESETS, _T(""), key, _countof(key), m_fileName );
	if( ok ) numPresets = StringToInt(key);

	ok = GetEncodedPrivateProfileString( mgrSection, PRESETCFG_NUMCONTEXTS, _T(""), key, _countof(key), m_fileName );
	if( ok ) numContexts = StringToInt(key);

	ok = GetEncodedPrivateProfileString( mgrSection, PRESETCFG_ICONMIN, _T(""), key, _countof(key), m_fileName );
	if( ok ) SetIconMin(StringToFloat( key ));
	ok = GetEncodedPrivateProfileString( mgrSection, PRESETCFG_ICONMAX, _T(""), key, _countof(key), m_fileName );
	if( ok ) SetIconMax( StringToFloat( key ) );

	for( int i=0; (i<numContexts) && ok; i++ ) {
		_stprintf( contextSection, _T("Context%d"), i+1 );

		Class_ID contextID = Class_ID(0,0);
		Class_ID pluginClassID = Class_ID(0,0);
		SClass_ID pluginSClassID = 0;

		ok = GetEncodedPrivateProfileString( contextSection, PRESETCFG_CONTEXTID, _T(""), key, _countof(key), m_fileName );
		if( ok ) contextID = StringToClassID( key );

		// bad contextID or already have info about context? don't create standin
		if( (contextID==Class_ID(0,0)) || (m_bpManager->GetContextIndex(contextID)>=0) || !ok ) continue;

		ok = GetEncodedPrivateProfileString( contextSection, PRESETCFG_CONTEXTNAME, _T(""), key, _countof(key), m_fileName );
		if( ok ) ReadToken( key, contextName, _T('\0') );

		ok = GetEncodedPrivateProfileString( contextSection, PRESETCFG_PLUGINCLASSID, _T(""), key, _countof(key), m_fileName );
		if( ok ) pluginClassID = StringToClassID( key );

		ok = GetEncodedPrivateProfileString( contextSection, PRESETCFG_PLUGINSUPERCLASSID, _T(""), key, _countof(key), m_fileName );
		if( ok ) pluginSClassID = StringToDWORD( key );

		if( ok ) StandinBrushPresetContext* standin = CreateStandinContext( contextID, contextName, pluginClassID, pluginSClassID );
	}

	// For each preset, find one section in the config file
	for( int i=0; (i<numPresets) && ok; i++ ) {
		StdBrushPreset* preset = NULL;
		int numContexts = -1;
		_stprintf( presetSection, _T("Preset%d"), i+1 );

		// Find entries for the presetID, presetName, presetButtonPos, and number of parameter sets (contexts)
		ok = GetEncodedPrivateProfileString( presetSection, PRESETCFG_PRESETID, _T(""), key, _countof(key), m_fileName );
		if( ok ) {
			int presetID = StringToInt( key );
			preset = m_bpManager->CreateStdPreset( presetID );
		}

		ok = GetEncodedPrivateProfileString( presetSection, PRESETCFG_NUMCONTEXTS, _T(""), key, _countof(key), m_fileName );
		if( ok ) numContexts = StringToInt( key );

		for( int j=0; (j<numContexts) && ok; j++ ) {
			TCHAR paramName[1024] = {0}, paramValueStr[1024] = {0};
			StandinBrushPresetParams* standin = NULL;
			Class_ID contextID = Class_ID(0,0);
			int numParams = -1;

			_stprintf( name, _T("Context%d"), j+1 );
			if( !GetEncodedPrivateProfileString( presetSection, name, _T(""), key, _countof(key), m_fileName ) )
				continue; //preset has no params defined for the context

			// Context entry indicates the contextID, contextName and number or params...
			ok = ReadCfgContextString( key, &contextID, contextName, &numParams );
			if( ok ) {
				Class_ID pluginClassID(0,0);
				SClass_ID pluginSClassID(0);
				standin = preset->CreateStandinParams( contextID );
				preset->AddParams( standin );
			}

			for( int k=0; (k<numParams) && ok; k++ ) {
				int paramID;
				FPValue paramVal;

				_stprintf( name, _T("Context%d_Param%d"), j+1, k+1 );

				// Param entry indicates the paramID, paramName, and the param value as a quoted string
				if( !GetEncodedPrivateProfileString( presetSection, name, _T(""), key, _countof(key), m_fileName ) )
					break; // error

				ok = ReadCfgParamString( key, &paramID, paramName, paramValueStr );
				if( ok ) {
					StringToFPValue( paramValueStr, &paramVal );
					standin->AddParam( paramID, paramVal );
					ReleaseIfInterface( paramVal );
				}
			}
		}

		// If there was an error loading the preset, delete it.
		if( (!ok) && (preset!=NULL) ) m_bpManager->DeletePreset( preset );
	}

	_tsetlocale(LC_ALL, prevLocale); //Restore previous locale
	if( prevLocale!=NULL ) free(prevLocale);

	return ok != FALSE;
}


bool BPRFormatIO::DetectEncoding()
{
	const UINT codePage = MaxSDK::Util::CodePageForLanguage(MaxSDK::Util::GetLocaleValue());
	MaxSDK::Util::TextFile::Reader reader;
	bool res = reader.Open( m_fileName, codePage );
	if (res)
	{
		mReadEncoding = reader.Encoding() & MaxSDK::Util::MSDE_CP_MASK;
		if ( mReadEncoding != codePage && mReadEncoding != MaxSDK::Util::MSDE_CP_UTF16 ) 
		{
			// We got an encoding we do not support
			DbgAssert(mReadEncoding == codePage || mReadEncoding == MaxSDK::Util::MSDE_CP_UTF16);
			reader.Close();
			return false;
		}
	}
	reader.Close();
	return res;
}

DWORD BPRFormatIO::GetEncodedPrivateProfileString(LPCTSTR lpAppName, 
	LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedString, 
	DWORD nSize, LPCTSTR lpFileName)
{
	TCHAR value[256] = {0};
	DWORD res = GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, value, _countof(value), lpFileName);

	if ( mReadEncoding == MaxSDK::Util::CodePageForLanguage(MaxSDK::Util::GetLocaleValue()) ) //CP_ACP
	{
#ifdef _UNICODE
		_tcscpy_s(lpReturnedString, nSize, TSTR::FromACP( TSTR(value).ToACP() ).data()); // we convert from ANSI wchar to UTF16 wchar
#else
		_tcscpy_s(lpReturnedString, nSize, value);
#endif
	}
	else if (mReadEncoding == MaxSDK::Util::MSDE_CP_UTF16) // Unicode
	{
#ifdef _UNICODE
		_tcscpy_s(lpReturnedString, nSize, value);
#else
		wchar_t realValue[1024] = {0};
		WStr lpAppNameW( WStr::FromACP(lpAppName) );
		WStr lpKeyNameW( WStr::FromACP(lpKeyName) );
		WStr lpDefaultW( WStr::FromACP(lpDefault) );
		WStr lpFileNameW( WStr::FromACP(lpFileName) );

		res = GetPrivateProfileStringW(lpAppNameW.data(), lpKeyNameW.data(), lpDefaultW.data(), realValue, 1000, lpFileNameW.data());

		_tcscpy_s(lpReturnedString, nSize, TSTR::FromUTF16( realValue ).data()); // we convert from UTF16 wchar to ANSI char
#endif
	}
	else 
	{
		DbgAssert(!_T("Could not detect a encoding of BPR file!"));
		res = 0;
	}
	return res;
}


HRESULT BPRFormatIO::CreateXMLElement(const TSTR& name, IXMLDOMElement **ppElement)
{
	HRESULT hr = S_OK;
	*ppElement = NULL;
	BSTR bstrName = NULL;
	try
	{
		bstrName = SysAllocString(name.ToOLESTR());
		CHK_ALLOC(bstrName);
		CHK_HR(m_pXMLDom->createElement(bstrName, ppElement));
	}
	catch(XmlFailedException)
	{	
		SysFreeString(bstrName);
		return hr;
	}
	SysFreeString(bstrName);
	return hr;
}

HRESULT BPRFormatIO::CreateAndAddPINode(const TSTR& target, const TSTR& strData)
{
	HRESULT hr = S_OK;
	IXMLDOMProcessingInstruction *pPI = NULL;
	BSTR bstrTarget = NULL;
	BSTR bstrData = NULL;
	try
	{
		bstrTarget = SysAllocString(target.ToOLESTR());
		bstrData = SysAllocString(strData.ToOLESTR());
		CHK_ALLOC(bstrTarget && bstrData);

		CHK_HR(m_pXMLDom->createProcessingInstruction(bstrTarget, bstrData, &pPI));
		CHK_HR(AppendChildToParent(pPI, m_pXMLDom));
	}
	catch(XmlFailedException)
	{
		SAFE_RELEASE(pPI);
		SysFreeString(bstrTarget);
		SysFreeString(bstrData);
		return hr;
	}
	SAFE_RELEASE(pPI);
	SysFreeString(bstrTarget);
	SysFreeString(bstrData);
	return hr;
}

HRESULT BPRFormatIO::CreateAndAddTextNode(const TSTR& text, IXMLDOMNode *pParent)
{
	HRESULT hr = S_OK;    
	IXMLDOMText *pText = NULL;

	BSTR bstrText = NULL;

	try
	{
		bstrText = SysAllocString(text.ToOLESTR());
		CHK_ALLOC(bstrText);

		CHK_HR(m_pXMLDom->createTextNode(bstrText, &pText));
		CHK_HR(AppendChildToParent(pText, pParent));
	}
	catch(XmlFailedException)
	{
		SAFE_RELEASE(pText);
		SysFreeString(bstrText);
		return hr;
	}
	SAFE_RELEASE(pText);
	SysFreeString(bstrText);
	return hr;
}

HRESULT BPRFormatIO::AppendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent)
{
	HRESULT hr = S_OK;
	IXMLDOMNode *pChildOut = NULL;
	try
	{
		CHK_HR(pParent->appendChild(pChild, &pChildOut));
	}
	catch(XmlFailedException)
	{
		SAFE_RELEASE(pChildOut);
		return hr;
	}
	SAFE_RELEASE(pChildOut);
	return hr;
}

HRESULT BPRFormatIO::CreateAndAddAttributeNode(const TSTR& name, const TSTR& value, IXMLDOMElement *pParent)
{
	HRESULT hr = S_OK;
	IXMLDOMAttribute *pAttribute = NULL;
	IXMLDOMAttribute *pAttributeOut = NULL; // Out param that is not used

	BSTR bstrName = NULL;
	VARIANT varValue;
	VariantInit(&varValue);

	try
	{
		bstrName = SysAllocString(name.ToOLESTR());
		CHK_ALLOC(bstrName);
		CHK_HR(VariantFromString(value, varValue));

		CHK_HR(m_pXMLDom->createAttribute(bstrName, &pAttribute));
		CHK_HR(pAttribute->put_value(varValue));
		CHK_HR(pParent->setAttributeNode(pAttribute, &pAttributeOut));
	}
	catch(XmlFailedException)
	{
		SAFE_RELEASE(pAttribute);
		SAFE_RELEASE(pAttributeOut);
		SysFreeString(bstrName);
		VariantClear(&varValue);
		return hr;
	}
	SAFE_RELEASE(pAttribute);
	SAFE_RELEASE(pAttributeOut);
	SysFreeString(bstrName);
	VariantClear(&varValue);
	return hr;
}

HRESULT BPRFormatIO::CreateAndAddAttributeNode(const TSTR& name, float value, IXMLDOMElement *pParent)
{
	TSTR strValue;
	strValue.printf(_T("%.3f"), value);
	return CreateAndAddAttributeNode(name, strValue, pParent);
}

HRESULT BPRFormatIO::CreateAndAddAttributeNode(const TSTR& name, int value, IXMLDOMElement *pParent)
{
	TSTR strValue;
	strValue.printf(_T("%d"), value);
	return CreateAndAddAttributeNode(name, strValue, pParent);
}

HRESULT BPRFormatIO::CreateAndAddElementNode(const TSTR& name, const TSTR& newline, IXMLDOMNode *pParent, IXMLDOMElement **ppElement)
{
	HRESULT hr = S_OK;
	IXMLDOMElement* pElement = NULL;

	try
	{
		CHK_HR(CreateXMLElement(name, &pElement));
		// Add NEWLINE+TAB for identation before this element.
		CHK_HR(CreateAndAddTextNode(newline.data(), pParent));
		// Append this element to parent.
		CHK_HR(AppendChildToParent(pElement, pParent));
	}
	catch(XmlFailedException)
	{
		if (ppElement)
			*ppElement = pElement;  // Caller is repsonsible to release this element.
		else
			SAFE_RELEASE(pElement); // Caller is not interested on this element, so release it.
		return hr;
	}
	if (ppElement)
		*ppElement = pElement;  // Caller is repsonsible to release this element.
	else
		SAFE_RELEASE(pElement); // Caller is not interested on this element, so release it.

	return hr;
}

HRESULT BPRFormatIO::FPValueToXML( FPValue& val, IXMLDOMElement *pParent )
{
	HRESULT hr = S_OK;

	XMLParamGuard guard;

	try
	{
		switch( val.type ) 
		{
			case TYPE_WORLD:
			case TYPE_FLOAT:		CHK_HR(CreateAndAddAttributeNode(VALUE, val.f, pParent));
				break;
			case TYPE_INT:
			case TYPE_BOOL:		CHK_HR(CreateAndAddAttributeNode(VALUE, val.i, pParent));
				break;
			case TYPE_POINT3_BV:
			case TYPE_POINT3:
			case TYPE_COLOR_BV:
			case TYPE_COLOR:		
				{
					CHK_HR(CreateAndAddAttributeNode(RED, val.p->x, pParent));
					CHK_HR(CreateAndAddAttributeNode(GREEN, val.p->y, pParent));
					CHK_HR(CreateAndAddAttributeNode(BLUE, val.p->z, pParent));
					break;
				}
			case TYPE_TSTR_BV:
			case TYPE_TSTR:		CHK_HR(CreateAndAddAttributeNode(VALUE, val.tstr->data(), pParent));
				break;
			case TYPE_STRING:		CHK_HR(CreateAndAddAttributeNode(VALUE, val.s, pParent));
				break;
			case TYPE_INTERFACE:
				{
					CurvePts* curve = GetCurvePtsInterface( &val );
					if (!curve)
					{
						DbgAssert(!_T("Curve is NULL for TYPE_INTERFACE type!"));
						break;
					}
					for (int k=0; k < curve->GetNumPts(); ++k)
					{
						CHK_HR(CreateAndAddElementNode(POINT, _T("\n\t\t"), pParent, &guard.pPointNode));
						const CurvePoint& point = curve->GetPoint(k);
						CHK_HR(CreateAndAddAttributeNode(X, point.p.x, guard.pPointNode));
						CHK_HR(CreateAndAddAttributeNode(INX, point.in.x, guard.pPointNode));
						CHK_HR(CreateAndAddAttributeNode(OUTX, point.out.x, guard.pPointNode));
						CHK_HR(CreateAndAddAttributeNode(Y, point.p.y, guard.pPointNode));
						CHK_HR(CreateAndAddAttributeNode(INY, point.in.y, guard.pPointNode));
						CHK_HR(CreateAndAddAttributeNode(OUTY, point.out.y, guard.pPointNode));
						CHK_HR(CreateAndAddAttributeNode(FLAGS, point.flags, guard.pPointNode));
					}
					CHK_HR(CreateAndAddTextNode(NEW_LINE_TAB, pParent));
				}
				break;
			default:
				DbgAssert(!_T("Unknown type!"));
				break;
		} // switch
	} // try
	catch(XmlFailedException)
	{
		DbgAssert(!"Got problems with FPValueToXML!");
		return hr;
	}
	return hr;
}

bool BPRFormatIO::WritePresetsXML()
{
	HRESULT hr = CoInitialize(NULL);
	if ( ! SUCCEEDED(hr))
		return false;

	hr = S_OK;
	
	XMLWriteGuard guard(m_pXMLDom);

	try
	{
		CHK_HR(CreateAndInitDOM());

		// Create a processing instruction element.
		CHK_HR(CreateAndAddPINode(_T("xml"), _T("version=\"1.0\" encoding=\"utf-8\"")));

		// Create the root element.
		CHK_HR(CreateXMLElement(ADSK_BPR_ROOT, &guard.pRoot));

		// BrushPresets_Config
		CHK_HR(CreateAndAddElementNode(BRUSHPRESETS_CONFIG, NEW_LINE_TAB, guard.pRoot, &guard.pNode));
		CHK_HR(CreateAndAddAttributeNode(ICONMIN, IconMinTxt().data(), guard.pNode));
		CHK_HR(CreateAndAddAttributeNode(ICONMAX, IconMaxTxt().data(), guard.pNode));

		//Context
		const int numContexts = m_bpManager->GetNumContexts();
		for (int i = 0; i < numContexts; ++i)
		{
			IBrushPresetContext* context = m_bpManager->GetContext( i ); // Context or standin
			if( ! context ) 
				continue;

			XMLParamGuard paramGuard;

			CHK_HR(CreateAndAddElementNode(CONTEXT, NEW_LINE_TAB, guard.pNode, &paramGuard.pContextNode));

			CHK_HR(CreateAndAddAttributeNode(NAME, context->GetContextName(), paramGuard.pContextNode));

			Class_ID contextID = context->ContextID();
			TCHAR strValue[MAX_PATH];
			ClassIDToString( contextID, strValue );
			CHK_HR(CreateAndAddAttributeNode(CONTEXTID, strValue, paramGuard.pContextNode));

			Class_ID pluginClassID = context->PluginClassID();
			ClassIDToString( pluginClassID, strValue );
			CHK_HR(CreateAndAddAttributeNode(PLUGINCLASSID, strValue, paramGuard.pContextNode));

			SClass_ID pluginSClassID = context->PluginSuperClassID();
			_stprintf( strValue, _T("%u"), pluginSClassID );
			CHK_HR(CreateAndAddAttributeNode(PLUGINSUPERCLASSID, strValue, paramGuard.pContextNode));
		}

		SAFE_RELEASE(guard.pNode);

		// PresetList
		CHK_HR(CreateAndAddElementNode(PRESETLIST, NEW_LINE_TAB, guard.pRoot, &guard.pNode));

		const int numPresets = m_bpManager->GetNumPresets();
		// Preset
		for(int i = 0; i < numPresets; ++i)
		{
			StdBrushPreset* preset = m_bpManager->GetStdPreset( m_bpManager->GetPresetID(i) );
			if( ! preset ) 
				continue;

			CHK_HR(CreateAndAddElementNode(PRESET, NEW_LINE_TAB, guard.pNode, &guard.pPresetNode));

			for (int j = 0; j < numContexts; ++j)
			{
				Class_ID contextID = preset->GetContextID( j );

				XMLParamGuard paramGuard;

				CHK_HR(CreateAndAddElementNode(CONTEXT, NEW_LINE_TAB, guard.pPresetNode, &paramGuard.pContextNode));
				TCHAR strValue[MAX_PATH] = {0};
				ClassIDToString( contextID, strValue );
				CHK_HR(CreateAndAddAttributeNode(CONTEXTID, strValue, paramGuard.pContextNode));
				
				IBrushPresetContext* context = m_bpManager->GetContext( m_bpManager->GetContextIndex(contextID) );
				IBrushPresetParams* params = preset->GetParams( preset->GetContextIndex(contextID) );
				if( params ) 
				{
					const int numParams = params->GetNumParams();

					for( int k=0; k<numParams; ++k )
					{
						const int paramID = params->GetParamID( k );
					
						if( context->IsTransientParam( paramID ) )
							continue; //transient param; do not write param or increment the entryIndex

						XMLParamGuard paramGuard02;

						CHK_HR(CreateAndAddElementNode(CONTEXT_PARAM, NEW_LINE_TAB, 
							paramGuard.pContextNode, &paramGuard02.pContextParamNode));
					
						CHK_HR(CreateAndAddAttributeNode(NAME, context->GetParamName( paramID ), 
							paramGuard02.pContextParamNode));

						FPValue val = params->GetParamValue( paramID );

						TCHAR strType[MAX_PATH] = {0};
						ParamType2ToString(&val, strType);
						CHK_HR(CreateAndAddAttributeNode(TYPE, strType, paramGuard02.pContextParamNode));
					
						FPValueToXML( val, paramGuard02.pContextParamNode );
					}
				}

				CHK_HR(CreateAndAddTextNode(NEW_LINE_TAB, paramGuard.pContextNode));
			}
			CHK_HR(CreateAndAddTextNode(NEW_LINE_TAB, guard.pPresetNode));
		}

		// Add NEWLINE for identation before </root>.
		CHK_HR(CreateAndAddTextNode(NEW_LINE, guard.pRoot));
		CHK_HR(AppendChildToParent(guard.pRoot, m_pXMLDom));

		CHK_HR(VariantFromString(m_fileName, guard.varFileName));
		CHK_HR(m_pXMLDom->save(guard.varFileName));

	}
	catch(XmlFailedException)
	{
		DbgAssert( ! _T("Something is wrong with BPR XML writing") );
		return false;
	}
	return true;
}

bool BPRFormatIO::WritePresetsProp()
{
	TCHAR cfgBuf[1024] = {0};
	TCHAR key[1024] = {0};
	TCHAR name[1024] = {0};
	TCHAR contextSection[1024] = {0}, presetSection[1024] = {0}, paramValueStr[1024] = {0};
	TCHAR* mgrSection = BRUSHPRESETS_CONFIG;

	TCHAR* prevLocale = _tsetlocale(LC_ALL, NULL); //Get and copy the locale string
	if( prevLocale!=NULL ) prevLocale = _tcsdup(prevLocale);
	_tsetlocale(LC_ALL, _T("C")); //Set the new locale

	const int numPresets = m_bpManager->GetNumPresets();
	const int numContexts = m_bpManager->GetNumContexts();

	_stprintf( key, _T("%d"), numPresets );
	MaxSDK::Util::WritePrivateProfileString( mgrSection, PRESETCFG_NUMPRESETS, key, m_fileName );

	_stprintf( key, _T("%d"), numContexts );
	MaxSDK::Util::WritePrivateProfileString( mgrSection, PRESETCFG_NUMCONTEXTS, key, m_fileName );

	MaxSDK::Util::WritePrivateProfileString( mgrSection, PRESETCFG_ICONMIN, IconMinTxt().data(), m_fileName );
	
	MaxSDK::Util::WritePrivateProfileString( mgrSection, PRESETCFG_ICONMAX, IconMaxTxt().data(), m_fileName );

	for( int i=0; i<numContexts; i++ ) 
	{
		IBrushPresetContext* context = m_bpManager->GetContext( i ); // Context or standin
		if( context==NULL ) continue;
		_stprintf( contextSection, _T("Context%d"), i+1 );

		_stprintf( key, _T("\"%s\""), context->GetContextName() );
		MaxSDK::Util::WritePrivateProfileString( contextSection, PRESETCFG_CONTEXTNAME, key, m_fileName );

		Class_ID contextID = context->ContextID();
		ClassIDToString( contextID, key );
		MaxSDK::Util::WritePrivateProfileString( contextSection, PRESETCFG_CONTEXTID, key, m_fileName );

		Class_ID pluginClassID = context->PluginClassID();
		ClassIDToString( pluginClassID, key );
		MaxSDK::Util::WritePrivateProfileString( contextSection, PRESETCFG_PLUGINCLASSID, key, m_fileName );

		SClass_ID pluginSClassID = context->PluginSuperClassID();
		_stprintf( key, _T("%u"), pluginSClassID );
		MaxSDK::Util::WritePrivateProfileString( contextSection, PRESETCFG_PLUGINSUPERCLASSID, key, m_fileName );
	}

	// For each preset, create one section in the config file
	for( int i=0; i<numPresets; i++ ) 
	{
		StdBrushPreset* preset = m_bpManager->GetStdPreset( m_bpManager->GetPresetID(i) );
		if( preset==NULL ) continue;

		_stprintf( presetSection, _T("Preset%d"), i+1 );

		// Create entries for the presetID, presetName, presetButtonPos, and number of parameter sets (contexts)
		_stprintf( key, _T("%d"), preset->PresetID() );
		MaxSDK::Util::WritePrivateProfileString( presetSection, PRESETCFG_PRESETID, key, m_fileName );

		int numContexts = preset->GetNumContexts();
		_stprintf( key, _T("%d"), numContexts );
		MaxSDK::Util::WritePrivateProfileString( presetSection, PRESETCFG_NUMCONTEXTS, key, m_fileName );

		DbgAssert( numContexts>1 );

		// Create entries for each set of params held by the preset (each context)
		for( int j=0; j<numContexts; j++ ) 
		{
			// TO DO: handling for context and preset names with Quotes and Slashes in the name and preset value
			Class_ID contextID = preset->GetContextID( j );
			IBrushPresetContext* context = m_bpManager->GetContext( m_bpManager->GetContextIndex(contextID) );
			IBrushPresetParams* params = preset->GetParams( preset->GetContextIndex(contextID) );
			if( params==NULL ) continue;

			const TCHAR* contextName = context->GetContextName();
			int numParams = params->GetNumParams();

			int numEntries = 0;
			for( int k=0; k<numParams; k++ )
				if( !context->IsTransientParam( params->GetParamID( k ) ) )
					numEntries++; //one entry for each non-transient parameter

			_stprintf( name, _T("Context%d"), j+1 );
			_stprintf( key, _T("(0x%lX,0x%lX)|\"%s\"|%d"), contextID.PartA(), contextID.PartB(), contextName, numEntries );

			// Context entry indicates the contextID, contextName and number or params...
			MaxSDK::Util::WritePrivateProfileString( presetSection, name, key, m_fileName );
			// ...Then one entry describing each of params in this context
			int entryIndex = 0;
			for( int k=0; k<numParams; k++ ) 
			{
				int paramID = params->GetParamID( k );
				if( context->IsTransientParam( paramID ) )
					continue; //transient param; do not write param or increment the entryIndex
				const TCHAR* paramName = context->GetParamName( paramID );
				//BOOL paramToggle = params->GetParamToggle( paramID );
				//TCHAR* paramToggleStr = (paramToggle? PRESETCFG_ENABLED : PRESETCFG_DISABLED );
				FPValue val = params->GetParamValue( paramID );
				FPValueToString( &val, paramValueStr );
				_stprintf( name, _T("Context%d_Param%d"), j+1, entryIndex+1 );
				// Param entry indicates the paramID, paramName, and the param value as a quoted string
				_stprintf( key, _T("%d|\"%s\"|\"%s\""), paramID, paramName, paramValueStr );
				//_stprintf( key, _T("%d|\"%s\"|%s|\"%s\""), paramID, paramName, paramToggleStr, paramValueStr );
				MaxSDK::Util::WritePrivateProfileString( presetSection, name, key, m_fileName );
				entryIndex++;
			}
		}
	}

	_tsetlocale(LC_ALL, prevLocale); //Restore previous locale
	if( prevLocale!=NULL ) free(prevLocale);

	return true;
}



//-----------------------------------------------------------------------------
//-- CUIFramePtr - Smart pointer

class CUIFramePtr {
public:
	ICUIFrame* frame;
	CUIFramePtr( HWND hwnd )						{ frame = (hwnd==NULL? NULL : GetICUIFrame(hwnd)); }
	CUIFramePtr( ICUIFrame* frame )					{ this->frame = frame; }
	~CUIFramePtr()									{ if( frame!=NULL ) ReleaseICUIFrame( frame ); }
	ICUIFrame* operator->()							{ return frame; }
	ICUIFrame* operator=( ICUIFrame* ptr )			{ frame=ptr; return frame; }
	BOOL operator==( ICUIFrame* ptr )				{ return frame==ptr; }
	BOOL operator!=( ICUIFrame* ptr )				{ return frame!=ptr; }
};


//-----------------------------------------------------------------------------
//-- ToolbarPtr - Smart pointer

class ToolbarPtr {
public:
	ICustToolbar* toolbar;
	ToolbarPtr( HWND hwnd )							{ toolbar = (hwnd==NULL? NULL : GetICustToolbar(hwnd)); }
	ToolbarPtr( ICustToolbar* toolbar )				{ this->toolbar = toolbar; }
	~ToolbarPtr()									{ if( toolbar!=NULL ) ReleaseICustToolbar( toolbar ); }
	ICustToolbar* operator->()						{ return toolbar; }
	ICustToolbar* operator=( ICustToolbar* ptr )	{ toolbar=ptr; return toolbar; }
	BOOL operator==( ICustToolbar* ptr )			{ return toolbar==ptr; }
	BOOL operator!=( ICustToolbar* ptr )			{ return toolbar!=ptr; }
};


//-----------------------------------------------------------------------------
//-- ButtonPtr - Smart pointer

class ButtonPtr {
public:
	ICustButton* button;
	ButtonPtr( HWND hwnd )							{ button = (hwnd==NULL? NULL : GetICustButton(hwnd)); }
	ButtonPtr( ICustButton* button )				{ this->button=button; }
	~ButtonPtr()									{ if( button!=NULL ) ReleaseICustButton( button ); }
	ICustButton* operator->()						{ return button; }
	ICustButton* operator=( ICustButton* ptr )		{ button=ptr; return button; }
	BOOL operator==( ICustButton* ptr )				{ return button==ptr; }
	BOOL operator!=( ICustButton* ptr )				{ return button!=ptr; }
};


//-----------------------------------------------------------------------------
// class StandinBrushPresetParams

StandinBrushPresetParams::StandinBrushPresetParams( Class_ID contextID, BrushPresetMgr* mgr )
{
	this->mgr = mgr;
	this->parent = NULL;
	this->contextID = contextID;
}

StandinBrushPresetParams::~StandinBrushPresetParams() {
	int count = GetNumParams();
	for( int i=0; i < count; i++ ) {
		ReleaseIfInterface( paramList[i]->val );
		delete paramList[i]; 
		paramList[i] = NULL;
	}
	paramList.SetCount(0);
}

int StandinBrushPresetParams::GetParamIndex( int paramID ) {
	int count = GetNumParams();
	for( int i=0; i<count; i++ )
		if( paramList[i]->paramID == paramID ) return i;
	return -1;
}

void StandinBrushPresetParams::AddParam( int paramID, FPValue val ) {
	int index = GetParamIndex( paramID );
	if( index < 0 || index >= paramList.Count() ) {
		index = paramList.Count();
		ParamItem* paramItem = new ParamItem;
		paramItem->paramID = paramID;
		paramList.Append( 1, &paramItem );
	}
	ParamItem* paramItem = paramList[index];
	ReleaseIfInterface(paramItem->val);
	paramItem->val = val;
	AcquireIfInterface(paramItem->val);
}

IBrushPresetParams*	StandinBrushPresetParams::ResolveParams() {
	IBrushPresetContext* context = (mgr->GetContext(contextID));
	if( context==NULL ) return NULL;

	mgr->SetIgnoreUpdates(TRUE);
	IBrushPresetParams* params = context->CreateParams();
	DbgAssert(NULL != params);
	if (params == NULL) return NULL;

	params->SetParent( this->parent );
	int count = GetNumParams();
	for( int i=0; i<count; i++ )
		params->SetParamValue( paramList[i]->paramID, paramList[i]->val );
	mgr->SetIgnoreUpdates(FALSE);
	return params;
}


//-----------------------------------------------------------------------------
// class StandinBrushPresetContext

StandinBrushPresetContext::StandinBrushPresetContext(
	Class_ID contextID, const TCHAR* name, Class_ID pluginClassID, SClass_ID pluginSClassID )
{
	this->contextID = contextID;
	this->pluginClassID = pluginClassID;
	this->pluginSClassID = pluginSClassID;
	this->name = name;
}

StandinBrushPresetContext::~StandinBrushPresetContext()
{

}

//-----------------------------------------------------------------------------
// Brush Param Descriptors

// NOTE: Enum ordering must match the paramDesc ordering
enum {
	paramFalloffCurve=1,
	paramName,
	paramMinSize, paramMaxSize,
	paramMinStr, paramMaxStr,
	paramPressureSenseOn, paramPressureAffects,
	paramMirrorOn, paramMirrorAxis, paramMirrorOffset, paramMirrorGizmoSize,
	// transient params for display only
	paramStrength, paramSize, paramMirror,
};

StdBrushPresetContext::ParamDesc StdBrushPresetContext::paramDescs[] =
{
	{ (ParamType2)TYPE_INTERFACE,	IDS_PARAM_FALLOFFCURVE,			_T(""),	-1	 }, //Note: these two params have
	{ (ParamType2)TYPE_STRING,		IDS_PARAM_NAME,					_T(""),	-1	 }, //special handling for display

	{ (ParamType2)TYPE_FLOAT,		IDS_PARAM_MINSIZE,				_T(""),	-1	 },
	{ (ParamType2)TYPE_WORLD,		IDS_PARAM_MAXSIZE,				_T(""),	-1	 },
	{ (ParamType2)TYPE_FLOAT,		IDS_PARAM_MINSTR,				_T(""),	-1	 },
	{ (ParamType2)TYPE_WORLD,		IDS_PARAM_MAXSTR,				_T(""),	-1	 },

	{ (ParamType2)TYPE_BOOL,		IDS_PARAM_PRESSURESENSEON,		_T(""),	-1	 },
	{ (ParamType2)TYPE_INT,			IDS_PARAM_PRESSUREAFFEECTS,		_T(""),	-1	 },

	{ (ParamType2)TYPE_BOOL,		IDS_PARAM_MIRRORON,				_T(""),	-1	 },
	{ (ParamType2)TYPE_INT,			IDS_PARAM_MIRRORAXIS,			_T(""),	-1	 },
	{ (ParamType2)TYPE_FLOAT,		IDS_PARAM_MIRROROFFSET,			_T(""),	-1	 },
	{ (ParamType2)TYPE_FLOAT,		IDS_PARAM_MIRRORGIZMOSIZE,		_T(""),	-1	 },

	{ (ParamType2)TYPE_VOID,		IDS_PARAM_STRENGTH,				_T(""),	0	 },
	{ (ParamType2)TYPE_VOID,		IDS_PARAM_SIZE,					_T(""),	1	 },
	{ (ParamType2)TYPE_VOID,		IDS_PARAM_MIRROR,				_T(""),	2	 },

	{ (ParamType2)TYPE_VOID, 0, _T(""), 0}
};


//-----------------------------------------------------------------------------
// class StdBrushPresetParams

StdBrushPresetParams::StdBrushPresetParams() {
	int numParams = GetStdBrushPresetContext()->GetNumParams();
	toggles.SetSize( numParams );
	toggles.SetAll();

	name[0] = 0;
	minSize = maxSize = 0;
	minStr  = maxStr  = 0;
	pressureSenseOn = mirrorOn = FALSE;
	pressureAffects = 0;
	mirrorAxis = 0;
	mirrorOffset = 0;
	mirrorGizmoSize = 0;
	falloffCurve.SetLifetime( BaseInterface::noRelease );
}

StdBrushPresetParams::~StdBrushPresetParams() {
}

int StdBrushPresetParams::GetNumParams() {
	return GetStdBrushPresetContext()->GetNumParams();
}

int StdBrushPresetParams::GetParamID( int paramIndex ) {
	if( (paramIndex<0) || (paramIndex>=GetNumParams()) ) return 0;
	return paramIndex+1;
}

int StdBrushPresetParams::GetParamIndex( int paramID ) {
	if( (paramID<1) || (paramID>GetNumParams()) ) return -1;
	return paramID-1;
}

void StdBrushPresetParams::ApplyParams() {
	IPainterInterface_V5* painter = GetPainterInterface();
	if( painter!=NULL ) {
		// FIXME: hack; set the falloff graph first...
		// updating the curve doesn't refresh the UI but the other Set() calls will.
		Assign( painter->GetFalloffGraph(), &falloffCurve );

		// FIXME: currently no handling for the param toggles
		painter->SetMinSize(minSize);
		painter->SetMaxSize(maxSize);
		painter->SetMinStr(minStr);
		painter->SetMaxStr(maxStr);
		painter->SetPressureEnable(pressureSenseOn);
		painter->SetPressureAffects(pressureAffects);
		painter->SetMirrorEnable(mirrorOn);
		painter->SetMirrorAxis(mirrorAxis);
		painter->SetMirrorOffset(mirrorOffset);
		//painter->SetMirrorSize(mirrorSize);
	}
}

void StdBrushPresetParams::FetchParams() {
	IPainterInterface_V5* painter = GetPainterInterface();
	if( painter!=NULL ) {
		minSize			= painter->GetMinSize();
		maxSize			= painter->GetMaxSize();
		minStr			= painter->GetMinStr();
		maxStr			= painter->GetMaxStr();
		pressureSenseOn	= painter->GetPressureEnable();
		pressureAffects	= painter->GetPressureAffects();
		mirrorOn		= painter->GetMirrorEnable();
		mirrorAxis		= painter->GetMirrorAxis();
		mirrorOffset	= painter->GetMirrorOffset();
		Assign( &falloffCurve, painter->GetFalloffGraph() );
	}
}

BOOL StdBrushPresetParams::GetParamToggle( int paramID ) {
	int paramIndex = GetParamIndex( paramID );
	return toggles[ paramIndex ];
}

void StdBrushPresetParams::SetParamToggle( int paramID, BOOL onOff ) {
	int paramIndex = GetParamIndex( paramID );
	toggles.Set( paramIndex, onOff );
}

FPValue StdBrushPresetParams::GetParamValue( int paramID ) {
	FPValue val;
	val.type = GetStdBrushPresetContext()->GetParamType(paramID);

	switch( paramID ) {
	case paramFalloffCurve:		val.fpi = &falloffCurve;		break;
	case paramName:				val.s = name;					break;
	case paramMinSize:			val.f = minSize;				break;
	case paramMaxSize:			val.f = maxSize;				break;
	case paramMinStr:			val.f = minStr;					break;
	case paramMaxStr:			val.f = maxStr;					break;
	case paramPressureSenseOn:	val.i = pressureSenseOn;		break;
	case paramPressureAffects:	val.i = pressureAffects;		break;
	case paramMirrorOn:			val.i = mirrorOn;				break;
	case paramMirrorAxis:		val.i = mirrorAxis;				break;
	case paramMirrorOffset:		val.f = mirrorOffset;			break;
	case paramMirrorGizmoSize:	val.f = mirrorGizmoSize;		break;
	}

	return val;
}

void StdBrushPresetParams::SetParamValue( int paramID, FPValue val ) {
	//FIXME: restore this check ... was removed because the Name parameter accepts several types
	//if( val.type != GetStdBrushPresetContext()->GetParamType(paramID) )
	//	return;

	switch( paramID ) {
	case paramFalloffCurve:
		{
			BaseInterface* iface = val.fpi->GetInterface( CURVEIOBJECT_INTERFACE_ID );
			if( iface!=NULL ) {
				Assign( &falloffCurve, (CurvePts*)val.fpi );
				GetBrushPresetMgr()->OnPresetIconUpdated( parentID );
			}
			break;
		}
	case paramName:
		name[0] = 0;
		if( ((val.type==TYPE_TSTR_BV) || (val.type==TYPE_TSTR)) && (val.tstr!=NULL) )
			_tcscpy( name, val.tstr->data() );
		if( (val.type==TYPE_STRING) && (val.s!=NULL) )
			_tcscpy( name, val.s );
		break;
	case paramMinSize:			minSize = val.f;				break;
	case paramMaxSize:			maxSize = val.f;				break;
	case paramMinStr:			minStr = val.f;					break;
	case paramMaxStr:			maxStr = val.f;					break;
	case paramPressureSenseOn:	pressureSenseOn = val.i;		break;
	case paramPressureAffects:	pressureAffects = val.i;		break;
	case paramMirrorOn:			mirrorOn = val.i;				break;
	case paramMirrorAxis:		mirrorAxis = val.i;				break;
	case paramMirrorOffset:		mirrorOffset = val.f;			break;
	case paramMirrorGizmoSize:	mirrorGizmoSize = val.f;		break;
	}
}

FPValue StdBrushPresetParams::GetDisplayParamValue( int paramID ) {
	if( (paramID==paramFalloffCurve) || (paramID==paramName) )
		return GetParamValue( paramID );

	TCHAR buf[256] = {0}, *text=buf;
	buf[0] = 0;

	switch( paramID ) {
	case paramStrength:		_stprintf( buf, _T("%.2f - %.2f"), minStr, maxStr );	break;
	case paramSize:	{
		buf[0] = _T('\0');
		_tcscat( buf, FormatUniverseValue(minSize) );
		_tcscat( buf, _T(" - ") );
		_tcscat( buf, FormatUniverseValue(maxSize) );
		break;
							}
	case paramMirror: {
		if( !mirrorOn ) _stprintf( buf, _T("-") );
		else {
			static TCHAR* axisStr[] = {_T("X"), _T("Y"), _T("Z") };
			_stprintf( buf, _T("%s: %s"), axisStr[mirrorAxis], FormatUniverseValue(mirrorOffset) );
		}
		break;
							}
	}

	FPValue val;
	val.type = TYPE_TSTR_BV;
	val.tstr = new TSTR( text );
	return val;
}

void StdBrushPresetParams::SetParent( IBrushPreset* parent ) {
	this->parentID = parent->PresetID();
}

const TCHAR* StdBrushPresetParams::GetName() {
	return name;
}

void StdBrushPresetParams::SetName( const TCHAR* name ) {
	if( name==NULL )	this->name[0] = 0;
	else				_tcscpy( this->name, name );
}


HIMAGELIST StdBrushPresetParams::GetIcon() {
	return icon.GetImageList();
}

void StdBrushPresetParams::UpdateIcon( float iconMin, float iconMax ) {
	CUIFrameMgr* frameMgr = GetCUIFrameMgr();
	int iconSize  = frameMgr->GetImageSize();
	if( (icon.Count()<4) || (icon.Width()!=iconSize) ) {
		icon.Init( iconSize, iconSize );
		icon.AddImages( 4 );
	}

	RGBTRIPLE fgColor, bgColor, textColor = GetStdColor( kText );
	RGBTRIPLE black = {0,0,0};
	int width = iconSize, height = iconSize;
	float xMid = ((width/2.0f)-0.5f), yMid = ((height/2.0f)-0.5f);

	float radiusMax = MAX( xMid, yMid );
	float w = Clamp( maxSize, iconMin, iconMax );
	w = (w - iconMin) / (iconMax - iconMin);
	float radius = Interp( w, 3, radiusMax );
	float distMax = radius;

	int index = 0;
	for( int i=0; i<2; i++ ) {
		for( int j=0; j<2; j++, index++ ) {
			// The 5 brush presets sphere icons are made up from kBackground color and kItemHilight color
			bgColor = GetStdColor( (j==0? kBackground:kItemHilight) );
			if( i==0 )	fgColor = textColor;
			else		fgColor = Interp( 0.7f, textColor, bgColor );
			RGBTRIPLE* pImage = icon.GetImage( index );
			RGBTRIPLE* pMask  = icon.GetMask( index );

			for( int y=0; y<height; y++ ) {
				float distY = y - yMid;
				for( int x=0; x<width; x++ ) {
					*pImage = fgColor;

					float distX = x - xMid;
					float dist = sqrt( distX*distX + distY*distY );
					float u = (dist>=distMax? 1.0f : dist/distMax);
					float w = (dist> distMax? 0.0f : falloffCurve.GetValue( u ));
					w = Clamp( w, 0, 1 );
					*pImage = Interp( w, bgColor, fgColor );
					*pMask = black;
					//pMask->rgbtRed = pMask->rgbtGreen = pMask->rgbtBlue = (w*255);
					pImage++, pMask++;
				}
			}

			icon.UpdateImage( index );
		}
	}

}


//-----------------------------------------------------------------------------
// class StdBrushPresetContext

StdBrushPresetContext::StdBrushPresetContext() {
}

StdBrushPresetContext::~StdBrushPresetContext() {
}

void StdBrushPresetContext::Init() {
	IBrushPresetMgr* mgr = GetIBrushPresetMgr();
	paramCount = displayCount = 0;

	GetString( IDS_STDBRUSHPRESETCONTEXT, contextName, _countof(contextName) );

	ParamDesc* desc = &(paramDescs[0]);
	for( int i=0; desc->nameResID!=0; i++ ) {
		GetString( desc->nameResID, desc->nameStr, _countof(desc->nameStr) );
		if( desc->displayIndex>=0 ) displayCount++;
		paramCount++;

		desc = &(paramDescs[i+1]);
	}

	mgr->RegisterContext( this );
}


void StdBrushPresetContext::DeleteParams( IBrushPresetParams* params ) {
	if( (params==NULL) || (params->ContextID()!=STDBRUSHPRESETCONTEXT_ID) )
		return; //invalid params
	delete ((StdBrushPresetParams*)params);
}

int StdBrushPresetContext::GetNumParams() {
	return paramCount;
}

int StdBrushPresetContext::GetParamID( int paramIndex ) {
	// ID numbers can be arbitrary but nonzero; this code uses the range [1..N]
	if( paramIndex<0 || paramIndex>GetNumParams() ) return 0;
	return (paramIndex + 1); //add one to the index to get the ID
}

int StdBrushPresetContext::GetParamIndex( int paramID ) {
	// ID numbers can be arbitrary but nonzero; this code uses the range [1..N]
	if( paramID<1 || paramID>(GetNumParams()+1) ) return -1;
	return (paramID - 1);  //subtract one from the ID to get the index
}

const TCHAR* StdBrushPresetContext::GetParamName( int paramID ) {
	int index = GetParamIndex(paramID);
	if( index<0 ) return NULL;
	return paramDescs[ index ].nameStr; //subtract one from the ID to get the index
}

ParamType2 StdBrushPresetContext::GetParamType( int paramID ) {
	int index = GetParamIndex(paramID);
	if( index<0 ) return TYPE_VOID;
	return paramDescs[ index ].type; //subtract one from the ID to get the index
}

int StdBrushPresetContext::GetNumDisplayParams() {
	return displayCount;
}

int StdBrushPresetContext::GetDisplayParamID( int displayIndex ) {
	for( int i=0; i<paramCount; i++ )
		if( paramDescs[i].displayIndex == displayIndex )
			return GetParamID(i);
	return 0;
}

int StdBrushPresetContext::GetDisplayParamIndex( int paramID ) {
	int paramIndex = GetParamIndex( paramID );
	if( paramIndex<0 ) return -1;
	return paramDescs[ paramIndex ].displayIndex;
}

BOOL StdBrushPresetContext::IsDisplayParam( int paramID ) {
	int paramIndex = GetParamIndex( paramID );
	if( paramIndex<0 ) return FALSE;
	return (paramDescs[ paramIndex ].displayIndex >= 0? TRUE:FALSE);
}

BOOL StdBrushPresetContext::IsTransientParam( int paramID ) {
	int paramIndex = GetParamIndex( paramID );
	if( paramIndex<0 ) return -1;
	return (paramDescs[ paramIndex ].type == TYPE_VOID? TRUE:FALSE);
}



//-----------------------------------------------------------------------------
// class StdBrushPreset

StdBrushPreset::StdBrushPreset( int presetID, BrushPresetMgr* mgr ) {
	this->presetID = presetID;
	this->mgr = mgr;
	DbgAssert( mgr!=NULL );

	int numContexts = mgr->GetNumContexts();
	for( int i=0; i<numContexts; i++ ) {
		Class_ID contextID = mgr->GetContextID( i );
		ParamsItem paramsItem( contextID );
		paramsList.Append( 1, &paramsItem );
	}
}

StdBrushPreset::~StdBrushPreset() {
	for( int i=0; i<paramsList.Count(); i++ ) RemoveParams(i);
}

void StdBrushPreset::Apply() {
	BrushPresetMgr* mgr = GetBrushPresetMgr();
	for( int index=0; index<paramsList.Count(); index++ ) {
		ParamsItem& paramsItem = paramsList[index];
		if( mgr->IsContextActive( paramsItem.contextID ) ) {
			if( (paramsItem.isFinal && paramsItem.params!=NULL) || ResolveParams(index) )
				// is not a standin, or was standin but resolved
				paramsItem.params->ApplyParams();
		}
	}
}

void StdBrushPreset::Fetch() {
	BrushPresetMgr* mgr = GetBrushPresetMgr();
	for( int index=0; index<paramsList.Count(); index++ ) {
		ParamsItem& paramsItem = paramsList[index];
		if( mgr->IsContextActive( paramsItem.contextID ) ) {
			if( (paramsItem.isFinal && paramsItem.params!=NULL) || ResolveParams(index) )
				// is not a standin, or was standin but resolved
				paramsItem.params->FetchParams();
		}
	}
}

IBrushPresetParams* StdBrushPreset::GetParams( Class_ID contextID ) {
	int index = GetContextIndex(contextID);
	if( index>=0 && index<paramsList.Count() ) {
		ParamsItem& paramsItem = paramsList[index];
		if( (paramsItem.isFinal && paramsItem.params!=NULL) || ResolveParams(index) )
			// is not a standin, or was standin but resolved
			return paramsItem.params;
	}
	return NULL;
}

Class_ID StdBrushPreset::GetContextID( int paramsIndex ) {
	if( (paramsIndex<0) || (paramsIndex>=paramsList.Count()) ) return Class_ID(0,0);
	return paramsList[paramsIndex].contextID;
}

int StdBrushPreset::GetContextIndex( Class_ID contextID ) {
	for( int i=0; i<paramsList.Count(); i++ )
		if( paramsList[i].contextID==contextID )
			return i;
	return -1;
}

void StdBrushPreset::Copy( StdBrushPreset* src ) {
	int numContexts = src->GetNumContexts();
	for( int i=0; i<numContexts; i++ ) {
		Class_ID contextID = src->GetContextID(i);
		// Don't force a resolve on the src params
		IBrushPresetParams* srcParams = src->GetParams( i );
		if( srcParams==NULL ) continue;

		// Don't force a resolve on the dest params; possibly create proxy
		IBrushPresetParams* destParams = GetParams( GetContextIndex(contextID) );
		StandinBrushPresetParams* destStandin = NULL;
		if( destParams==NULL ) {
			destStandin = CreateStandinParams(contextID);
			AddParams( destStandin );
		}

		int numParams = srcParams->GetNumParams();
		for( int j=0; j<numParams; j++ ) {
			int paramID = srcParams->GetParamID( j );
			FPValue paramValue = srcParams->GetParamValue(paramID);

			if( destParams!=NULL ) {
				if( destParams->GetParamIndex(paramID) < 0 ) continue;
				destParams->SetParamValue( paramID, paramValue );
			}
			else destStandin->AddParam( paramID, paramValue );
		}
	}
}

StandinBrushPresetParams* StdBrushPreset::CreateStandinParams( Class_ID contextID ) {
	StandinBrushPresetParams* standin = new StandinBrushPresetParams( contextID, mgr );
	AddParams( standin );
	return standin;
}

IBrushPresetParams* StdBrushPreset::GetParams( int index ) {
	if( index>=0 && index<paramsList.Count() )
		return paramsList[index].params;
	return NULL;
}

int StdBrushPreset::AddParams( IBrushPresetParams* params ) {
	if( params==NULL ) return -1;
	int index = GetContextIndex( params->ContextID() );
	if( index>=0 && index<paramsList.Count() ) {
		if( paramsList[index].params!=params ) RemoveParams(index);
	}
	else {
		index = paramsList.Count();
		ParamsItem paramsItem( params->ContextID() );
		paramsList.Append( 1, &paramsItem );
	}
	ParamsItem& paramsItem = paramsList[index];
	paramsItem.params = params;
	paramsItem.isStandin = FALSE;
	paramsItem.isFinal = FALSE;
	if( params!=NULL ) params->SetParent(this);
	return index;
}

int StdBrushPreset::AddParams( StandinBrushPresetParams* standin ) {
	// Use method for regular params, but set isStandin flag
	int index = AddParams( (IBrushPresetParams*)standin );
	if( index>=0 && index<paramsList.Count() )
		paramsList[index].isStandin = TRUE;
	return index;
}

void StdBrushPreset::RemoveParams( int index ) {
	if( index >= 0 && index < paramsList.Count() ) {
		ParamsItem& paramsItem = paramsList[index];
		if( paramsItem.params!=NULL ) {
			if( paramsItem.isStandin ) {
				delete (StandinBrushPresetParams*)(paramsItem.params);
			}
			else {
				BrushPresetMgr* mgr = BrushPresetMgr::GetInstance();
				if (mgr != NULL) {
					IBrushPresetContext* context = mgr->GetContext( paramsItem.contextID );
					if( context!=NULL ) {
						context->DeleteParams(paramsItem.params);
					}
				}
			}
			paramsItem.params = NULL;
		}
		paramsItem.isStandin = FALSE;
		paramsItem.isFinal = FALSE;
	}
}

BOOL StdBrushPreset::ResolveParams( int index ) {
	if( index>=0 && index<paramsList.Count() ) {
		ParamsItem& paramsItem = paramsList[index];

		// Handle two special cases for unresolved params...
		if( !paramsItem.isFinal ) {
			// Case 1: params is null, assign a standin
			if( paramsItem.params==NULL  ) {
				paramsItem.isStandin = TRUE;
				paramsItem.params = CreateStandinParams( paramsItem.contextID );
			}
			// Case 2: params is not null or a standin, just mark it as resolved
			else if( !paramsItem.isStandin ) paramsItem.isFinal = TRUE;
		}

		// Don't resolve if we already resolved before (isFinal)
		if( paramsItem.isFinal ) return TRUE;

		// Attempt to resolve the standin
		StandinBrushPresetParams* standin = (StandinBrushPresetParams*)(paramsItem.params);
		IBrushPresetParams* params = standin->ResolveParams();
		paramsItem.isFinal = TRUE;
		if( params!=NULL && params!=paramsItem.params )  {
			paramsItem.params = params;
			paramsItem.isStandin = FALSE;
			delete standin;
		}

		return (paramsItem.params==NULL?  FALSE : TRUE);
	}
	return FALSE;
}

StdBrushPreset::ParamsItem::ParamsItem( Class_ID contextID )
{ this->contextID = contextID, this->isStandin = FALSE, this->params = NULL, this->isFinal = FALSE; }


//-----------------------------------------------------------------------------
// class BrushPresetMgr

LRESULT CALLBACK BrushPresetMgr::ToolbarWndProc(
	HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_COMMAND: {
		int id = LOWORD(wParam);
		if( (id>=ID_TOOL_PRESET_MIN) && (id<=ID_TOOL_PRESET_MAX) )
			GetBrushPresetMgr()->OnApplyPresetBtn( id );
		else if( id==ID_TOOL_ADDPRESET )
			GetBrushPresetMgr()->OnAddPresetBtn();
		else if( id==ID_TOOL_PRESETMGR )
			GetBrushPresetMgr()->OnPresetMgrBtn();
		break;
						  }

	default: {
		WNDPROC wndproc = GetBrushPresetMgr()->defaultToolbarWndProc;
		if( wndproc==NULL )	return FALSE;
		return CallWindowProc( wndproc, hwnd, msg, wParam, lParam );
				}
	}
	return TRUE;
}

BrushPresetMgr::BrushPresetMgr()
	:	IBrushPresetMgr
	(	//Interface descriptor for IBrushPresetMgr
#ifndef NO_BRUSH_PRESETS
	IBRUSHPRESETMGR_INTERFACE_ID, _T("BrushPresetMgr"), 0, NULL, FP_CORE,
#else //NO_BRUSH_PRESETS
	IBRUSHPRESETMGR_INTERFACE_ID, _T("BrushPresetMgr"), 0, NULL, FP_CORE + FP_TEST_INTERFACE,
#endif //NO_BRUSH_PRESETS
	fnIdIsActive, _T("IsActive"), 0, TYPE_BOOL, 0, 0,
	fnIdOpenPresetMgr, _T("OpenPresetMgr"), 0, TYPE_VOID, 0, 0,
	fnIdAddPreset, _T("AddPreset"), 0, TYPE_VOID, 0, 0,
	fnIdLoadPresetFile, _T("LoadPresetFile"), 0, TYPE_VOID, 0, 1,
	_T("file"), NULL, TYPE_FILENAME,
	fnIdSavePresetFile, _T("SavePresetFile"), 0, TYPE_VOID, 0, 1,
	_T("file"), NULL, TYPE_FILENAME,
	p_end
	)
{
	initialized = FALSE;

	activePresetID = 0;
	ignoreUpdates = FALSE;
	iconMin = 5, iconMax = 20;
	iconMinSpinner = iconMaxSpinner = NULL;
	icon1 = icon2 = icon3 = NULL;
	hToolWindow = hToolbar = hDialog = NULL;
	dialogPosX = dialogPosY = dialogWidth = dialogHeight = -1;
	defaultToolbarWndProc = NULL;
#ifndef NO_BRUSH_PRESETS
	RegisterNotification( OnSystemStartup,		this, NOTIFY_SYSTEM_STARTUP );
	RegisterNotification( OnSystemShutdown,		this, NOTIFY_SYSTEM_SHUTDOWN );
	RegisterNotification( OnColorChange,		this, NOTIFY_COLOR_CHANGE );
	RegisterNotification( OnUnitsChange,		this, NOTIFY_UNITS_CHANGE );
	RegisterNotification( OnToolbarsPreLoad,	this, NOTIFY_TOOLBARS_PRE_LOAD );
	RegisterNotification( OnToolbarsPostLoad,	this, NOTIFY_TOOLBARS_POST_LOAD );
#endif //NO_BRUSH_PRESETS
}

BrushPresetMgr::~BrushPresetMgr() {
	if( icon1!=NULL ) { delete icon1; icon1 = NULL; }
	if( icon2!=NULL ) { delete icon2; icon2 = NULL; }
	if( icon3!=NULL ) { delete icon3; icon3 = NULL; }

	for (int i = 0; i < presets.Count(); i++)
	{
		delete presets[i];
		presets[i] = NULL;
	}

	for( int i = GetNumContexts()-1; i >= 0; i-- )
	{
		RemoveContext(i);
	}
}

#define NOTIFY_SYSTEM_SHUTDOWN			0x00000049	// Max is about to exit,  (system shutdown starting)
#define NOTIFY_SYSTEM_STARTUP			0x00000050	// Max just went live

void BrushPresetMgr::Init( IPainterInterface_V5* painterInterface ) {
	//FIXME: is setting a global static like this a hack?  Or is it valid a singleton pattern?
	thePainterInterface = painterInterface;
}

void BrushPresetMgr::Reset() {
	SetActivePreset(0);

	while( presets.Count()>0 )
		DeletePreset( presets[0] );

	if( hToolWindow!=NULL ) {
		DestroyWindow( hToolWindow );
		GetCUIFrameMgr()->RecalcLayout(TRUE);
	}
	hToolWindow = NULL;
	hToolbar = NULL;
}

void BrushPresetMgr::RegisterContext( IBrushPresetContext* context ) {
#ifdef NO_BRUSH_PRESETS
	return;
#endif //NO_BRUSH_PRESETS
	if( context == NULL ) return; //invalid input
	AddContext( context );

	int numContexts = contextList.Count();
	activeContexts.SetSize( numContexts, TRUE );
	activeContexts.Set( (numContexts-1), FALSE );
}

void BrushPresetMgr::UnRegisterContext( IBrushPresetContext* context ) {
	if( context == NULL ) return; //invalid input
	int index = GetContextIndex( context->ContextID() );
	if( index<0 ) return; //context is null, or not registered

	int numContexts = contextList.Count();
	for( int i=index; i<(numContexts-1); i++ )
		activeContexts.Set( i, activeContexts[i+1] );
	activeContexts.SetSize( (numContexts-1), TRUE );
}

IBrushPresetContext* BrushPresetMgr::GetContext( Class_ID contextID ) {
	int index = GetContextIndex(contextID);
	DbgAssert(index >= 0);
	DbgAssert(index < contextList.Count());
	if( index >= 0 && index < contextList.Count() ) 
	{
		ContextItem& contextItem = contextList[index];
		if( (contextItem.isFinal) || (!contextItem.isStandin) || ResolveStandinContext(index) )
		{
			// is not a standin, or was standin but resolved
			DbgAssert(NULL != contextItem.context);
			return contextItem.context;
		}
	}
	return NULL;
}

Class_ID BrushPresetMgr::GetContextID( int contextIndex ) {
	if( (contextIndex<0) || (contextIndex>=contextList.Count()) ) return Class_ID(0,0);
	return contextList[contextIndex].contextID;
}

int BrushPresetMgr::GetContextIndex( Class_ID contextID ) {
	for( int i=0; i<contextList.Count(); i++ )
		if( contextList[i].contextID==contextID )
			return i;
	return -1;
}

int BrushPresetMgr::BeginContext( Class_ID contextID ) {
	int index = GetContextIndex(contextID);
	if( index<0 ) return 0;
	if( activeContexts[index]==TRUE ) return 0;

	//NOTE: Begin the standard paint context, before any other context launches.
	//FIXME: It's possible that a Painter object does not exist yet.
	// Creating the painter calls the Init() method; must create the painter here.
	Class_ID stdID = GetStdBrushPresetContext()->ContextID();
	if( (contextID!=stdID) && !IsContextActive(stdID) )		BeginContext( stdID );
	if( (contextID==stdID) && !initialized )				CreatePainterInterface();

	//if( activeContexts.IsEmpty() ) { //starting up the first context?
	//	ReadConfig(); //load the config data
	//	if( !IsToolbarVisible() ) ShowToolbar();
	//}

	activeContexts.Set( GetContextIndex(contextID), TRUE );

	SetFocusContextID( contextID );
	if( IsDialogVisible() ) UpdateDialog();
	if( contextID==stdID  ) UpdateToolbar( updateToolbar_Toggles );
	return 1;
}

int BrushPresetMgr::EndContext( Class_ID contextID ) {
	int index = GetContextIndex(contextID);
	if( index<0 ) return 0;
	if( activeContexts[index]==FALSE ) return 0;

	activeContexts.Set(index, FALSE);

	Class_ID stdID = GetStdBrushPresetContext()->ContextID();
	int activeCount = activeContexts.NumberSet();
	//if( activeCount==0 ) { //closing down the last context?
	//	WriteConfig(); //save the config data
	//	HideToolbar();
	//}
	if( activeCount==1 ) {
		//NOTE: End the standard paint context, after all other contexts shut down.
		SetActivePreset(0);
		EndContext( stdID );
		HideDialog();
		UpdateToolbar( updateToolbar_Toggles );
	}

	return 1;
}

BOOL BrushPresetMgr::IsContextActive( Class_ID contextID ) {
	int index = GetContextIndex(contextID);
	if( index<0 ) return FALSE;
	return activeContexts[index];
}

StdBrushPreset* BrushPresetMgr::GetStdPreset( int presetID ) {
	int index = GetPresetIndex(presetID);
	return (index<0? NULL : presets[ index ]);	
}

StdBrushPreset* BrushPresetMgr::CreateStdPreset( int presetID ) {
	StdBrushPreset* preset = new StdBrushPreset( presetID, this );
	presets.Append(1,&preset);
	return preset;
}


IBrushPreset* BrushPresetMgr::CreatePreset() {
	// Create the brush preset object
	int presetID = CreatePresetID();
	StdBrushPreset* preset = CreateStdPreset( presetID ); 
	presetIndices.SetCount(0); //reset the lookup; will be updated when needed

	// Initialize all the params for the preset
	preset->Fetch();

	return preset;
}

void BrushPresetMgr::DeletePreset( IBrushPreset* preset ) {
	if( preset==NULL ) return;
	int presetID = preset->PresetID();
	int presetIndex = GetPresetIndex( presetID );
	if( presetIndex>=0 ) {
		delete presets[ presetIndex ];
		presets.Delete( presetIndex, 1 );
		presetIndices.SetCount(0); //reset the lookup; will be updated when needed
	}
}

IBrushPreset* BrushPresetMgr::GetPreset( int presetID ) {
	int index = GetPresetIndex(presetID);
	return (index<0? NULL : presets[ index ]);
}

int BrushPresetMgr::GetPresetID( int presetIndex ) {
	if( (presetIndex<0) || (presetIndex>=presets.Count()) ) return 0;
	if( presets[presetIndex] == NULL ) return 0;
	return presets[presetIndex]->PresetID();
}

int BrushPresetMgr::GetPresetIndex( int presetID ) {
	if( presetID<1 || presetID>MAX_NUM_PRESETS ) return -1;
	if( presetIndices.Count() != GetNumPresets() )
		UpdatePresetIndices();
	if( presetIndices.Count()<=presetID ) return -1; // invalid presetID
	return presetIndices[ presetID ];
}

int BrushPresetMgr::GetActivePreset() {
	return activePresetID;
}

void BrushPresetMgr::SetActivePreset( int presetID ) {
	activePresetID = presetID;
}

//FIXME: Call this ActivatePreset() ?
void BrushPresetMgr::ApplyPreset( int presetID ) {
	SetIgnoreUpdates(TRUE);

	int toolID = GetPresetToolID( presetID );
	ToolbarPtr toolbar( hToolbar );
	ButtonPtr btn( toolbar->GetICustButton( toolID ) );

	// Update all the params for the active preset, before deactivating
	if( activePresetID!=0 )
		GetPreset(activePresetID)->Fetch();

	if( presetID!=GetActivePreset() ) {
		IBrushPreset* preset = GetPreset( presetID );
		SetActivePreset( presetID );
		if( preset!=NULL )
			preset->Apply();
	}
	else SetActivePreset( 0 );

	SetIgnoreUpdates(FALSE);
}

// Called by a context when its brush parameters have changed
void BrushPresetMgr::OnContextUpdated( Class_ID contextID ) {
	if( GetIgnoreUpdates() ) return;

	IBrushPreset* preset = GetPreset( GetActivePreset() );
	if( (preset!=NULL) && IsContextActive(contextID) ) {
		IBrushPresetParams* params = preset->GetParams( contextID );
		params->FetchParams();

		StdBrushPresetParams* stdParams = GetStdBrushPresetParams(preset);
		if( contextID == stdParams->ContextID() )
			UpdateToolbarIcon( preset->PresetID() );
		UpdateDialogItem( preset->PresetID() );
	}
}

// Called by a preset when its icon image is changed
void BrushPresetMgr::OnPresetIconUpdated( int presetID ) {
	if( GetIgnoreUpdates() ) return;

	UpdateToolbarIcon( presetID );
}

int BrushPresetMgr::ReadConfig(const TCHAR *cfg) {

	int retval = 0;

	BPRFormatIO bprReader(this, cfg);
	bprReader.SetIconMax(iconMax);
	bprReader.SetIconMin(iconMin);
	retval = bprReader.ReadPresets();
	if( retval ) 
	{
		iconMax = bprReader.IconMax();
		iconMin = bprReader.IconMin();
		if (IsDialogVisible())
			UpdateDialog();
	}

	if( bprReader.FileName().isNull() || retval!=0 ) {
		//If cfg is NULL, this will create a new default toolbar
		retval = ReadToolbar( bprReader.FileName() );
		if( retval ) UpdateToolbar();
	}

	return retval;
}

int BrushPresetMgr::WriteConfig(const TCHAR *cfg) {

	WritePresets( cfg );
	WriteToolbar( cfg );

	return TRUE;
}

Class_ID BrushPresetMgr::GetFocusContextID() {
	while( focusContexts.Count()>0 ) {
		int i = focusContexts.Count()-1;
		return focusContexts[i];
	}
	return Class_ID(0,0);
}

void BrushPresetMgr::SetFocusContextID( Class_ID contextID ) {
	for( int i=0; i<focusContexts.Count(); ) {
		if( focusContexts[i]==contextID )
			focusContexts.Delete(i,1);
		else i++;
	}
	focusContexts.Append( 1, &contextID );
}

IBrushPresetContext* BrushPresetMgr::GetContext( int index ) {
	if( index>=0 && index<contextList.Count() )
		return contextList[index].context;
	return NULL;
}

int BrushPresetMgr::AddContext( IBrushPresetContext* context ) {
	if( context==NULL ) return -1;
	int index = GetContextIndex( context->ContextID() );
	if( index>=0 && index<contextList.Count() ) {
		RemoveContext(index);
	}
	else {
		index = contextList.Count();
		ContextItem contextItem( context->ContextID() );
		contextList.Append( 1, &contextItem );
	}

	int numContexts = contextList.Count();
	activeContexts.SetSize( numContexts, TRUE );
	activeContexts.Set( index, FALSE );

	ContextItem& contextItem = contextList[index];
	contextItem.context = context;
	contextItem.isStandin = FALSE;
	contextItem.isFinal = FALSE;
	return index;
}

int BrushPresetMgr::AddContext( StandinBrushPresetContext* standin ) {
	// Use method for regular params, but set isStandin flag
	int index = AddContext( (IBrushPresetContext*)standin );
	if( index>=0 && index<contextList.Count() )
		contextList[index].isStandin = TRUE;
	return index;
}

void BrushPresetMgr::RemoveContext( int index ) {
	DbgAssert( index >= 0 );
	DbgAssert( index < contextList.Count() );
	if( index>=0 && index<contextList.Count() ) {
		ContextItem& contextItem = contextList[index];
		if( contextItem.context!=NULL ) {
			if( contextItem.isStandin ) {
				delete (StandinBrushPresetContext*)(contextItem.context);
			}
		}
		contextItem.context = NULL;
		contextItem.isStandin = FALSE;
		contextItem.isFinal = FALSE;
	}
}

BOOL BrushPresetMgr::IsStandinContext( int index ) {
	if( index>=0 && index<contextList.Count() )
		return contextList[index].isStandin;
	return FALSE;
}

BOOL BrushPresetMgr::ResolveStandinContext( int index ) {
	if( index>=0 && index<contextList.Count() ) {
		ContextItem& contextItem = contextList[index];

		// Special cases for unresolved params: if not null or a standin, just mark as resolved 
		if( !contextItem.isFinal && !contextItem.isStandin ) contextItem.isFinal = TRUE;

		// Don't resolve if we already resolved before (isFinal)
		if( contextItem.isFinal ) return TRUE;

		// Attempt to resolve the standin by loading the standin's plugin
		StandinBrushPresetContext* standin = (StandinBrushPresetContext*)(contextItem.context);
		LoadClass( standin->PluginClassID(), standin->PluginSuperClassID() );
		contextItem.isFinal = TRUE;

		// Loaded plugin should call RegisterContext(), to replace standin with the real value.
		// Success if the context is no longer a standin
		return (IsStandinContext(index)?  FALSE : TRUE);
	}
	return FALSE;
}


void BrushPresetMgr::ShowToolbar() {
	if( hToolWindow==NULL ) {
		ReadToolbar(NULL);
		UpdateToolbar();
	}

	CUIFramePtr toolWindow( hToolWindow );
	if( toolWindow!=NULL ) {
		toolWindow->Hide(FALSE);
		UpdateToolbar( updateToolbar_Toggles );
	}
}

void BrushPresetMgr::HideToolbar() {
	CUIFramePtr toolWindow( hToolWindow );
	if( toolWindow!=NULL ) toolWindow->Hide(TRUE);
}

BOOL BrushPresetMgr::IsToolbarVisible() {
	CUIFramePtr toolWindow( hToolWindow );
	if( toolWindow==NULL ) return FALSE;
	return (toolWindow->IsHidden()? FALSE:TRUE);
}

void BrushPresetMgr::ShowDialog() {
	if( !IsDialogVisible() )
		OpenDialog();
}

void BrushPresetMgr::HideDialog() {
	if( IsDialogVisible() ) {
		::EndDialog( hDialog, 0 );
		DestroyWindow( hDialog );
		listView.Free();
		listViewIcons.Free();
		ReleaseISpinner( iconMinSpinner );
		ReleaseISpinner( iconMaxSpinner );
		iconMinSpinner = iconMaxSpinner = NULL;
		hDialog = NULL;

		UpdateToolbar( updateToolbar_Toggles );
	}
}

BOOL BrushPresetMgr::IsDialogVisible() {
	return (hDialog==NULL? FALSE:TRUE);
}

void BrushPresetMgr::SetDialogSize( int width, int height ) {
	::SetWindowPos( hDialog, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
	dialogWidth=width;
	dialogHeight=height;
}

void BrushPresetMgr::SetDialogPos( int x, int y, HWND relativeTo ) {
	::SetWindowPos( hDialog, x, y, relativeTo );

	RECT rect;
	::GetWindowRect( hDialog, &rect );
	dialogPosX=rect.left;
	dialogPosY=rect.top;
}




int BrushPresetMgr::ReadPresets(const TCHAR *cfg) {

	TCHAR* mgrSection = BRUSHPRESETS_CONFIG;
	StickyFlag ok( true ); // once set to false, will not be reset to true

	BPRFormatIO bprReader(this, cfg);
	bprReader.SetIconMax(iconMax);
	bprReader.SetIconMin(iconMin);
	bool res = bprReader.ReadPresets();
	if (res)
	{
		iconMax = bprReader.IconMax();
		iconMin = bprReader.IconMin();
	}
	return res ? TRUE : FALSE;
}

int BrushPresetMgr::WritePresets(const TCHAR *cfg) 
{
	BPRFormatIO bprWriter(this, cfg);
	bprWriter.SetIconMax(iconMax);
	bprWriter.SetIconMin(iconMin);
	return bprWriter.WritePresets() ? TRUE : FALSE;
}

int BrushPresetMgr::ReadToolbar(const TCHAR *cfg) {
	//TO DO: get notification when the toolbar is closed

	Interface* ip = GetCOREInterface();
	CUIFrameMgr* frameMgr = GetCUIFrameMgr();


	//***************************************************************************
	// Create icons
	int btnWidth  = frameMgr->GetButtonWidth();
	int btnHeight = frameMgr->GetButtonHeight();
	int iconSize  = frameMgr->GetImageSize();

	if( icon1==NULL ) icon1 = new MaxBmpFileIcon( _T("LayerToolbar"), 2 );
	if( icon2==NULL ) icon2 = new MaxBmpFileIcon( _T("Radiosity"), 2 );
	if( icon3==NULL ) icon3 = new MaxBmpFileIcon( _T("Maxscript"), 1 );


	//***************************************************************************
	// Create the foating Tool Window for the Toolbar
	HWND hWnd_cui = frameMgr->GetAppHWnd();
	HWND hParent = hWnd_cui;
	TCHAR* toolWindowName = GetString(IDS_BRUSHPRESETS_TOOLBAR);	

	if( hToolWindow==NULL )
		hToolWindow = CreateCUIFrameWindow( hParent, toolWindowName, 0, 0, 0, 0 );
	CUIFramePtr toolWindow( hToolWindow );

	if(hToolWindow == NULL)	{
		// the rest of this code assumes this toolWindow exists
		return FALSE;
	}

	toolWindow->SetPosType( (~CUI_DONT_SAVE) & toolWindow->GetPosType() );

	BOOL cfgLoaded;
	if( DoesFileExist(cfg) )
		cfgLoaded = toolWindow->ReadConfig( cfg, 1 );
	else cfgLoaded = FALSE;

	if( (!cfgLoaded) && (hToolbar==NULL) ) { //setup a default toolbar instead
		SIZE size = {40,60};
		RECT rect = {150,110,190,170};

		toolWindow->SetContentType(CUI_TOOLBAR);
		toolWindow->SetPosType( CUI_ALL_DOCK | CUI_FLOATABLE | CUI_SM_HANDLES );
		hToolbar = CreateWindow( CUSTTOOLBARWINDOWCLASS,
			toolWindowName, WS_CHILD | WS_VISIBLE, 0, 0, size.cx, size.cy, hToolWindow, NULL, hInstance, NULL );

		ToolbarPtr toolbar( hToolbar );
		toolbar->LinkToCUIFrame( hToolWindow, NULL ); //Put the toolbar into the frame window
		MoveWindow(hToolWindow, 0, 0, size.cx, size.cy, FALSE); //Resize the window
		::SetWindowPos( hToolWindow, rect.left, rect.top, hParent ); //Adjust the window position
		ShowWindow( hToolWindow, SW_HIDE );
	}
	toolWindow->SetPosType( CUI_DONT_SAVE | toolWindow->GetPosType() );
	toolWindow->SetSystemWindow( TRUE );

	//***************************************************************************
	// Get the toolbar & set its WndProc
	hToolbar = toolWindow->GetContentHandle();
	ToolbarPtr toolbar( hToolbar );

	WNDPROC proc = DLGetWindowProc( hToolbar);
	if( proc != ToolbarWndProc ) {
		defaultToolbarWndProc = proc;
		DLSetWindowLongPtr( hToolbar, ToolbarWndProc);
	}

	//***************************************************************************
	// Add buttons to the toolbar

	ButtonPtr mgrButton( toolbar->GetICustButton( ID_TOOL_PRESETMGR ) );
	if( mgrButton==NULL ) {
		toolbar->AddTool( ToolButtonItem(CTB_CHECKBUTTON, icon3, iconSize, iconSize, btnWidth, btnHeight, ID_TOOL_PRESETMGR) );
		mgrButton = toolbar->GetICustButton( ID_TOOL_PRESETMGR );
		mgrButton->SetIcon( icon3, iconSize, iconSize );
		mgrButton->SetInIcon( icon3, iconSize, iconSize );
	}
	mgrButton->SetTooltip( TRUE, GetString( IDS_OPENBRUSHPRESETMGR ) );

	ButtonPtr createButton( toolbar->GetICustButton( ID_TOOL_ADDPRESET ) );
	if( createButton==NULL ) {
		toolbar->AddTool( ToolButtonItem(CTB_PUSHBUTTON, icon1, iconSize, iconSize, btnWidth, btnHeight, ID_TOOL_ADDPRESET) );
		createButton = toolbar->GetICustButton( ID_TOOL_ADDPRESET );
		createButton->SetIcon( icon1, iconSize, iconSize );
		createButton->SetInIcon( icon1, iconSize, iconSize );
	}
	createButton->SetTooltip( TRUE, GetString( IDS_ADDBRUSHPRESET ) );

	// The loop deletes any invalid tool items.  Iterating from the last item down handles deleted indices safely.
	for( int i=(toolbar->GetNumItems()-1); i>=0; i-- ) {
		int toolID = toolbar->GetItemID(i);
		if( toolID>=ID_TOOL_PRESET_MIN && toolID<=ID_TOOL_PRESET_MAX ) {
			int presetID = GetToolPresetID( toolID );
			if( GetPreset(presetID)==NULL ) {
				// Preset does not exist.  Tool entry is invalid, so delete the tool entry.
				toolbar->DeleteItemByID( toolID );
			}
			else
			{
				ButtonPtr btn( toolbar->GetICustButton(toolID) );
				btn->Execute( I_EXEC_BUTTON_DAD_ENABLE, FALSE ); // Disable button drag & drop within the toolbar
			}
		}
	}

	ResizeFloatingTB(hToolWindow);
	GetCUIFrameMgr()->RecalcLayout(TRUE);

	//***************************************************************************
	// Setup the button icons
	UpdateToolbar();

	return TRUE;
}

int BrushPresetMgr::WriteToolbar( const TCHAR* cfg ) {

	TSTR cfgBuf(cfg);

	if( cfgBuf.isNull() ) {
		if( ! BPRFormatIO::GetWriteFileName(cfgBuf) )
			return FALSE;
	}

	ICUIFrame *toolWindow = GetICUIFrame( hToolWindow );
	if( toolWindow!=NULL ) {
		hToolbar = toolWindow->GetContentHandle();
		toolWindow->SetPosType( (~CUI_DONT_SAVE) & toolWindow->GetPosType() );
		toolWindow->WriteConfig( cfgBuf.data() );
		toolWindow->SetPosType(   CUI_DONT_SAVE  | toolWindow->GetPosType() );

		ReleaseICUIFrame( toolWindow );
	}

	return TRUE;
}

void BrushPresetMgr::UpdateToolbar( int updateChannels ) {
	int count = GetNumPresets();
	if( updateChannels & updateToolbar_Icons ) {
		for( int i=0; i<count; i++ )
			UpdateToolbarIcon( GetPresetID(i) );
	}

	if( updateChannels & updateToolbar_Toggles )
		UpdateToolbarToggles();

	if( updateChannels & updateToolbar_Size )
		UpdateToolbarSize();
}

void BrushPresetMgr::UpdateToolbarSize() {
	CUIFrameMgr* frameMgr = GetCUIFrameMgr();
	CUIFramePtr  toolWindow( hToolWindow );

	if( toolWindow!=NULL ) {
		if( toolWindow->IsFloating() )
			ResizeFloatingTB(hToolWindow);
		else frameMgr->RecalcLayout(TRUE);
	}
}

void BrushPresetMgr::UpdateToolbarIcon( int presetID ) {
	int toolID = GetPresetToolID( presetID );
	ToolbarPtr toolbar( hToolbar );
	if( toolbar==NULL )	return;

	ButtonPtr btn( toolbar->GetICustButton( toolID ) );
	if( btn==NULL )		return;

	StdBrushPreset* preset = GetStdPreset( presetID );
	StdBrushPresetParams* params = GetStdBrushPresetParams( preset );
	int iconSize  = GetCUIFrameMgr()->GetImageSize();

	//FIXME: add blank icons, otherwise the system thinks it's a text button
	btn->SetIcon( icon2, iconSize, iconSize );
	btn->SetInIcon( icon2, iconSize, iconSize );

	// Create and draw the icon image,
	// and assign the image to the button
	params->UpdateIcon( iconMin, iconMax );
	btn->SetImage( params->GetIcon(), 0, 1, 2, 3, iconSize, iconSize );

	btn->SetTooltip( TRUE, params->GetName() );
	btn->SetCaptionText( _T("") );
}

void BrushPresetMgr::UpdateToolbarToggles() {
	ToolbarPtr toolbar( hToolbar );
	if( toolbar==NULL )	return;

	BOOL enabledState = (activeContexts.IsEmpty()? FALSE:TRUE);

	ButtonPtr presetMgrBtn( toolbar->GetICustButton( ID_TOOL_PRESETMGR ) );
	BOOL checkedState = IsDialogVisible();
	if( checkedState != presetMgrBtn->IsChecked() ) presetMgrBtn->SetCheck( checkedState );
	if( enabledState != presetMgrBtn->IsEnabled() ) presetMgrBtn->Enable( enabledState );

	ButtonPtr addPresetBtn( toolbar->GetICustButton( ID_TOOL_ADDPRESET ) );
	if( enabledState != addPresetBtn->IsEnabled() ) addPresetBtn->Enable( enabledState );

	for( int i=0; i<GetNumPresets(); i++ ) {
		int presetID = GetPresetID(i);
		int toolID = GetPresetToolID( presetID );
		ButtonPtr btn( toolbar->GetICustButton( toolID ) );

		if( btn!=NULL ) {
			BOOL checkedState = (presetID==GetActivePreset()? TRUE:FALSE);
			if(  checkedState != btn->IsChecked() ) btn->SetCheck(checkedState);
			if(  enabledState != btn->IsEnabled() ) btn->Enable(enabledState);
		}
	}
}

void BrushPresetMgr::AddPresetButton( int presetID ) {
	IBrushPreset* preset = GetPreset( presetID );

	ToolbarPtr toolbar( hToolbar );
	if( toolbar==NULL ) return;

	CUIFrameMgr* frameMgr = GetCUIFrameMgr();
	int btnWidth  = frameMgr->GetButtonWidth();
	int btnHeight = frameMgr->GetButtonHeight();
	int iconSize  = frameMgr->GetImageSize();
	int toolID = GetPresetToolID( presetID );
	int helpID = idh_dialog_createbrushpreset;

	ToolButtonItem item(CTB_CHECKBUTTON, NULL, iconSize, iconSize, btnWidth, btnHeight, toolID, helpID );
	toolbar->AddTool( item );

	ButtonPtr btn( toolbar->GetICustButton( toolID ) );
	btn->Execute( I_EXEC_BUTTON_DAD_ENABLE, FALSE ); // Disable button drag & drop within the toolbar

	UpdateToolbarIcon( presetID );
	UpdateToolbar( updateToolbar_Size );
}

void BrushPresetMgr::RemovePresetButton( int presetID ) {
	ToolbarPtr toolbar( hToolbar );
	if( toolbar==NULL ) return;

	int toolID = GetPresetToolID( presetID );
	int toolIndex = toolbar->FindItem( toolID );
	toolbar->DeleteTools( toolIndex, 1 );

	UpdateToolbar( updateToolbar_Size );
}

HWND BrushPresetMgr::GetPresetButton( int presetID ) {
	if( hToolbar==NULL ) return NULL;
	int toolID = GetPresetToolID( presetID );

	ToolbarPtr toolbar( hToolbar );
	ButtonPtr btn( toolbar->GetICustButton( toolID ) );

	HWND hWnd = (btn==NULL? NULL : btn->GetHwnd());
	return hWnd;
}

int BrushPresetMgr::GetPresetToolID( int presetID ) {
	return (presetID + ID_TOOL_PRESET_MIN) - 1;
}

int BrushPresetMgr::GetToolPresetID( int toolID ) {
	return (toolID - ID_TOOL_PRESET_MIN) + 1;
}

BOOL BrushPresetMgr::PromptFileName( TCHAR* buf, int type ) {
	TCHAR drive[MAX_PATH] = {0}, dir[MAX_PATH] = {0}, fname[MAX_PATH] = {0}, 
		ext[MAX_PATH] = {0};
	TCHAR path[MAX_PATH] = {0}, file[MAX_PATH] = {0};
	TSTR tstrFile;

	if( promptFileName.isNull() ) {
		BPRFormatIO::GetWriteFileName(tstrFile);
		promptFileName = tstrFile;
	}
	_tsplitpath( promptFileName.data(), drive, dir, fname, ext);
	_stprintf( path, _T("%s%s"), drive, dir );
	tstrFile.printf(_T("%s%s"), fname, ext );

	const TSTR star(_T("*"));
	FilterList filterList;
	filterList.Append( GetString(IDS_PRESET_FILES_BPRX) );
	filterList.Append( star + GetString(IDS_BPRX_EXTENTION) );

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(ofn) );
	ofn.lStructSize = sizeof(OPENFILENAME);  // No OFN_ENABLEHOOK
	ofn.hwndOwner = GetCOREInterface()->GetMAXHWnd();
	ofn.hInstance = hInstance;
	ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.FlagsEx = OFN_EX_NOPLACESBAR;
	ofn.lpstrFilter = filterList;
	ofn.nFilterIndex = 1;
	ofn.lpstrDefExt = GetString(IDS_BPR_EXTENTION);
	ofn.lpstrInitialDir = path;
	_tcscpy_s(file, _countof(file), tstrFile.data());
	ofn.lpstrFile = file;
	ofn.nMaxFile = MAX_PATH;
	if( type==promptFileRead )	ofn.lpstrTitle = GetString(IDS_LOAD_PRESET_FILE);
	if( type==promptFileWrite )	ofn.lpstrTitle = GetString(IDS_SAVE_PRESET_FILE);

	BOOL retval = FALSE;
	if( type==promptFileRead )	retval = GetOpenFileName(&ofn);
	if( type==promptFileWrite )	retval = GetSaveFileName(&ofn);

	if( retval ) {
		promptFileName = ofn.lpstrFile;
		_tcscpy( buf, ofn.lpstrFile );
	}
	return retval;
}



void BrushPresetMgr::OpenDialog() {
	if( IsDialogVisible() ) return;

	HWND hParent = GetCOREInterface()->GetMAXHWnd();
	hDialog = CreateDialogParam( hInstance, MAKEINTRESOURCE(IDD_PRESETMGR), hParent, DialogWndProc, (LPARAM)this );

	iconMinSpinner = GetISpinner( GetDlgItem(hDialog, IDC_PRESET_ICONMIN_SPIN) );
	HWND hIconMinEdit = GetDlgItem( hDialog, IDC_PRESET_ICONMIN_EDIT );
	iconMinSpinner->SetLimits( 0.0f, 9999999.0f );
	iconMinSpinner->SetAutoScale( TRUE );
	iconMinSpinner->SetValue( iconMin, FALSE );
	iconMinSpinner->LinkToEdit( hIconMinEdit, EDITTYPE_UNIVERSE );

	iconMaxSpinner = GetISpinner( GetDlgItem(hDialog, IDC_PRESET_ICONMAX_SPIN) );
	HWND hIconMaxEdit = GetDlgItem( hDialog, IDC_PRESET_ICONMAX_EDIT );
	iconMaxSpinner->SetLimits( 0.0f, 9999999.0f );
	iconMaxSpinner->SetAutoScale( TRUE );
	iconMaxSpinner->SetValue( iconMax, FALSE );
	iconMaxSpinner->LinkToEdit( hIconMaxEdit, EDITTYPE_UNIVERSE );

	UpdateDialog();
}

void SetCell( ListViewCell* listCell, FPValue* value, WinImageList* imageList ) {
	BOOL isColor = ((value->type==TYPE_COLOR) || (value->type==TYPE_COLOR_BV));
	BOOL isCurve = (GetCurvePtsInterface(value)!=NULL);
	BOOL isWorld = (value->type==TYPE_WORLD);
	int imageWidth = 0;
	int imageHeight = 0;
	int imageIndex = 0;
	RGBTRIPLE *image = NULL;
	RGBTRIPLE *mask = NULL;

	listCell->SetEditable( FALSE );

	if( isColor || isCurve ) {
		imageIndex = imageList->AddImages(1);
		image = imageList->GetImage( imageIndex );
		mask = imageList->GetMask( imageIndex );
		imageWidth = imageList->Width(), imageHeight = imageList->Height();
		listCell->SetImage( imageIndex );
	}

	if( isColor ) {
		RGBTRIPLE white = {255,255,255}, black = {0,0,0};
		RGBTRIPLE borderColor = GetStdColor( kText );
		RGBTRIPLE outerColor = GetStdColor( kWindow );
		RGBTRIPLE centerColor = ColorToRGBTRIPLE( *(value->clr) );

		int left=1, right=imageWidth-2, top=1, bottom=imageHeight-2;
		RGBTRIPLE *pImage = image, *pMask = mask;
		for( int y=0; y<imageHeight; y++ ) {
			for( int x=0; x<imageWidth; x++ ) {
				if( x<left || x>right || y<top || y>bottom )
					*pMask = black, *pImage = outerColor;
				//*pMask = white, *pImage = black;
				else if( x==left || x==right || y==top || y==bottom )
					*pMask = black, *pImage = borderColor;
				else *pMask = black, *pImage = centerColor;
				pImage++, pMask++;
			}
		}
		imageList->UpdateImage(imageIndex);
	}
	else if( isCurve ) {
		CurvePts* curve = GetCurvePtsInterface( value );

		RGBTRIPLE fgColor = GetStdColor( kText );
		RGBTRIPLE bgColor = GetStdColor( kWindow );
		RGBTRIPLE *pImage = image, *pMask = mask;
		float xMid = ((imageWidth/2.0f)-0.5f), yMid = ((imageHeight/2.0f)-0.5f);
		float distMax = (xMid>yMid? xMid:yMid);

		for( int y=0; y<imageHeight; y++ ) {
			float distY = y - yMid;
			for( int x=0; x<imageWidth; x++ ) {
				float distX = x - xMid;
				float dist = sqrt( distX*distX + distY*distY );
				float u = (dist>=distMax? 1.0f : dist/distMax);
				float w = (dist> distMax? 0.0f : curve->GetValue( u ));
				w = Clamp( w, 0, 1 );

				*pImage = Interp( w, bgColor, fgColor );
				//*pImage = fgColor;
				//pMask->rgbtRed = pMask->rgbtGreen = pMask->rgbtBlue = ((1.0f-w)*255);
				pImage++, pMask++;
			}
		}
		imageList->UpdateImage(imageIndex);
	}
	else if( isWorld ) {
		listCell->SetText( FormatUniverseValue(value->f) );
	}
	else {
		TCHAR valueStr[256] = {0};
		FPValueToUIString( value, valueStr );
		listCell->SetText( valueStr );
	}
}

void BrushPresetMgr::UpdateDialog( int updateChannels ) {
	if( !IsDialogVisible() ) return;

	IBrushPresetContext* stdContext = GetStdBrushPresetContext();
	if ( ! stdContext)
	{
		DbgAssert(NULL != stdContext);
		return;
	}
	int numStdParams = stdContext->GetNumDisplayParams();

	IBrushPresetContext* focusContext = GetContext( GetFocusContextID() );
	if (!focusContext)
	{
		DbgAssert(NULL != focusContext);
		return;
	}
	int numFocusParams = focusContext->GetNumDisplayParams();

	int numContexts = GetNumContexts();
	int numPresets = GetNumPresets();

	if( updateChannels & updateDialog_Spinners ) {
		iconMinSpinner->SetValue( iconMin, FALSE );
		iconMaxSpinner->SetValue( iconMax, FALSE );
	}

	if( updateChannels & updateDialog_ComboBox ) {
		HWND hComboBox = GetDlgItem( hDialog, IDC_PRESET_CONTEXT );
		int comboIndex=0, comboSel = 0;
		SendMessage( hComboBox, CB_RESETCONTENT, 0, 0 );
		for( int i=0; i<numContexts; i++ ) {
			IBrushPresetContext* context = GetContext( i );
			if( context->ContextID() == stdContext->ContextID() )
				continue; //don't display the standard context in the dropdown
			comboIndex = SendMessage( hComboBox, CB_INSERTSTRING, -1, (LPARAM)(context->GetContextName()) );
			SendMessage( hComboBox, CB_SETITEMDATA, comboIndex, (LPARAM)context );
			if( context->ContextID() == focusContext->ContextID() )
				comboSel = comboIndex;
		}
		SendMessage( hComboBox, CB_SETCURSEL, comboSel, 0 ); 
	} //end updateComboBox


	if( updateChannels & updateDialog_ListView ) {
		int iconWidth = GetSystemMetrics(SM_CXSMICON), iconHeight = GetSystemMetrics(SM_CYSMICON);
		listViewIcons.Init( iconWidth, iconHeight );

		HWND hListView = GetDlgItem( hDialog, IDC_PRESET_LIST );
		listView.Init( hListView );
		listView.SetNotifyProc( ListViewNotifyProc, this );

		listView.SetNumColumns( numStdParams + numFocusParams + 1 ); //Special handling for the first column
		listView.SetNumRows( numPresets );
		listView.SetImageList( listViewIcons.GetImageList() );

		int offset = 0;
		for( int i=0; i<2; i++ ) {
			IBrushPresetContext* context = (i==0? stdContext:focusContext);
			int numParams = context->GetNumDisplayParams();

			if( i==0 ) { //Special handling for the first column
				listView.GetColumn(0)->SetName( GetString(IDS_PARAM_NAME) );
				offset = 1;
			}

			for( int displayIndex=0; displayIndex<numParams; displayIndex++ ) {
				int paramID = context->GetDisplayParamID( displayIndex );
				const TCHAR* name = context->GetDisplayParamName( paramID );
				ListViewColumn* column = listView.GetColumn( displayIndex + offset );
				column->SetName(name);
			}
			offset+=numParams;
		}

		for( int presetIndex=0; presetIndex<numPresets; presetIndex++ )
			UpdateDialogItem( GetPresetID(presetIndex) );

		listView.Update();
	} //end updateListView

	if( updateChannels & (updateDialog_ListView | updateDialog_ListViewSel) ) {
		int activePreset = GetActivePreset();
		listView.SetSelRow( GetPresetIndex(activePreset) );

		BOOL enable = activePreset>0? TRUE:FALSE;
		ButtonPtr duplicateBtn( GetDlgItem( hDialog, IDC_PRESET_DUPLICATE ) );
		ButtonPtr deleteBtn( GetDlgItem( hDialog, IDC_PRESET_DELETE ) );
		duplicateBtn->Enable( enable );
		deleteBtn->Enable( enable );
	}
}

void BrushPresetMgr::UpdateDialogItem( int presetID ) {
	if( !IsDialogVisible() ) return;

	IBrushPresetContext* stdContext = GetStdBrushPresetContext();
	IBrushPresetContext* focusContext = GetContext( GetFocusContextID() );

	int offset = 0;
	int row = GetPresetIndex( presetID );
	IBrushPreset* preset = GetPreset( presetID );

	for( int i=0; i<2; i++ ) {
		IBrushPresetContext* context = (i==0? stdContext:focusContext);
		Class_ID contextID = context->ContextID();
		int numParams = context->GetNumDisplayParams();

		IBrushPresetParams* params = preset->GetParams( contextID );
		ListViewRow* listRow = listView.GetRow( row );

		if( i==0 ) {
			//Special handling for the stdContext, first column
			ListViewCell* listCell = listRow->GetCell( 0 );
			FPValue value1 = params->GetParamValue( 1 );
			SetCell( listCell, &value1, &listViewIcons );
			FPValue value2 = params->GetParamValue( 2 );
			SetCell( listCell, &value2, &listViewIcons );
			listCell->SetEditable( TRUE );
			offset = 1;
		}

		for( int displayIndex=0; displayIndex<numParams; displayIndex++ ) {
			int col = displayIndex + offset;
			int paramID = context->GetDisplayParamID( displayIndex );
			ListViewCell* listCell = listRow->GetCell( col );

			FPValue value = params->GetDisplayParamValue( paramID );
			SetCell( listCell, &value, &listViewIcons );
		}

		offset += numParams;
	}

	listView.UpdateRow( row );
}

TCHAR invalidChars[] = {
	'\"', '\t',
	0
};

bool BrushPresetMgr::ListViewNotifyProc( ListView::NotifyInfo& info, void* param ) {
	BrushPresetMgr* parent = (BrushPresetMgr*)param;

	if( info.message==ListView::notifySelCell ) {
		int presetID = parent->GetPresetID(info.row);
		//parent->SetFocusPresetID( presetID );
		if( presetID != parent->GetActivePreset() ) {
			parent->ApplyPreset( presetID );
			parent->UpdateToolbar( updateToolbar_Toggles );
			parent->UpdateDialog( updateDialog_ListViewSel );
		}
	}
	if( info.message==ListView::notifyEditing ) {
		for( int i=0; invalidChars[i]!=0; i++ )
			if( info.editChar==invalidChars[i] )
				return false;
	}
	if( info.message==ListView::notifyEndEdit ) {
		TSTR& text = info.editText;
		int count, index = 0;
		while ( text[index] == _T(' ') || text[index] == _T('\t') )
			index++;
		text.remove(0, index);	// trim leading spaces
		count = text.length();
		while (count && (text[count-1] == _T(' ') || text[count-1] == _T('\t')))
			count--;
		text.remove(count);		// trim trailing spaces

		int presetID = parent->GetPresetID(info.row);
		IBrushPreset* preset = parent->GetPreset( presetID );
		StdBrushPresetParams* params = GetStdBrushPresetParams( preset );
		params->SetName( text.data() );
		parent->UpdateToolbarIcon( presetID );
	}
	return true;
}

IBrushPreset* BrushPresetMgr::CreatePreset( IBrushPreset* src ) {
	if( GetNumPresets()>=MAX_NUM_PRESETS ) {
		MessageBox( NULL, GetString(IDS_TOOMANYPRESETS), GetString(IDS_ERROR), MB_ICONERROR );
		return NULL;
	}

	TSTR name = CreatePresetName( src );
	if( name.isNull() ) return NULL;

	IBrushPreset* preset = CreatePreset();
	if( preset==NULL ) return NULL;

	if( src!=NULL ) {
		StdBrushPreset* stdSrc = GetStdPreset( src->PresetID() );
		StdBrushPreset* stdDest = GetStdPreset( preset->PresetID() );
		stdDest->Copy( stdSrc );
	}

	StdBrushPresetParams* params = GetStdBrushPresetParams( preset );
	params->SetName( name.data() );

	AddPresetButton( preset->PresetID() );
	ApplyPreset( preset->PresetID() );

	UpdateToolbar( updateToolbar_Toggles );
	UpdateDialog( updateDialog_ListView );
	return preset;
}

int IntegerComparator( const void* left, const void* right ) {
	int ileft = *(int*)left, iright = *(int*)right;
	if( ileft<iright ) return -1;
	if( ileft>iright ) return 1;
	return 0;
}
int BrushPresetMgr::CreatePresetID() {
	int count = GetNumPresets();
	if( count==0 ) return 1;
	if( count==1 ) return (GetPresetID(0)==1?  2:1);

	BitArray bits( MAX_NUM_PRESETS );
	bits.ClearAll();
	int i;
	for( i=0; i<count; i++ )
		bits.Set( GetPresetID(i), TRUE );

	for( i=0; i<MAX_NUM_PRESETS; i++ )
		if( !bits[i+1] ) break;

	return i+1;
}

// Parses an expresstion in the form "xxx copy yy"
// Where xxx is any text (the name of the preset) and yy is decimal digits (the copy number)
void ParseExpression_CopiedName( TCHAR* prefix, int* copyNum, const TCHAR* src ) {
	//NOTE: should maybe use CAtlRegExp Class

	TCHAR buf[256] = {0}, copyDigits[256] = {0}, copyLabel[256] = {0};
	TCHAR* str = buf;
	int start=0;
	int end, cur, length;

	_tcscpy( buf, src );

	// Strip leading & trailing whitespace
	start = 0, cur = end = length = static_cast<int>(_tcslen(str));
	while( start<end && _istspace(str[start]) )	start++, length--;
	while( end>start && _istspace(str[end-1]) )	end--, length--;

	cur = end;
	str[end] = _T('\0');

	// Extract trailing digits
	while( cur>start && _istdigit(str[cur-1]) )	cur--;
	_tcscpy(copyDigits, str+cur);
	str[cur] = _T('\0');

	// Strip whitespace before the trailing digits
	while( cur>start && _istspace(str[cur-1]) )	cur--;
	str[cur] = _T('\0');

	// Look for the trailing word "copy"
	_tcscpy( copyLabel, GetString(IDS_COPY) );
	cur -= static_cast<int>(_tcslen(copyLabel));

	if( (cur>=start) && _tcsicmp(str+cur,copyLabel)==0) {
		// Found trailing word "copy", parse the digits
		if( _stscanf( copyDigits, _T("%i"), copyNum ) != 1 ) *copyNum = 1;
		// Strip remaining whitespace
		end = cur;
		while( end>start && _istspace(str[end-1]) ) end--;
	}
	else *copyNum = -1; // No trailing word "copy", ignore the digits

	// Prefix is the [start,end] substring
	if( end>start ) memcpy( prefix, src+start, (end-start) * sizeof(TCHAR) );
	prefix[end-start] = _T('\0');
}

TSTR BrushPresetMgr::CreatePresetName( IBrushPreset* src ) {
	TCHAR buf[256] = {0};
	int numPresets = GetNumPresets();

	// Make a copied name if applicable
	if( src!=NULL ) {
		TCHAR srcPrefix[256] = {0}, curPrefix[256] = {0};
		int srcCopyNum, curCopyNum;

		const TCHAR* name = GetStdBrushPresetParams( src )->GetName();
		ParseExpression_CopiedName( srcPrefix, &srcCopyNum, name );

		int maxCopyNum = srcCopyNum;
		for( int i=0; i<numPresets; i++ ) {
			name = GetStdBrushPresetParams( presets[i] )->GetName();
			ParseExpression_CopiedName( curPrefix, &curCopyNum, name );
			if( _tcsicmp( curPrefix, srcPrefix )==0 ) maxCopyNum = MAX(maxCopyNum, curCopyNum);
		}

		if( maxCopyNum>=0 )	_stprintf( buf, _T("%s %s %i"), srcPrefix, GetString(IDS_COPY), maxCopyNum + 1 );
		else				_stprintf( buf, _T("%s %s"), srcPrefix, GetString(IDS_COPY) );
	}
	else // Make a generic, non-copied name
		_stprintf( buf, _T("%s%i"), GetString(IDS_PRESET), numPresets+1 );

	// Throw up the Create Brush Preset dialog
	CreatePresetDialog createPresetDialog;
	createPresetDialog.SetPresetName( buf );
	int result = createPresetDialog.DoDialog();
	if( result<=0 ) return TSTR(); //bail out if the user hits cancel
	_tcscpy( buf, createPresetDialog.GetPresetName() );

	return buf;
}

void BrushPresetMgr::UpdatePresetIndices() {
	presetIndices.SetCount(MAX_NUM_PRESETS+1);

	int lookupCount = presetIndices.Count();
	for( int i=0; i<lookupCount; i++ )
		presetIndices[i] = -1;

	int presetCount = presets.Count();
	for( int i=0; i<presetCount; i++ )
		presetIndices[ presets[i]->PresetID() ] = i;
}


// Event handlers
void BrushPresetMgr::OnPresetMgrBtn() {
	BOOL enabledState = (activeContexts.IsEmpty()? FALSE:TRUE);

	if( enabledState ) { // Prevent access to the dialog when manager is disabled
		if( !IsDialogVisible() )
			ShowDialog();
		else HideDialog();
	}
}

void BrushPresetMgr::OnLoadPresetsBtn() {
	TCHAR file[MAX_PATH] = {0};
	if( !PromptFileName( file, promptFileRead ) ) return;
	ReadConfig( file );
}

void BrushPresetMgr::OnSavePresetsBtn() {
	TCHAR file[MAX_PATH] = {0};
	if( !PromptFileName( file, promptFileWrite ) ) return;
	WriteConfig( file );
}

void BrushPresetMgr::OnAddPresetBtn() {
	IBrushPreset* preset = CreatePreset( NULL );
}

void BrushPresetMgr::OnDuplicatePresetBtn() {
	StdBrushPreset* src = GetStdPreset( GetActivePreset() ); //GetStdPreset( GetFocusPresetID() );
	if( src==NULL ) return;

	IBrushPreset* preset = CreatePreset( src );
}

void BrushPresetMgr::OnRemovePresetBtn() {
	// This message should only come from the dialog,
	// because the preset for deletion can only be selected in the dialog
	DbgAssert( IsDialogVisible() );

	int presetID = GetActivePreset(); //GetFocusPresetID();
	if( presetID == 0 ) return; //Nothing to do
	if( presetID == GetActivePreset() ) SetActivePreset(0);

	int presetIndex = GetPresetIndex( presetID );
	//SetFocusPresetID( GetPresetID(presetIndex-1) );

	DeletePreset( GetPreset(presetID) );

	RemovePresetButton(presetID);
	UpdateToolbar( updateToolbar_Toggles );
	UpdateDialog( updateDialog_ListView );
}


void BrushPresetMgr::OnApplyPresetBtn( int toolID ) {
	int presetID = GetToolPresetID( toolID );
	ApplyPreset( presetID );
	UpdateToolbar( updateToolbar_Toggles );
	UpdateDialog( updateDialog_ListViewSel );
}

void BrushPresetMgr::OnSpinnerChange() {
	if( iconMinSpinner==NULL || iconMaxSpinner==NULL )
		return; //Can happen during dialog shutdown
	float newIconMin = iconMinSpinner->GetFVal();
	float newIconMax = iconMaxSpinner->GetFVal();
	if( newIconMin>=newIconMax ) {
		iconMinSpinner->SetValue(iconMin,FALSE);
		iconMaxSpinner->SetValue(iconMax,FALSE);
	}
	else {
		iconMin=newIconMin, iconMax=newIconMax;
		UpdateToolbar();
	}
}

void BrushPresetMgr::OnInitDialog(HWND hWnd) {
	hDialog = hWnd;

	SendMessage( hDialog, WM_SETICON, ICON_SMALL, GetClassLongPtr( GetCOREInterface()->GetMAXHWnd(), GCLP_HICONSM ) );

	GetCOREInterface()->RegisterDlgWnd( hDialog );


	if( dialogPosX>=0 && dialogPosY>=0 )
		SetDialogPos( dialogPosX, dialogPosY );
	else 
		CenterWindow( hDialog, GetCOREInterface()->GetMAXHWnd() );

	if( dialogWidth>=0 && dialogHeight>=0 )
		SetDialogSize( dialogWidth, dialogHeight );

	RECT rect;
	::GetWindowRect( hDialog, &rect );
	dialogPosX = rect.left;
	dialogPosY = rect.top;
	dialogWidth = rect.right - rect.left;
	dialogHeight = rect.bottom - rect.top;
}

void BrushPresetMgr::OnDestroyDialog(HWND hWnd) {
	if( hDialog==hWnd ) {
		GetCOREInterface()->UnRegisterDlgWnd( hDialog );
		hDialog=NULL;
	}
}

void BrushPresetMgr::OnDialogResized() {
	RECT rect;
	::GetWindowRect( hDialog, &rect );
	dialogWidth = rect.right - rect.left;
	dialogHeight = rect.bottom - rect.top;
}

void BrushPresetMgr::OnDialogMoved() {
	RECT rect;
	::GetWindowRect( hDialog, &rect );
	dialogPosX = rect.left;
	dialogPosY = rect.top;
}

void BrushPresetMgr::OnSystemStartup(void *param, NotifyInfo *info) {
	BrushPresetMgr* parent = (BrushPresetMgr*)param;
	StdBrushPresetContext* context = GetStdBrushPresetContext();
	context->Init();

	if( parent->ReadConfig() )
		parent->initialized = TRUE;
	//FIXME: hack... we're actually registered to a viewport change,
	//which happens earlier than system startup.  Must remove the notification
	UnRegisterNotification( OnSystemStartup, param, NOTIFY_VIEWPORT_CHANGE );
}

void BrushPresetMgr::OnSystemShutdown(void *param, NotifyInfo *info) {
	BrushPresetMgr* parent = (BrushPresetMgr*)param;
	if( parent->initialized ) //Don't write unless we successfully read a config
		parent->WriteConfig();
}

void BrushPresetMgr::OnColorChange(void *param, NotifyInfo *info) {
	BrushPresetMgr* parent = (BrushPresetMgr*)param;
	int count = parent->GetNumPresets();
	for( int i=0; i<count; i++ )
		parent->UpdateToolbarIcon( parent->GetPresetID(i) );
}

void BrushPresetMgr::OnUnitsChange(void *param, NotifyInfo *info) {
	BrushPresetMgr* parent = (BrushPresetMgr*)param;
	if( parent->IsDialogVisible() )
		parent->UpdateDialog();
}

void BrushPresetMgr::OnToolbarsPreLoad(void *param, NotifyInfo *info) {
	BrushPresetMgr* parent = (BrushPresetMgr*)param;
	if( parent->hToolWindow==NULL ) return;

	// All system toolbars are destroyed and rebuilt during toolbar load.
	// Temporarily mark the preset toolbar as non-system, since it has a separate loading scheme
	CUIFramePtr toolWindow( parent->hToolWindow );
	if(toolWindow != NULL)	{
		toolWindow->SetSystemWindow( FALSE );
	}
}

void BrushPresetMgr::OnToolbarsPostLoad(void *param, NotifyInfo *info) {
	BrushPresetMgr* parent = (BrushPresetMgr*)param;
	if( parent->hToolWindow==NULL ) return;

	CUIFramePtr toolWindow( parent->hToolWindow );
	if(toolWindow != NULL )	{
		toolWindow->SetSystemWindow( TRUE );
	}
}

// Function publishing
BaseInterface* BrushPresetMgr::GetInterface(Interface_ID id) {
	if( id==IBRUSHPRESETMGR_INTERFACE_ID )
		return this;
	return NULL;
}

BOOL BrushPresetMgr::fnIsActive() {
	return (activeContexts.IsEmpty()? FALSE:TRUE);
}

void BrushPresetMgr::fnOpenPresetMgr() {
	OnPresetMgrBtn();
}

void BrushPresetMgr::fnAddPreset() {
	OnAddPresetBtn();
}

void BrushPresetMgr::fnLoadPresetFile( const TCHAR* file ) {
	ReadConfig( file );
}

void BrushPresetMgr::fnSavePresetFile( const TCHAR* file ) {
	WriteConfig( file );
}

BrushPresetMgr::ContextItem::ContextItem( Class_ID contextID )
{ this->contextID = contextID, this->isStandin = FALSE, this->isFinal = FALSE, this->context = NULL; }


/*
Michael Russo says:
mhBookmarkWnd = CreateCUIFrameWindow(Max()->GetMAXHWnd(), GetString(IDS_FRAME_NAME), 0, 0,
mpBookmarkPosData->GetWidth(0,0), mpBookmarkPosData->GetHeight(0,0));
Michael Russo says:
mhBookmarkDialog = CreateDialogParam( hInstance, MAKEINTRESOURCE(IDD_BOOKMARK), 
mhBookmarkWnd, BookmarkDlgProc, (LPARAM)this);
*/


//-----------------------------------------------------------------------------
// class CreatePresetDialog

void CreatePresetDialog::SetPresetName( const TCHAR* name ) {
	if( name==NULL )	presetName[0] = 0;
	else				_tcscpy( presetName, name );
}

INT_PTR CALLBACK CreatePresetDialog::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CreatePresetDialog* parent;

	if(msg == WM_INITDIALOG) {
		parent = (CreatePresetDialog*)(lParam);
		parent->hWnd = hWnd;
		DLSetWindowLongPtr(hWnd, lParam);

		TSTR name = parent->GetPresetName();
		SetDlgItemText( hWnd, IDC_PRESET_NAME, name );

		CenterWindow( hWnd, GetWindow( hWnd, GW_OWNER ));
		SetFocus( GetDlgItem( hWnd,IDC_PRESET_NAME ) );
		SetWindowContextHelpId( hWnd, idh_dialog_createbrushpreset );
	}
	else
		parent = DLGetWindowLongPtr<CreatePresetDialog*>(hWnd);

	switch (msg)  {
	case WM_SYSCOMMAND:
		if((wParam & 0xfff0) == SC_CONTEXTHELP) {
			MaxSDK::IHelpSystem::GetInstance()->ShowProductHelpForTopic(idh_dialog_createbrushpreset);
			return 0;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))  {
		case IDCANCEL:
			parent->EndDialog( FALSE );
			break;
		case IDOK: {
			TCHAR buf[256] = {0};
			GetDlgItemText( hWnd, IDC_PRESET_NAME, buf, 256 );

			parent->SetPresetName( buf );
			parent->EndDialog( TRUE );
			break;
					  }
		}
		break;
	}

	return FALSE;
}

int CreatePresetDialog::DoDialog() {
	//int numPresets = GetIBrushPresetMgr()->GetNumPresets();
	//_stprintf( presetName, "%s%i", GetString(IDS_PRESET), numPresets+1 );


	HWND hParent = GetCOREInterface()->GetMAXHWnd();
	return DialogBoxParam(
		hInstance, 
		MAKEINTRESOURCE(IDD_CREATEPRESET), 
		hParent, 
		DlgProc,
		(LPARAM)this);
}

void CreatePresetDialog::EndDialog( BOOL result ) {
	::EndDialog( hWnd, result );
}

__declspec( dllexport ) int LibShutdown()
{
	// Order of destruction is significant (important) since the BrushPresetManager
	// deletes the StdBrushPresets it owns, which in turn remove the preset contexts
	// from the manager. Thus, if the StdBrushPresetContext is destroyed first, the
	// StdBrushPreset ends up with a dangling pointer...
	BrushPresetMgr::DestroyInstance();
	StdBrushPresetContext::DestroyInstance();
	return 0;
}

