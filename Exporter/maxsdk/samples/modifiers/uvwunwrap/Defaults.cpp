#include "unwrap.h"
#include <Util\IniUtil.h> // MaxSDK::Util::WritePrivateProfileString

#include <locale.h>

#define UNWRAPCONFIGNAME _T("unwrapuvw.ini")
#define WSDSECTION    _T("Default State")
#define GETFACESELECTIONFROMSTACK		_T("GetFaceSelectionFromStack")
#define FALLOFFSTR						_T("FalloffStr")
#define WINDOWPOSX1						_T("WindposX1")
#define WINDOWPOSY1						_T("WindposY1")
#define WINDOWPOSX2						_T("WindposX2")
#define WINDOWPOSY2						_T("WindposY2")
#define LOCKASPECT						_T("LockAspect")
#define SHOWMAP							_T("ShowMap")
#define FORCEUPDATE						_T("ForceUpdate")
#define RENDERW							_T("RenderW")
#define RENDERH							_T("RenderH")
#define SHOWVERTS						_T("ShowVerts")
#define USEBITMAPRES					_T("UseBitmapRes")
#define PIXELSNAP						_T("PixelSnap")


#define CHANNEL							_T("Channel")
#define FALLOFF							_T("Falloff")
#define FALLOFFSPACE					_T("FalloffSpace")

#define NORMALSPACING					_T("NormalMapSpacing")
#define NORMALNORMALIZE					_T("NormalMapNormalize")
#define NORMALROTATE					_T("NormalMapRotate")
#define NORMALALIGNWIDTH				_T("NormalMapAlignWidth")
#define NORMALMETHOD					_T("NormalMapMethod")

#define FLATTENANGLE					_T("FlattenMapAngle")
#define FLATTENSPACING					_T("FlattenMapSpacing")
#define FLATTENNORMALIZE				_T("FlattenMapNormalize")
#define FLATTENROTATE					_T("FlattenMapRotate")
#define FLATTENCOLLAPSE					_T("FlattenMapCollapse")

#define UNFOLDNORMALIZE					_T("UnfoldMapNormalize")
#define UNFOLDMETHOD					_T("UnfoldMapMethod")

#define STITCHBIAS						_T("StitchBias")
#define STITCHALIGN						_T("StitchAlign")
#define STITCHSCALE						_T("StitchScale")

#define TILEON							_T("TileOn")
#define TILEBRIGHTNESS					_T("TileBrightness")
#define TILELIMIT						_T("TileLimit")

#define SOFTSELLIMIT					_T("SoftSelLimit")
#define SOFTSELRANGE					_T("SoftSelRange")

#define GEOMELEMENTMODE					_T("GeomElementMode")
#define PLANARTHRESHOLD					_T("PlanarThreshold")
#define PLANARMODE						_T("PlanarMode")
#define IGNOREBACKFACECULL				_T("IgnoreBackFaceCull")
#define OLDSELMETHOD					_T("OldSelectionMethod")

	
#define TVELEMENTMODE					_T("TVElementMode")
#define ALWAYSEDIT						_T("AlwaysEdit")

#define PACKMETHOD						_T("PackMethod")
#define PACKSPACING						_T("Packpacing")
#define PACKNORMALIZE					_T("PackNormalize")
#define PACKROTATE						_T("PackRotate")
#define PACKCOLLAPSE					_T("PackCollapse")
#define PACKRESCALECLUSTER			_T("PackRescaleCluster")

#define FILLMODE						_T("FaceFillMode")
#define DISPLAYOPENEDGES				_T("DisplayOpenEdges")
#define THICKOPENEDGES					_T("ThickOpenEdges")
#define VIEWPORTOPENEDGES				_T("ViewportOpenEdges")

#define UVEDGEMMODE						_T("UVEdgeMode")
#define OPENEDGEMMODE					_T("OpenEdgeMode")
#define DISPLAYHIDDENEDGES				_T("DisplayHiddenEdges")

#define SKETCHSELMODE					_T("SketchSelMode")
#define SKETCHTYPE						_T("SketchType")
#define SKETCHINTERACTIVE				_T("SketchInteactiveMode")
#define SKETCHDISPLAYPOINTS				_T("SketchDisplayPoints")

#define HITSIZE							_T("HitSize")

#define RESETSELONPIVOT					_T("ResetSelOnPivot")
#define POLYMODE						_T("PolygonMode")
#define ALLOWSELECTIONINSIDEGIZMO		_T("AllowSelectionInsideGizmo")

#define WELDTHRESHOLD					_T("WeldThreshold")
#define CONSTANTUPDATE					_T("ConstantUpdate")
#define MIDPIXELSNAP					_T("MidPixelSnap")

#define LINECOLOR						_T("LineColor")
#define SELCOLOR						_T("SelColor")
#define OPENEDGECOLOR					_T("OpenEdgeColor")
#define HANDLECOLOR						_T("HandleColor")
#define TRANSFORMGIZMOCOLOR				_T("TransformGizmoColor")

#define SHOWSHARED						_T("ShowShared")
#define SHAREDCOLOR						_T("SharedColor")

#define ICONLIST						_T("DisplayIconList")

#define SYNCSELECTION					_T("SyncSelection")

#define BRIGHTCENTERTILE				_T("BrightnessAffectsCenterTile")
#define BLENDTILE						_T("BlendTilesToBackground")
#define PAINTSIZE						_T("PaintSelectSize")

#define TICKSIZE						_T("TickSize")

//new
#define GRIDSIZE						_T("GridSize")
#define GRIDSNAP						_T("GridSnap")
#define GRIDVISIBLE						_T("GridVisible")
#define GRIDSTR							_T("GridStr")
#define AUTOMAP							_T("AutoBackground")

#define ENABLESOFTSELECTION				_T("EnableSoftSelection")


//5.1.05
#define AUTOBACKGROUND					_T("EnableAutoBackground")

//5.1.06
#define RELAXAMOUNT						_T("RelaxAmount")
#define RELAXITERATIONS					_T("RelaxIterations")
#define RELAXBOUNDARY					_T("RelaxBoundary")
#define RELAXCORNER						_T("RelaxCorner")
#define RELAXSTRETCH					_T("RelaxStretch")
#define RELAXTYPE						_T("RelaxType")


#define ABSOLUTETYPEIN					_T("AbsoluteTypeIn")
#define MOVE							_T("MoveState")
#define SCALE							_T("ScaleState")
#define MIRROR							_T("MirrorState")
#define UVW							_T("UVWState")
#define PIXELSNAP							_T("PixelSnap")
#define GRIDSNAP							_T("GridSnap")
#define SNAPSTATE							_T("SnapState")
#define ZOOMEXTENT							_T("ZoomExtentsState")
#define TVSUBOBJECTMODE							_T("TVSubObjectMode")



#define ROTATIONSRESPECTASPECT			_T("RotationsRespectAspect")

#define RELAXBYSPRINGSTRETCH						_T("RelaxBySpringStretch")
#define RELAXBYSPRINGITERATION						_T("RelaxBySpringIteration")
#define RELAXBYSPRINGUSEONLYVEDGES					_T("RelaxBySpringUseOnlyVEdges")


#define RENDERTEMPLATE_WIDTH					_T("RenderTemplateWidth")
#define RENDERTEMPLATE_HEIGHT					_T("RenderTemplateHeight")

#define RENDERTEMPLATE_EDGECOLOR					_T("RenderTemplateEdgeColor")
#define RENDERTEMPLATE_EDGEALPHA					_T("RenderTemplateEdgeAlpha")

#define RENDERTEMPLATE_SEAMCOLOR					_T("RenderTemplateSeamColor")


#define RENDERTEMPLATE_RENDERVISIBLEEDGES					_T("RenderTemplateRenderVisibleEdges")
#define RENDERTEMPLATE_RENDERINVISIBLEEDGES					_T("RenderTemplateRenderInvisibleEdges")
#define RENDERTEMPLATE_RENDERSEAMEDGES					_T("RenderTemplateRenderSeamEdges")


#define RENDERTEMPLATE_SHOWFRAMEBUFFER					_T("RenderTemplateShowFrameBuffer")
#define RENDERTEMPLATE_FORCE2SIDED					_T("RenderTemplateForce2Sided")

#define RENDERTEMPLATE_FILLMODE					_T("RenderTemplateFillMode")
#define RENDERTEMPLATE_FILLALPHA					_T("RenderTemplateFillAlpha")
#define RENDERTEMPLATE_FILLCOLOR					_T("RenderTemplateFillColor")

#define RENDERTEMPLATE_OVERLAP					_T("RenderTemplateOverlap")
#define RENDERTEMPLATE_OVERLAPCOLOR					_T("RenderTemplateOverlapColor")

// GetCfgFilename()
void UnwrapMod::GetCfgFilename( TCHAR *filename ) 
	{
		//unwrapuvw.ini is moved to plugcfg_ln
	_tcscpy(filename,TheManager->GetDir(APP_PLUGCFG_LN_DIR));
	int len = static_cast<int>(_tcslen(filename));	// SR DCAST64: Downcast to 2G limit.
	if (len) 
		{
	   if (_tcscmp(&filename[len-1],_T("\\")))
		 _tcscat(filename,_T("\\"));

		}
	_tcscat(filename,UNWRAPCONFIGNAME);
	}
	
void	UnwrapMod::fnSetAsDefaults()
{
	windowPos.length = sizeof(WINDOWPLACEMENT); 
	GetWindowPlacement(hDialogWnd,&windowPos);

	TCHAR filename[MAX_PATH];
	TSTR str;

	GetCfgFilename(filename);
	
	TCHAR* saved_lc = NULL;
	TCHAR* lc = _tsetlocale(LC_NUMERIC, NULL); // query  
	if (lc)  
 		saved_lc = _tcsdup(lc);  
	lc = _tsetlocale(LC_NUMERIC, _T("C"));  
   
   

	str.printf(_T("%d"),getFaceSelectionFromStack);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,GETFACESELECTIONFROMSTACK,str,filename);

	str.printf(_T("%f"),falloffStr);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,FALLOFFSTR,str,filename);

	
	str.printf(_T("%d"),windowPos.rcNormalPosition.left);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,WINDOWPOSX1,str,filename);
	str.printf(_T("%d"),windowPos.rcNormalPosition.bottom);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,WINDOWPOSY1,str,filename);
	str.printf(_T("%d"),windowPos.rcNormalPosition.right);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,WINDOWPOSX2,str,filename);
	str.printf(_T("%d"),windowPos.rcNormalPosition.top);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,WINDOWPOSY2,str,filename);


	str.printf(_T("%d"),forceUpdate);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,FORCEUPDATE,str,filename);

	str.printf(_T("%d"),lockAspect);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,LOCKASPECT,str,filename);

	str.printf(_T("%d"),showMap);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SHOWMAP,str,filename);

	str.printf(_T("%d"),showVerts);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SHOWVERTS,str,filename);

	str.printf(_T("%d"),rendW);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERW,str,filename);
	str.printf(_T("%d"),rendH);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERH,str,filename);
	str.printf(_T("%d"),useBitmapRes);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,USEBITMAPRES,str,filename);

	str.printf(_T("%d"),pixelSnap);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,PIXELSNAP,str,filename);

	str.printf(_T("%d"),channel);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,CHANNEL,str,filename);

	str.printf(_T("%d"),falloff);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,FALLOFF,str,filename);
	str.printf(_T("%d"),falloffSpace);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,FALLOFFSPACE,str,filename);

	str.printf(_T("%f"),normalSpacing);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,NORMALSPACING,str,filename);
	str.printf(_T("%d"),normalNormalize);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,NORMALNORMALIZE,str,filename);
	str.printf(_T("%d"),normalRotate);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,NORMALROTATE,str,filename);
	str.printf(_T("%d"),normalAlignWidth);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,NORMALALIGNWIDTH,str,filename);
	str.printf(_T("%d"),normalMethod);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,NORMALMETHOD,str,filename);


	str.printf(_T("%f"),flattenAngleThreshold);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,FLATTENANGLE,str,filename);
	str.printf(_T("%f"),flattenSpacing);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,FLATTENSPACING,str,filename);
	str.printf(_T("%d"),flattenNormalize);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,FLATTENNORMALIZE,str,filename);
	str.printf(_T("%d"),flattenRotate);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,FLATTENROTATE,str,filename);
	str.printf(_T("%d"),flattenCollapse);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,FLATTENCOLLAPSE,str,filename);

	str.printf(_T("%d"),unfoldNormalize);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,UNFOLDNORMALIZE,str,filename);
	str.printf(_T("%d"),unfoldMethod);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,UNFOLDMETHOD,str,filename);

	str.printf(_T("%d"),bStitchAlign);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,STITCHBIAS,str,filename);
	str.printf(_T("%f"),fStitchBias);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,STITCHALIGN,str,filename);

	str.printf(_T("%d"),bTile);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,TILEON,str,filename);
	str.printf(_T("%f"),fContrast);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,TILEBRIGHTNESS,str,filename);
	str.printf(_T("%d"),iTileLimit);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,TILELIMIT,str,filename);

	str.printf(_T("%d"),limitSoftSel);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SOFTSELLIMIT,str,filename);
	str.printf(_T("%d"),limitSoftSelRange);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SOFTSELRANGE,str,filename);

	str.printf(_T("%d"),geomElemMode);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,GEOMELEMENTMODE,str,filename);
	str.printf(_T("%f"),planarThreshold);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,PLANARTHRESHOLD,str,filename);
	str.printf(_T("%d"),planarMode);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,PLANARMODE,str,filename);
	str.printf(_T("%d"),ignoreBackFaceCull);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,IGNOREBACKFACECULL,str,filename);
	str.printf(_T("%d"),oldSelMethod);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,OLDSELMETHOD,str,filename);

	str.printf(_T("%d"),tvElementMode);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,TVELEMENTMODE,str,filename);
	str.printf(_T("%d"),alwaysEdit);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,ALWAYSEDIT,str,filename);

	str.printf(_T("%d"),packMethod);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,PACKMETHOD,str,filename);
	str.printf(_T("%f"),packSpacing);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,PACKSPACING,str,filename);
	str.printf(_T("%d"),packNormalize);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,PACKNORMALIZE,str,filename);
	str.printf(_T("%d"),packRotate);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,PACKROTATE,str,filename);
	str.printf(_T("%d"),packFillHoles);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,PACKCOLLAPSE,str,filename);
	str.printf(_T("%d"),packRescaleCluster);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,PACKRESCALECLUSTER,str,filename);


	str.printf(_T("%d"),fillMode);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,FILLMODE,str,filename);
	str.printf(_T("%d"),displayOpenEdges);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,DISPLAYOPENEDGES,str,filename);
	str.printf(_T("%d"),uvEdgeMode);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,UVEDGEMMODE,str,filename);
	str.printf(_T("%d"),openEdgeMode);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,OPENEDGEMMODE,str,filename);
	str.printf(_T("%d"),displayHiddenEdges);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,DISPLAYHIDDENEDGES,str,filename);

	str.printf(_T("%d"),sketchSelMode);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SKETCHSELMODE,str,filename);
	str.printf(_T("%d"),sketchType);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SKETCHTYPE,str,filename);
	str.printf(_T("%d"),sketchInteractiveMode);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SKETCHINTERACTIVE,str,filename);
	str.printf(_T("%d"),sketchDisplayPoints);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SKETCHDISPLAYPOINTS,str,filename);


	str.printf(_T("%d"),hitSize);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,HITSIZE,str,filename);

	str.printf(_T("%d"),resetPivotOnSel);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RESETSELONPIVOT,str,filename);

	str.printf(_T("%d"),polyMode);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,POLYMODE,str,filename);

	str.printf(_T("%d"),allowSelectionInsideGizmo);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,ALLOWSELECTIONINSIDEGIZMO,str,filename);

	str.printf(_T("%f"),weldThreshold);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,WELDTHRESHOLD,str,filename);

	str.printf(_T("%d"),update);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,CONSTANTUPDATE,str,filename);

	str.printf(_T("%d"),midPixelSnap);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,MIDPIXELSNAP,str,filename);

/*
	str.printf(_T("%d %d %d "),(int) GetRValue(lineColor),(int) GetGValue(lineColor),(int) GetBValue(lineColor));
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,LINECOLOR,str,filename);

	str.printf(_T("%d %d %d "),(int) GetRValue(selColor),(int) GetGValue(selColor),(int) GetBValue(selColor));
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SELCOLOR,str,filename);

	str.printf(_T("%d %d %d "),(int) GetRValue(openEdgeColor),(int) GetGValue(openEdgeColor),(int) GetBValue(openEdgeColor));
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,OPENEDGECOLOR,str,filename);

	str.printf(_T("%d %d %d "),(int) GetRValue(handleColor),(int) GetGValue(handleColor),(int) GetBValue(handleColor));
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,HANDLECOLOR,str,filename);

	str.printf(_T("%d %d %d "),(int) GetRValue(freeFormColor),(int) GetGValue(freeFormColor),(int) GetBValue(freeFormColor));
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,TRANSFORMGIZMOCOLOR,str,filename);

	str.printf(_T("%d %d %d "),(int) GetRValue(sharedColor),(int) GetGValue(sharedColor),(int) GetBValue(sharedColor));
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SHAREDCOLOR,str,filename);
*/

	str.printf(_T("%d"),showShared );
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SHOWSHARED,str,filename);

	str.printf(_T("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"),
				showIconList[1],showIconList[2],showIconList[3],showIconList[4],showIconList[5],showIconList[6],showIconList[7],showIconList[8],showIconList[9],showIconList[10],
				showIconList[11],showIconList[12],showIconList[13],showIconList[14],showIconList[15],showIconList[16],showIconList[17],showIconList[18],showIconList[19],showIconList[20],
				showIconList[21],showIconList[22],showIconList[23],showIconList[24],showIconList[25],showIconList[26],showIconList[27],showIconList[28],showIconList[29],showIconList[30],
				showIconList[31]
				);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,ICONLIST,str,filename);


	str.printf(_T("%d"),syncSelection );
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SYNCSELECTION,str,filename);

	str.printf(_T("%d"),brightCenterTile );
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,BRIGHTCENTERTILE,str,filename);

	str.printf(_T("%d"),blendTileToBackGround );
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,BLENDTILE,str,filename);

	str.printf(_T("%d"),paintSize );
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,PAINTSIZE,str,filename);

	str.printf(_T("%d"),tickSize );
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,TICKSIZE,str,filename);

//new	
	str.printf(_T("%f"),gridSize );
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,GRIDSIZE,str,filename);

	str.printf(_T("%d"),gridSnap );
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,GRIDSNAP,str,filename);

	str.printf(_T("%d"),gridVisible );
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,GRIDVISIBLE,str,filename);

	str.printf(_T("%f"),gridStr );
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,GRIDSTR,str,filename);

	str.printf(_T("%d"),autoMap );
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,AUTOMAP,str,filename);

	str.printf(_T("%d"),enableSoftSelection );
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,ENABLESOFTSELECTION,str,filename);

	//5.1.05
	str.printf(_T("%d"),this->autoBackground);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,AUTOBACKGROUND,str,filename);

//5.1.06
	str.printf(_T("%f"),this->relaxAmount);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RELAXAMOUNT,str,filename);

	str.printf(_T("%d"),this->relaxIteration);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RELAXITERATIONS,str,filename);

	str.printf(_T("%d"),this->relaxBoundary);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RELAXBOUNDARY,str,filename);

	str.printf(_T("%d"),this->relaxSaddle);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RELAXCORNER,str,filename);

	str.printf(_T("%d"),thickOpenEdges);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,THICKOPENEDGES,str,filename);

	str.printf(_T("%d"),viewportOpenEdges);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,VIEWPORTOPENEDGES,str,filename);

	str.printf(_T("%d"),this->absoluteTypeIn);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,ABSOLUTETYPEIN,str,filename);

	str.printf(_T("%d"),bStitchScale);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,STITCHSCALE,str,filename);

	str.printf(_T("%d"),rotationsRespectAspect);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,ROTATIONSRESPECTASPECT,str,filename);


	str.printf(_T("%d"),move);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,MOVE,str,filename);

	str.printf(_T("%d"),scale);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,SCALE,str,filename);

	str.printf(_T("%d"),mirror);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,MIRROR,str,filename);
	
	str.printf(_T("%d"),uvw);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,UVW,str,filename);

	str.printf(_T("%d"),pixelSnap);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,PIXELSNAP,str,filename);

	str.printf(_T("%d"),fnGetGridSnap());
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,GRIDSNAP,str,filename);


	str.printf(_T("%d"),zoomext);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,ZOOMEXTENT,str,filename);

	str.printf(_T("%d"),mTVSubObjectMode);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,TVSUBOBJECTMODE,str,filename);

	str.printf(_T("%f"),relaxBySpringStretch);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RELAXBYSPRINGSTRETCH,str,filename);

	str.printf(_T("%d"),relaxBySpringIteration);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RELAXBYSPRINGITERATION,str,filename);


	str.printf(_T("%f"),this->relaxStretch);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RELAXSTRETCH,str,filename);

	str.printf(_T("%d"),this->relaxType);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RELAXTYPE,str,filename);

	int width;
	pblock->GetValue(unwrap_renderuv_width,0,width,FOREVER);
	str.printf(_T("%d"),width);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_WIDTH,str,filename);


	int height;
	pblock->GetValue(unwrap_renderuv_height,0,height,FOREVER);
	str.printf(_T("%d"),height);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_HEIGHT,str,filename);


 	Color edgeColor;
	pblock->GetValue(unwrap_renderuv_edgecolor,0,edgeColor,FOREVER);
	str.printf(_T("%f %f %f"),edgeColor.r,edgeColor.g,edgeColor.b);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_EDGECOLOR,str,filename);

	float edgeAlpha;
	pblock->GetValue(unwrap_renderuv_edgealpha,0,edgeAlpha,FOREVER);
	str.printf(_T("%f"),edgeAlpha);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_EDGEALPHA,str,filename);

 	Color seamColor;
	pblock->GetValue(unwrap_renderuv_seamcolor,0,seamColor,FOREVER);
	str.printf(_T("%f %f %f"),seamColor.r,seamColor.g,seamColor.b);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_SEAMCOLOR,str,filename);

	int r;
	pblock->GetValue(unwrap_renderuv_visible,0,r,FOREVER);
	str.printf(_T("%d"),r);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_RENDERVISIBLEEDGES,str,filename);

	pblock->GetValue(unwrap_renderuv_invisible,0,r,FOREVER);
	str.printf(_T("%d"),r);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_RENDERINVISIBLEEDGES,str,filename);

	pblock->GetValue(unwrap_renderuv_seamedges,0,r,FOREVER);
	str.printf(_T("%d"),r);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_RENDERSEAMEDGES,str,filename);


	pblock->GetValue(unwrap_renderuv_showframebuffer,0,r,FOREVER);
	str.printf(_T("%d"),r);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_SHOWFRAMEBUFFER,str,filename);

	pblock->GetValue(unwrap_renderuv_force2sided,0,r,FOREVER);
	str.printf(_T("%d"),r);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_FORCE2SIDED,str,filename);


	pblock->GetValue(unwrap_renderuv_fillmode,0,r,FOREVER);
	str.printf(_T("%d"),r);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_FILLMODE,str,filename);

	float fillAlpha;
	pblock->GetValue(unwrap_renderuv_fillalpha,0,fillAlpha,FOREVER);
	str.printf(_T("%f"),fillAlpha);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_FILLALPHA,str,filename);

 	Color fillColor;
	pblock->GetValue(unwrap_renderuv_fillcolor,0,fillColor,FOREVER);
	str.printf(_T("%f %f %f"),fillColor.r,fillColor.g,fillColor.b);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_FILLCOLOR,str,filename);


	pblock->GetValue(unwrap_renderuv_overlap,0,r,FOREVER);
	str.printf(_T("%d"),r);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_OVERLAP,str,filename);

 	Color overlapColor;
	pblock->GetValue(unwrap_renderuv_overlapcolor,0,overlapColor,FOREVER);
	str.printf(_T("%f %f %f"),overlapColor.r,overlapColor.g,overlapColor.b);
	MaxSDK::Util::WritePrivateProfileString(WSDSECTION,RENDERTEMPLATE_OVERLAPCOLOR,str,filename);


	if (saved_lc)  
 	{  
 		lc = _tsetlocale(LC_NUMERIC, saved_lc);  
 		free(saved_lc);  
 		saved_lc = NULL;  
	}  	
	

}

void	UnwrapMod::fnLoadDefaults()
{
	TCHAR filename[MAX_PATH];
	GetCfgFilename(filename);
	TCHAR str[MAX_PATH];
	TSTR def(_T("DISCARD"));


	TCHAR* saved_lc = NULL;
	TCHAR* lc = _tsetlocale(LC_NUMERIC, NULL); // query  
	if (lc)  
    saved_lc = _tcsdup(lc);  
	lc = _tsetlocale(LC_NUMERIC, _T("C"));  
	

	int res = GetPrivateProfileString(WSDSECTION,GETFACESELECTIONFROMSTACK,def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&getFaceSelectionFromStack);
		
	res = GetPrivateProfileString(WSDSECTION,FALLOFFSTR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&falloffStr);

	int full = 0;
	res = GetPrivateProfileString(WSDSECTION,WINDOWPOSX1,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
		{
		_stscanf(str,_T("%d"),&windowPos.rcNormalPosition.left);
		full++;
		}
	res = GetPrivateProfileString(WSDSECTION,WINDOWPOSY1,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
		{
		_stscanf(str,_T("%d"),&windowPos.rcNormalPosition.bottom);
		full++;
		}
	res = GetPrivateProfileString(WSDSECTION,WINDOWPOSX2,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
		{	
		_stscanf(str,_T("%d"),&windowPos.rcNormalPosition.right);
		full++;
		}
	res = GetPrivateProfileString(WSDSECTION,WINDOWPOSY2,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
		{
		_stscanf(str,_T("%d"),&windowPos.rcNormalPosition.top);
		full++;
		}
	if (full == 4) windowPos.length = sizeof(WINDOWPLACEMENT); 

	if ( windowPos.length && hDialogWnd ) {
		windowPos.flags = 0;
		windowPos.showCmd = SW_SHOWNORMAL;
		SetWindowPlacement(hDialogWnd,&windowPos);
	}

	res = GetPrivateProfileString(WSDSECTION,FORCEUPDATE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&forceUpdate);

	res = GetPrivateProfileString(WSDSECTION,LOCKASPECT,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&lockAspect);

	res = GetPrivateProfileString(WSDSECTION,SHOWMAP,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&showMap);
	res = GetPrivateProfileString(WSDSECTION,SHOWVERTS,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&showVerts);
	res = GetPrivateProfileString(WSDSECTION,RENDERW,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&rendW);
	res = GetPrivateProfileString(WSDSECTION,RENDERH,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&rendH);
	res = GetPrivateProfileString(WSDSECTION,USEBITMAPRES,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&useBitmapRes);
	res = GetPrivateProfileString(WSDSECTION,PIXELSNAP,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&pixelSnap);

	res = GetPrivateProfileString(WSDSECTION,CHANNEL,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&channel);

	res = GetPrivateProfileString(WSDSECTION,FALLOFF,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&falloff);
	res = GetPrivateProfileString(WSDSECTION,FALLOFFSPACE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&falloffSpace);

	res = GetPrivateProfileString(WSDSECTION,NORMALSPACING,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&normalSpacing);
	res = GetPrivateProfileString(WSDSECTION,NORMALNORMALIZE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&normalNormalize);
	res = GetPrivateProfileString(WSDSECTION,NORMALROTATE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&normalRotate);
	res = GetPrivateProfileString(WSDSECTION,NORMALALIGNWIDTH,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&normalAlignWidth);
	res = GetPrivateProfileString(WSDSECTION,NORMALMETHOD,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&normalMethod);


	res = GetPrivateProfileString(WSDSECTION,FLATTENANGLE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&flattenAngleThreshold);
	res = GetPrivateProfileString(WSDSECTION,FLATTENSPACING,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&flattenSpacing);
	res = GetPrivateProfileString(WSDSECTION,FLATTENNORMALIZE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&flattenNormalize);
	res = GetPrivateProfileString(WSDSECTION,FLATTENROTATE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&flattenRotate);
	res = GetPrivateProfileString(WSDSECTION,FLATTENCOLLAPSE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&flattenCollapse);

	res = GetPrivateProfileString(WSDSECTION,UNFOLDNORMALIZE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&unfoldNormalize);
	res = GetPrivateProfileString(WSDSECTION,UNFOLDMETHOD,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&unfoldMethod);


	res = GetPrivateProfileString(WSDSECTION,STITCHBIAS,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&bStitchAlign);
	res = GetPrivateProfileString(WSDSECTION,STITCHALIGN,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&fStitchBias);


	res = GetPrivateProfileString(WSDSECTION,TILEON,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&bTile);
	res = GetPrivateProfileString(WSDSECTION,TILEBRIGHTNESS,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&fContrast);
	res = GetPrivateProfileString(WSDSECTION,TILELIMIT,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&iTileLimit);


	res = GetPrivateProfileString(WSDSECTION,SOFTSELLIMIT,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&limitSoftSel);
	res = GetPrivateProfileString(WSDSECTION,SOFTSELRANGE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&limitSoftSelRange);


	res = GetPrivateProfileString(WSDSECTION,GEOMELEMENTMODE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&geomElemMode);
	res = GetPrivateProfileString(WSDSECTION,PLANARTHRESHOLD,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&planarThreshold);
	res = GetPrivateProfileString(WSDSECTION,PLANARMODE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&planarMode);
	res = GetPrivateProfileString(WSDSECTION,IGNOREBACKFACECULL,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&ignoreBackFaceCull);
	res = GetPrivateProfileString(WSDSECTION,OLDSELMETHOD,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&oldSelMethod);

	res = GetPrivateProfileString(WSDSECTION,TVELEMENTMODE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&tvElementMode);
	res = GetPrivateProfileString(WSDSECTION,ALWAYSEDIT,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&alwaysEdit);


	res = GetPrivateProfileString(WSDSECTION,PACKMETHOD,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&packMethod);
	res = GetPrivateProfileString(WSDSECTION,PACKSPACING,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&packSpacing);
	res = GetPrivateProfileString(WSDSECTION,PACKNORMALIZE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&packNormalize);
	res = GetPrivateProfileString(WSDSECTION,PACKROTATE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&packRotate);
	res = GetPrivateProfileString(WSDSECTION,PACKCOLLAPSE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&packFillHoles);
	res = GetPrivateProfileString(WSDSECTION,PACKRESCALECLUSTER,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&packRescaleCluster);


	res = GetPrivateProfileString(WSDSECTION,FILLMODE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&fillMode);
	res = GetPrivateProfileString(WSDSECTION,DISPLAYOPENEDGES,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&displayOpenEdges);
	res = GetPrivateProfileString(WSDSECTION,UVEDGEMMODE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&uvEdgeMode);
	res = GetPrivateProfileString(WSDSECTION,OPENEDGEMMODE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&openEdgeMode);
	res = GetPrivateProfileString(WSDSECTION,DISPLAYHIDDENEDGES,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&displayHiddenEdges);


	res = GetPrivateProfileString(WSDSECTION,SKETCHSELMODE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&sketchSelMode);
	res = GetPrivateProfileString(WSDSECTION,SKETCHTYPE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&sketchType);
	res = GetPrivateProfileString(WSDSECTION,SKETCHINTERACTIVE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&sketchInteractiveMode);
	res = GetPrivateProfileString(WSDSECTION,SKETCHDISPLAYPOINTS,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&sketchDisplayPoints);

	res = GetPrivateProfileString(WSDSECTION,HITSIZE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&hitSize);

	res = GetPrivateProfileString(WSDSECTION,RESETSELONPIVOT,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&resetPivotOnSel);

	res = GetPrivateProfileString(WSDSECTION,POLYMODE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&polyMode);

	res = GetPrivateProfileString(WSDSECTION,ALLOWSELECTIONINSIDEGIZMO,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&allowSelectionInsideGizmo);

	res = GetPrivateProfileString(WSDSECTION,WELDTHRESHOLD,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&weldThreshold);

	res = GetPrivateProfileString(WSDSECTION,CONSTANTUPDATE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&update);

	res = GetPrivateProfileString(WSDSECTION,MIDPIXELSNAP,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&midPixelSnap);

/*
	res = GetPrivateProfileString(WSDSECTION,LINECOLOR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
		{
		int r,g,b;
		_stscanf(str,_T("%d %d %d"),&r,&g,&b);
		lineColor = RGB(r,g,b);
		}

	res = GetPrivateProfileString(WSDSECTION,SELCOLOR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
		{
		int r,g,b;
		_stscanf(str,_T("%d %d %d"),&r,&g,&b);
		selColor = RGB(r,g,b);
		}

	res = GetPrivateProfileString(WSDSECTION,OPENEDGECOLOR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
		{
		int r,g,b;
		_stscanf(str,_T("%d %d %d"),&r,&g,&b);
		openEdgeColor = RGB(r,g,b);
		}

	res = GetPrivateProfileString(WSDSECTION,HANDLECOLOR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
		{
		int r,g,b;
		_stscanf(str,_T("%d %d %d"),&r,&g,&b);
		handleColor = RGB(r,g,b);
		}

	res = GetPrivateProfileString(WSDSECTION,TRANSFORMGIZMOCOLOR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
		{
		int r,g,b;
		_stscanf(str,_T("%d %d %d"),&r,&g,&b);
		freeFormColor = RGB(r,g,b);
		}

	res = GetPrivateProfileString(WSDSECTION,SHAREDCOLOR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
		{
		int r,g,b;
		_stscanf(str,_T("%d %d %d"),&r,&g,&b);
		sharedColor = RGB(r,g,b);
		}
*/

	res = GetPrivateProfileString(WSDSECTION,SHOWSHARED,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&showShared);

	res = GetPrivateProfileString(WSDSECTION,ICONLIST,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
		{
		int sIconList[32];
		_stscanf(str,_T("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"),
				&sIconList[1],&sIconList[2],&sIconList[3],&sIconList[4],&sIconList[5],&sIconList[6],&sIconList[7],&sIconList[8],&sIconList[9],&sIconList[10],
				&sIconList[11],&sIconList[12],&sIconList[13],&sIconList[14],&sIconList[15],&sIconList[16],&sIconList[17],&sIconList[18],&sIconList[19],&sIconList[20],
				&sIconList[21],&sIconList[22],&sIconList[23],&sIconList[24],&sIconList[25],&sIconList[26],&sIconList[27],&sIconList[28],&sIconList[29],&sIconList[30],
				&sIconList[31]
				);
		for (int i = 1; i < 32; i++)
			showIconList.Set(i,sIconList[i]);
		}


	res = GetPrivateProfileString(WSDSECTION,SYNCSELECTION,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&syncSelection);

	res = GetPrivateProfileString(WSDSECTION,BRIGHTCENTERTILE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&brightCenterTile);

	res = GetPrivateProfileString(WSDSECTION,BLENDTILE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&blendTileToBackGround);

	res = GetPrivateProfileString(WSDSECTION,PAINTSIZE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&paintSize);

	res = GetPrivateProfileString(WSDSECTION,TICKSIZE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&tickSize);


//new	
	res = GetPrivateProfileString(WSDSECTION,GRIDSIZE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&gridSize);

	res = GetPrivateProfileString(WSDSECTION,GRIDSNAP,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&gridSnap);

	res = GetPrivateProfileString(WSDSECTION,GRIDVISIBLE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&gridVisible);

	res = GetPrivateProfileString(WSDSECTION,GRIDSTR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&gridStr);

	res = GetPrivateProfileString(WSDSECTION,AUTOMAP,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&autoMap);

	res = GetPrivateProfileString(WSDSECTION,ENABLESOFTSELECTION,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&enableSoftSelection);

//5.1.04
	res = GetPrivateProfileString(WSDSECTION,AUTOBACKGROUND,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&autoBackground);

//5.1.06
	res = GetPrivateProfileString(WSDSECTION,RELAXAMOUNT,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&relaxAmount);

	res = GetPrivateProfileString(WSDSECTION,RELAXITERATIONS,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&relaxIteration);

	res = GetPrivateProfileString(WSDSECTION,RELAXBOUNDARY,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&relaxBoundary);

	res = GetPrivateProfileString(WSDSECTION,RELAXCORNER,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&relaxSaddle);

	res = GetPrivateProfileString(WSDSECTION,THICKOPENEDGES,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&thickOpenEdges);

	res = GetPrivateProfileString(WSDSECTION,VIEWPORTOPENEDGES,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&viewportOpenEdges);

	res = GetPrivateProfileString(WSDSECTION,ABSOLUTETYPEIN,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&absoluteTypeIn);

	res = GetPrivateProfileString(WSDSECTION,STITCHSCALE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&bStitchScale);


	res = GetPrivateProfileString(WSDSECTION,ROTATIONSRESPECTASPECT,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&rotationsRespectAspect);

	res = GetPrivateProfileString(WSDSECTION,MOVE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&move);

	res = GetPrivateProfileString(WSDSECTION,SCALE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&scale);

	res = GetPrivateProfileString(WSDSECTION,MIRROR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&mirror);

	res = GetPrivateProfileString(WSDSECTION,UVW,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&uvw);

	res = GetPrivateProfileString(WSDSECTION,PIXELSNAP,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&pixelSnap);

	res = GetPrivateProfileString(WSDSECTION,GRIDSNAP,  def,str,MAX_PATH,filename);
	BOOL snap;
	if ((res) && (_tcscmp(str,def))) 
	{
		_stscanf(str,_T("%d"),&snap);
		fnSetGridSnap(snap);
	}

	res = GetPrivateProfileString(WSDSECTION,SNAPSTATE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&initialSnapState);

	res = GetPrivateProfileString(WSDSECTION,ZOOMEXTENT,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&zoomext);

	res = GetPrivateProfileString(WSDSECTION,TVSUBOBJECTMODE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&mTVSubObjectMode);
        if (mTVSubObjectMode < 0) 
            mTVSubObjectMode = 0;


	res = GetPrivateProfileString(WSDSECTION,RELAXBYSPRINGSTRETCH,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&relaxBySpringStretch);

	res = GetPrivateProfileString(WSDSECTION,RELAXBYSPRINGITERATION,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&relaxBySpringIteration);

	res = GetPrivateProfileString(WSDSECTION,RELAXSTRETCH,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%f"),&relaxStretch);

	res = GetPrivateProfileString(WSDSECTION,RELAXTYPE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) _stscanf(str,_T("%d"),&relaxType);


	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_WIDTH,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		int width;
		_stscanf(str,_T("%d"),&width);
		pblock->SetValue(unwrap_renderuv_width,0,width);
	}

	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_HEIGHT,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		int height;
		_stscanf(str,_T("%d"),&height);
		pblock->SetValue(unwrap_renderuv_height,0,height);
	}


	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_EDGECOLOR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		Point3 c;
		_stscanf(str,_T("%f %f %f"),&c.x,&c.y,&c.z);
		pblock->SetValue(unwrap_renderuv_edgecolor,0,c);
	}

	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_EDGEALPHA,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		float a;
		_stscanf(str,_T("%f"),&a);
		pblock->SetValue(unwrap_renderuv_edgealpha,0,a);
	}

	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_SEAMCOLOR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		Point3 c;
		_stscanf(str,_T("%f %f %f"),&c.x,&c.y,&c.z);
		pblock->SetValue(unwrap_renderuv_seamcolor,0,c);
	}



	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_RENDERVISIBLEEDGES,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		int r;
		_stscanf(str,_T("%d"),&r);
		pblock->SetValue(unwrap_renderuv_visible,0,r);
	}

	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_RENDERINVISIBLEEDGES,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		int r;
		_stscanf(str,_T("%d"),&r);
		pblock->SetValue(unwrap_renderuv_invisible,0,r);
	}

	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_RENDERSEAMEDGES,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		int r;
		_stscanf(str,_T("%d"),&r);
		pblock->SetValue(unwrap_renderuv_seamedges,0,r);
	}

	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_SHOWFRAMEBUFFER,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		int r;
		_stscanf(str,_T("%d"),&r);
		pblock->SetValue(unwrap_renderuv_showframebuffer,0,r);
	}

	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_FORCE2SIDED,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		int r;
		_stscanf(str,_T("%d"),&r);
		pblock->SetValue(unwrap_renderuv_force2sided,0,r);
	}

	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_FILLMODE,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		int r;
		_stscanf(str,_T("%d"),&r);
		pblock->SetValue(unwrap_renderuv_fillmode,0,r);
	}

	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_FILLALPHA,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		float a;
		_stscanf(str,_T("%f"),&a);
		pblock->SetValue(unwrap_renderuv_fillalpha,0,a);
	}


	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_FILLCOLOR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		Point3 c;
		_stscanf(str,_T("%f %f %f"),&c.x,&c.y,&c.z);
		pblock->SetValue(unwrap_renderuv_fillcolor,0,c);
	}


	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_OVERLAP,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		int r;
		_stscanf(str,_T("%d"),&r);
		pblock->SetValue(unwrap_renderuv_overlap,0,r);
	}


	res = GetPrivateProfileString(WSDSECTION,RENDERTEMPLATE_OVERLAPCOLOR,  def,str,MAX_PATH,filename);
	if ((res) && (_tcscmp(str,def))) 
	{
		Point3 c;
		_stscanf(str,_T("%f %f %f"),&c.x,&c.y,&c.z);
		pblock->SetValue(unwrap_renderuv_overlapcolor,0,c);
	}




	if (saved_lc)  
 	{  
 		lc = _tsetlocale(LC_NUMERIC, saved_lc);  
 		free(saved_lc);  
 		saved_lc = NULL;  
	}  	


	InvalidateView();

}



