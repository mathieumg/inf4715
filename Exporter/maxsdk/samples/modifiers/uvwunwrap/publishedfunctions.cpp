/*

Copyright 2010 Autodesk, Inc.  All rights reserved. 

Use of this software is subject to the terms of the Autodesk license agreement provided at 
the time of installation or download, or which otherwise accompanies this software in either 
electronic or hard copy form. 

*/

#include "unwrap.h"
#include "modsres.h"
#include "modstack.h"
#include "PerformanceTools.h"
#include "TvConnectionInfo.h"

static UnwrapClassDesc unwrapDesc;



ClassDesc* GetUnwrapModDesc() {return &unwrapDesc;}

static FPInterfaceDesc unwrap_interface6(
	UNWRAP_INTERFACE6, _T("unwrap6"), 0, &unwrapDesc, FP_MIXIN,

	unwrap_getvertexpositionByNode, _T("GetVertexPositionByNode"),0,TYPE_POINT3,0,3,
							_T("time"),0,TYPE_TIMEVALUE,
							_T("index"),0,TYPE_INT,
							_T("node"),0,TYPE_INODE,

	unwrap_numberverticesByNode, _T("numberVerticesByNode"),0,TYPE_INT,0,1,
							_T("node"),0,TYPE_INODE,
	unwrap_getselectedpolygonsByNode,_T("getSelectedPolygonsByNode"),0,TYPE_BITARRAY,0,1,
							_T("node"),0,TYPE_INODE,
	unwrap_selectpolygonsByNode, _T("selectPolygonsByNode"),0,TYPE_VOID,0,2,
							_T("selection"),0,TYPE_BITARRAY,
							_T("node"),0,TYPE_INODE,
	unwrap_ispolygonselectedByNode, _T("isPolygonSelectedByNode"),0,TYPE_BOOL,0,2,
							_T("index"),0,TYPE_INT,
							_T("node"),0,TYPE_INODE,
	unwrap_numberpolygonsByNode, _T("numberPolygonsByNode"),0, TYPE_INT, 0, 1,
							_T("node"),0,TYPE_INODE,

	unwrap_markasdeadByNode, _T("markAsDeadByNode"),0,TYPE_VOID,0,2,
							_T("index"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,

	unwrap_numberpointsinfaceByNode, _T("numberPointsInFaceByNode"),0,TYPE_INT,0,2,
							_T("index"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,
	unwrap_getvertexindexfromfaceByNode, _T("getVertexIndexFromFaceByNode"),0,TYPE_INT,0,3,
							_T("faceIndex"), 0, TYPE_INT,
							_T("ithVertex"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,
	unwrap_gethandleindexfromfaceByNode, _T("getHandleIndexFromFaceByNode"),0,TYPE_INT,0,3,
							_T("faceIndex"), 0, TYPE_INT,
							_T("ithVertex"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,
	unwrap_getinteriorindexfromfaceByNode, _T("getHandleIndexFromFaceByNode"),0,TYPE_INT,0,3,
							_T("faceIndex"), 0, TYPE_INT,
							_T("ithVertex"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,
	unwrap_getvertexgindexfromfaceByNode, _T("getVertexGeomIndexFromFaceByNode"),0,TYPE_INT,0,3,
							_T("faceIndex"), 0, TYPE_INT,
							_T("ithVertex"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,
	unwrap_gethandlegindexfromfaceByNode, _T("getHandleGeomIndexFromFaceByNode"),0,TYPE_INT,0,3,
							_T("faceIndex"), 0, TYPE_INT,
							_T("ithVertex"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,
	unwrap_getinteriorgindexfromfaceByNode, _T("getInteriorGeomIndexFromFaceByNode"),0,TYPE_INT,0,3,
							_T("faceIndex"), 0, TYPE_INT,
							_T("ithVertex"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,

	unwrap_addpointtofaceByNode, _T("setFaceVertexByNode"),0,TYPE_VOID,0,5,
							_T("pos"), 0, TYPE_POINT3,
							_T("faceIndex"), 0, TYPE_INT,
							_T("ithVertex"), 0, TYPE_INT,
							_T("sel"), 0, TYPE_BOOL,
							_T("node"),0,TYPE_INODE,
	unwrap_addpointtohandleByNode, _T("setFaceHandleByNode"),0,TYPE_VOID,0,5,
							_T("pos"), 0, TYPE_POINT3,
							_T("faceIndex"), 0, TYPE_INT,
							_T("ithVertex"), 0, TYPE_INT,
							_T("sel"), 0, TYPE_BOOL,
							_T("node"),0,TYPE_INODE,
	unwrap_addpointtointeriorByNode, _T("setFaceInteriorByNode"),0,TYPE_VOID,0,5,
							_T("pos"), 0, TYPE_POINT3,
							_T("faceIndex"), 0, TYPE_INT,
							_T("ithVertex"), 0, TYPE_INT,
							_T("sel"), 0, TYPE_BOOL,
							_T("node"),0,TYPE_INODE,

	unwrap_setfacevertexindexByNode, _T("setFaceVertexIndexByNode"),0,TYPE_VOID,0,4,
							_T("faceIndex"), 0, TYPE_INT,
							_T("ithVertex"), 0, TYPE_INT,
							_T("vertexIndex"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,
	unwrap_setfacehandleindexByNode, _T("setFaceHandleIndexByNode"),0,TYPE_VOID,0,4,
							_T("faceIndex"), 0, TYPE_INT,
							_T("ithVertex"), 0, TYPE_INT,
							_T("vertexIndex"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,
	unwrap_setfaceinteriorindexByNode, _T("setFaceInteriorIndexByNode"),0,TYPE_VOID,0,4,
							_T("faceIndex"), 0, TYPE_INT,
							_T("ithVertex"), 0, TYPE_INT,
							_T("vertexIndex"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,

	unwrap_getareaByNode, _T("getAreaByNode"), 0, TYPE_VOID, 0, 4,
							_T("faceSelection"), 0, TYPE_BITARRAY,
							_T("areaUVW"), 0, TYPE_FLOAT_BR, f_inOut, FPP_OUT_PARAM,
							_T("areaGeom"), 0, TYPE_FLOAT_BR, f_inOut, FPP_OUT_PARAM,
							_T("node"),0,TYPE_INODE,

	unwrap_getboundsByNode, _T("getAreaByNode"), 0, TYPE_VOID, 0, 6,
							_T("faceSelection"), 0, TYPE_BITARRAY,
							_T("x"), 0, TYPE_FLOAT_BR, f_inOut, FPP_OUT_PARAM,
							_T("y"), 0, TYPE_FLOAT_BR, f_inOut, FPP_OUT_PARAM,
							_T("width"), 0, TYPE_FLOAT_BR, f_inOut, FPP_OUT_PARAM,
							_T("height"), 0, TYPE_FLOAT_BR, f_inOut, FPP_OUT_PARAM,
							_T("node"),0,TYPE_INODE,


	unwrap_pelt_getseamselectionByNode, _T("getPeltSelectedSeamsByNode"),0,TYPE_BITARRAY,0,1,
							_T("node"),0,TYPE_INODE,
	unwrap_pelt_setseamselectionByNode, _T("setPeltSelectedSeamsByNode"),0,TYPE_VOID,0,2,
							_T("selection"),0,TYPE_BITARRAY,
							_T("node"),0,TYPE_INODE,
	unwrap_selectpolygonsupdateByNode, _T("selectPolygonsUpdateByNode"),0,TYPE_VOID,0,3,
							_T("selection"),0,TYPE_BITARRAY,
							_T("update"),0,TYPE_BOOL,
							_T("node"),0,TYPE_INODE,

	unwrap_sketchByNode, _T("sketchByNode"),0, TYPE_VOID, 0, 3,
							_T("indexList"),0,TYPE_INT_TAB,
							_T("positionList"),0,TYPE_POINT3_TAB,
							_T("node"),0,TYPE_INODE,

	unwrap_getnormalByNode, _T("getNormalByNode"),0,TYPE_POINT3,0,2,
							_T("faceIndex"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,

	unwrap_setvertexpositionByNode, _T("setVertexPositionByNode"),0,TYPE_VOID,0,4,
							_T("time"), 0, TYPE_TIMEVALUE,
							_T("index"), 0, TYPE_INT,
							_T("pos"), 0, TYPE_POINT3,
							_T("node"),0,TYPE_INODE,

	unwrap_setvertexposition2ByNode, _T("setVertexPosition2ByNode"),0,TYPE_VOID,0,6,
							_T("time"), 0, TYPE_TIMEVALUE,
							_T("index"), 0, TYPE_INT,
							_T("pos"), 0, TYPE_POINT3,
							_T("hold"), 0, TYPE_BOOL,
							_T("update"), 0, TYPE_BOOL,
							_T("node"),0,TYPE_INODE,

	unwrap_getvertexweightByNode, _T("getVertexWeightByNode"), 0, TYPE_FLOAT, 0, 2,
							_T("index"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,
	unwrap_setvertexweightByNode, _T("setVertexWeightByNode"), 0, TYPE_VOID, 0, 3,
							_T("index"), 0, TYPE_INT,
							_T("weight"), 0, TYPE_FLOAT,
							_T("node"),0,TYPE_INODE,

	unwrap_isweightmodifiedByNode, _T("isWeightModifiedByNode"), 0, TYPE_BOOL, 0, 2,
							_T("index"), 0, TYPE_INT,
							_T("node"),0,TYPE_INODE,
	unwrap_modifyweightByNode, _T("modifyWeightByNode"), 0, TYPE_VOID, 0, 3,
							_T("index"), 0, TYPE_INT,
							_T("modify"), 0, TYPE_BOOL,
							_T("node"),0,TYPE_INODE,

	unwrap_getselectedvertsByNode, _T("getSelectedVerticesByNode"),0,TYPE_BITARRAY,0,1,
							_T("node"),0,TYPE_INODE,
	unwrap_selectvertsByNode, _T("selectVerticesByNode"),0,TYPE_VOID,0,2,
							_T("selection"),0,TYPE_BITARRAY,
							_T("node"),0,TYPE_INODE,

	unwrap_isvertexselectedByNode, _T("isVertexSelectedByNode"),0,TYPE_BOOL,0,2,
							_T("index"),0,TYPE_INT,
							_T("node"),0,TYPE_INODE,

	unwrap_getselectedfacesByNode, _T("getSelectedFacesByNode"),0,TYPE_BITARRAY,0,1,
							_T("node"),0,TYPE_INODE,
	unwrap_selectfacesByNode, _T("selectFacesByNode"),0,TYPE_VOID,0,2,
							_T("selection"),0,TYPE_BITARRAY,
							_T("node"),0,TYPE_INODE,
	unwrap_isfaceselectedByNode, _T("isFaceSelectedByNode"),0,TYPE_BOOL,0,2,
							_T("index"),0,TYPE_INT,
							_T("node"),0,TYPE_INODE,

	unwrap_getselectededgesByNode, _T("getSelectedEdgesByNode"),0,TYPE_BITARRAY,0,1,
							_T("node"),0,TYPE_INODE,
	unwrap_selectedgesByNode, _T("selectEdgesByNode"),0,TYPE_VOID,0,2,
							_T("selection"),0,TYPE_BITARRAY,
							_T("node"),0,TYPE_INODE,

	unwrap_isedgeselectedByNode, _T("isEdgeSelectedByNode"),0,TYPE_BOOL,0,2,
							_T("index"),0,TYPE_INT,
							_T("node"),0,TYPE_INODE,

	unwrap_getgeomvertexselectionByNode, _T("getSelectedGeomVertsByNode"),0,TYPE_BITARRAY,0,1,
						_T("node"),0,TYPE_INODE,
	unwrap_setgeomvertexselectionByNode, _T("setSelectedGeomVertsByNode"),0,TYPE_VOID,0,2,
						_T("selection"),0,TYPE_BITARRAY,
						_T("node"),0,TYPE_INODE,

	unwrap_getgeomedgeselectionByNode, _T("getSelectedGeomEdgesByNode"),0,TYPE_BITARRAY,0,1,
						_T("node"),0,TYPE_INODE,
	unwrap_setgeomedgeselectionByNode, _T("setSelectedGeomEdgesByNode"),0,TYPE_VOID,0,2,
						_T("selection"),0,TYPE_BITARRAY,
						_T("node"),0,TYPE_INODE,

	unwrap_getsg, _T("getSG"),0, TYPE_INT, 0, 0,
	unwrap_setsg, _T("setSG"),0, TYPE_VOID, 0, 1,
						_T("sgID"),0,TYPE_INT,

	unwrap_getMatIDSelect, _T("getSelectMatID"),0, TYPE_INT, 0, 0,
	unwrap_setMatIDSelect, _T("setSelectMatID"),0, TYPE_VOID, 0, 1,
						_T("matID"),0,TYPE_INT,


	unwrap_splinemap, _T("splineMap"),0, TYPE_VOID, 0, 0,

	unwrap_splinemap_selectSpline, _T("splineMap_selectSpline"),0,TYPE_VOID,0,2,
						_T("splineIndex"),0,TYPE_INDEX,
						_T("sel"),0,TYPE_BOOL,
	unwrap_splinemap_isSplineSelected, _T("splineMap_isSplineSelected"),0,TYPE_BOOL,0,1,
						_T("splineIndex"),0,TYPE_INDEX,	


	unwrap_splinemap_selectCrossSection, _T("splineMap_selectCrossSection"),0,TYPE_VOID,0,3,
						_T("splineIndex"),0,TYPE_INDEX,
						_T("crossSectionIndex"),0,TYPE_INDEX,
						_T("sel"),0,TYPE_BOOL,
	unwrap_splinemap_isCrossSectionSelected, _T("splineMap_isCrossSectionSelected"),0,TYPE_BOOL,0,2,
						_T("splineIndex"),0,TYPE_INDEX,	
						_T("crossSectionIndex"),0,TYPE_INDEX,


	unwrap_splinemap_clearSelectSpline, _T("splineMap_ClearSelectSpline"),0,TYPE_VOID,0,0,
	unwrap_splinemap_clearSelectCrossSection, _T("splineMap_ClearSelectCrossSection"),0,TYPE_VOID,0,0,


	unwrap_splinemap_numberOfSplines, _T("splineMap_numberSplines"),0,TYPE_INT,0,0,
	unwrap_splinemap_numberOfCrossSections, _T("splineMap_numberCrossSection"),0,TYPE_INT,0,1,
						_T("splineIndex"),0,TYPE_INDEX,	

	unwrap_splinemap_getCrossSection_Pos, _T("splineMap_GetCrossSection_Pos"),0,TYPE_POINT3,0,2,
						_T("splineIndex"),0,TYPE_INDEX,	
						_T("crossSectionIndex"),0,TYPE_INDEX,	
	unwrap_splinemap_getCrossSection_ScaleX, _T("splineMap_GetCrossSection_ScaleX"),0,TYPE_FLOAT,0,2,
						_T("splineIndex"),0,TYPE_INDEX,	
						_T("crossSectionIndex"),0,TYPE_INDEX,	
	unwrap_splinemap_getCrossSection_ScaleY, _T("splineMap_GetCrossSection_ScaleY"),0,TYPE_FLOAT,0,2,
						_T("splineIndex"),0,TYPE_INDEX,	
						_T("crossSectionIndex"),0,TYPE_INDEX,	
	unwrap_splinemap_getCrossSection_Twist, _T("splineMap_GetCrossSection_Twist"),0,TYPE_FLOAT,0,2,
						_T("splineIndex"),0,TYPE_INDEX,	
						_T("crossSectionIndex"),0,TYPE_INDEX,	

	unwrap_splinemap_setCrossSection_Pos, _T("splineMap_SetCrossSection_Pos"),0,TYPE_VOID,0,3,
						_T("splineIndex"),0,TYPE_INDEX,	
						_T("crossSectionIndex"),0,TYPE_INDEX,	
						_T("pos"),0,TYPE_POINT3,	
	unwrap_splinemap_setCrossSection_ScaleX, _T("splineMap_SetCrossSection_ScaleX"),0,TYPE_VOID,0,3,
						_T("splineIndex"),0,TYPE_INDEX,	
						_T("crossSectionIndex"),0,TYPE_INDEX,	
						_T("scaleX"),0,TYPE_FLOAT,	
 	unwrap_splinemap_setCrossSection_ScaleY, _T("splineMap_SetCrossSection_ScaleY"),0,TYPE_VOID,0,3,
						_T("splineIndex"),0,TYPE_INDEX,	
						_T("crossSectionIndex"),0,TYPE_INDEX,	
						_T("scaleY"),0,TYPE_FLOAT,	
	unwrap_splinemap_setCrossSection_Twist, _T("splineMap_SetCrossSection_Twist"),0,TYPE_VOID,0,3,
						_T("splineIndex"),0,TYPE_INDEX,	
						_T("crossSectionIndex"),0,TYPE_INDEX,	
						_T("Twist"),0,TYPE_FLOAT,	

	unwrap_splinemap_recomputeCrossSections, _T("splineMap_RecomputeCrossSections"),0, TYPE_VOID, 0, 0,

	unwrap_splinemap_fit, _T("splineMap_Fit"),0, TYPE_VOID, 0, 2,
						_T("fitAll"),0,TYPE_BOOL,
						_T("extraScale"),0,TYPE_FLOAT,


	unwrap_splinemap_align, _T("splineMap_Align"),0, TYPE_VOID, 0, 3,
						_T("splineIndex"),0,TYPE_INDEX,	
						_T("crossSectionIndex"),0,TYPE_INDEX,	
						_T("vec"),0,TYPE_POINT3,							
	unwrap_splinemap_alignSelected, _T("splineMap_AlignSelected"),0, TYPE_VOID, 0, 1,
						_T("vec"),0,TYPE_POINT3,							
	unwrap_splinemap_aligncommandmode, _T("splineMap_AlignCommandMode"),0, TYPE_VOID, 0, 0,
	unwrap_splinemap_alignsectioncommandmode, _T("splineMap_AlignSectionCommandMode"),0, TYPE_VOID, 0, 0,

	unwrap_splinemap_copy, _T("splineMap_Copy"),0, TYPE_VOID, 0, 0,
	unwrap_splinemap_paste, _T("splineMap_Paste"),0, TYPE_VOID, 0, 0,
	unwrap_splinemap_pasteToSelected, _T("splineMap_PasteToSelected"),0, TYPE_VOID, 0, 2,
						_T("splineIndex"),0,TYPE_INDEX,	
						_T("crossSectionIndex"),0,TYPE_INDEX,	

	unwrap_splinemap_delete, _T("splineMap_Delete"),0, TYPE_VOID, 0, 0,
	
	unwrap_splinemap_addCrossSectionCommandmode, _T("splineMap_AddCrossSectionMode"),0, TYPE_VOID, 0, 0,


	unwrap_splinemap_moveSelectedCrossSection, _T("splineMap_moveSelectedCrossSection"),0, TYPE_VOID, 0, 1,
						_T("u"),0,TYPE_FLOAT,	
	unwrap_splinemap_rotateSelectedCrossSection, _T("splineMap_rotateSelectedCrossSection"),0, TYPE_VOID, 0, 1,
						_T("twist"),0,TYPE_FLOAT,	
	unwrap_splinemap_scaleSelectedCrossSection, _T("splineMap_scaleSelectedCrossSection"),0, TYPE_VOID, 0, 1,
						_T("scale"),0,TYPE_POINT2,	

	unwrap_splinemap_insertCrossSection, _T("splineMap_InsertCrossSection"),0, TYPE_VOID, 0, 2,
						_T("splineIndex"),0,TYPE_INDEX,	
						_T("u"),0,TYPE_FLOAT,	


	unwrap_splinemap_dump, _T("splineMap_Dump"),0, TYPE_VOID, 0, 0,	

	unwrap_splinemap_resample, _T("splineMap_Resample"),0, TYPE_VOID, 0, 1,	
						_T("samples"),0,TYPE_INT,	


	unwrap_gettweakmode, _T("getTweakMode"),0, TYPE_BOOL, 0, 0,	
	unwrap_settweakmode, _T("setTweakMode"),0, TYPE_VOID, 0, 1,	
						_T("on"),0,TYPE_BOOL,	

	unwrap_uvloop, _T("uvLoop"),0, TYPE_VOID, 0, 1,	
						_T("mode"),0,TYPE_INT,	
	unwrap_uvring, _T("uvRing"),0, TYPE_VOID, 0, 1,	
						_T("mode"),0,TYPE_INT,	

	unwrap_align, _T("align"),0, TYPE_VOID, 0, 1,	
						_T("horizontal"),0,TYPE_BOOL,	
	unwrap_space, _T("space"),0, TYPE_VOID, 0, 1,	
						_T("horizontal"),0,TYPE_BOOL,	

	unwrap_regularmap_start, _T("RegularMapStart"),0,TYPE_VOID,0,2,
							_T("node"),0,TYPE_INODE,
							_T("showUI"),0,TYPE_BOOL,
	unwrap_regularmap_reset, _T("RegularMapReset"),0,TYPE_VOID,0,0,
	unwrap_regularmap_advanceUV, _T("RegularMapAdvanceUV"),0,TYPE_VOID,0,5,
							_T("uPos"),0,TYPE_BOOL,
							_T("vPos"),0,TYPE_BOOL,
							_T("uNeg"),0,TYPE_BOOL,
							_T("vNeg"),0,TYPE_BOOL,
							_T("singleStep"),0,TYPE_BOOL,
	unwrap_regularmap_advanceSelected, _T("RegularMapAdvanceSelected"),0,TYPE_VOID,0,1,
							_T("singleStep"),0,TYPE_BOOL,

	unwrap_regularmap_end, _T("RegularMapEnd"),0,TYPE_VOID,0,0,


	unwrap_regularmap_getautoweld, _T("RegularMapGetAutoWeld"),0,TYPE_BOOL,0,0,
	unwrap_regularmap_setautoweld, _T("RegularMapSetAutoWeld"),0,TYPE_VOID,0,1,
							_T("enable"),0,TYPE_BOOL,

	unwrap_regularmap_getlimit, _T("RegularMapGetLimit"),0,TYPE_INT,0,0,
	unwrap_regularmap_setlimit, _T("RegularMapSetLimit"),0,TYPE_VOID,0,1,
							_T("limit"),0,TYPE_INT,


	unwrap_regularmap_getautoweldthreshold, _T("RegularMapGetAutoWeldThreshold"),0,TYPE_FLOAT,0,0,
	unwrap_regularmap_setautoweldthreshold, _T("RegularMapSetAutoWeldThreshold"),0,TYPE_VOID,0,1,
							_T("threshold"),0,TYPE_FLOAT,

	unwrap_regularmap_geticonsize, _T("RegularMapGetIconSize"),0,TYPE_FLOAT,0,0,
	unwrap_regularmap_seticonsize, _T("RegularMapSetIconSize"),0,TYPE_VOID,0,1,
							_T("size"),0,TYPE_FLOAT,

	unwrap_regularmap_getautofit, _T("RegularMapGetAutoFit"),0,TYPE_INT,0,0,
	unwrap_regularmap_setautofit, _T("RegularMapSetAutoFit"),0,TYPE_VOID,0,1,
							_T("autofit"),0,TYPE_INT,

	unwrap_regularmap_getsinglestep, _T("RegularMapGetSingleStep"),0,TYPE_BOOL,0,0,
	unwrap_regularmap_setsinglestep, _T("RegularMapSetSingleStep"),0,TYPE_VOID,0,1,
							_T("enable"),0,TYPE_BOOL,

	unwrap_regularmap_getnormalize, _T("RegularMapGetNormalize"),0,TYPE_BOOL,0,0,
	unwrap_regularmap_setnormalize, _T("RegularMapSetNormalize"),0,TYPE_VOID,0,1,
							_T("enable"),0,TYPE_BOOL,

	unwrap_regularmap_resetfaces, _T("RegularMapResetFaces"),0,TYPE_VOID,0,0,


	unwrap_regularmap_startnewcluster, _T("RegularMapStartNewCluster"),0,TYPE_VOID,0,2,
								_T("node"),0,TYPE_INODE,
								_T("index"),0,TYPE_INDEX,

	unwrap_regularmap_expand, _T("RegularMapExpand"),0,TYPE_VOID,0,1,
								_T("expandBy"),0,TYPE_ENUM,regularmap_expansionenums,

	unwrap_ispinned, _T("IsPinned"),0,TYPE_BOOL,0,2,
							_T("index"),0,TYPE_INDEX,
							_T("node"),0,TYPE_INODE,

	unwrap_pin, _T("Pin"),0,TYPE_VOID,0,2,
							_T("index"),0,TYPE_INDEX,
							_T("node"),0,TYPE_INODE,

	unwrap_unpin, _T("Unpin"),0,TYPE_VOID,0,2,
							_T("index"),0,TYPE_INDEX,
							_T("node"),0,TYPE_INODE,

	unwrap_pinselected, _T("PinSelected"),0,TYPE_VOID,0,1,
							_T("node"),0,TYPE_INODE,
	unwrap_unpinselected, _T("UnpinSelected"),0,TYPE_VOID,0,1,
							_T("node"),0,TYPE_INODE,


	unwrap_lscm_interactive, _T("LSCMInteractive"),0,TYPE_VOID,0,1,
							_T("useExistingMapping"),0,TYPE_BOOL,
	unwrap_lscm_solve, _T("LSCMSolve"),0,TYPE_VOID,0,0,
	unwrap_lscm_reset, _T("LSCMReset"),0,TYPE_VOID,0,0,

	unwrap_rescalecluster,_T("RescaleCluster"),0,TYPE_VOID,0,2,
							_T("facesel"),0,TYPE_BITARRAY,
							_T("node"),0,TYPE_INODE,

	unwrap_getpackrescalecluster, _T("GetPackRescaleCluster"),0,TYPE_BOOL,0,0,
	unwrap_setpackrescalecluster, _T("SetPackRescaleCluster"),0,TYPE_VOID,0,1,
							_T("rescale"),0,TYPE_BOOL,

	
	unwrap_showtoolbar, _T("ShowToolBar"),0,TYPE_VOID,0,1,
							_T("visible"),0,TYPE_BOOL,

	unwrap_addfloater, _T("AddFloater"),0,TYPE_VOID,0,1,
							_T("name"),0,TYPE_STRING,
	unwrap_deletefloater, _T("DeleteFloater"),0,TYPE_VOID,0,1,
							_T("name"),0,TYPE_STRING,
	unwrap_showfloater, _T("ShowFloater"),0,TYPE_VOID,0,2,
							_T("name"),0,TYPE_STRING,
							_T("vis"),0,TYPE_BOOL,

	unwrap_weldallshared, _T("WeldAllShared"),0,TYPE_VOID,0,0,
	unwrap_weldselectedshared, _T("WeldSelectedShared"),0,TYPE_VOID,0,0,

	unwrap_relaxoneclick, _T("RelaxOneClick"),0,TYPE_VOID,0,0,

	unwrap_group_create, _T("GroupCreate"),0,TYPE_VOID,0,1,
							_T("name"),0,TYPE_STRING,
	unwrap_group_delete, _T("GroupDelete"),0,TYPE_VOID,0,1,
							_T("name"),0,TYPE_STRING,
	unwrap_group_rename, _T("GroupRename"),0,TYPE_VOID,0,2,
							_T("name"),0,TYPE_STRING,
							_T("newName"),0,TYPE_STRING,
	unwrap_group_select, _T("GroupSelect"),0,TYPE_VOID,0,1,
							_T("name"),0,TYPE_STRING,

	unwrap_group_create_byselection,_T("GroupCreateBySelection"),0,TYPE_VOID,0,0,
	unwrap_group_delete_byselection,_T("GroupDeleteBySelection"),0,TYPE_VOID,0,0,
	unwrap_group_select_byselection,_T("GroupSelectBySelection"),0,TYPE_VOID,0,0,
	unwrap_group_gettexeldensity,_T("GroupGetTexelDensity"),0,TYPE_FLOAT,0,0,
	unwrap_group_settexeldensity,_T("GroupSetTexelDensity"),0,TYPE_FLOAT,0,1,
							_T("value"),0,TYPE_FLOAT,

	unwrap_addtoolbar, _T("AddToolBar"),0,TYPE_VOID,0,7,
                                                 _T("owner"),0,TYPE_INT,
						_T("name"),0,TYPE_STRING,
						_T("pos"),0,TYPE_INT,
						_T("x"),0,TYPE_INT,
						_T("y"),0,TYPE_INT,
						_T("width"),0,TYPE_INT,
						_T("popup"),0,TYPE_BOOL,

	unwrap_straighten, _T("Straighten"),0,TYPE_VOID,0,0,


	unwrap_flatten_bysmoothinggroup, _T("FlattenBySmoothingGroup"),0,TYPE_VOID,0,3,
					_T("rescale"),0,TYPE_BOOL,
					_T("rotate"),0,TYPE_BOOL,
					_T("padding"),0,TYPE_FLOAT,

	unwrap_flatten_bymatid, _T("FlattenByMaterialID"),0,TYPE_VOID,0,3,
					_T("rescale"),0,TYPE_BOOL,
					_T("rotate"),0,TYPE_BOOL,
					_T("padding"),0,TYPE_FLOAT,


	enums,
		regularmap_expansionenums,9,
		_T("smgrp"), regularmap_expand_smgrp,
		_T("planar"), regularmap_expand_planar,
		_T("edgeSel"), regularmap_expand_edgesel,
		_T("faceSel"), regularmap_expand_facesel,
		_T("outerBorder"), regularmap_expand_outerborder,
		_T("innerBorder"), regularmap_expand_innerborder,
		_T("ring"), regularmap_expand_ring,
		_T("peltseams"), regularmap_expand_peltseams,
		_T("loop"), regularmap_expand_loop,

	p_end
	);




class UnwrapPBAccessor : public PBAccessor
{ 
	public:
		void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t)    // set from v
		{
			UnwrapMod* p = (UnwrapMod*)owner;
			if (id == unwrap_splinemap_node)
			{
				INode *node = (INode *) v.r;
				p->SetSplineMappingNode(node);
			}
			if ((id == unwrap_splinemap_node) ||
				(id == unwrap_splinemap_manualseams) ||
				(id == unwrap_splinemap_projectiontype) 
				)
			{
				p->fnSplineMap_UpdateUI();
			}
		}
};


static UnwrapPBAccessor unwrap_accessor;


static ParamBlockDesc2 unwrap_param_blk ( unwrap_params, _T("params"),  0, &unwrapDesc, 
										 P_AUTO_CONSTRUCT + + P_AUTO_UI+ P_MULTIMAP, PBLOCK_REF, 
										 // params
										 1,
										 unwrap_renderuv_params,  IDD_UNWRAP_RENDERUVS_PARAMS, IDS_RENDERUV_PARAMETERS, 0, 0, NULL,

										 unwrap_texmaplist, 	_T("texMapList"),		TYPE_TEXMAP_TAB, 		0,	P_VARIABLE_SIZE, 0,
										 p_end, 

										 unwrap_checkmtl,	_T("checkerMaterial"), 		TYPE_MTL, 	0, 	0, 
										 p_end,

										 unwrap_originalmtl,	_T("baseMaterial"), 		TYPE_MTL, 	0, 	0, 
										 p_end,

										 unwrap_texmapidlist, 	_T("texMapIDList"),		TYPE_INT_TAB, 		0,	P_VARIABLE_SIZE, 0,
										 p_end, 

										 unwrap_gridsnap,	_T("gridSnap"),		TYPE_BOOL, 		0,	 IDS_GRIDSNAP,
										 p_default, TRUE,
										 p_end, 
										 unwrap_vertexsnap,	_T("vertexSnap"),		TYPE_BOOL, 		0,	 IDS_VERTEXSNAP,
										 p_default, TRUE,
										 p_end, 
										 unwrap_edgesnap,	_T("edgeSnap"),		TYPE_BOOL, 		0,	 IDS_EDGESNAP,
										 p_default, TRUE,
										 p_end, 

										 unwrap_showimagealpha,	_T("showImageAlpha"),		TYPE_BOOL, 		0,	 IDS_SHOWIMAGEALPHA,
										 p_default, FALSE,
										 p_end, 

										 unwrap_renderuv_width, 	_T("renderuv_width"), 	TYPE_INT, 	 0, 	IDS_RENDERUV_WIDTH, 
										 p_default, 		1024, 
										 p_range, 		1, 10000, 
										 p_ui, 			unwrap_renderuv_params,TYPE_SPINNER, EDITTYPE_INT, IDC_WIDTH, IDC_WIDTHSPIN, SPIN_AUTOSCALE,  
										 p_end, 

										 unwrap_renderuv_height, 	_T("renderuv_height"), 	TYPE_INT, 	 0, 	IDS_RENDERUV_HEIGHT, 
										 p_default, 		1024, 
										 p_range, 		1, 10000, 
										 p_ui, 			unwrap_renderuv_params,TYPE_SPINNER, EDITTYPE_INT, IDC_HEIGHT, IDC_HEIGHTSPIN, SPIN_AUTOSCALE,  
										 p_end, 

										 unwrap_renderuv_edgecolor,	 _T("renderuv_edgeColor"),	TYPE_RGBA,		0,	IDS_EDGECOLOR,	
										 p_default,		Color(1,1,1), 
										 p_ui,			unwrap_renderuv_params,	TYPE_COLORSWATCH, IDC_UNWRAP_EDGECOLOR, 
										 p_end,

										 unwrap_renderuv_edgealpha, 	_T("renderuv_edgealpha"), 	TYPE_FLOAT, 	 0, 	IDS_RENDERUV_EDGEALPHA, 
										 p_default, 		1.0f, 
										 p_range, 		0.0f, 1.0f, 
										 p_ui, 			unwrap_renderuv_params,TYPE_SPINNER, EDITTYPE_FLOAT, IDC_EDGEALPHA, IDC_EDGEALPHASPIN, SPIN_AUTOSCALE,  
										 p_end, 

										 unwrap_renderuv_seamcolor,	 _T("renderuv_seamColor"),	TYPE_RGBA,		0,	IDS_SEAMCOLOR,	
										 p_default,		Color(0,1,0), 
										 p_ui,			unwrap_renderuv_params,TYPE_COLORSWATCH, IDC_UNWRAP_SEAMCOLOR, 
										 p_end,

										 unwrap_renderuv_visible,	_T("renderuv_visibleedges"),		TYPE_BOOL, 		0,	 IDS_VISIBLEEDGES,
										 p_default, TRUE,
										 p_ui,unwrap_renderuv_params,TYPE_SINGLECHEKBOX, IDC_VISIBLE_CHECK, 
										 p_end,
										 unwrap_renderuv_invisible,	_T("renderuv_invisibleedges"),		TYPE_BOOL, 		0,	 IDS_INVISIBLEEDGES,
										 p_default, FALSE,
										 p_ui,unwrap_renderuv_params,TYPE_SINGLECHEKBOX, IDC_INVISIBLE_CHECK, 
										 p_end,

										 unwrap_renderuv_seamedges,	_T("renderuv_seamedges"),		TYPE_BOOL, 		0,	 IDS_SEAMEDGES,
										 p_default, TRUE,
										 p_ui,unwrap_renderuv_params,TYPE_SINGLECHEKBOX, IDC_SEAM_CHECK, 
										 p_end,

										 unwrap_renderuv_showframebuffer,	_T("renderuv_showframebuffer"),		TYPE_BOOL, 		0,	 IDS_SHOWFRAMEBUFFER,
										 p_default, TRUE,
										 p_end,

										 unwrap_renderuv_force2sided,	_T("renderuv_force2sided"),		TYPE_BOOL, 		0,	 IDS_FORCE2SIDED,
										 p_default, TRUE,
										 p_ui,unwrap_renderuv_params,TYPE_SINGLECHEKBOX, IDC_FORCE2SIDE_CHECK, 
										 p_end,

										 unwrap_renderuv_fillmode, 			_T("renderuv_fillmode"), 		TYPE_INT, 	0, 	IDS_FILLMODE, 
										 p_default, 		0, 
										 p_ui,unwrap_renderuv_params, TYPE_INTLISTBOX, IDC_FILLMODECOMBO, 
										 4, IDS_NONE, IDS_SOLID, IDS_NORMAL, IDS_SHADED,		
										 p_end,

										 unwrap_renderuv_fillalpha, 	_T("renderuv_fillalpha"), 	TYPE_FLOAT, 	 0, 	IDS_RENDERUV_FILLALPHA, 
										 p_default, 		1.0f, 
										 p_range, 		0.0f, 1.0f, 
										 p_ui, 			unwrap_renderuv_params,TYPE_SPINNER, EDITTYPE_FLOAT, IDC_FILLALPHA, IDC_FILLALPHASPIN, SPIN_AUTOSCALE,  
										 p_end, 

										 unwrap_renderuv_fillcolor,	 _T("renderuv_fillColor"),	TYPE_RGBA,		0,	IDS_FILLCOLOR,	
										 p_default,		Color(0.5,0.5,0.5), 
										 p_ui,			unwrap_renderuv_params,TYPE_COLORSWATCH, IDC_UNWRAP_FILLCOLOR, 
										 p_end,


										 unwrap_renderuv_overlap,	_T("renderuv_showoverlap"),		TYPE_BOOL, 		0,	 IDS_OVERLAP,
										 p_default, TRUE,
										 p_ui,unwrap_renderuv_params,TYPE_SINGLECHEKBOX, IDC_OVERLAP_CHECK, 
										 p_end,

										 unwrap_renderuv_overlapcolor,	 _T("renderuv_overlapColor"),	TYPE_RGBA,		0,	IDS_OVERLAPCOLOR,	
										 p_default,		Color(1.0,0.0,0.0), 
										 p_ui,			unwrap_renderuv_params,TYPE_COLORSWATCH, IDC_UNWRAP_OVERLAPCOLOR, 
										 p_end,


										 unwrap_qmap_preview,	_T("quick_map_preview"),		TYPE_BOOL, 		0,	 IDS_PREVIEW,
										 p_default, TRUE,
										 p_end,

										 unwrap_qmap_align,	_T("quick_map_align"),		TYPE_INT, 		0,	 IDS_QMAPALIGN,
										 p_default, 3,
										 p_end,

										 unwrap_originalmtl_list,	_T("baseMaterial_list"), 		TYPE_MTL_TAB, 		0,	P_VARIABLE_SIZE, 0,
										 p_end,


										 unwrap_splinemap_node,	_T("splinemap_node"), 		TYPE_INODE, 		0,	 0,
										 p_accessor,	&unwrap_accessor,
										 p_end,

										 unwrap_splinemap_manualseams,	_T("splinemap_manualseams"),		TYPE_BOOL, 		0,	 0,
										 p_default, FALSE,
										 p_accessor,	&unwrap_accessor,
										 p_end,

										 unwrap_splinemap_projectiontype,	_T("splinemap_projectiontype"),		TYPE_INT, 		0,	 0,
										 p_default, 0,
										 p_accessor,	&unwrap_accessor,
										 p_end,

										 unwrap_splinemap_display,	_T("splinemap_display"),		TYPE_BOOL, 		0,	 0,
										 p_default, TRUE,
										 p_end,


										 unwrap_splinemap_uscale,	_T("splinemap_uscale"),		TYPE_FLOAT, 		0,	 0,
										 p_default, 1.0f,
										 p_end,

										 unwrap_splinemap_vscale,	_T("splinemap_vscale"),		TYPE_FLOAT, 		0,	 0,
										 p_default, 1.0f,
										 p_end,

										 unwrap_splinemap_uoffset,	_T("splinemap_uoffset"),		TYPE_FLOAT, 		0,	 0,
										 p_default, 0.0f,
										 p_end,

										 unwrap_splinemap_voffset,	_T("splinemap_voffset"),		TYPE_FLOAT, 		0,	 0,
										 p_default, 0.0f,
										 p_end,

										 unwrap_localDistorion,	_T("localDistortion"),		TYPE_BOOL, 		0,	 0,
										 p_default, FALSE,
										 p_end,

										 unwrap_splinemap_iterations,	_T("splinemap_iterations"),		TYPE_INT, 		0,	 0,
										  p_range, 		1, 64, 
										 p_default, 16,
										 p_end,


										 unwrap_splinemap_resample_count,	_T("splinemap_resampleCount"),		TYPE_INT, 		0,	 0,
										 p_range, 		2, 256, 
										 p_default, 4,
										 p_end,

										 unwrap_splinemap_method,	_T("splinemap_advanceMethod"),		TYPE_INT, 		0,	 0,										 
										 p_default, 1,
										 p_end,

										 unwrap_toolbar_visible,	_T("toolBarVisible"),		TYPE_BOOL, 		0,	 0,										 
										 p_default, FALSE,
										 p_end,

										 unwrap_floaters, _T("floaters"), TYPE_STRING_TAB,0,P_VARIABLE_SIZE,0,
											p_end,
										unwrap_floaters_visible, _T("floatersVisible"), TYPE_BOOL_TAB,0,P_VARIABLE_SIZE,0,
											p_end,


										 unwrap_buttonpanel_visible,	_T("buttonpanel_visible"),		TYPE_BOOL, 		0,	 0,										 
										 p_default, FALSE,
										 p_end,
										 unwrap_buttonpanel_width,	_T("buttonpanel_width"),		TYPE_INT, 		0,	 0,										 
										 p_default, 360,
										 p_end,
										 unwrap_buttonpanel_height1,	_T("buttonpanel_height1"),		TYPE_INT, 		0,	 0,										 
										 p_default, 360,
										 p_end,
										 unwrap_buttonpanel_height2,	_T("buttonpanel_height2"),		TYPE_INT, 		0,	 0,										 
										 p_default, 700,
										 p_end,


										 unwrap_weldonlyshared,	_T("weldOnlyShared"),		TYPE_BOOL, 		0,	 0,										 
										 p_default, TRUE,
										 p_end,



										 unwrap_group_name, _T("groupName"), TYPE_STRING_TAB,0,P_VARIABLE_SIZE,0,
											p_end,
										 unwrap_group_density, _T("groupDensity"), TYPE_FLOAT_TAB,0,P_VARIABLE_SIZE,0,
											p_end,

										 unwrap_group_display, _T("groupDisplay"),		TYPE_BOOL, 		0,	 0,										 
											p_default, FALSE,
											p_end,

										unwrap_autopin, _T("autoPin"),		TYPE_BOOL, 		0,	 0,										 
											p_default, TRUE,
											p_end, 
										unwrap_filterpin, _T("filterPin"),		TYPE_BOOL, 		0,	 0,										 
											p_default, FALSE,
											p_end,
										//dont want maxscript access to this, they can through the fpn FlattenByMaterialID or FlattenBySmoothingGroup
										unwrap_flattenby, _T(""),		TYPE_INT, 		0,	 0,										 
											p_default, 0,
											p_end,
										unwrap_peel_autoedit, _T("peelAutoEdit"),		TYPE_BOOL, 		0,	 0,										 
											p_default, TRUE,
											p_end,


										unwrap_alignbypivot_horizontal, _T("alignByPivotHorizontal"),		TYPE_VOID, 		0,	 0,										 
											p_end,
										unwrap_alignbypivot_vertical, _T("alignByPivotVertical"),		TYPE_VOID, 		0,	 0,										 
											p_end,



										 p_end


										 );

UnwrapMod::UnwrapMod()
{
	mMapGizmoTM.IdentityMatrix();
	mRelaxThreadHandle = NULL;	
	mPeltThreadHandle = NULL;	
	mGizmoTM = Matrix3(1);
	mHasMax9Data = FALSE;
	
	suppressWarning = FALSE;

	minimized = FALSE;
	renderUVWindow = NULL;
	showCounter = FALSE;
	xWindowOffset = 0;
	yWindowOffset = 0;
	maximizeHeight = 0;
	maximizeWidth = 0;
	CurrentMap = 0;
	relaxBySpringUseOnlyVEdges = FALSE;
	relaxBySpringIteration = 10;
	relaxBySpringStretch = 0.0f;

	edgeDistortionScale = 1.0f;
	showEdgeDistortion = FALSE;
	normalizeMap = TRUE;
	alwaysShowSeams = TRUE;
	mapMapMode = NOMAP;
	initialSnapState = 0;
	suspendNotify = FALSE;
	flattenMax5 = FALSE;
	absoluteTypeIn = TRUE;
	rotationsRespectAspect = TRUE;
	//5.1.06
	relaxAmount = 0.1f;
	relaxType = 1;
	relaxIteration = 100;
	relaxStretch = 0.0f;
	relaxBoundary = FALSE;
	relaxSaddle = FALSE;
	relaxWindowPos.length = 0;
	peltWindowPos.length = 0;

	popUpDialog = TRUE;
	//5.1.05
	autoBackground = TRUE;

	bringUpPanel = FALSE;
	modifierInstanced = FALSE;

	mMouseHitLocalData = NULL;
	mMouseHitVert = 0;

	loadDefaults = TRUE;
	subObjCount = 3;

	getFaceSelectionFromStack = FALSE;
	falloffStr = 0.0f;
	forceUpdate = TRUE;

	windowPos.length =0;
	mapScale = .0f;
	lockAspect = TRUE;
	move    = 0;
	zoom    = 0.75f;
	aspect	= 1.0f;
	xscroll = 0.0f;
	yscroll = 0.0f;

	image = NULL;
	iw = ih = 0;
	uvw = 0;
	scale =0;
	zoomext =0;
	showMap = TRUE;	
	rendW = 256;
	rendH = 256;
	channel = 0;
	//	CurrentMap = 0;
	isBitmap = 0;
	useBitmapRes = TRUE;
	pixelSnap =0;

	tmControl = NULL;

	flags = CONTROL_CENTER|CONTROL_FIT|CONTROL_INIT;

	version = CURRENTVERSION;
	lockSelected = 0;
	mirror = 0;
	filterSelectedFaces = 0;
	hide = 0;
	freeze = 0;
	incSelected = 0;
	falloff = 0;
	falloffSpace = 0;

	oldDataPresent = FALSE;

	updateCache = FALSE;

	//UNFOLD
	updateCache = TRUE;
	showVertexClusterList = FALSE;
	gDebugLevel = 1;

	normalSpacing = 0.02f;
	normalNormalize = TRUE;
	normalRotate = TRUE;
	normalAlignWidth = TRUE;
	normalMethod = 0;
	normalWindowPos.length = 0;
	normalHWND =  NULL;

	flattenAngleThreshold = 45.0f;
	flattenSpacing = 0.02f;
	flattenNormalize = TRUE;
	flattenRotate = TRUE;
	flattenCollapse = TRUE;
	packRescaleCluster = TRUE;
	flattenWindowPos.length = 0;
	flattenHWND =  NULL;

	unfoldWindowPos.length = 0;
	unfoldHWND =  NULL;
	unfoldNormalize = TRUE;
	unfoldMethod = 1;

	//STITCH
	bStitchAlign = TRUE;
	bStitchScale = TRUE;
	fStitchBias = 0.5f;
	stitchHWND = NULL;
	stitchWindowPos.length = 0;

	//tile stuff
	bTile = FALSE;
	fContrast = 0.8f;
	iTileLimit = 1;

	//put back after finish menu
	if (GetCOREInterface()->GetMenuManager()->RegisterMenuBarContext(kUnwrapMenuBar, _T("UVW Unwrap Menu Bar"))) 
	{
		IMenu* pMenu = GetCOREInterface()->GetMenuManager()->FindMenu(_T("UVW Unwrap Menu Bar"));

		if (!pMenu) 
		{
			pMenu = GetIMenu();
			pMenu->SetTitle(_T("UVW Unwrap Menu Bar"));
			GetCOREInterface()->GetMenuManager()->RegisterMenu(pMenu, 0);

			IMenuBarContext* pContext = (IMenuBarContext*) GetCOREInterface()->GetMenuManager()->GetContext(kUnwrapMenuBar);
			pContext->SetMenu(pMenu);
			pContext->CreateWindowsMenu();

		}
	}

	limitSoftSel = TRUE;
	limitSoftSelRange = 16;

	//elem mode
	geomElemMode = FALSE;

	planarThreshold = 15.0f;
	planarMode = FALSE;

	ignoreBackFaceCull = TRUE;
	oldSelMethod = FALSE;

	tvElementMode = FALSE;
	alwaysEdit = FALSE;

	packMethod = 0;
	packSpacing = 0.02f;
	packNormalize = TRUE;
	packRotate = FALSE;
	packFillHoles = FALSE;
	packWindowPos.length = 0;

	mTVSubObjectMode = TVOBJECTMODE;
	fillMode = FILL_MODE_FDIAGONAL;

	displayOpenEdges = TRUE;
	thickOpenEdges = TRUE;
	viewportOpenEdges = TRUE;

	uvEdgeMode = FALSE;
	openEdgeMode = FALSE;
	displayHiddenEdges = FALSE;

	freeFormPivotOffset = Point3(0.0f,0.0f,0.0f);
	inRotation = FALSE;

	sketchSelMode = SKETCH_SELDRAG;
	sketchType = SKETCH_LINE;
	restoreSketchSettings = FALSE;
	sketchWindowPos.length = 0;

	sketchInteractiveMode = FALSE;
	sketchDisplayPoints = TRUE;

	hitSize = 4;

	resetPivotOnSel = TRUE;

	polyMode = TRUE;

	allowSelectionInsideGizmo = FALSE;

	showShared = TRUE;

	showIconList.SetSize(32);
	showIconList.SetAll();

	syncSelection = TRUE;
	brightCenterTile = TRUE;
	blendTileToBackGround = FALSE;

	paintSize = 8;

	sketchCursorSize = 8;
	tickSize = 2;

	//new
	gridSize = 0.1f;
	gridSnap = FALSE;
	gridVisible = FALSE;
	gridStr = 0.1f;
	autoMap = FALSE;
	gridColor = RGB(150,150,150);

	preventFlattening = FALSE;

	enableSoftSelection = FALSE;

	for (int i =0;i<16;i++)
		circleCur[i] = NULL;

	BOOL iret;
	TSTR name,category;

	name.printf(_T("%s"),GetString(IDS_PW_LINECOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(LINECOLORID,name,category, lineColor);

	name.printf(_T("%s"),GetString(IDS_PW_SELCOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(SELCOLORID, name, category, selColor);

	name.printf(_T("%s"),GetString(IDS_PW_OPENEDGECOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(OPENEDGECOLORID, name, category, openEdgeColor);

	name.printf(_T("%s"),GetString(IDS_PW_HANDLECOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(HANDLECOLORID, name,category, handleColor);

	name.printf(_T("%s"),GetString(IDS_PW_FREEFORMCOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(FREEFORMCOLORID, name, category, freeFormColor);

	name.printf(_T("%s"),GetString(IDS_PW_SHAREDCOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(SHAREDCOLORID, name, category, sharedColor);

	name.printf(_T("%s"),GetString(IDS_PW_BACKGROUNDCOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(BACKGROUNDCOLORID, name, category, backgroundColor);

	//new
	name.printf(_T("%s"),GetString(IDS_PW_GRIDCOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(GRIDCOLORID, name, category, gridColor);

	COLORREF geomEdgeColor = RGB(230,0,000);
	name.printf(_T("%s"),GetString(IDS_PW_GEOMEDGECOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(GEOMEDGECOLORID, name, category, geomEdgeColor);

	COLORREF geomVertexColor = RGB(230,0,000);
	name.printf(_T("%s"),GetString(IDS_PW_GEOMVERTEXCOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(GEOMVERTCOLORID, name, category, geomVertexColor);

	COLORREF peltSeamColor = RGB(13,178,255);
	name.printf(_T("%s"),GetString(IDS_PELT_SEAMCOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(PELTSEAMCOLORID, name, category, peltSeamColor);

	COLORREF edgeDistortionColor = RGB(255,0,0);
	name.printf(_T("%s"),GetString(IDS_PW_EDGEDISTORTIONGOALCOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(EDGEDISTORTIONGOALCOLORID, name, category, edgeDistortionColor);

	edgeDistortionColor = RGB(0,255,0);
	name.printf(_T("%s"),GetString(IDS_PW_EDGEDISTORTIONCOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(EDGEDISTORTIONCOLORID, name, category, edgeDistortionColor);

	edgeDistortionColor = RGB(0,255,0);
	name.printf(_T("%s"),GetString(IDS_PW_EDGEDISTORTIONCOLOR));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(EDGEDISTORTIONCOLORID, name, category, edgeDistortionColor);

	COLORREF checkerA = RGB(128,128,128);
	name.printf(_T("%s"),GetString(IDS_CHECKA));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(CHECKERACOLORID, name, category, checkerA);

	COLORREF checkerB = RGB(255,255,255);
	name.printf(_T("%s"),GetString(IDS_CHECKB));
	category.printf(_T("%s"),GetString(IDS_PW_UNWRAPCOLOR));
	iret = ColorMan()->RegisterColor(CHECKERBCOLORID, name, category, checkerB);

	//	fnLoadDefaults();

	floaterWindowActive = FALSE;

	optionsDialogActive = FALSE;
	applyToWholeObject = FALSE;

	for (int i = 0; i < 90; i++)
		map[i] = NULL;
	checkerMat = NULL;
	originalMat = NULL;

	pblock = NULL;
	unwrapDesc.MakeAutoParamBlocks(this);

	//create our texture reference

	StdMat *stdMat = NewDefaultStdMat();
	if (stdMat != NULL)
	{
		stdMat->SetName(_T("UnwrapChecker"));
	}
	Texmap *checkerMap = (Texmap *) CreateInstance(TEXMAP_CLASS_ID, Class_ID(CHECKER_CLASS_ID,0));
	StdUVGen *uvGen = NULL;
	if (checkerMap != NULL)
	{
		uvGen = (StdUVGen*)checkerMap->GetTheUVGen();
		uvGen->SetUScl(10.0f, 0);
		uvGen->SetVScl(10.0f, 0);
		checkerMap->SetName(_T("CheckerPattern"));

		enum { 	checker_blur, checker_color1, checker_color2 };//hack here since we dont expose the param ids for checker but they should be static

		IParamBlock2 *checkerPBlock =  checkerMap->GetParamBlock(0);

		COLORREF cColor = ColorMan()->GetColor(CHECKERACOLORID);
		Point3 color( (float) GetRValue(cColor)/255.0f,
			(float) GetGValue(cColor)/255.0f,
			(float) GetBValue(cColor)/255.0f );

		checkerPBlock->SetValue(checker_color1,0,color);

		COLORREF c2Color = ColorMan()->GetColor(CHECKERBCOLORID);
		Point3 color2( (float) GetRValue(c2Color)/255.0f,
			(float) GetGValue(c2Color)/255.0f,
			(float) GetBValue(c2Color)/255.0f );
		checkerPBlock->SetValue(checker_color2,0,color2);
	}

	if (stdMat != NULL)
	{
		stdMat->SetSubTexmap(1, checkerMap);
		pblock->SetValue(unwrap_checkmtl,0,stdMat);
	}
	//	ReplaceReference(100,stdMat);

	TCHAR cfgFile[1024];
	_tcscpy(cfgFile, GetCOREInterface()->GetDir(APP_PLUGCFG_DIR));
	mToolBarIniFileName.printf(_T("%s\\UnwrapToolBar.ini"),cfgFile);

	mUIManager.SetMod(this, hInstance);

	mSideBarUI.Init(this);
	mModifierPanelUI.Init(this);

	matid = -1;
	mToolGrouping.Init(this);


	
	Point3 color(0.0f,0.0f,0.0f);
	mColors.Append(1,&color,30);
	
	color = Point3(0.9f,0.0f,0.0f);
	mColors.Append(1,&color,30);

	color = Point3(0.0f,0.9f,0.0f);
	mColors.Append(1,&color,30);

	color = Point3(0.0f,0.0f,0.9f);
	mColors.Append(1,&color,30);

	color = Point3(0.9f,0.90f,0.0f);
	mColors.Append(1,&color,30);

	color = Point3(0.0f,0.9f,0.9f);
	mColors.Append(1,&color,30);

	color = Point3(0.9f,0.0f,0.9f);
	mColors.Append(1,&color,30);



	color = Point3(0.49f,0.0f,0.0f);
	mColors.Append(1,&color,30);

	color = Point3(0.0f,0.49f,0.0f);
	mColors.Append(1,&color,30);

	color = Point3(0.0f,0.0f,0.49f);
	mColors.Append(1,&color,30);

	color = Point3(0.49f,0.49f,0.0f);
	mColors.Append(1,&color,30);

	color = Point3(0.0f,0.49f,0.49f);
	mColors.Append(1,&color,30);


	color = Point3(0.9f,0.49f,0.0f);
	mColors.Append(1,&color,30);

	color = Point3(0.0f,0.49f,0.9f);
	mColors.Append(1,&color,30);


	color = Point3(0.49f,0.9f,0.0f);
	mColors.Append(1,&color,30);

	color = Point3(0.0f,0.9f,0.49f);
	mColors.Append(1,&color,30);


	color = Point3(0.49f,0.0f,0.49f);
	mColors.Append(1,&color,30);

	color = Point3(0.9f,0.0f,0.49f);
	mColors.Append(1,&color,30);

	color = Point3(0.49f,0.0f,0.9f);
	mColors.Append(1,&color,30);

	color = Point3(0.49f,0.49f,0.9f);
	mColors.Append(1,&color,30);

	color = Point3(0.9f,0.49f,0.49f);
	mColors.Append(1,&color,30);

	color = Point3(0.9f,0.49f,0.9f);
	mColors.Append(1,&color,30);

	color = Point3(0.9f,0.9f,0.49f);
	mColors.Append(1,&color,30);

	color = Point3(0.49f,0.9f,0.9f);
	mColors.Append(1,&color,30);


	color = Point3(0.49f,0.49f,0.49f);
	mColors.Append(1,&color,30);

	color = Point3(0.9f,0.9f,0.9f);
	mColors.Append(1,&color,30);


	mPackTempRescale = true;
	mPackTempRotate = false;
	mPackTempPadding = 0.02f;


	scaleCorner = 0;
	scaleCornerOpposite = 2;

}



//Function Publishing descriptor for Mixin interface
//*****************************************************
static FPInterfaceDesc unwrap_interface(
										UNWRAP_INTERFACE, _T("unwrap"), 0, &unwrapDesc, FP_MIXIN,
										unwrap_planarmap, _T("planarMap"), 0, TYPE_VOID, 0, 0,
										unwrap_save, _T("save"), 0, TYPE_VOID, 0, 0,
										unwrap_load, _T("load"), 0, TYPE_VOID, 0, 0,

										unwrap_reset, _T("reset"), 0, TYPE_VOID, 0, 0,
										unwrap_edit, _T("edit"), 0, TYPE_VOID, 0, 0,

										unwrap_setMapChannel, _T("setMapChannel"),0, TYPE_VOID, 0, 1,
										_T("mapChannel"), 0, TYPE_INT,
										unwrap_getMapChannel, _T("getMapChannel"),0, TYPE_INT, 0, 0,

										unwrap_setProjectionType, _T("setProjectionType"),0, TYPE_VOID, 0, 1,
										_T("mapChannel"), 0, TYPE_INT,
										unwrap_getProjectionType, _T("getProjectionType"),0, TYPE_INT, 0, 0,

										unwrap_setVC, _T("setVC"),0, TYPE_VOID, 0, 1,
										_T("vertexColor"), 0, TYPE_BOOL,
										unwrap_getVC, _T("getVC"),0, TYPE_BOOL, 0, 0,

										unwrap_move, _T("move"),0, TYPE_VOID, 0, 0,
										unwrap_moveh, _T("moveh"),0, TYPE_VOID, 0, 0,
										unwrap_movev, _T("movev"),0, TYPE_VOID, 0, 0,

										unwrap_rotate, _T("rotate"),0, TYPE_VOID, 0, 0,

										unwrap_scale, _T("scale"),0, TYPE_VOID, 0, 0,
										unwrap_scaleh, _T("scaleh"),0, TYPE_VOID, 0, 0,
										unwrap_scalev, _T("scalev"),0, TYPE_VOID, 0, 0,

										unwrap_mirrorh, _T("mirrorH"),0, TYPE_VOID, 0, 0,
										unwrap_mirrorv, _T("mirrorV"),0, TYPE_VOID, 0, 0,

										unwrap_expandsel, _T("expandSelection"),0, TYPE_VOID, 0, 0,
										unwrap_contractsel, _T("contractSelection"),0, TYPE_VOID, 0, 0,

										unwrap_setFalloffType, _T("setFalloffType"),0, TYPE_VOID, 0, 1,
										_T("falloffType"), 0, TYPE_INT,
										unwrap_getFalloffType, _T("getFalloffType"),0, TYPE_INT, 0, 0,
										unwrap_setFalloffSpace, _T("setFalloffSpace"),0, TYPE_VOID, 0, 1,
										_T("falloffSpace"), 0, TYPE_INT,
										unwrap_getFalloffSpace, _T("getFalloffSpace"),0, TYPE_INT, 0, 0,
										unwrap_setFalloffDist, _T("setFalloffDist"),0, TYPE_VOID, 0, 1,
										_T("falloffDist"), 0, TYPE_FLOAT,
										unwrap_getFalloffDist, _T("getFalloffDist"),0, TYPE_FLOAT, 0, 0,

										unwrap_breakselected, _T("breakSelected"),0, TYPE_VOID, 0, 0,
										unwrap_weld, _T("weld"),0, TYPE_VOID, 0, 0,
										unwrap_weldselected, _T("weldSelected"),0, TYPE_VOID, 0, 0,

										unwrap_updatemap, _T("updateMap"),0, TYPE_VOID, 0, 0,
										unwrap_displaymap, _T("DisplayMap"),0, TYPE_VOID, 0, 1,
										_T("displayMap"), 0, TYPE_BOOL,
										unwrap_ismapdisplayed, _T("IsMapDisplayed"),0, TYPE_BOOL, 0, 0,

										unwrap_setuvspace, _T("setUVSpace"),0, TYPE_VOID, 0, 1,
										_T("UVSpace"), 0, TYPE_INT,
										unwrap_getuvspace, _T("getUVSpace"),0, TYPE_INT, 0, 0,

										unwrap_options, _T("options"),0, TYPE_VOID, 0, 0,

										unwrap_lock, _T("lock"),0, TYPE_VOID, 0, 0,

										unwrap_hide, _T("hide"),0, TYPE_VOID, 0, 0,
										unwrap_unhide, _T("unhide"),0, TYPE_VOID, 0, 0,

										unwrap_freeze, _T("freeze"),0, TYPE_VOID, 0, 0,
										unwrap_thaw, _T("unfreeze"),0, TYPE_VOID, 0, 0,
										unwrap_filterselected, _T("filterselected"),0, TYPE_VOID, 0, 0,

										unwrap_pan, _T("pan"),0, TYPE_VOID, 0, 0,
										unwrap_zoom, _T("zoom"),0, TYPE_VOID, 0, 0,
										unwrap_zoomregion, _T("zoomRegion"),0, TYPE_VOID, 0, 0,
										unwrap_fit, _T("fit"),0, TYPE_VOID, 0, 0,
										unwrap_fitselected, _T("fitselected"),0, TYPE_VOID, 0, 0,

										unwrap_snap, _T("snap"),0, TYPE_VOID, 0, 0,

										unwrap_getcurrentmap, _T("getCurrentMap"),0, TYPE_INT, 0, 0,
										unwrap_setcurrentmap, _T("setCurrentMap"),0, TYPE_VOID, 0, 1,
										_T("map"), 0, TYPE_INT,
										unwrap_numbermaps, _T("numberMaps"),0, TYPE_INT, 0, 0,

										unwrap_getlinecolor, _T("getLineColor"),0, TYPE_POINT3, 0, 0,
										unwrap_setlinecolor, _T("setLineColor"),0, TYPE_VOID, 0, 1,
										_T("color"), 0, TYPE_POINT3,

										unwrap_getselectioncolor, _T("getSelectionColor"),0, TYPE_POINT3, 0, 0,
										unwrap_setselectioncolor, _T("setSelectionColor"),0, TYPE_VOID, 0, 1,
										_T("color"), 0, TYPE_POINT3,



										unwrap_getrenderwidth, _T("getRenderWidth"),0, TYPE_INT, 0, 0,
										unwrap_setrenderwidth, _T("setRenderWidth"),0, TYPE_VOID, 0, 1,
										_T("width"), 0, TYPE_INT,
										unwrap_getrenderheight, _T("getRenderHeight"),0, TYPE_INT, 0, 0,
										unwrap_setrenderheight, _T("setRenderHeight"),0, TYPE_VOID, 0, 1,
										_T("height"), 0, TYPE_INT,

										unwrap_getusebitmapres, _T("getUseBitmapRes"),0, TYPE_BOOL, 0, 0,
										unwrap_setusebitmapres, _T("setUseBitmapRes"),0, TYPE_VOID, 0, 1,
										_T("useRes"), 0, TYPE_BOOL,

										unwrap_getweldtheshold, _T("getWeldThreshold"),0, TYPE_FLOAT, 0, 0,
										unwrap_setweldtheshold, _T("setWeldThreshold"),0, TYPE_VOID, 0, 1,
										_T("height"), 0, TYPE_FLOAT,

										unwrap_getconstantupdate, _T("getConstantUpdate"),0, TYPE_BOOL, 0, 0,
										unwrap_setconstantupdate, _T("setConstantUpdate"),0, TYPE_VOID, 0, 1,
										_T("update"), 0, TYPE_BOOL,

										unwrap_getshowselectedvertices, _T("getShowSelectedVertices"),0, TYPE_BOOL, 0, 0,
										unwrap_setshowselectedvertices, _T("setShowSelectedVertices"),0, TYPE_VOID, 0, 1,
										_T("show"), 0, TYPE_BOOL,

										unwrap_getmidpixelsnap, _T("getMidPixelSnap"),0, TYPE_BOOL, 0, 0,
										unwrap_setmidpixelsnap, _T("setMidPixelSnap"),0, TYPE_VOID, 0, 1,
										_T("snap"), 0, TYPE_BOOL,

										unwrap_getmatid, _T("getMatID"),0, TYPE_INT, 0, 0,
										unwrap_setmatid, _T("setMatID"),0, TYPE_VOID, 0, 1,
										_T("matid"), 0, TYPE_INT,
										unwrap_numbermatids, _T("numberMatIDs"),0, TYPE_INT, 0, 0,

										unwrap_getselectedverts, _T("getSelectedVertices"),0,TYPE_BITARRAY,0,0,
										unwrap_selectverts, _T("selectVertices"),0,TYPE_VOID,0,1,
										_T("selection"),0,TYPE_BITARRAY,

										unwrap_isvertexselected, _T("isVertexSelected"),0,TYPE_BOOL,0,1,
										_T("index"),0,TYPE_INT,

										unwrap_moveselectedvertices, _T("MoveSelectedVertices"),0,TYPE_VOID,0,1,
										_T("offset"),0,TYPE_POINT3,

										unwrap_rotateselectedverticesc, _T("RotateSelectedVerticesCenter"),0,TYPE_VOID,0,1,
										_T("angle"),0,TYPE_FLOAT,
										unwrap_rotateselectedvertices, _T("RotateSelectedVertices"),0,TYPE_VOID,0,2,
										_T("angle"),0,TYPE_FLOAT,
										_T("axis"),0,TYPE_POINT3,
										unwrap_scaleselectedverticesc, _T("ScaleSelectedVerticesCenter"),0,TYPE_VOID,0,2,
										_T("scale"),0,TYPE_FLOAT,
										_T("dir"),0,TYPE_INT,
										unwrap_scaleselectedvertices, _T("ScaleSelectedVertices"),0,TYPE_VOID,0,3,
										_T("scale"),0,TYPE_FLOAT,
										_T("dir"),0,TYPE_INT,
										_T("axis"),0,TYPE_POINT3,



										unwrap_getvertexposition, _T("GetVertexPosition"),0,TYPE_POINT3,0,2,
										_T("time"),0,TYPE_TIMEVALUE,
										_T("index"),0,TYPE_INT,
										unwrap_numbervertices, _T("numberVertices"),0,TYPE_INT,0,0,

										unwrap_movex, _T("moveX"),0,TYPE_VOID,0,1,
										_T("p"),0,TYPE_FLOAT,
										unwrap_movey, _T("moveY"),0,TYPE_VOID,0,1,
										_T("p"),0,TYPE_FLOAT,
										unwrap_movez, _T("moveZ"),0,TYPE_VOID,0,1,
										_T("p"),0,TYPE_FLOAT,

										unwrap_getselectedpolygons,_T("getSelectedPolygons"),0,TYPE_BITARRAY,0,0,
										unwrap_selectpolygons, _T("selectPolygons"),0,TYPE_VOID,0,1,
										_T("selection"),0,TYPE_BITARRAY,
										unwrap_ispolygonselected, _T("isPolygonSelected"),0,TYPE_BOOL,0,1,
										_T("index"),0,TYPE_INT,
										unwrap_numberpolygons, _T("numberPolygons"),0, TYPE_INT, 0, 0,
										unwrap_detachedgeverts, _T("detachEdgeVertices"),0, TYPE_VOID, 0, 0,

										unwrap_fliph, _T("flipHorizontal"),0, TYPE_VOID, 0, 0,
										unwrap_flipv, _T("flipVertical"),0, TYPE_VOID, 0, 0,

										unwrap_getlockaspect, _T("getLockAspect"),0, TYPE_BOOL, 0, 0,
										unwrap_setlockaspect, _T("setLockAspect"),0, TYPE_VOID, 0, 1,
										_T("aspect"), 0, TYPE_BOOL,

										unwrap_getmapscale, _T("getMapScale"),0, TYPE_FLOAT, 0, 0,
										unwrap_setmapscale, _T("setMapScale"),0, TYPE_VOID, 0, 1,
										_T("scale"), 0, TYPE_FLOAT,

										unwrap_getselectionfromface, _T("getSelectionFromFace"),0, TYPE_VOID, 0, 0,

										unwrap_forceupdate, _T("forceUpdate"),0, TYPE_VOID, 0, 1,
										_T("update"), 0, TYPE_BOOL,

										unwrap_zoomtogizmo, _T("zoomToGizmo"),0, TYPE_VOID, 0, 1,
										_T("all"), 0, TYPE_BOOL,

										unwrap_setvertexposition, _T("setVertexPosition"),0,TYPE_VOID,0,3,
										_T("time"), 0, TYPE_TIMEVALUE,
										_T("index"), 0, TYPE_INT,
										_T("pos"), 0, TYPE_POINT3,
										unwrap_markasdead, _T("markAsDead"),0,TYPE_VOID,0,1,
										_T("index"), 0, TYPE_INT,

										unwrap_numberpointsinface, _T("numberPointsInFace"),0,TYPE_INT,0,1,
										_T("index"), 0, TYPE_INT,

										unwrap_getvertexindexfromface, _T("getVertexIndexFromFace"),0,TYPE_INT,0,2,
										_T("faceIndex"), 0, TYPE_INT,
										_T("ithVertex"), 0, TYPE_INT,
										unwrap_gethandleindexfromface, _T("getHandleIndexFromFace"),0,TYPE_INT,0,2,
										_T("faceIndex"), 0, TYPE_INT,
										_T("ithVertex"), 0, TYPE_INT,
										unwrap_getinteriorindexfromface, _T("getInteriorIndexFromFace"),0,TYPE_INT,0,2,
										_T("faceIndex"), 0, TYPE_INT,
										_T("ithVertex"), 0, TYPE_INT,

										unwrap_getvertexgindexfromface, _T("getVertexGeomIndexFromFace"),0,TYPE_INT,0,2,
										_T("faceIndex"), 0, TYPE_INT,
										_T("ithVertex"), 0, TYPE_INT,
										unwrap_gethandlegindexfromface, _T("getHandleGeomIndexFromFace"),0,TYPE_INT,0,2,
										_T("faceIndex"), 0, TYPE_INT,
										_T("ithVertex"), 0, TYPE_INT,
										unwrap_getinteriorgindexfromface, _T("getInteriorGeomIndexFromFace"),0,TYPE_INT,0,2,
										_T("faceIndex"), 0, TYPE_INT,
										_T("ithVertex"), 0, TYPE_INT,

										unwrap_addpointtoface, _T("setFaceVertex"),0,TYPE_VOID,0,4,
										_T("pos"), 0, TYPE_POINT3,
										_T("faceIndex"), 0, TYPE_INT,
										_T("ithVertex"), 0, TYPE_INT,
										_T("sel"), 0, TYPE_BOOL,
										unwrap_addpointtohandle, _T("setFaceHandle"),0,TYPE_VOID,0,4,
										_T("pos"), 0, TYPE_POINT3,
										_T("faceIndex"), 0, TYPE_INT,
										_T("ithVertex"), 0, TYPE_INT,
										_T("sel"), 0, TYPE_BOOL,

										unwrap_addpointtointerior, _T("setFaceInterior"),0,TYPE_VOID,0,4,
										_T("pos"), 0, TYPE_POINT3,
										_T("faceIndex"), 0, TYPE_INT,
										_T("ithVertex"), 0, TYPE_INT,
										_T("sel"), 0, TYPE_BOOL,

										unwrap_setfacevertexindex, _T("setFaceVertexIndex"),0,TYPE_VOID,0,3,
										_T("faceIndex"), 0, TYPE_INT,
										_T("ithVertex"), 0, TYPE_INT,
										_T("vertexIndex"), 0, TYPE_INT,
										unwrap_setfacehandleindex, _T("setFaceHandleIndex"),0,TYPE_VOID,0,3,
										_T("faceIndex"), 0, TYPE_INT,
										_T("ithVertex"), 0, TYPE_INT,
										_T("vertexIndex"), 0, TYPE_INT,
										unwrap_setfaceinteriorindex, _T("setFaceInteriorIndex"),0,TYPE_VOID,0,3,
										_T("faceIndex"), 0, TYPE_INT,
										_T("ithVertex"), 0, TYPE_INT,
										_T("vertexIndex"), 0, TYPE_INT,
										unwrap_updateview, _T("updateView"),0,TYPE_VOID,0,0,
										unwrap_getfaceselfromstack, _T("getFaceSelectionFromStack"),0,TYPE_VOID,0,0,




										p_end
										);



static FPInterfaceDesc unwrap_interface2(
	UNWRAP_INTERFACE2, _T("unwrap2"), 0, &unwrapDesc, FP_MIXIN,

	//UNFOLD STUFF

	unwrap_selectpolygonsupdate, _T("selectPolygonsUpdate"),0,TYPE_VOID,0,2,
	_T("selection"),0,TYPE_BITARRAY,
	_T("update"),0,TYPE_BOOL,
	unwrap_selectfacesbynormal, _T("selectFacesByNormal"), 0, TYPE_VOID, 0, 3,
	_T("normal"), 0, TYPE_POINT3,
	_T("threshold"), 0, TYPE_FLOAT,
	_T("update"),0,TYPE_BOOL,
	unwrap_selectclusterbynormal, _T("selectClusterByNormal"), 0, TYPE_VOID, 0, 4,
	_T("threshold"), 0, TYPE_FLOAT,
	_T("faceIndexSeed"), 0, TYPE_INT,
	_T("relative"),0,TYPE_BOOL,
	_T("update"),0,TYPE_BOOL,

	unwrap_flattenmap, _T("flattenMap"), 0, TYPE_VOID, 0, 7,
	_T("angleThreshold"), 0, TYPE_FLOAT,
	_T("normalList"), 0, TYPE_POINT3_TAB,
	_T("spacing"), 0, TYPE_FLOAT,
	_T("normalize"), 0, TYPE_BOOL,
	_T("layOutType"), 0, TYPE_INT,
	_T("rotateClusters"), 0, TYPE_BOOL,
	_T("fillHoles"), 0, TYPE_BOOL,


	unwrap_normalmap, _T("normalMap"),0, TYPE_VOID, 0, 6,
	_T("normalList"), 0, TYPE_POINT3_TAB,
	_T("spacing"), 0, TYPE_FLOAT,
	_T("normalize"), 0, TYPE_BOOL,
	_T("layOutType"), 0, TYPE_INT,
	_T("rotateClusters"), 0, TYPE_BOOL,
	_T("alignWidth"), 0, TYPE_BOOL,
	unwrap_normalmapnoparams, _T("normalMapNoParams"),0, TYPE_VOID, 0, 0,
	unwrap_normalmapdialog, _T("normalMapDialog"),0, TYPE_VOID, 0, 0,

	unwrap_unfoldmap, _T("unfoldMap"), 0, TYPE_VOID, 0, 1,
	_T("unfoldMethod"), 0, TYPE_INT,
	unwrap_unfoldmapnoparams, _T("unfoldMapNoParams"), 0, TYPE_VOID, 0, 0,
	unwrap_unfoldmapdialog, _T("unfoldMapDialog"), 0, TYPE_VOID, 0, 0,

	unwrap_hideselectedpolygons, _T("hideSelectedPolygons"), 0, TYPE_VOID, 0, 0,
	unwrap_unhideallpolygons, _T("unhideAllPolygons"), 0, TYPE_VOID, 0, 0,


	unwrap_getnormal, _T("getNormal"),0,TYPE_POINT3,0,1,
	_T("faceIndex"), 0, TYPE_INT,
	unwrap_setseedface, _T("setSeedFace"),0, TYPE_VOID, 0, 0,
	unwrap_showvertexconnectionlist, _T("toggleVertexConnection"),0, TYPE_VOID, 0, 0,
	//COPYPASTE STUF
	unwrap_copy, _T("copy"),0, TYPE_VOID, 0, 0,
	unwrap_paste, _T("paste"),0, TYPE_VOID, 0, 1,
	_T("rotate"), 0, TYPE_BOOL,

	unwrap_pasteinstance, _T("pasteInstance"),0, TYPE_VOID, 0, 0,

	unwrap_setdebuglevel, _T("setDebugLevel"), 0, TYPE_VOID, 0, 1,
	_T("level"), 0, TYPE_INT,

	unwrap_stitchverts, _T("stitchVerts"), 0, TYPE_VOID, 0, 2,
	_T("align"), 0, TYPE_BOOL,
	_T("bias"), 0, TYPE_FLOAT,

	unwrap_stitchvertsnoparams, _T("stitchVertsNoParams"), 0, TYPE_VOID, 0, 0,
	unwrap_stitchvertsdialog, _T("stitchVertsDialog"), 0, TYPE_VOID, 0, 0,

	unwrap_selectelement, _T("selectElement"), 0, TYPE_VOID, 0, 0,

	unwrap_flattenmapdialog, _T("flattenMapDialog"), 0, TYPE_VOID, 0, 0,
	unwrap_flattenmapnoparams, _T("flattenMapNoParams"), 0, TYPE_VOID, 0, 0,

	//TILE STUFF
	unwrap_gettilemap, _T("getTileMap"), 0, TYPE_BOOL, 0, 0,
	unwrap_settilemap, _T("setTileMap"), 0, TYPE_VOID, 0, 1,
	_T("tile"), 0, TYPE_BOOL,

	unwrap_gettilemaplimit, _T("getTileMapLimit"), 0, TYPE_INT, 0, 0,
	unwrap_settilemaplimit, _T("setTileMapLimit"), 0, TYPE_VOID, 0, 1,
	_T("limit"), 0, TYPE_INT,

	unwrap_gettilemapcontrast, _T("getTileMapBrightness"), 0, TYPE_FLOAT, 0, 0,
	unwrap_settilemapcontrast, _T("setTileMapBrightness"), 0, TYPE_VOID, 0, 1,
	_T("contrast"), 0, TYPE_FLOAT,

	unwrap_getshowmap, _T("getShowMap"), 0, TYPE_BOOL, 0, 0,
	unwrap_setshowmap, _T("setShowMap"), 0, TYPE_VOID, 0, 1,
	_T("showMap"), 0, TYPE_BOOL,


	unwrap_setlimitsoftsel, _T("getLimitSoftSel"), 0, TYPE_BOOL, 0, 0,
	unwrap_getlimitsoftsel, _T("setLimitSoftSel"), 0, TYPE_VOID, 0, 1,
	_T("limit"), 0, TYPE_BOOL,

	unwrap_setlimitsoftselrange, _T("getLimitSoftSelRange"), 0, TYPE_INT, 0, 0,
	unwrap_getlimitsoftselrange, _T("setLimitSoftSelRange"), 0, TYPE_VOID, 0, 1,
	_T("range"), 0, TYPE_INT,


	unwrap_getvertexweight, _T("getVertexWeight"), 0, TYPE_FLOAT, 0, 1,
	_T("index"), 0, TYPE_INT,
	unwrap_setvertexweight, _T("setVertexWeight"), 0, TYPE_VOID, 0, 2,
	_T("index"), 0, TYPE_INT,
	_T("weight"), 0, TYPE_FLOAT,

	unwrap_isweightmodified, _T("isWeightModified"), 0, TYPE_BOOL, 0, 1,
	_T("index"), 0, TYPE_INT,
	unwrap_modifyweight, _T("modifyWeight"), 0, TYPE_VOID, 0, 2,
	_T("index"), 0, TYPE_INT,
	_T("modify"), 0, TYPE_BOOL,


	unwrap_getgeom_elemmode, _T("getGeomSelectElementMode"), 0, TYPE_BOOL, 0, 0,
	unwrap_setgeom_elemmode, _T("setGeomSelectElementMode"), 0, TYPE_VOID, 0, 1,
	_T("mode"), 0, TYPE_BOOL,


	unwrap_getgeom_planarmode, _T("getGeomPlanarThresholdMode"), 0, TYPE_BOOL, 0, 0,
	unwrap_setgeom_planarmode, _T("setGeomPlanarThresholdMode"), 0, TYPE_VOID, 0, 1,
	_T("mode"), 0, TYPE_BOOL,

	unwrap_getgeom_planarmodethreshold, _T("getGeomPlanarThreshold"), 0, TYPE_FLOAT, 0, 0,
	unwrap_setgeom_planarmodethreshold, _T("setGeomPlanarThreshold"), 0, TYPE_VOID, 0, 1,
	_T("angle"), 0, TYPE_FLOAT,


	unwrap_getwindowx, _T("getWindowX"), 0, TYPE_INT, 0, 0,
	unwrap_getwindowy, _T("getWindowY"), 0, TYPE_INT, 0, 0,
	unwrap_getwindoww, _T("getWindowW"), 0, TYPE_INT, 0, 0,
	unwrap_getwindowh, _T("getWindowH"), 0, TYPE_INT, 0, 0,


	unwrap_getbackfacecull, _T("getIgnoreBackFaceCull"), 0, TYPE_BOOL, 0, 0,
	unwrap_setbackfacecull, _T("setIgnoreBackFaceCull"), 0, TYPE_VOID, 0, 1,
	_T("ignoreBackFaceCull"), 0, TYPE_BOOL,

	unwrap_getoldselmethod, _T("getOldSelMethod"), 0, TYPE_BOOL, 0, 0,
	unwrap_setoldselmethod, _T("setOldSelMethod"), 0, TYPE_VOID, 0, 1,
	_T("oldSelMethod"), 0, TYPE_BOOL,

	unwrap_selectbymatid, _T("selectByMatID"), 0, TYPE_VOID, 0, 1,
	_T("matID"), 0, TYPE_INT,

	unwrap_selectbysg, _T("selectBySG"), 0, TYPE_VOID, 0, 1,
	_T("sg"), 0, TYPE_INT,

	unwrap_gettvelementmode, _T("getTVElementMode"), 0, TYPE_BOOL, 0, 0,
	unwrap_settvelementmode, _T("setTVElementMode"), 0, TYPE_VOID, 0, 1,
	_T("mode"), 0, TYPE_BOOL,


	unwrap_geomexpandsel, _T("expandGeomFaceSelection"),0, TYPE_VOID, 0, 0,
	unwrap_geomcontractsel, _T("contractGeomFaceSelection"),0, TYPE_VOID, 0, 0,


	unwrap_getalwaysedit, _T("getAlwaysEdit"), 0, TYPE_BOOL, 0, 0,
	unwrap_setalwaysedit, _T("setAlwaysEdit"), 0, TYPE_VOID, 0, 1,
	_T("always"), 0, TYPE_BOOL,

	unwrap_getshowvertexconnectionlist, _T("getShowVertexConnections"), 0, TYPE_BOOL, 0, 0,
	unwrap_setshowvertexconnectionlist, _T("setShowVertexConnections"), 0, TYPE_VOID, 0, 1,
	_T("show"), 0, TYPE_BOOL,

	unwrap_getfilterselected, _T("getFilterSelected"), 0, TYPE_BOOL, 0, 0,
	unwrap_setfilterselected, _T("setFilterSelected"), 0, TYPE_VOID, 0, 1,
	_T("filter"), 0, TYPE_BOOL,

	unwrap_getsnap, _T("getSnap"), 0, TYPE_BOOL, 0, 0,
	unwrap_setsnap, _T("setSnap"), 0, TYPE_VOID, 0, 1,
	_T("snap"), 0, TYPE_BOOL,

	unwrap_getlock, _T("getLock"), 0, TYPE_BOOL, 0, 0,
	unwrap_setlock, _T("setLock"), 0, TYPE_VOID, 0, 1,
	_T("lock"), 0, TYPE_BOOL,

	unwrap_pack, _T("pack"), 0, TYPE_VOID, 0, 5,
	_T("method"), 0, TYPE_INT,
	_T("spacing"), 0, TYPE_FLOAT,
	_T("normalize"), 0, TYPE_BOOL,
	_T("rotate"), 0, TYPE_BOOL,
	_T("fillholes"), 0, TYPE_BOOL,

	unwrap_packnoparams, _T("packNoParams"), 0, TYPE_VOID, 0, 0,
	unwrap_packdialog, _T("packDialog"), 0, TYPE_VOID, 0, 0,


	unwrap_gettvsubobjectmode, _T("getTVSubObjectMode"), 0, TYPE_INT, 0, 0,
	unwrap_settvsubobjectmode, _T("setTVSubObjectMode"), 0, TYPE_VOID, 0, 1,
	_T("mode"), 0, TYPE_INT,


	unwrap_getselectedfaces, _T("getSelectedFaces"),0,TYPE_BITARRAY,0,0,
	unwrap_selectfaces, _T("selectFaces"),0,TYPE_VOID,0,1,
	_T("selection"),0,TYPE_BITARRAY,

	unwrap_isfaceselected, _T("isFaceSelected"),0,TYPE_BOOL,0,1,
	_T("index"),0,TYPE_INT,

	unwrap_getfillmode, _T("getFillMode"), 0, TYPE_INT, 0, 0,
	unwrap_setfillmode, _T("setFillMode"), 0, TYPE_VOID, 0, 1,
	_T("mode"), 0, TYPE_INT,

	unwrap_moveselected, _T("MoveSelected"),0,TYPE_VOID,0,1,
	_T("offset"),0,TYPE_POINT3,

	unwrap_rotateselectedc, _T("RotateSelectedCenter"),0,TYPE_VOID,0,1,
	_T("angle"),0,TYPE_FLOAT,
	unwrap_rotateselected, _T("RotateSelected"),0,TYPE_VOID,0,2,
	_T("angle"),0,TYPE_FLOAT,
	_T("axis"),0,TYPE_POINT3,
	unwrap_scaleselectedc, _T("ScaleSelectedCenter"),0,TYPE_VOID,0,2,
	_T("scale"),0,TYPE_FLOAT,
	_T("dir"),0,TYPE_INT,
	unwrap_scaleselected, _T("ScaleSelected"),0,TYPE_VOID,0,3,
	_T("scale"),0,TYPE_FLOAT,
	_T("dir"),0,TYPE_INT,
	_T("axis"),0,TYPE_POINT3,

	unwrap_getselectededges, _T("getSelectedEdges"),0,TYPE_BITARRAY,0,0,
	unwrap_selectedges, _T("selectEdges"),0,TYPE_VOID,0,1,
	_T("selection"),0,TYPE_BITARRAY,

	unwrap_isedgeselected, _T("isEdgeSelected"),0,TYPE_BOOL,0,1,
	_T("index"),0,TYPE_INT,


	unwrap_getdisplayopenedge, _T("getDisplayOpenEdges"), 0, TYPE_BOOL, 0, 0,
	unwrap_getdisplayopenedge, _T("setDisplayOpenEdges"), 0, TYPE_VOID, 0, 1,
	_T("displayOpenEdges"), 0, TYPE_BOOL,



	unwrap_getopenedgecolor, _T("getOpenEdgeColor"),0, TYPE_POINT3, 0, 0,
	unwrap_setopenedgecolor, _T("setOpenEdgeColor"),0, TYPE_VOID, 0, 1,
	_T("color"), 0, TYPE_POINT3,

	unwrap_getuvedgemode, _T("getUVEdgeMode"), 0, TYPE_BOOL, 0, 0,
	unwrap_setuvedgemode, _T("setUVEdgeMode"), 0, TYPE_VOID, 0, 1,
	_T("uvEdgeMode"), 0, TYPE_BOOL,

	unwrap_getopenedgemode, _T("getOpenEdgeMode"), 0, TYPE_BOOL, 0, 0,
	unwrap_setopenedgemode, _T("setOpenEdgeMode"), 0, TYPE_VOID, 0, 1,
	_T("uvOpenMode"), 0, TYPE_BOOL,


	unwrap_uvedgeselect, _T("uvEdgeSelect"), 0, TYPE_VOID, 0, 0,
	unwrap_openedgeselect, _T("openEdgeSelect"), 0, TYPE_VOID, 0, 0,


	unwrap_selectverttoedge, _T("vertToEdgeSelect"), 0, TYPE_VOID, 0, 0,
	unwrap_selectverttoface, _T("vertToFaceSelect"), 0, TYPE_VOID, 0, 0,

	unwrap_selectedgetovert, _T("edgeToVertSelect"), 0, TYPE_VOID, 0, 0,
	unwrap_selectedgetoface, _T("edgeToFaceSelect"), 0, TYPE_VOID, 0, 0,

	unwrap_selectfacetovert, _T("faceToVertSelect"), 0, TYPE_VOID, 0, 0,
	unwrap_selectfacetoedge, _T("faceToEdgeSelect"), 0, TYPE_VOID, 0, 0,

	unwrap_getdisplayhiddenedge, _T("getDisplayHiddenEdges"), 0, TYPE_BOOL, 0, 0,
	unwrap_setdisplayhiddenedge, _T("setDisplayHiddenEdges"), 0, TYPE_VOID, 0, 1,
	_T("displayHiddenEdges"), 0, TYPE_BOOL,

	unwrap_gethandlecolor, _T("getHandleColor"),0, TYPE_POINT3, 0, 0,
	unwrap_sethandlecolor, _T("setHandleColor"),0, TYPE_VOID, 0, 1,
	_T("color"), 0, TYPE_POINT3,

	unwrap_getfreeformmode, _T("getFreeFormMode"), 0, TYPE_BOOL, 0, 0,
	unwrap_setfreeformmode, _T("setFreeFormMode"), 0, TYPE_VOID, 0, 1,
	_T("freeFormMode"), 0, TYPE_BOOL,

	unwrap_getfreeformcolor, _T("getFreeFormColor"),0, TYPE_POINT3, 0, 0,
	unwrap_setfreeformcolor, _T("setFreeFormColor"),0, TYPE_VOID, 0, 1,
	_T("color"), 0, TYPE_POINT3,

	unwrap_scaleselectedxy, _T("ScaleSelectedXY"),0,TYPE_VOID,0,3,
	_T("scaleX"),0,TYPE_FLOAT,
	_T("scaleY"),0,TYPE_FLOAT,
	_T("axis"),0,TYPE_POINT3,

	unwrap_snappivot, _T("snapPivot"),0,TYPE_VOID,0,1,
	_T("pos"),0,TYPE_INT,

	unwrap_getpivotoffset, _T("getPivotOffset"),0, TYPE_POINT3, 0, 0,
	unwrap_setpivotoffset, _T("setPivotOffset"),0, TYPE_VOID, 0, 1,
	_T("offset"), 0, TYPE_POINT3,
	unwrap_getselcenter, _T("getSelCenter"),0, TYPE_POINT3, 0, 0,

	unwrap_sketch, _T("sketch"),0, TYPE_VOID, 0, 2,
	_T("indexList"),0,TYPE_INT_TAB,
	_T("positionList"),0,TYPE_POINT3_TAB,
	unwrap_sketchnoparams, _T("sketchNoParams"),0, TYPE_VOID, 0, 0,
	unwrap_sketchdialog, _T("sketchDialog"),0, TYPE_VOID, 0, 0,
	unwrap_sketchreverse, _T("sketchReverse"),0, TYPE_VOID, 0, 0,


	unwrap_gethitsize, _T("getHitSize"), 0, TYPE_INT, 0, 0,
	unwrap_sethitsize, _T("SetHitSize"), 0, TYPE_VOID, 0, 1,
	_T("size"), 0, TYPE_INT,


	unwrap_getresetpivotonsel, _T("getResetPivotOnSelection"), 0, TYPE_BOOL, 0, 0,
	unwrap_setresetpivotonsel, _T("SetResetPivotOnSelection"), 0, TYPE_VOID, 0, 1,
	_T("reset"), 0, TYPE_BOOL,

	unwrap_getpolymode, _T("getPolygonMode"), 0, TYPE_BOOL, 0, 0,
	unwrap_setpolymode, _T("setPolygonMode"), 0, TYPE_VOID, 0, 1,
	_T("mode"), 0, TYPE_BOOL,
	unwrap_polyselect, _T("PolygonSelect"), 0, TYPE_VOID, 0, 0,



	unwrap_getselectioninsidegizmo, _T("getAllowSelectionInsideGizmo"), 0, TYPE_BOOL, 0, 0,
	unwrap_setselectioninsidegizmo, _T("SetAllowSelectionInsideGizmo"), 0, TYPE_VOID, 0, 1,
	_T("select"), 0, TYPE_BOOL,



	unwrap_setasdefaults, _T("SaveCurrentSettingsAsDefault"), 0, TYPE_VOID, 0, 0,
	unwrap_loaddefaults, _T("LoadDefault"), 0, TYPE_VOID, 0, 0,



	unwrap_getshowshared, _T("getShowShared"), 0, TYPE_BOOL, 0, 0,
	unwrap_setshowshared, _T("setShowShared"), 0, TYPE_VOID, 0, 1,
	_T("select"), 0, TYPE_BOOL,


	unwrap_getsharedcolor, _T("getSharedColor"),0, TYPE_POINT3, 0, 0,
	unwrap_setsharedcolor, _T("setSharedColor"),0, TYPE_VOID, 0, 1,
	_T("color"), 0, TYPE_POINT3,


	unwrap_showicon, _T("showIcon"), 0, TYPE_VOID, 0, 2,
	_T("index"), 0, TYPE_INT,
	_T("show"), 0, TYPE_BOOL,


	unwrap_getsyncselectionmode, _T("getSyncSelectionMode"), 0, TYPE_BOOL, 0, 0,
	unwrap_setsyncselectionmode, _T("setSyncSelectionMode"), 0, TYPE_VOID, 0, 1,
	_T("sync"), 0, TYPE_BOOL,


	unwrap_synctvselection, _T("syncTVSelection"), 0, TYPE_VOID, 0, 0,
	unwrap_syncgeomselection, _T("syncGeomSelection"), 0, TYPE_VOID, 0, 0,

	unwrap_getbackgroundcolor, _T("getBackgroundColor"),0, TYPE_POINT3, 0, 0,
	unwrap_setbackgroundcolor, _T("setBackgroundColor"),0, TYPE_VOID, 0, 1,
	_T("color"), 0, TYPE_POINT3,

	unwrap_updatemenubar, _T("updateMenuBar"),0, TYPE_VOID, 0, 0,


	unwrap_getbrightcentertile, _T("getBrightnessAffectsCenterTile"), 0, TYPE_BOOL, 0, 0,
	unwrap_setbrightcentertile, _T("setBrightnessAffectsCenterTile"), 0, TYPE_VOID, 0, 1,
	_T("bright"), 0, TYPE_BOOL,

	unwrap_getblendtoback, _T("getBlendTileToBackground"), 0, TYPE_BOOL, 0, 0,
	unwrap_setblendtoback, _T("setBlendTileToBackground"), 0, TYPE_VOID, 0, 1,
	_T("blend"), 0, TYPE_BOOL,

	unwrap_getpaintmode, _T("getPaintSelectMode"), 0, TYPE_BOOL, 0, 0,
	unwrap_setpaintmode, _T("setPaintSelectMode"), 0, TYPE_VOID, 0, 1,
	_T("paint"), 0, TYPE_BOOL,

	unwrap_getpaintsize, _T("getPaintSelectSize"), 0, TYPE_INT, 0, 0,
	unwrap_setpaintsize, _T("setPaintSelectSize"), 0, TYPE_VOID, 0, 1,
	_T("size"), 0, TYPE_INT,

	unwrap_incpaintsize, _T("PaintSelectIncSize"), 0, TYPE_VOID, 0, 0,
	unwrap_decpaintsize, _T("PaintSelectDecSize"), 0, TYPE_VOID, 0, 0,


	unwrap_getticksize, _T("getTickSize"), 0, TYPE_INT, 0, 0,
	unwrap_setticksize, _T("setTickSize"), 0, TYPE_VOID, 0, 1,
	_T("size"), 0, TYPE_INT,


	//new
	unwrap_getgridsize, _T("getGridSize"), 0, TYPE_FLOAT, 0, 0,
	unwrap_setgridsize, _T("setGridSize"), 0, TYPE_VOID, 0, 1,
	_T("size"), 0, TYPE_FLOAT,

	unwrap_getgridsnap, _T("getGridSnap"), 0, TYPE_BOOL, 0, 0,
	unwrap_setgridsnap, _T("setGridSnap"), 0, TYPE_VOID, 0, 1,
	_T("snap"), 0, TYPE_BOOL,
	unwrap_getgridvisible, _T("getGridVisible"), 0, TYPE_BOOL, 0, 0,
	unwrap_setgridvisible, _T("setGridVisible"), 0, TYPE_VOID, 0, 1,
	_T("visible"), 0, TYPE_BOOL,

	unwrap_getgridcolor, _T("getGridColor"),0, TYPE_POINT3, 0, 0,
	unwrap_setgridcolor, _T("setGridColor"),0, TYPE_VOID, 0, 1,
	_T("color"), 0, TYPE_POINT3,

	unwrap_getgridstr, _T("getGridStr"), 0, TYPE_FLOAT, 0, 0,
	unwrap_setgridstr, _T("setGridStr"), 0, TYPE_VOID, 0, 1,
	_T("str"), 0, TYPE_FLOAT,

	unwrap_getautomap, _T("getAutoBackground"), 0, TYPE_BOOL, 0, 0,
	unwrap_setautomap, _T("setAutoBackground"), 0, TYPE_VOID, 0, 1,
	_T("enable"), 0, TYPE_BOOL,


	unwrap_getflattenangle, _T("getFlattenAngle"), 0, TYPE_FLOAT, 0, 0,
	unwrap_setflattenangle, _T("setFlattenAngle"), 0, TYPE_VOID, 0, 1,
	_T("angle"), 0, TYPE_FLOAT,

	unwrap_getflattenspacing, _T("getFlattenSpacing"), 0, TYPE_FLOAT, 0, 0,
	unwrap_setflattenspacing, _T("setFlattenSpacing"), 0, TYPE_VOID, 0, 1,
	_T("spacing"), 0, TYPE_FLOAT,

	unwrap_getflattennormalize, _T("getFlattenNormalize"), 0, TYPE_BOOL, 0, 0,
	unwrap_setflattennormalize, _T("setFlattenNormalize"), 0, TYPE_VOID, 0, 1,
	_T("normalize"), 0, TYPE_BOOL,

	unwrap_getflattenrotate, _T("getFlattenRotate"), 0, TYPE_BOOL, 0, 0,
	unwrap_setflattenrotate, _T("setFlattenRotate"), 0, TYPE_VOID, 0, 1,
	_T("rotate"), 0, TYPE_BOOL,

	unwrap_getflattenfillholes, _T("getFlattenFillHoles"), 0, TYPE_BOOL, 0, 0,
	unwrap_setflattenfillholes, _T("setFlattenFillHoles"), 0, TYPE_VOID, 0, 1,
	_T("fillHoles"), 0, TYPE_BOOL,

	unwrap_getpreventflattening, _T("getPreventFlattening"), 0, TYPE_BOOL, 0, 0,
	unwrap_setpreventflattening, _T("setPreventFlattening"), 0, TYPE_VOID, 0, 1,
	_T("preventFlattening"), 0, TYPE_BOOL,

	unwrap_getenablesoftselection, _T("getEnableSoftSelection"), 0, TYPE_BOOL, 0, 0,
	unwrap_setenablesoftselection, _T("setEnableSoftSelection"), 0, TYPE_VOID, 0, 1,
	_T("enable"), 0, TYPE_BOOL,

	unwrap_getapplytowholeobject, _T("getApplyToWholeObject"), 0, TYPE_BOOL, 0, 0,
	unwrap_setapplytowholeobject, _T("setApplyToWholeObject"), 0, TYPE_VOID, 0, 1,
	_T("whole"), 0, TYPE_BOOL,


	unwrap_setvertexposition2, _T("setVertexPosition2"),0,TYPE_VOID,0,5,
	_T("time"), 0, TYPE_TIMEVALUE,
	_T("index"), 0, TYPE_INT,
	_T("pos"), 0, TYPE_POINT3,
	_T("hold"), 0, TYPE_BOOL,
	_T("update"), 0, TYPE_BOOL,


	unwrap_relax, _T("relax"),0,TYPE_VOID,0,4,
	_T("iterations"), 0, TYPE_INT,
	_T("strength"), 0, TYPE_FLOAT,
	_T("lockEdges"), 0, TYPE_BOOL,
	_T("matchArea"), 0, TYPE_BOOL,

	unwrap_fitrelax, _T("fitRelax"),0,TYPE_VOID,0,2,
	_T("iterations"), 0, TYPE_INT,
	_T("strength"), 0, TYPE_FLOAT,


	p_end
	);

//5.1.05
static FPInterfaceDesc unwrap_interface3(
	UNWRAP_INTERFACE3, _T("unwrap3"), 0, &unwrapDesc, FP_MIXIN,

	//UNFOLD STUFF

	unwrap_getautobackground, _T("getAutoBackground"),0,TYPE_BOOL,0,0,
	unwrap_setautobackground, _T("setAutoBackground"), 0, TYPE_VOID, 0, 1,
	_T("autoBackground"), 0, TYPE_BOOL,

	unwrap_getrelaxamount, _T("getRelaxAmount"),0,TYPE_FLOAT,0,0,
	unwrap_setrelaxamount, _T("setRelaxAmount"), 0, TYPE_VOID, 0, 1,
	_T("amount"), 0, TYPE_FLOAT,

	unwrap_getrelaxiter, _T("getRelaxIteration"),0,TYPE_INT,0,0,
	unwrap_setrelaxiter, _T("setRelaxIteration"), 0, TYPE_VOID, 0, 1,
	_T("amount"), 0, TYPE_INT,


	unwrap_getrelaxboundary, _T("getRelaxBoundary"),0,TYPE_BOOL,0,0,
	unwrap_setrelaxboundary, _T("setRelaxBoundary"), 0, TYPE_VOID, 0, 1,
	_T("boundary"), 0, TYPE_BOOL,

	unwrap_getrelaxsaddle, _T("getRelaxSaddle"),0,TYPE_BOOL,0,0,
	unwrap_setrelaxsaddle, _T("setRelaxSaddle"), 0, TYPE_VOID, 0, 1,
	_T("saddle"), 0, TYPE_BOOL,


	unwrap_relax2, _T("relax2"), 0, TYPE_VOID, 0, 0,
	unwrap_relax2dialog, _T("relax2dialog"), 0, TYPE_VOID, 0, 0,

	p_end
	);

static FPInterfaceDesc unwrap_interface4(
	UNWRAP_INTERFACE4, _T("unwrap4"), 0, &unwrapDesc, FP_MIXIN,

	//UNFOLD STUFF

	unwrap_getthickopenedges, _T("getThickOpenEdges"),0,TYPE_BOOL,0,0,
	unwrap_setthickopenedges, _T("setThickOpenEdges"), 0, TYPE_VOID, 0, 1,
	_T("thick"), 0, TYPE_BOOL,

	unwrap_getviewportopenedges, _T("getViewportOpenEdges"),0,TYPE_BOOL,0,0,
	unwrap_setviewportopenedges, _T("setViewportOpenEdges"), 0, TYPE_VOID, 0, 1,
	_T("show"), 0, TYPE_BOOL,

	unwrap_selectinvertedfaces, _T("selectInvertedFaces"), 0, TYPE_VOID, 0, 0,

	unwrap_getrelativetypein, _T("getRelativeTypeIn"),0,TYPE_BOOL,0,0,
	unwrap_setrelativetypein, _T("setRelativeTypeIn"), 0, TYPE_VOID, 0, 1,
	_T("relative"), 0, TYPE_BOOL,

	unwrap_addmap, _T("addMap"), 0, TYPE_VOID, 0, 1,
	_T("map"), 0, TYPE_TEXMAP,


	unwrap_flattenmapbymatid, _T("flattenMapByMatID"), 0, TYPE_VOID, 0, 6,
	_T("angleThreshold"), 0, TYPE_FLOAT,
	_T("spacing"), 0, TYPE_FLOAT,
	_T("normalize"), 0, TYPE_BOOL,
	_T("layOutType"), 0, TYPE_INT,
	_T("rotateClusters"), 0, TYPE_BOOL,
	_T("fillHoles"), 0, TYPE_BOOL,


	unwrap_getarea, _T("getArea"), 0, TYPE_VOID, 0, 7,
	_T("faceSelection"), 0, TYPE_BITARRAY,
	_T("x"), 0, TYPE_FLOAT_BR, f_inOut, FPP_OUT_PARAM,
	_T("y"), 0, TYPE_FLOAT_BR, f_inOut, FPP_OUT_PARAM,
	_T("width"), 0, TYPE_FLOAT_BR, f_inOut, FPP_OUT_PARAM,
	_T("height"), 0, TYPE_FLOAT_BR, f_inOut, FPP_OUT_PARAM,
	_T("areaUVW"), 0, TYPE_FLOAT_BR, f_inOut, FPP_OUT_PARAM,
	_T("areaGeom"), 0, TYPE_FLOAT_BR, f_inOut, FPP_OUT_PARAM,


	unwrap_getrotationsrespectaspect, _T("getRotationsRespectAspect"),0,TYPE_BOOL,0,0,
	unwrap_setrotationsrespectaspect, _T("setRotationsRespectAspect"), 0, TYPE_VOID, 0, 1,
	_T("respect"), 0, TYPE_BOOL,
	unwrap_setmax5flatten, _T("setMax5Flatten"), 0, TYPE_VOID, 0, 1,
	_T("like5"), 0, TYPE_BOOL,



	

	p_end
	);


static FPInterfaceDesc unwrap_interface5(
	UNWRAP_INTERFACE5, _T("unwrap5"), 0, &unwrapDesc, FP_MIXIN,


	unwrap_pelt_getmapmode, _T("getPeltMapMode"),0,TYPE_BOOL,0,0,
	unwrap_pelt_setmapmode, _T("setPeltMapMode"), 0, TYPE_VOID, 0, 1,
	_T("mode"), 0, TYPE_BOOL,

	unwrap_pelt_geteditseamsmode, _T("getPeltEditSeamsMode"),0,TYPE_BOOL,0,0,
	unwrap_pelt_seteditseamsmode, _T("setPeltEditSeamsMode"), 0, TYPE_VOID, 0, 1,
	_T("mode"), 0, TYPE_BOOL,

	unwrap_pelt_getseamselection, _T("getPeltSelectedSeams"),0,TYPE_BITARRAY,0,0,
	unwrap_pelt_setseamselection, _T("setPeltSelectedSeams"),0,TYPE_VOID,0,1,
	_T("selection"),0,TYPE_BITARRAY,

	unwrap_pelt_getpointtopointseamsmode, _T("getPeltPointToPointSeamsMode"),0,TYPE_BOOL,0,0,
	unwrap_pelt_setpointtopointseamsmode, _T("setPeltPointToPointSeamsMode"), 0, TYPE_VOID, 0, 1,
	_T("mode"), 0, TYPE_BOOL,

	unwrap_pelt_expandtoseams, _T("peltExpandSelectionToSeams"),0,TYPE_VOID,0,0,
	unwrap_pelt_dialog, _T("peltDialog"),0,TYPE_VOID,0,0,

	unwrap_peltdialog_resetrig, _T("peltDialogResetStretcher"),0,TYPE_VOID,0,0,
	unwrap_peltdialog_selectrig, _T("peltDialogSelectStretcher"),0,TYPE_VOID,0,0,
	unwrap_peltdialog_selectpelt, _T("peltDialogSelectPelt"),0,TYPE_VOID,0,0,
	unwrap_peltdialog_snaprigtoedges, _T("peltDialogSnapToSeams"),0,TYPE_VOID,0,0,
	unwrap_peltdialog_mirrorrig, _T("peltDialogMirrorStretcher"),0,TYPE_VOID,0,0,


	unwrap_peltdialog_getstraightenmode, _T("getPeltDialogStraightenMode"),0,TYPE_BOOL,0,0,
	unwrap_peltdialog_setstraightenmode, _T("setPeltDialogStraightenMode"), 0, TYPE_VOID, 0, 1,
	_T("mode"), 0, TYPE_BOOL,

	unwrap_peltdialog_straighten, _T("peltDialogStraighten"), 0, TYPE_VOID, 0, 2,
	_T("vert1"), 0, TYPE_INDEX,
	_T("vert2"), 0, TYPE_INDEX,

	unwrap_peltdialog_run, _T("peltDialogRun"),0,TYPE_VOID,0,0,
	unwrap_peltdialog_relax1, _T("peltDialogRelaxLight"),0,TYPE_VOID,0,0,
	unwrap_peltdialog_relax2, _T("peltDialogRelaxHeavy"),0,TYPE_VOID,0,0,


	unwrap_peltdialog_getframes, _T("getPeltDialogFrames"),0,TYPE_INT,0,0,
	unwrap_peltdialog_setframes, _T("setPeltDialogFrames"), 0, TYPE_VOID, 0, 1,
	_T("frames"), 0, TYPE_INT,

	unwrap_peltdialog_getsamples, _T("getPeltDialogSamples"),0,TYPE_INT,0,0,
	unwrap_peltdialog_setsamples, _T("setPeltDialogSamples"), 0, TYPE_VOID, 0, 1,
	_T("samples"), 0, TYPE_INT,

	unwrap_peltdialog_getrigstrength, _T("getPeltDialogStretcherStrength"),0,TYPE_FLOAT,0,0,
	unwrap_peltdialog_setrigstrength, _T("setPeltDialogStretcherStrength"), 0, TYPE_VOID, 0, 1,
	_T("strength"), 0, TYPE_FLOAT,

	unwrap_peltdialog_getstiffness, _T("getPeltDialogStiffness"),0,TYPE_FLOAT,0,0,
	unwrap_peltdialog_setstiffness, _T("setPeltDialogStiffness"), 0, TYPE_VOID, 0, 1,
	_T("stiffness"), 0, TYPE_FLOAT,

	unwrap_peltdialog_getdampening, _T("getPeltDialogDampening"),0,TYPE_FLOAT,0,0,
	unwrap_peltdialog_setdampening, _T("setPeltDialogDampening"), 0, TYPE_VOID, 0, 1,
	_T("dampening"), 0, TYPE_FLOAT,

	unwrap_peltdialog_getdecay, _T("getPeltDialogDecay"),0,TYPE_FLOAT,0,0,
	unwrap_peltdialog_setdecay, _T("setPeltDialogDecay"), 0, TYPE_VOID, 0, 1,
	_T("decay"), 0, TYPE_FLOAT,

	unwrap_peltdialog_getmirroraxis, _T("getPeltDialogMirrorAxis"),0,TYPE_FLOAT,0,0,
	unwrap_peltdialog_setmirroraxis, _T("setPeltDialogMirrorAxis"), 0, TYPE_VOID, 0, 1,
	_T("axis"), 0, TYPE_FLOAT,

	unwrap_mapping_align, _T("mappingAlign"), 0, TYPE_VOID, 0, 1,
	_T("axis"), 0, TYPE_INT,

	unwrap_mappingmode, _T("mappingMode"), 0, TYPE_VOID, 0, 1,
	_T("mode"), 0, TYPE_INT,

	unwrap_setgizmotm, _T("setGizmoTM"), 0, TYPE_VOID, 0, 1,
	_T("tm"), 0, TYPE_MATRIX3,

	unwrap_getgizmotm, _T("getGizmoTM"), 0, TYPE_MATRIX3, 0, 0,

	unwrap_gizmofit, _T("mappingFit"), 0, TYPE_VOID, 0, 0,
	unwrap_gizmocenter, _T("mappingCenter"), 0, TYPE_VOID, 0, 0,
	unwrap_gizmoaligntoview, _T("mappingAlignToView"), 0, TYPE_VOID, 0, 0,

	unwrap_geomedgeselectionexpand, _T("expandGeomEdgeSelection"), 0, TYPE_VOID, 0, 0,
	unwrap_geomedgeselectioncontract, _T("contractGeomEdgeSelection"), 0, TYPE_VOID, 0, 0,

	unwrap_geomedgeloop, _T("geomEdgeLoopSelection"), 0, TYPE_VOID, 0, 0,
	unwrap_geomedgering, _T("geomEdgeRingSelection"), 0, TYPE_VOID, 0, 0,

	unwrap_geomvertexselectionexpand, _T("expandGeomVertexSelection"), 0, TYPE_VOID, 0, 0,
	unwrap_geomvertexselectioncontract, _T("contractGeomVertexSelection"), 0, TYPE_VOID, 0, 0,

	unwrap_getgeomvertexselection, _T("getSelectedGeomVerts"),0,TYPE_BITARRAY,0,0,
	unwrap_setgeomvertexselection, _T("setSelectedGeomVerts"),0,TYPE_VOID,0,1,
	_T("selection"),0,TYPE_BITARRAY,

	unwrap_getgeomedgeselection, _T("getSelectedGeomEdges"),0,TYPE_BITARRAY,0,0,
	unwrap_setgeomedgeselection, _T("setSelectedGeomEdges"),0,TYPE_VOID,0,1,
	_T("selection"),0,TYPE_BITARRAY,


	unwrap_pelt_getshowseam, _T("getPeltAlwaysShowSeams"),0,TYPE_BOOL,0,0,
	unwrap_pelt_setshowseam, _T("setPeltAlwaysShowSeams"), 0, TYPE_VOID, 0, 1,
	_T("show"), 0, TYPE_BOOL,

	unwrap_pelt_seamtosel, _T("peltSeamToEdgeSel"), 0, TYPE_VOID, 0, 1,
	_T("replace"), 0, TYPE_BOOL,

	unwrap_pelt_seltoseam, _T("peltEdgeSelToSeam"), 0, TYPE_VOID, 0, 1,
	_T("replace"), 0, TYPE_BOOL,

	unwrap_getnormalizemap, _T("getNormalizeMap"),0,TYPE_BOOL,0,0,
	unwrap_setnormalizemap, _T("setNormalizeMap"), 0, TYPE_VOID, 0, 1,
	_T("normalize"), 0, TYPE_BOOL,


	unwrap_getshowedgedistortion, _T("getShowEdgeDistortion"),0,TYPE_BOOL,0,0,
	unwrap_setshowedgedistortion, _T("setShowEdgeDistortion"), 0, TYPE_VOID, 0, 1,
	_T("show"), 0, TYPE_BOOL,


	unwrap_getlockedgesprings, _T("getPeltLockOpenEdges"),0,TYPE_BOOL,0,0,
	unwrap_setlockedgesprings, _T("setPeltLockOpenEdges"), 0, TYPE_VOID, 0, 1,
	_T("lock"), 0, TYPE_BOOL,

	unwrap_selectoverlap, _T("selectOverlappedFaces"), 0, TYPE_VOID, 0, 0,

	unwrap_gizmoreset, _T("mappingReset"), 0, TYPE_VOID, 0, 0,

	unwrap_getedgedistortionscale, _T("getEdgeDistortionScale"),0,TYPE_FLOAT,0,0,
	unwrap_setedgedistortionscale, _T("setEdgeDistortionScale"), 0, TYPE_VOID, 0, 1,
	_T("scale"), 0, TYPE_FLOAT,

	unwrap_relaxspring, _T("relaxBySpring"), 0, TYPE_VOID, 0, 3,
	_T("frames"), 0, TYPE_INT,
	_T("stretch"), 0, TYPE_FLOAT,
	_T("useOnlyVEdges"), 0, TYPE_BOOL,

	unwrap_relaxspringdialog, _T("relaxBySpringDialog"), 0, TYPE_VOID, 0, 0,

	unwrap_relaxspringdialog, _T("relaxBySpringDialog"), 0, TYPE_VOID, 0, 0,


	unwrap_getrelaxspringstretch, _T("getRelaxBySpringStretch"),0,TYPE_FLOAT,0,0,
	unwrap_setrelaxspringstretch, _T("setRelaxBySpringStretch"), 0, TYPE_VOID, 0, 1,
	_T("stretch"), 0, TYPE_FLOAT,

	unwrap_getrelaxspringiteration, _T("getRelaxBySpringIteration"),0,TYPE_INT,0,0,
	unwrap_setrelaxspringiteration, _T("setRelaxBySpringIteration"), 0, TYPE_VOID, 0, 1,
	_T("iteration"), 0, TYPE_INT,


	unwrap_getrelaxspringvedges, _T("getRelaxBySpringUseOnlyVEdges"),0,TYPE_BOOL,0,0,
	unwrap_setrelaxspringvedges, _T("setRelaxBySpringUseOnlyVEdges"), 0, TYPE_VOID, 0, 1,
	_T("useOnlyVEdges"), 0, TYPE_BOOL,

	unwrap_relaxbyfaceangle, _T("relaxByFaceAngle"), 0, TYPE_VOID, 0, 4,
	_T("iterations"), 0, TYPE_INT,
	_T("stretch"), 0, TYPE_FLOAT,
	_T("strength"), 0, TYPE_FLOAT,
	_T("lockBoundaries"), 0, TYPE_BOOL,

	unwrap_relaxbyedgeangle, _T("relaxByEdgeAngle"), 0, TYPE_VOID, 0, 4,
	_T("iterations"), 0, TYPE_INT,
	_T("stretch"), 0, TYPE_FLOAT,
	_T("strength"), 0, TYPE_FLOAT,
	_T("lockBoundaries"), 0, TYPE_BOOL,

	unwrap_setwindowxoffset, _T("setWindowXOffset"), 0, TYPE_VOID, 0, 1,
	_T("offset"), 0, TYPE_INT,
	unwrap_setwindowyoffset, _T("setWindowYOffset"), 0, TYPE_VOID, 0, 1,
	_T("offset"), 0, TYPE_INT,

	unwrap_frameselectedelement, _T("fitSelectedElement"), 0, TYPE_VOID, 0, 0,


	unwrap_getshowcounter, _T("getShowSubObjectCounter"),0,TYPE_BOOL,0,0,
	unwrap_setshowcounter, _T("setShowSubObjectCounter"), 0, TYPE_VOID, 0, 1,
	_T("show"), 0, TYPE_BOOL,

	unwrap_renderuv_dialog, _T("renderUVDialog"),0,TYPE_VOID,0,0,
	unwrap_renderuv, _T("renderUV"),0,TYPE_VOID,0,1,
	_T("fileName"), 0, TYPE_FILENAME,

	unwrap_ismesh, _T("isMesh"), 0, TYPE_BOOL, 0, 0,

	unwrap_qmap, _T("quickPlanarMap"),0,TYPE_VOID,0,0,

	unwrap_qmap_getpreview, _T("getQuickMapGizmoPreview"),0,TYPE_BOOL,0,0,
	unwrap_qmap_setpreview, _T("setQuickMapGizmoPreview"), 0, TYPE_VOID, 0, 1,
	_T("preview"), 0, TYPE_BOOL,

	unwrap_getshowmapseams, _T("getShowMapSeams"),0,TYPE_BOOL,0,0,
	unwrap_setshowmapseams, _T("setShowMapSeams"), 0, TYPE_VOID, 0, 1,
	_T("show"), 0, TYPE_BOOL,


	p_end
	);




//  Get Descriptor method for Mixin Interface
//  *****************************************


void *UnwrapClassDesc::Create(BOOL loading)
{

	AddInterface(&unwrap_interface);
	AddInterface(&unwrap_interface2);
	//5.1.05
	AddInterface(&unwrap_interface3);
	AddInterface(&unwrap_interface4);
	AddInterface(&unwrap_interface5);

	return new UnwrapMod;
}

const TCHAR *	UnwrapClassDesc::ClassName() {return UNWRAP_NAME;}
const TCHAR* 	UnwrapClassDesc::Category() {return GetString(IDS_RB_DEFSURFACE);}

FPInterfaceDesc* IUnwrapMod::GetDesc()
{
	return &unwrap_interface;
}

FPInterfaceDesc* IUnwrapMod2::GetDesc()
{
	return &unwrap_interface2;
}
//5.1.05
FPInterfaceDesc* IUnwrapMod3::GetDesc()
{
	return &unwrap_interface3;
}

FPInterfaceDesc* IUnwrapMod4::GetDesc()
{
	return &unwrap_interface4;
}


FPInterfaceDesc* IUnwrapMod5::GetDesc()
{
	return &unwrap_interface5;
}


FPInterfaceDesc* IUnwrapMod6::GetDesc()
{
	return &unwrap_interface6;
}





void UnwrapMod::fnPlanarMap()
{
	//align to normals
	//call fit
	fnAlignAndFit(3);
	

	flags |= CONTROL_FIT|CONTROL_CENTER|CONTROL_HOLD;
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
	ApplyGizmo();
	CleanUpDeadVertices();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
}

void UnwrapMod::fnSave()
{
	DragAcceptFiles(ip->GetMAXHWnd(), FALSE);
	SaveUVW(hDialogWnd);
	DragAcceptFiles(ip->GetMAXHWnd(), TRUE);
}
void UnwrapMod::fnLoad()
{
	DragAcceptFiles(ip->GetMAXHWnd(), FALSE);
	LoadUVW(hDialogWnd);
	DragAcceptFiles(ip->GetMAXHWnd(), TRUE);
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
	InvalidateView();
}

void UnwrapMod::fnEdit()
{
	HWND hWnd = hParams;
	RegisterClasses();
	if (ip)
	{
		RebuildEdges();
		if (!this->hDialogWnd) {
			HWND floaterHwnd = CreateDialogParam(
				hInstance,
				MAKEINTRESOURCE(IDD_UNWRAP_FLOATER),
				//			hWnd,
				ip->GetMAXHWnd (),
				UnwrapFloaterDlgProc,
				(LPARAM)this);
//			LoadMaterials();

			IMenuBarContext* pContext = (IMenuBarContext*) GetCOREInterface()->GetMenuManager()->GetContext(kUnwrapMenuBar);
			assert(pContext);
			pContext->CreateWindowsMenu();
			SetMenu(floaterHwnd, pContext->GetCurWindowsMenu());
			DrawMenuBar(floaterHwnd);
			pContext->UpdateWindowsMenu();
			LaunchScriptUI();
			SetFocus(floaterHwnd);
			if (ip->GetSubObjectLevel()== 0)
				fnSetTVSubMode(TVVERTMODE);
		} 
		else 
		{
			SetActiveWindow(this->hDialogWnd);
			ShowWindow(this->hDialogWnd,SW_RESTORE);
		}
	}
}

void UnwrapMod::fnReset()
{
	TSTR buf1 = GetString(IDS_RB_RESETUNWRAPUVWS);
	TSTR buf2 = GetString(IDS_RB_UNWRAPMOD);
	if (IDYES==MessageBox(ip->GetMAXHWnd(),buf1,buf2,MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL)) 
	{
		theHold.Begin();
		Reset();
		theHold.Accept(GetString(IDS_RB_RESETUVWS));
		ip->RedrawViews(ip->GetTime());
		InvalidateView();

	}
}


void UnwrapMod::fnSetMapChannel(int incChannel)
{
	int tempChannel = incChannel;
	if (tempChannel == 1) tempChannel = 0;

	if (tempChannel != channel)
	{
		if (iMapID) iMapID->SetValue(incChannel,TRUE);

		if (loadDefaults)
		{
			fnLoadDefaults();
			loadDefaults = FALSE;
		}

		theHold.Begin();		
		channel = incChannel;
		if (channel == 1) channel = 0;
		theHold.Accept(GetString(IDS_RB_SETCHANNEL));					

		SetCheckerMapChannel();
		NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);

		if (ip)
		{
			ip->RedrawViews(ip->GetTime());
			InvalidateView();
		}
	}

}
int	UnwrapMod::fnGetMapChannel()
{
	return channel;
}

void UnwrapMod::fnSetProjectionType(int proj)
{
	SetQMapAlign(proj-1);

	mUIManager.SetFlyOut(ID_QUICKMAP_ALIGN,proj,FALSE);


}
int	UnwrapMod::fnGetProjectionType()
{
	return GetQMapAlign()+1;
}

void UnwrapMod::fnSetQMapPreview(BOOL preview)
{
	SetQMapPreview(preview);
	mUIManager.UpdateCheckButtons();
}
BOOL UnwrapMod::fnGetQMapPreview()
{
	return GetQMapPreview();
}

void UnwrapMod::fnSetVC(BOOL vc)
{
	suppressWarning = TRUE;
	if (vc)
	{
		CheckRadioButton(  hParams, IDC_MAP_CHAN1, IDC_MAP_CHAN2,IDC_MAP_CHAN2);
		SendMessage(hParams,WM_COMMAND,IDC_MAP_CHAN2,0);
	}
	else 
	{
		CheckRadioButton(  hParams, IDC_MAP_CHAN1, IDC_MAP_CHAN2,IDC_MAP_CHAN1);
		SendMessage(hParams,WM_COMMAND,IDC_MAP_CHAN1,0);
	}	
	suppressWarning = FALSE;

}
BOOL UnwrapMod::fnGetVC()
{
	if (channel == 1)
		return TRUE;
	else return FALSE;
}


void UnwrapMod::fnMove()
{
	move = 0;
	mUIManager.SetFlyOut(ID_MOVE,move,FALSE);
	SetMode(ID_MOVE);
}
void UnwrapMod::fnMoveH()
{	
	move = 1;
	mUIManager.SetFlyOut(ID_MOVE,move,FALSE);
	SetMode(ID_MOVE);
}
void UnwrapMod::fnMoveV()
{
	move = 2;
	mUIManager.SetFlyOut(ID_MOVE,move,FALSE);
	SetMode(ID_MOVE);
}


void UnwrapMod::fnRotate()
{
	SetMode(ID_ROTATE);
}	

void UnwrapMod::fnScale()
{
	scale = 0;
	mUIManager.SetFlyOut(ID_SCALE,scale,FALSE);
	SetMode(ID_SCALE);
}
void UnwrapMod::fnScaleH()
{
	scale = 1;
	mUIManager.SetFlyOut(ID_SCALE,scale,FALSE);
	SetMode(ID_SCALE);
}
void UnwrapMod::fnScaleV()
{
	scale = 2;
	mUIManager.SetFlyOut(ID_SCALE,scale,FALSE);
	SetMode(ID_SCALE);
}



void UnwrapMod::fnMirrorH()
{

	mirror = 0;
	mUIManager.SetFlyOut(ID_MIRROR,mirror,FALSE);
	MirrorPoints( mirror);
}

void UnwrapMod::fnMirrorV()
{
	mirror = 1;
	mUIManager.SetFlyOut(ID_MIRROR,mirror,FALSE);
	MirrorPoints( mirror);
}


void UnwrapMod::fnExpandSelection()
{

	theHold.Begin();
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		theHold.Put(new TSelRestore(this,ld));
	}
	theHold.Accept(GetString(IDS_PW_SELECT_UVW));

	theHold.Suspend();
	if ((fnGetTVSubMode() == TVEDGEMODE)  )
	{
		BOOL openEdgeSel = FALSE;
		for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
		{
			MeshTopoData *ld = mMeshTopoData[ldID];
			BitArray esel = ld->GetTVEdgeSelection();
			int edgeCount = esel.GetSize();
			for (int i = 0; i < edgeCount; i++)
			{
				if (esel[i])
				{
					int ct = ld->GetTVEdgeNumberTVFaces(i);//TVMaps.ePtrList[i]->faceList.Count();
					if (ct <= 1)
					{
						openEdgeSel = TRUE;
						i = edgeCount;
						ldID = mMeshTopoData.Count();
					}
				}
			}
		}
		if (openEdgeSel)
			GrowSelectOpenEdge();
		GrowUVLoop(FALSE);
	}
	else ExpandSelection(0);

	if (fnGetSyncSelectionMode()) 
		fnSyncGeomSelection();

	theHold.Resume();

	InvalidateView();
	UpdateWindow(hDialogWnd);

	
}


void UnwrapMod::fnContractSelection()
{
	//	if (iIncSelected) iIncSelected->SetCurFlyOff(1,TRUE);

	if ((fnGetTVSubMode() == TVEDGEMODE)  )
		ShrinkSelectOpenEdge();
	else ExpandSelection(1);

	if (fnGetSyncSelectionMode()) 
		fnSyncGeomSelection();

	InvalidateView();
	UpdateWindow(hDialogWnd);
}

void UnwrapMod::fnSetFalloffType(int falloff)
{
	this->falloff = falloff-1;
	mUIManager.SetFlyOut(ID_FALLOFF,this->falloff,FALSE);
	RebuildDistCache();
	InvalidateView();
}
int	UnwrapMod::fnGetFalloffType()
{
	return falloff+1;
}
void UnwrapMod::fnSetFalloffSpace(int space)
{
	falloffSpace = space -1;
	mUIManager.SetFlyOut(ID_FALLOFF_SPACE,this->falloffSpace,FALSE);
	RebuildDistCache();
	InvalidateView();
}
int	UnwrapMod::fnGetFalloffSpace()
{
	return falloffSpace +1;
}

void UnwrapMod::fnSetFalloffDist(float dist)
{
	mUIManager.SetSpinFValue(ID_SOFTSELECTIONSTR_SPINNER,dist);
}
float UnwrapMod::fnGetFalloffDist()
{
	return mUIManager.GetSpinFValue(ID_SOFTSELECTIONSTR_SPINNER);//-1.0f;
}

void UnwrapMod::fnBreakSelected()
{
	ClearAFlag(A_HELD);
	BreakSelected();
	InvalidateView();
}
void UnwrapMod::fnWeld()
{
	if (mode == ID_WELD)
		SetMode(oldMode);
	else SetMode(ID_WELD);
}
void UnwrapMod::fnWeldSelected()
{
	ClearAFlag(A_HELD);
	WeldSelected(TRUE,TRUE);
}


void UnwrapMod::fnUpdatemap()
{
	SetupImage();
	UpdateListBox();
	InvalidateView();
	TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.updateMap"));
	macroRecorder->FunctionCall(mstr, 0, 0);
	macroRecorder->EmitScript();
}

void UnwrapMod::fnDisplaymap(BOOL update)
{
	showMap = update;
	mUIManager.UpdateCheckButtons();
}
BOOL UnwrapMod::fnIsMapDisplayed()
{
	return showMap;
}


void UnwrapMod::fnSetUVSpace(int space)
{
	uvw = space -1;
	mUIManager.SetFlyOut(ID_UVW,uvw,FALSE);
	InvalidateView();
}

int	UnwrapMod::fnGetUVSpace()
{
	return uvw+1;
}

void UnwrapMod::fnOptions()
{
	PropDialog();
}


void UnwrapMod::fnLock()
{
	lockSelected = !lockSelected;
	mUIManager.UpdateCheckButtons();
}

BOOL	UnwrapMod::fnGetLock()
{
	return lockSelected;
}
void	UnwrapMod::fnSetLock(BOOL lock)
{
	lockSelected = lock;
	mUIManager.UpdateCheckButtons();

}

void UnwrapMod::fnHide()
{
	HideSelected();
	InvalidateView();
}
void UnwrapMod::fnUnhide()
{
	UnHideAll();
	InvalidateView();
}

void UnwrapMod::fnFreeze()
{
	FreezeSelected();
	InvalidateView();

}
void UnwrapMod::fnThaw()
{
	UnFreezeAll();
	InvalidateView();
}	

void UnwrapMod::fnFilterSelected()
{

	filterSelectedFaces = !filterSelectedFaces;
	BuildFilterSelectedFacesData();
	mUIManager.UpdateCheckButtons();

}

BOOL	UnwrapMod::fnGetFilteredSelected()
{
	return filterSelectedFaces;
}
void	UnwrapMod::fnSetFilteredSelected(BOOL filter)
{
	filterSelectedFaces = filter;
	BuildFilterSelectedFacesData();
	mUIManager.UpdateCheckButtons();
}



void UnwrapMod::fnPan()
{
	SetMode(ID_PAN);
}
void UnwrapMod::fnZoom()
{
	SetMode(ID_ZOOMTOOL);
}
void UnwrapMod::fnZoomRegion()
{
	SetMode(ID_ZOOMREGION);
}
void UnwrapMod::fnFit()
{
	ZoomExtents();
}
void UnwrapMod::fnFitSelected()
{
	ZoomSelected();
}

void UnwrapMod::fnSnap()
{
	pixelSnap = !pixelSnap;	
	gridSnap = FALSE;
	mUIManager.UpdateCheckButtons();


}

BOOL	UnwrapMod::fnGetSnap()
{
	return pixelSnap;
}
void	UnwrapMod::fnSetSnap(BOOL snap)
{
	pixelSnap = snap;
	if (pixelSnap)
		gridSnap = FALSE;
	mUIManager.UpdateCheckButtons();

}


int	UnwrapMod::fnGetCurrentMap()
{
	return CurrentMap+1;
}
void UnwrapMod::fnSetCurrentMap(int map)
{
	map--;
	int ct = SendMessage( hTextures, CB_GETCOUNT, 0, 0 )-3;

	if ( (map < ct) && (CurrentMap!=map))
	{
		CurrentMap = map;
		SendMessage(hTextures, CB_SETCURSEL, map, 0 );
		SetupImage();
	}
}	
int	UnwrapMod::fnNumberMaps()
{
	int ct = SendMessage( hTextures, CB_GETCOUNT, 0, 0 )-3;
	return ct;
}

Point3 *UnwrapMod::fnGetLineColor()
{
	AColor c(lineColor);

	lColor.x = c.r;
	lColor.y = c.g;
	lColor.z = c.b;
	return &lColor;
}
void UnwrapMod::fnSetLineColor(Point3 color)
{
	AColor c;
	c.r = color.x;
	c.g = color.y;
	c.b = color.z;
	lineColor = c.toRGB();
	InvalidateView();
}

Point3 *UnwrapMod::fnGetSelColor()
{
	AColor c(selColor);
	lColor.x = c.r;
	lColor.y = c.g;
	lColor.z = c.b;
	return &lColor;
}

void UnwrapMod::fnSetSelColor(Point3 color)
{
	AColor c;
	c.r = color.x;
	c.g = color.y;
	c.b = color.z;

	selColor = c.toRGB();
	InvalidateView();
}

void UnwrapMod::fnSetRenderWidth(int dist)
{
	rendW = dist;
	if (rendW!=iw)
		SetupImage();
	InvalidateView();
}
int UnwrapMod::fnGetRenderWidth()
{
	return rendW;
}
void UnwrapMod::fnSetRenderHeight(int dist)
{
	rendH = dist;
	if (rendH!=ih)
		SetupImage();
	InvalidateView();
}
int UnwrapMod::fnGetRenderHeight()
{
	return rendH;
}

void UnwrapMod::fnSetWeldThreshold(float dist)
{
	weldThreshold = dist;
}
float UnwrapMod::fnGetWeldThresold()
{
	return weldThreshold;
}

void UnwrapMod::fnSetUseBitmapRes(BOOL useBitmapRes)
{
	BOOL change= FALSE;
	if (this->useBitmapRes != useBitmapRes)
		change = TRUE;
	this->useBitmapRes = useBitmapRes;
	if (change)
		SetupImage();
	InvalidateView();
}
BOOL UnwrapMod::fnGetUseBitmapRes()
{
	return useBitmapRes;
}



BOOL UnwrapMod::fnGetConstantUpdate()
{
	return update;
}
void UnwrapMod::fnSetConstantUpdate(BOOL constantUpdates)
{
	update = constantUpdates;
}

BOOL UnwrapMod::fnGetShowSelectedVertices()
{
	return showVerts;
}
void UnwrapMod::fnSetShowSelectedVertices(BOOL show)
{
	showVerts = show;
	NotifyDependents(FOREVER,PART_SELECT,REFMSG_CHANGE);
	InvalidateView();

}

BOOL UnwrapMod::fnGetMidPixelSnape()
{
	return midPixelSnap;
}

void UnwrapMod::fnSetMidPixelSnape(BOOL midPixel)
{
	midPixelSnap = midPixel;
}


int	UnwrapMod::fnGetMatID()
{
	return matid+2;
}
void UnwrapMod::fnSetMatID(int mid)
{
	mid--;
	int ct = SendMessage( hMatIDs, CB_GETCOUNT, 0, 0 );

	if ( (mid < ct) && (matid!=(mid-1)))
	{		
		SendMessage(hMatIDs, CB_SETCURSEL, mid, 0 );
		mid-=1;

		matid = mid;
		InvalidateView();
	}

}
int	UnwrapMod::fnNumberMatIDs()
{
	int ct = SendMessage( hMatIDs, CB_GETCOUNT, 0, 0 );
	return ct;
}


void UnwrapMod::fnMoveSelectedVertices(Point3 offset)
{
	Point2 pt;
	pt.x = offset.x;
	pt.y = offset.y;
	theHold.Begin();
	MovePoints(pt);
	theHold.Accept(GetString(IDS_PW_MOVE_UVW));
}
void UnwrapMod::fnRotateSelectedVertices(float angle, Point3 axis)
{
	theHold.Begin();
	//	centeron=TRUE;
	//	center.x = axis.x;
	//	center.y = axis.y;
	//	center.z = axis.z;
	RotateAroundAxis(tempHwnd, angle, axis);
	//	RotatePoints(tempHwnd, angle);
	theHold.Accept(GetString(IDS_PW_ROTATE_UVW));

}
void UnwrapMod::fnRotateSelectedVertices(float angle)
{
	theHold.Begin();
	centeron=FALSE;
	RotatePoints(tempHwnd, angle);
	theHold.Accept(GetString(IDS_PW_ROTATE_UVW));
}
void UnwrapMod::fnScaleSelectedVertices(float scale,int dir)
{
	theHold.Begin();
	centeron=FALSE;
	ScalePoints(tempHwnd, scale,dir);
	theHold.Accept(GetString(IDS_PW_SCALE_UVW));
}


void UnwrapMod::fnScaleSelectedVertices(float scale,int dir, Point3 axis)
{
	theHold.Begin();
	centeron=TRUE;
	center.x = axis.x;
	center.y = axis.y;
	ScalePoints(tempHwnd, scale,dir);
	theHold.Accept(GetString(IDS_PW_SCALE_UVW));
}

Point3* UnwrapMod::fnGetVertexPosition(TimeValue t,  int index, INode *node)
{
	index--;
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		tempVert = ld->GetTVVert(index);
	}	
	return &tempVert;
}

Point3* UnwrapMod::fnGetVertexPosition(TimeValue t,  int index)
{
	index--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];		
		if (ld)
		{
			tempVert = ld->GetTVVert(index);
		}
	}
	
	return &tempVert;
}


int	UnwrapMod::fnNumberVertices(INode *node)
{
	
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		return ld->GetNumberTVVerts();//TVMaps.v.Count();
	}
	
	return 0;
}

int	UnwrapMod::fnNumberVertices()
{
	
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
			return ld->GetNumberTVVerts();//TVMaps.v.Count();
	}
	
	return 0;
}


void UnwrapMod::fnMoveX(float p)
{
	ChannelChanged(0, p);

}
void UnwrapMod::fnMoveY(float p)
{
	ChannelChanged(1, p);
}
void UnwrapMod::fnMoveZ(float p)
{
	ChannelChanged(2, p);
}




// private namespace
namespace
{
	class sMyEnumProc : public DependentEnumProc 
	{
	public :
		virtual int proc(ReferenceMaker *rmaker); 
		INodeTab Nodes;              
	};

	int sMyEnumProc::proc(ReferenceMaker *rmaker) 
	{ 
		if (rmaker->SuperClassID()==BASENODE_CLASS_ID)    
		{
			Nodes.Append(1, (INode **)&rmaker);  
			return DEP_ENUM_SKIP;
		}

		return DEP_ENUM_CONTINUE;
	}
}



class ModDataOnStack : public GeomPipelineEnumProc
	{
public:  
   ModDataOnStack(ReferenceTarget *me) : mRef(me),mModData(NULL) {}
   PipeEnumResult proc(ReferenceTarget *object, IDerivedObject *derObj, int index);
   ReferenceTarget *mRef;
   MeshTopoData *mModData;
protected:
   ModDataOnStack(); // disallowed
   ModDataOnStack(ModDataOnStack& rhs); // disallowed
   ModDataOnStack& operator=(const ModDataOnStack& rhs); // disallowed
};

PipeEnumResult ModDataOnStack::proc(
   ReferenceTarget *object, 
   IDerivedObject *derObj, 
   int index)
{
   if ((derObj != NULL) && object == mRef) //found it!
   {
		ModContext *mc = derObj->GetModContext(index);
		mModData = (MeshTopoData*)mc->localData;
        return PIPE_ENUM_STOP;
   }
   return PIPE_ENUM_CONTINUE;
}



//new
MeshTopoData *UnwrapMod::GetModData()
{
	sMyEnumProc dep;              
	DoEnumDependents(&dep);
	if (dep.Nodes.Count() > 0)
	{
		INode *node = dep.Nodes[0];
		Object* obj = node->GetObjectRef();
		ModDataOnStack pipeEnumProc(this);
        EnumGeomPipeline(&pipeEnumProc, obj);
		return pipeEnumProc.mModData;
	}
	return NULL;
}


BitArray* UnwrapMod::fnGetSelectedPolygons(INode *node)
{

	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		return ld->GetFaceSelectionPtr();
	}
	return NULL;
}

BitArray* UnwrapMod::fnGetSelectedPolygons()
{
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
			return ld->GetFaceSelectionPtr();
	}
	return NULL;
}


void UnwrapMod::fnSelectPolygons(BitArray *sel, INode *node)
{
		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			ld->ClearFaceSelection();
			for (int i =0; i < ld->GetNumberFaces(); i++)
			{
				if (i < sel->GetSize())
				{
					if ((*sel)[i]) 
						ld->SetFaceSelected(i,TRUE);//md->faceSel.Set(i);
				}
			}
			fnSyncTVSelection();
			NotifyDependents(FOREVER,PART_SELECT,REFMSG_CHANGE);
			InvalidateView();
		}
}



void UnwrapMod::fnSelectPolygons(BitArray *sel)
{
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			ld->ClearFaceSelection();
			for (int i =0; i < ld->GetNumberFaces(); i++)
			{
				if (i < sel->GetSize())
				{
					if ((*sel)[i]) 
						ld->SetFaceSelected(i,TRUE);//md->faceSel.Set(i);
				}
			}
			fnSyncTVSelection();
			NotifyDependents(FOREVER,PART_SELECT,REFMSG_CHANGE);
			InvalidateView();
		}
	}
}


BOOL UnwrapMod::fnIsPolygonSelected(int index, INode *node)
{
	index--;
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		return ld->GetFaceSelected(index);
	}

	return FALSE;	
}

BOOL UnwrapMod::fnIsPolygonSelected(int index)
{
	index--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			return ld->GetFaceSelected(index);
		}
	}

	return FALSE;	
}

int	UnwrapMod::fnNumberPolygons(INode *node)
{
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		return ld->GetNumberFaces();
	}
	return 0;
}

int	UnwrapMod::fnNumberPolygons()
{
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			return ld->GetNumberFaces();
		}
	}
	return 0;
}

void UnwrapMod::fnDetachEdgeVerts()
{
	DetachEdgeVerts();
}

void UnwrapMod::fnFlipH()
{

	mirror = 2;
	mUIManager.SetFlyOut(ID_MIRROR,mirror,FALSE);
	FlipPoints(mirror-2);


}
void UnwrapMod::fnFlipV()
{

	mirror = 3;		
	mUIManager.SetFlyOut(ID_MIRROR,mirror,FALSE);
	FlipPoints(mirror-2);
}

BOOL UnwrapMod::fnGetLockAspect()
{
	return lockAspect;
}
void UnwrapMod::fnSetLockAspect(BOOL a)
{
	lockAspect = a;
	InvalidateView();
}


float UnwrapMod::fnGetMapScale()
{
	return mapScale;
}
void UnwrapMod::fnSetMapScale(float sc)
{
	mapScale = sc;
	NotifyDependents(FOREVER,PART_SELECT,REFMSG_CHANGE);
	InvalidateView();

}

void UnwrapMod::fnGetSelectionFromFace()
{
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		if (ld)
		{
			BitArray vsel = ld->GetTVVertSelection();
			BitArray tempSel(vsel);
			tempSel.ClearAll();

			for (int i = 0; i < ld->GetNumberFaces(); i++)//TVMaps.f.Count(); i++)
			{
				if (!ld->GetFaceDead(i))//(TVMaps.f[i]->flags & FLAG_DEAD))
				{
					if (ld->GetFaceSelected(i))//TVMaps.f[i]->flags & FLAG_SELECTED)
					{
						int degree = ld->GetFaceDegree(i);
						for (int j = 0; j < degree; j++)
						{
							int id = ld->GetFaceTVVert(i,j);//TVMaps.f[i]->t[j];
							tempSel.Set(id,TRUE);
						}
					}
				}
			}
			if (fnGetTVSubMode() == TVVERTMODE)
			{
				ld->SetTVVertSelection(tempSel);//	vsel = tempSel;
			}
			else if (fnGetTVSubMode() == TVEDGEMODE)
			{
				BitArray holdSel(vsel);
				ld->SetTVVertSelection(tempSel);//vsel = tempSel;
				BitArray esel;
				ld->GetEdgeSelFromVert(esel,FALSE);
				ld->SetTVEdgeSelection(esel);
				ld->SetTVVertSelection(holdSel);//vsel = holdSel;
			}
			else if (fnGetTVSubMode() == TVFACEMODE)
			{
				BitArray holdSel(vsel);
				ld->SetTVVertSelection(tempSel);//vsel = tempSel;
				BitArray fsel;
				ld->GetFaceSelFromVert(fsel,FALSE);
				ld->SetFaceSelection(fsel);//
				ld->SetTVVertSelection(holdSel);//vsel = holdSel;
			}



		}
	}
	InvalidateView();
}

void UnwrapMod::fnForceUpdate(BOOL update)
{
	forceUpdate = update;
}

void UnwrapMod::fnZoomToGizmo(BOOL all)
{
	if (ip)
	{
		if (!gizmoBounds.IsEmpty() && ip && (ip->GetSubObjectLevel() == 3))
			ip->ZoomToBounds(all,gizmoBounds);
	}		
}

void UnwrapMod::fnSetVertexPosition(TimeValue t, int index, Point3 pos, INode *node)
{
	index --;
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
		SetVertexPosition(ld,t, index, pos);
	
}

void UnwrapMod::fnSetVertexPosition(TimeValue t, int index, Point3 pos)
{
	index --;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
			SetVertexPosition(ld,t, index, pos);
	}
	
}

void UnwrapMod::fnSetVertexPosition2(TimeValue t, int index, Point3 pos, BOOL hold, BOOL update, INode *node)
{
	index --;
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
		SetVertexPosition(ld,t, index, pos, hold, update);

}

void UnwrapMod::fnSetVertexPosition2(TimeValue t, int index, Point3 pos, BOOL hold, BOOL update)
{
	index --;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
			SetVertexPosition(ld,t, index, pos, hold, update);
	}

}

void UnwrapMod::fnMarkAsDead(int index, INode *node)
{
	index--;
		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			ld->SetTVVertDead(index,TRUE);
		}

}

void UnwrapMod::fnMarkAsDead(int index)
{
	index--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			ld->SetTVVertDead(index,TRUE);
		}
	}

}


int UnwrapMod::fnNumberPointsInFace(int index, INode *node)
{
	index--;
		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			return ld->GetFaceDegree(index);
		}
	return 0;
}


int UnwrapMod::fnNumberPointsInFace(int index)
{
	index--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			return ld->GetFaceDegree(index);
		}
	}
	return 0;
}

int UnwrapMod::fnGetVertexIndexFromFace(int index,int vertexIndex, INode *node)
{

	index--;
	vertexIndex--;
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		return ld->GetFaceTVVert(index,vertexIndex)+1;
	}

	return 0;
}

int UnwrapMod::fnGetVertexIndexFromFace(int index,int vertexIndex)
{

	index--;
	vertexIndex--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			return ld->GetFaceTVVert(index,vertexIndex)+1;
		}
	}

	return 0;
}


int UnwrapMod::fnGetHandleIndexFromFace(int index,int vertexIndex, INode *node)
{
	index--;
	vertexIndex--;

	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		if (ld->GetFaceHasVectors(index))
			return ld->GetFaceTVHandle(index,vertexIndex)+1;
	}
	return 0;
}


int UnwrapMod::fnGetHandleIndexFromFace(int index,int vertexIndex)
{
	index--;
	vertexIndex--;

	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			if (ld->GetFaceHasVectors(index))
				return ld->GetFaceTVHandle(index,vertexIndex)+1;
		}
	}
	return 0;
}

int UnwrapMod::fnGetInteriorIndexFromFace(int index,int vertexIndex, INode *node)
{

	index--;
	vertexIndex--;

		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			if (ld->GetFaceHasVectors(index))
				return ld->GetFaceTVInterior(index,vertexIndex)+1;
		}
	return 0;
}


int UnwrapMod::fnGetInteriorIndexFromFace(int index,int vertexIndex)
{

	index--;
	vertexIndex--;

	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			if (ld->GetFaceHasVectors(index))
				return ld->GetFaceTVInterior(index,vertexIndex)+1;
		}
	}
	return 0;
}


int UnwrapMod::fnGetVertexGIndexFromFace(int index,int vertexIndex, INode *node)
{

	index--;
	vertexIndex--;
		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			return ld->GetFaceGeomVert(index,vertexIndex)+1;
		}
	return 0;
}


int UnwrapMod::fnGetVertexGIndexFromFace(int index,int vertexIndex)
{

	index--;
	vertexIndex--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			return ld->GetFaceGeomVert(index,vertexIndex)+1;
		}
	}
	return 0;
}

int UnwrapMod::fnGetHandleGIndexFromFace(int index,int vertexIndex, INode *node)
{

	index--;
	vertexIndex--;


		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			if (ld->GetFaceHasVectors(index))
				return ld->GetFaceGeomHandle(index,vertexIndex)+1;
		}
	return 0;
}


int UnwrapMod::fnGetHandleGIndexFromFace(int index,int vertexIndex)
{

	index--;
	vertexIndex--;

	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			if (ld->GetFaceHasVectors(index))
				return ld->GetFaceGeomHandle(index,vertexIndex)+1;
		}
	}
	return 0;
}

int UnwrapMod::fnGetInteriorGIndexFromFace(int index,int vertexIndex, INode *node)
{

	index--;
	vertexIndex--;
		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			if (ld->GetFaceHasVectors(index))
				return ld->GetFaceGeomInterior(index,vertexIndex)+1;
		}
	return 0;
}


int UnwrapMod::fnGetInteriorGIndexFromFace(int index,int vertexIndex)
{

	index--;
	vertexIndex--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			if (ld->GetFaceHasVectors(index))
				return ld->GetFaceGeomInterior(index,vertexIndex)+1;
		}
	}
	return 0;
}


void UnwrapMod::fnAddPoint(Point3 pos, int fIndex,int vIndex, BOOL sel, INode *node)
{

	fIndex--;
	vIndex--;
		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			ld->AddTVVert(0, pos,fIndex,vIndex,this,sel);
		}
}

void UnwrapMod::fnAddPoint(Point3 pos, int fIndex,int vIndex, BOOL sel)
{

	fIndex--;
	vIndex--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			ld->AddTVVert(0, pos,fIndex,vIndex,this,sel);
		}
	}
}

void UnwrapMod::fnAddHandle(Point3 pos, int fIndex,int vIndex, BOOL sel,INode *node)
{


	fIndex--;
	vIndex--;
		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			ld->AddTVHandle(0, pos,fIndex,vIndex,this,sel);
		}
}

void UnwrapMod::fnAddHandle(Point3 pos, int fIndex,int vIndex, BOOL sel)
{


	fIndex--;
	vIndex--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			ld->AddTVHandle(0, pos,fIndex,vIndex,this,sel);
		}
	}
}

void UnwrapMod::fnAddInterior(Point3 pos, int fIndex,int vIndex, BOOL sel, INode *node)
{

	fIndex--;
	vIndex--;
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		ld->AddTVInterior(0, pos,fIndex,vIndex,this,sel);
	}
}


void UnwrapMod::fnAddInterior(Point3 pos, int fIndex,int vIndex, BOOL sel)
{

	fIndex--;
	vIndex--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			ld->AddTVInterior(0, pos,fIndex,vIndex,this,sel);
		}
	}
}


void UnwrapMod::fnSetFaceVertexIndex(int fIndex,int ithV, int vIndex, INode *node)
{

	fIndex--;
	ithV--;
	vIndex--;

		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			ld->SetFaceTVVert(fIndex,ithV,vIndex);
		}
}


void UnwrapMod::fnSetFaceVertexIndex(int fIndex,int ithV, int vIndex)
{

	fIndex--;
	ithV--;
	vIndex--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			ld->SetFaceTVVert(fIndex,ithV,vIndex);
		}
	}
}

void UnwrapMod::fnSetFaceHandleIndex(int fIndex,int ithV, int vIndex, INode *node)
{

	fIndex--;
	ithV--;
	vIndex--;
		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			if (ld->GetFaceHasVectors(fIndex))
				ld->SetFaceTVHandle(fIndex,ithV,vIndex);
		}
}

void UnwrapMod::fnSetFaceHandleIndex(int fIndex,int ithV, int vIndex)
{

	fIndex--;
	ithV--;
	vIndex--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			if (ld->GetFaceHasVectors(fIndex))
				ld->SetFaceTVHandle(fIndex,ithV,vIndex);
		}
	}
}

void UnwrapMod::fnSetFaceInteriorIndex(int fIndex,int ithV, int vIndex, INode *node)
{

	fIndex--;
	ithV--;
	vIndex--;
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		if (ld->GetFaceHasVectors(fIndex))
			ld->SetFaceTVInterior(fIndex,ithV,vIndex);
	}
}


void UnwrapMod::fnSetFaceInteriorIndex(int fIndex,int ithV, int vIndex)
{

	fIndex--;
	ithV--;
	vIndex--;
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			if (ld->GetFaceHasVectors(fIndex))
				ld->SetFaceTVInterior(fIndex,ithV,vIndex);
		}
	}
}

void UnwrapMod::fnUpdateViews()
{
	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	InvalidateView();
}

void UnwrapMod::fnGetFaceSelFromStack()
{
	getFaceSelectionFromStack = TRUE;
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	if (ip) ip->RedrawViews(ip->GetTime());
	InvalidateView();
}

void UnwrapMod::fnSetDebugLevel(int level)
{
	gDebugLevel = level;
}


BOOL	UnwrapMod::fnGetTile()
{
	return bTile;
}
void	UnwrapMod::fnSetTile(BOOL tile)
{
	bTile = tile;
	tileValid = FALSE;
	InvalidateView();
}

int		UnwrapMod::fnGetTileLimit()
{
	return iTileLimit;

}
void	UnwrapMod::fnSetTileLimit(int limit)
{
	iTileLimit = limit;
	if (iTileLimit < 0 ) iTileLimit = 0;
	if (iTileLimit > 50 ) iTileLimit = 50;
	tileValid = FALSE;
	InvalidateView();
}

float	UnwrapMod::fnGetTileContrast()
{
	return fContrast;
}
void	UnwrapMod::fnSetTileContrast(float fcontrast)
{
	this->fContrast = fcontrast;
	tileValid = FALSE;
	InvalidateView();
}



BOOL	UnwrapMod::fnGetShowMap()
{
	return showMap;
}	

void	UnwrapMod::fnSetShowMap(BOOL smap)
{
	showMap = smap;
	InvalidateView();
	mUIManager.UpdateCheckButtons();
}

void	UnwrapMod::fnShowMap()
{
	showMap = !showMap;
	InvalidateView();
}

#pragma warning (disable  : 4530)

void	UnwrapMod::InitScriptUI()
{
	return;
	if (executedStartUIScript==FALSE)
	{
		TSTR scriptUI;

		scriptUI.printf(_T("mcrfile = openFile   \"UI\\MacroScripts\\Macro_UnwrapUI.mcr\" ; execute mcrfile"));


		init_thread_locals();
		push_alloc_frame();
		one_typed_value_local(StringStream* util_script);
		save_current_frames();
//		set_error_trace_back_active( FALSE );

		try 
		{
			vl.util_script = new StringStream (scriptUI);
			vl.util_script->execute_vf(NULL, 0);
		}
		catch (MAXScriptException& e)
		{
			clear_error_source_data();
			restore_current_frames();
			MAXScript_signals = 0;
			if (progress_bar_up)
				MAXScript_interface->ProgressEnd(), progress_bar_up = FALSE;
			error_message_box(e, _T("Unwrap UI Script Macro_UnwrapUI.mcr not found"));
		}
		catch (...)
		{
			clear_error_source_data();
			restore_current_frames();
			if (progress_bar_up)
				MAXScript_interface->ProgressEnd(), progress_bar_up = FALSE;
			error_message_box(UnknownSystemException (), _T("Unwrap UI Script Macro_UnwrapUI.mcr not found"));
		}
		vl.util_script->close();
		pop_value_locals();
		pop_alloc_frame();
		executedStartUIScript=TRUE;
	}

}

void	UnwrapMod::LaunchScriptUI()
{
	return;
	InitScriptUI();

	TSTR scriptUI;

	scriptUI.printf(_T("macros.run \"UVW Unwrap\" \"OpenUnwrapUI\" "));


	init_thread_locals();
	push_alloc_frame();
	one_typed_value_local(StringStream* util_script);
	save_current_frames();
//	set_error_trace_back_active( FALSE );

	try 
	{
		vl.util_script = new StringStream (scriptUI);
		vl.util_script->execute_vf(NULL, 0);
	}
	catch (MAXScriptException& e)
	{
		clear_error_source_data();
		restore_current_frames();
		MAXScript_signals = 0;
		if (progress_bar_up)
			MAXScript_interface->ProgressEnd(), progress_bar_up = FALSE;
		error_message_box(e, _T("Unwrap UI Script OpenUnwrapUI Macro not found"));
	}
	catch (...)
	{
		clear_error_source_data();
		restore_current_frames();
		if (progress_bar_up)
			MAXScript_interface->ProgressEnd(), progress_bar_up = FALSE;
		error_message_box(UnknownSystemException (), _T("Unwrap UI Script  OpenUnwrapUI Macro not found"));
	}
	vl.util_script->close();
	pop_value_locals();
	pop_alloc_frame();
}

void	UnwrapMod::EndScriptUI()
{
	return;
	DbgAssert(executedStartUIScript);
	TSTR scriptUI;

	scriptUI.printf(_T("macros.run \"UVW Unwrap\" \"CloseUnwrapUI\" "));

	init_thread_locals();
	push_alloc_frame();
	one_typed_value_local(StringStream* util_script);
	save_current_frames();
//	set_error_trace_back_active( FALSE );

	try 
	{
		vl.util_script = new StringStream (scriptUI);
		vl.util_script->execute_vf(NULL, 0);
	}
	catch (MAXScriptException& e)
	{
		clear_error_source_data();
		restore_current_frames();
		MAXScript_signals = 0;
		if (progress_bar_up)
			MAXScript_interface->ProgressEnd(), progress_bar_up = FALSE;
		error_message_box(e, _T("Unwrap UI Script CloseUnwrapUI Macro not found"));
	}
	catch (...)
	{
		clear_error_source_data();
		restore_current_frames();
		if (progress_bar_up)
			MAXScript_interface->ProgressEnd(), progress_bar_up = FALSE;
		error_message_box(UnknownSystemException (), _T("Unwrap UI Script  CloseUnwrapUI Macro not found"));
	}
	vl.util_script->close();
	pop_value_locals();
	pop_alloc_frame();


}

void	UnwrapMod::MoveScriptUI()
{
	return;
	InitScriptUI();

	TSTR scriptUI;

	scriptUI.printf(_T("macros.run \"UVW Unwrap\" \"MoveUnwrapUI\" "));

	init_thread_locals();
	push_alloc_frame();
	one_typed_value_local(StringStream* util_script);
	save_current_frames();
//	set_error_trace_back_active( FALSE );

	try 
	{
		vl.util_script = new StringStream (scriptUI);
		vl.util_script->execute_vf(NULL, 0);
	}
	catch (MAXScriptException& e)
	{
		clear_error_source_data();
		restore_current_frames();
		MAXScript_signals = 0;
		if (progress_bar_up)
			MAXScript_interface->ProgressEnd(), progress_bar_up = FALSE;
		error_message_box(e, _T("Unwrap UI Script MoveUnwrapUI Macro not found"));
	}
	catch (...)
	{
		clear_error_source_data();
		restore_current_frames();
		if (progress_bar_up)
			MAXScript_interface->ProgressEnd(), progress_bar_up = FALSE;
		error_message_box(UnknownSystemException (), _T("Unwrap UI Script  MoveUnwrapUI Macro not found"));
	}
	vl.util_script->close();
	pop_value_locals();
	pop_alloc_frame();


}


#pragma warning (default  : 4530)

int		UnwrapMod::fnGetWindowX()
{
	WINDOWPLACEMENT floaterPos;
	GetWindowPlacement(hDialogWnd,&floaterPos);



	WINDOWPLACEMENT maxPos;
	Interface *ip = GetCOREInterface();
	HWND maxHwnd = ip->GetMAXHWnd();
	GetWindowPlacement(maxHwnd,&maxPos);

	RECT rect;
	GetWindowRect(  hDialogWnd ,&rect);

	if (floaterPos.showCmd == SW_MINIMIZE)
		return 0;
	else return rect.left;//return floaterPos.rcNormalPosition.left;
}
int		UnwrapMod::fnGetWindowY()
{
	WINDOWPLACEMENT floaterPos;
	GetWindowPlacement(hDialogWnd,&floaterPos);

	RECT rect;
	GetWindowRect(  hDialogWnd ,&rect);	

	if (floaterPos.showCmd == SW_MINIMIZE)
		return 0;
	else return rect.top;//floaterPos.rcNormalPosition.top;
}
int		UnwrapMod::fnGetWindowW()
{
	WINDOWPLACEMENT floaterPos;
	floaterPos.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hDialogWnd,&floaterPos);
	if (floaterPos.showCmd == SW_MAXIMIZE)
		return maximizeWidth-xWindowOffset-2;
	if (floaterPos.showCmd == SW_MINIMIZE)
		return 0;
	else return (floaterPos.rcNormalPosition.right-floaterPos.rcNormalPosition.left);

}
int		UnwrapMod::fnGetWindowH()
{
	WINDOWPLACEMENT floaterPos;
	floaterPos.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hDialogWnd,&floaterPos);

	if (floaterPos.showCmd == SW_MAXIMIZE)
		return maximizeHeight-yWindowOffset;
	if ((floaterPos.showCmd == SW_MINIMIZE) || (minimized))
		return 0;
	else return (floaterPos.rcNormalPosition.bottom-floaterPos.rcNormalPosition.top);
}


BOOL	UnwrapMod::fnGetBackFaceCull()
{
	return ignoreBackFaceCull;
}	

void	UnwrapMod::fnSetBackFaceCull(BOOL backFaceCull)
{
	ignoreBackFaceCull = backFaceCull;
	//update UI
	mUIManager.UpdateCheckButtons();
}

BOOL	UnwrapMod::fnGetOldSelMethod()
{
	return oldSelMethod;
}
void	UnwrapMod::fnSetOldSelMethod(BOOL oldSelMethod)
{
	this->oldSelMethod = oldSelMethod;
}

BOOL	UnwrapMod::fnGetAlwaysEdit()
{
	return alwaysEdit;
}
void	UnwrapMod::fnSetAlwaysEdit(BOOL always)
{
	this->alwaysEdit = always;
}


int		UnwrapMod::fnGetTVSubMode()
{
	return mTVSubObjectMode;
}
void	UnwrapMod::fnSetTVSubMode(int smode)
{
	mTVSubObjectMode = smode;

	if (ip)
	{
		if (smode != ip->GetSubObjectLevel())
			ip->SetSubObjectLevel(smode);
	}
	if (fnGetSyncSelectionMode())
	{
		theHold.Suspend();
		fnSyncGeomSelection();
		theHold.Resume();
	}

	if (smode == 3)
	{
		if ( mode == ID_WELD )
			SetMode(oldMode);
	}

	if ( (ip) &&(hDialogWnd) )
	{	
		IMenuBarContext* pContext = (IMenuBarContext*) GetCOREInterface()->GetMenuManager()->GetContext(kUnwrapMenuBar);
		if (pContext)
			pContext->UpdateWindowsMenu();
	}

	InvalidateView();
}

int		UnwrapMod::fnGetFillMode()
{
	return fillMode;
}

void	UnwrapMod::fnSetFillMode(int mode)
{
	fillMode = mode;
	InvalidateView();
}

void UnwrapMod::fnMoveSelected(Point3 offset)
{
	Point2 pt;
	pt.x = offset.x;
	pt.y = offset.y;
	TransferSelectionStart();
	theHold.Begin();
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		theHold.Put(new TVertRestore(this,ld,FALSE));
	}
	theHold.Accept(GetString(IDS_PW_MOVE_UVW));
	MovePoints(pt);
	TransferSelectionEnd(FALSE,FALSE);
}
void UnwrapMod::fnRotateSelected(float angle, Point3 axis)
{
	TransferSelectionStart();
	theHold.Begin();
	//	centeron=TRUE;
	//	center.x = axis.x;
	//	center.y = axis.y;
	//	center.z = axis.z;
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		theHold.Put(new TVertRestore(this,ld,FALSE));
	}
	theHold.Accept(GetString(IDS_PW_ROTATE_UVW));

	RotateAroundAxis(tempHwnd, angle,axis);
	TransferSelectionEnd(FALSE,FALSE);

}
void UnwrapMod::fnRotateSelected(float angle)
{
	TransferSelectionStart();
	theHold.Begin();
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		theHold.Put(new TVertRestore(this,ld,FALSE));
	}
	theHold.Accept(GetString(IDS_PW_ROTATE_UVW));
	centeron=FALSE;
	RotatePoints(tempHwnd, angle);
	TransferSelectionEnd(FALSE,FALSE);
}
void UnwrapMod::fnScaleSelected(float scale,int dir)
{
	TransferSelectionStart();
	theHold.Begin();
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		theHold.Put(new TVertRestore(this,ld,FALSE));
	}
	theHold.Accept(GetString(IDS_PW_SCALE_UVW));
	centeron=FALSE;
	ScalePoints(tempHwnd, scale,dir);
	TransferSelectionEnd(FALSE,FALSE);
}

void UnwrapMod::fnScaleSelected(float scale,int dir, Point3 axis)
{
	TransferSelectionStart();
	theHold.Begin();
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		theHold.Put(new TVertRestore(this,ld,FALSE));
	}
	theHold.Accept(GetString(IDS_PW_SCALE_UVW));

	centeron=TRUE;
	center.x = axis.x;
	center.y = axis.y;

	ScalePoints(tempHwnd, scale,dir);
	TransferSelectionEnd(FALSE,FALSE);
}

void UnwrapMod::fnScaleSelectedXY(float scaleX,float scaleY, Point3 axis)
{
	TransferSelectionStart();
	theHold.Begin();
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		theHold.Put(new TVertRestore(this,ld,FALSE));
	}
	theHold.Accept(GetString(IDS_PW_SCALE_UVW));

	ScaleAroundAxis(tempHwnd, scaleX,scaleY,axis);
	TransferSelectionEnd(FALSE,FALSE);
}


BOOL	UnwrapMod::fnGetDisplayOpenEdges()
{
	return displayOpenEdges;
}
void	UnwrapMod::fnSetDisplayOpenEdges(BOOL openEdgeDisplay)
{
	displayOpenEdges = openEdgeDisplay;
	InvalidateView();
}

BOOL	UnwrapMod::fnGetThickOpenEdges()
{
	return thickOpenEdges;
}
void	UnwrapMod::fnSetThickOpenEdges(BOOL thick)
{
	if (theHold.Holding())
	{
		theHold.Put(new UnwrapSeamAttributesRestore(this));
	}

	thickOpenEdges = thick;

	CheckDlgButton(hParams,IDC_THICKSEAM,FALSE);
	CheckDlgButton(hParams,IDC_THINSEAM,FALSE);
	if (thick)
		CheckDlgButton(hParams,IDC_THICKSEAM,TRUE);
	else CheckDlgButton(hParams,IDC_THINSEAM,TRUE);

	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	if (ip) 
		ip->RedrawViews(ip->GetTime());

	InvalidateView();
}

BOOL	UnwrapMod::fnGetViewportOpenEdges()
{
	return viewportOpenEdges;
}
void	UnwrapMod::fnSetViewportOpenEdges(BOOL show)
{

	if (theHold.Holding())
	{
		theHold.Put(new UnwrapSeamAttributesRestore(this));
	}

	viewportOpenEdges = show;
	CheckDlgButton(hParams,IDC_SHOWMAPSEAMS_CHECK,show);
	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	if (ip) 
		ip->RedrawViews(ip->GetTime());
	InvalidateView();
}


Point3 *UnwrapMod::fnGetOpenEdgeColor()
{
	AColor c(openEdgeColor);

	lColor.x = c.r;
	lColor.y = c.g;
	lColor.z = c.b;
	return &lColor;
}
void UnwrapMod::fnSetOpenEdgeColor(Point3 color)
{
	AColor c;
	c.r = color.x;
	c.g = color.y;
	c.b = color.z;
	openEdgeColor = c.toRGB();
	InvalidateView();
}

BOOL	 UnwrapMod::fnGetDisplayHiddenEdges()
{
	return displayHiddenEdges;
}
void	 UnwrapMod::fnSetDisplayHiddenEdges(BOOL hiddenEdgeDisplay)
{
	displayHiddenEdges = hiddenEdgeDisplay;
	InvalidateView();
}



Point3 *UnwrapMod::fnGetHandleColor()
{
	AColor c(handleColor);

	lColor.x = c.r;
	lColor.y = c.g;
	lColor.z = c.b;
	return &lColor;
}
void UnwrapMod::fnSetHandleColor(Point3 color)
{
	AColor c;
	c.r = color.x;
	c.g = color.y;
	c.b = color.z;
	handleColor = c.toRGB();
	InvalidateView();
}


BOOL	UnwrapMod::fnGetFreeFormMode()
{
	if (mode ==ID_FREEFORMMODE)
		return TRUE; 
	else return FALSE; 

}
void	UnwrapMod::fnSetFreeFormMode(BOOL freeFormMode)
{
	if (freeFormMode)
	{
		SetMode(ID_FREEFORMMODE);
	}
}


Point3 *UnwrapMod::fnGetFreeFormColor()
{
	AColor c(freeFormColor);

	lColor.x = c.r;
	lColor.y = c.g;
	lColor.z = c.b;
	return &lColor;
}
void UnwrapMod::fnSetFreeFormColor(Point3 color)
{
	AColor c;
	c.r = color.x;
	c.g = color.y;
	c.b = color.z;
	freeFormColor = c.toRGB();
	InvalidateView();
}

void	UnwrapMod::fnSnapPivot(int pos)
{
	if (!theHold.IsSuspended())
	{
		theHold.Begin();
		theHold.Put(new UnwrapPivotRestore(this));
		theHold.Accept(GetString(IDS_PW_PIVOTRESTORE));
	}

	//snap to center
	if (pos == 1)
		freeFormPivotOffset = Point3(0.0f,0.0f,0.0f);
	else if (pos == 2)
		freeFormPivotOffset = selCenter - freeFormCorners[3];
	else if (pos == 3)
		freeFormPivotOffset = selCenter - freeFormEdges[2];
	else if (pos == 4)
		freeFormPivotOffset = selCenter - freeFormCorners[2];
	else if (pos == 5)
		freeFormPivotOffset = selCenter - freeFormEdges[3];
	else if (pos == 6)
		freeFormPivotOffset = selCenter - freeFormCorners[0];
	else if (pos == 7)
		freeFormPivotOffset = selCenter - freeFormEdges[0];
	else if (pos == 8)
		freeFormPivotOffset = selCenter - freeFormCorners[1];
	else if (pos == 9)
		freeFormPivotOffset = selCenter - freeFormEdges[1];

	InvalidateView();

}

Point3*	UnwrapMod::fnGetPivotOffset()
{
	return &freeFormPivotOffset;
}
void	UnwrapMod::fnSetPivotOffset(Point3 offset)
{
	freeFormPivotOffset = offset;
	InvalidateView();
}
Point3*	UnwrapMod::fnGetSelCenter()
{
	RebuildFreeFormData();
	return &selCenter;
}

BOOL	UnwrapMod::fnGetResetPivotOnSel()
{
	return resetPivotOnSel;
}
void	UnwrapMod::fnSetResetPivotOnSel(BOOL reset)
{
	resetPivotOnSel = reset;
}

BOOL	UnwrapMod::fnGetAllowSelectionInsideGizmo()
{
	return allowSelectionInsideGizmo;
}
void	UnwrapMod::fnSetAllowSelectionInsideGizmo(BOOL select)
{
	allowSelectionInsideGizmo = select;
}


void	UnwrapMod::fnSetSharedColor(Point3 color)
{
	AColor c;
	c.r = color.x;
	c.g = color.y;
	c.b = color.z;
	sharedColor = c.toRGB();
	InvalidateView();
}

Point3*	UnwrapMod::fnGetSharedColor()
{
	AColor c(sharedColor);

	lColor.x = c.r;
	lColor.y = c.g;
	lColor.z = c.b;
	return &lColor;
}

BOOL	UnwrapMod::fnGetShowShared()
{
	return showShared;
}

void	UnwrapMod::fnSetShowShared(BOOL share)
{
	showShared = share;
	InvalidateView();
}

void	UnwrapMod::fnShowIcon(int id,BOOL show)
{
	if ((id > 0) && (id < 30))
		showIconList.Set(id,show);
}


Point3* UnwrapMod::fnGetBackgroundColor()
{
	AColor c(backgroundColor);

	lColor.x = c.r;
	lColor.y = c.g;
	lColor.z = c.b;
	return &lColor;
}

void UnwrapMod::fnSetBackgroundColor(Point3 color)
{
	AColor c;
	c.r = color.x;
	c.g = color.y;
	c.b = color.z;
	backgroundColor = c.toRGB();
	if (iBuf) iBuf->SetBkColor(backgroundColor);
	if (iTileBuf) iTileBuf->SetBkColor(backgroundColor);
	ColorMan()->SetColor(BACKGROUNDCOLORID,  backgroundColor);
	InvalidateView();
}

void	UnwrapMod::fnUpdateMenuBar()
{
	if ( (ip) &&(hDialogWnd) )
	{
		IMenuBarContext* pContext = (IMenuBarContext*) GetCOREInterface()->GetMenuManager()->GetContext(kUnwrapMenuBar);
		if (pContext)
			pContext->UpdateWindowsMenu();

	}
}

BOOL	UnwrapMod::fnGetBrightCenterTile()
{
	return brightCenterTile;
}

void	UnwrapMod::fnSetBrightCenterTile(BOOL bright)
{
	brightCenterTile = bright;
	tileValid = FALSE;
	InvalidateView();
}

BOOL	UnwrapMod::fnGetBlendToBack()
{
	return blendTileToBackGround;
}

void	UnwrapMod::fnSetBlendToBack(BOOL blend)
{
	blendTileToBackGround = blend;
	tileValid = FALSE;
	InvalidateView();
}

int		UnwrapMod::fnGetTickSize()
{
	return tickSize;
}
void	UnwrapMod::fnSetTickSize(int size)
{
	tickSize = size;
	if (tickSize <1 ) tickSize = 1;
	InvalidateView();
}


//new


float	UnwrapMod::fnGetGridSize()
{
	return gridSize;
}
void	UnwrapMod::fnSetGridSize(float size)
{
	gridSize = size;
	InvalidateView();
}

BOOL	UnwrapMod::fnGetGridSnap()
{
	return gridSnap;
}
void	UnwrapMod::fnSetGridSnap(BOOL snap)
{
	gridSnap = snap;

	if (gridSnap)
		pixelSnap = FALSE;
	mUIManager.UpdateCheckButtons();

}
BOOL	UnwrapMod::fnGetGridVisible()
{
	return gridVisible;

}
void	UnwrapMod::fnSetGridVisible(BOOL visible)
{
	gridVisible = visible;
	InvalidateView();
}

void	UnwrapMod::fnSetGridColor(Point3 color)
{
	AColor c;
	c.r = color.x;
	c.g = color.y;
	c.b = color.z;
	gridColor = c.toRGB();
	if (gridVisible) InvalidateView();
}

Point3*	UnwrapMod::fnGetGridColor()
{
	AColor c(gridColor);

	lColor.x = c.r;
	lColor.y = c.g;
	lColor.z = c.b;
	return &lColor;
}

float	UnwrapMod::fnGetGridStr()
{

	return gridStr * 2.0f;
}
void	UnwrapMod::fnSetGridStr(float str)
{

	str = str *0.5f;

	if (str < 0.0f) str = 0.0f;
	if (str > 0.5f) str = 0.5f;

	gridStr = str;
}


BOOL	UnwrapMod::fnGetAutoMap()
{
	return autoMap;

}
void	UnwrapMod::fnSetAutoMap(BOOL autoMap)
{
	this->autoMap = autoMap;
}




float	UnwrapMod::fnGetFlattenAngle()
{
	return flattenAngleThreshold;
}
void	UnwrapMod::fnSetFlattenAngle(float angle)
{
	if (loadDefaults)
	{
		fnLoadDefaults();
		loadDefaults = FALSE;
	}

	if (angle < 0.0f) angle = 0.0f;
	if (angle > 180.0f) angle = 180.0f;

	flattenAngleThreshold = angle;

}

float	UnwrapMod::fnGetFlattenSpacing()
{
	return flattenSpacing;
}
void	UnwrapMod::fnSetFlattenSpacing(float spacing)
{
	if (loadDefaults)
	{
		fnLoadDefaults();
		loadDefaults = FALSE;
	}

	if (spacing < 0.0f) spacing = 0.0f;
	if (spacing > 1.0f) spacing = 1.0f;

	flattenSpacing = spacing;

}

BOOL	UnwrapMod::fnGetFlattenNormalize()
{
	return flattenNormalize;
}
void	UnwrapMod::fnSetFlattenNormalize(BOOL normalize)
{
	if (loadDefaults)
	{
		fnLoadDefaults();
		loadDefaults = FALSE;
	}
	flattenNormalize = normalize;
}

BOOL	UnwrapMod::fnGetFlattenRotate()
{
	return flattenRotate;
}
void	UnwrapMod::fnSetFlattenRotate(BOOL rotate)
{
	if (loadDefaults)
	{
		fnLoadDefaults();
		loadDefaults = FALSE;
	}
	flattenRotate = rotate;
}

BOOL	UnwrapMod::fnGetFlattenFillHoles()
{
	return flattenCollapse;
}
void	UnwrapMod::fnSetFlattenFillHoles(BOOL fillHoles)
{
	if (loadDefaults)
	{
		fnLoadDefaults();
		loadDefaults = FALSE;
	}
	flattenCollapse = fillHoles;
}




BOOL	UnwrapMod::fnGetPreventFlattening()
{
	return preventFlattening;
}
void	UnwrapMod::fnSetPreventFlattening(BOOL preventFlattening)
{

	if (loadDefaults)
	{
		fnLoadDefaults();
		loadDefaults = FALSE;
	}

	if (theHold.Holding())
	{
		theHold.Put(new UnwrapSeamAttributesRestore(this));
	}

	this->preventFlattening = preventFlattening;

	if ( (ip) &&(hDialogWnd) )
	{	
		IMenuBarContext* pContext = (IMenuBarContext*) GetCOREInterface()->GetMenuManager()->GetContext(kUnwrapMenuBar);
		if (pContext)
			pContext->UpdateWindowsMenu();
		//update UI

	}

	CheckDlgButton(hParams,IDC_DONOTREFLATTEN_CHECK,preventFlattening);


}


BOOL	UnwrapMod::fnGetEnableSoftSelection()
{
	return enableSoftSelection;
}

void	UnwrapMod::fnSetEnableSoftSelection(BOOL enable)
{
	enableSoftSelection = enable;
	RebuildDistCache();
	InvalidateView();

}


BOOL	UnwrapMod::fnGetApplyToWholeObject()
{
	return applyToWholeObject;
}

void	UnwrapMod::fnSetApplyToWholeObject(BOOL whole)
{
	applyToWholeObject = whole;

}






//5.1.05
BOOL	UnwrapMod::fnGetAutoBackground()
{
	return this->autoBackground;
}
void	UnwrapMod::fnSetAutoBackground(BOOL autoBackground)
{
	this->autoBackground = autoBackground;
}

BOOL	UnwrapMod::fnGetRelativeTypeInMode()
{
	if (absoluteTypeIn)
		return FALSE; 
	else return TRUE;
}

void	UnwrapMod::SetAbsoluteTypeInMode(BOOL absolute)
{
	absoluteTypeIn = absolute;
	typeInsValid = FALSE;
	SetupTypeins();
}

void	UnwrapMod::fnSetRelativeTypeInMode(BOOL relative)
{
	if (relative)
		absoluteTypeIn = FALSE;
	else absoluteTypeIn = TRUE;
	typeInsValid = FALSE;
	SetupTypeins();
	mUIManager.UpdateCheckButtons();
}


void	UnwrapMod::fnAddMap(Texmap *map)
{
	this->AddMaterial(map);
}

extern float AreaOfTriangle(Point3 a, Point3 b, Point3 c);
extern float AreaOfPolygon(Tab<Point3> &points);


void UnwrapMod::GetArea(BitArray *faceSelection, 
						  float &x, float &y,
						  float &width, float &height,
						  float &uvArea, float &geomArea,
						  MeshTopoData *ld)
{
	Box3 bounds;
	bounds.Init();
	uvArea = 0.0f;
	geomArea = 0.0f;

		if (ld)
		{
			for (int i = 0; i < ld->GetNumberFaces(); i++)//TVMaps.f.Count(); i++)
			{
				if ( i < faceSelection->GetSize() && !ld->GetFaceDead(i))
				{
					if ((*faceSelection)[i])
					{
						int ct = ld->GetFaceDegree(i);//TVMaps.f[i]->count;

						Tab<Point3> plistGeom;
						plistGeom.SetCount(ct);
						Tab<Point3> plistUVW;
						plistUVW.SetCount(ct);

						for (int j = 0; j < ct; j++)
						{
							int index = ld->GetFaceTVVert(i,j);//TVMaps.f[i]->t[j];
							plistUVW[j] = ld->GetTVVert(index);//TVMaps.v[index].p;
							bounds += plistUVW[j];

							index = ld->GetFaceGeomVert(i,j);//TVMaps.f[i]->v[j];
							plistGeom[j] = ld->GetGeomVert(index);//TVMaps.geomPoints[index];
						}
						if (ct == 3)
						{
							geomArea += AreaOfTriangle(plistGeom[0],plistGeom[1],plistGeom[2]);
							uvArea += AreaOfTriangle(plistUVW[0],plistUVW[1],plistUVW[2]);
						}
						else
						{
							geomArea += AreaOfPolygon(plistGeom);
							uvArea += AreaOfPolygon(plistUVW);
						}

					}
				}
			}
			x = bounds.pmin.x;
			y = bounds.pmin.y;
			width = bounds.pmax.x - bounds.pmin.x;
			height = bounds.pmax.y - bounds.pmin.y;

		}
	

}

void UnwrapMod::fnGetArea(BitArray *faceSelection, 
						  float &x, float &y,
						  float &width, float &height,
						  float &uvArea, float &geomArea)
{
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		GetArea(faceSelection,x,y,width, height, uvArea, geomArea,ld);

	}
}

void UnwrapMod::fnGetArea(BitArray *faceSelection, 
						  float &uvArea, float &geomArea, INode *node)
{
	MeshTopoData *ld = GetMeshTopoData(node);
	float x,y,width,height;
	GetArea(faceSelection,x,y,width, height, uvArea, geomArea, ld);
}

void UnwrapMod::fnGetBounds(BitArray *faceSelection, 
						  float &x, float &y,
						  float &width, float &height,
						  INode *node)
{
	MeshTopoData *ld = GetMeshTopoData(node);
	float uvArea,geomArea;
	GetArea(faceSelection,x,y,width, height, uvArea, geomArea, ld);
}

BOOL UnwrapMod::fnGetRotationsRespectAspect()
{
	return rotationsRespectAspect;
}
void UnwrapMod::fnSetRotationsRespectAspect(BOOL respect)
{
	rotationsRespectAspect = respect;
}


BOOL UnwrapMod::fnGetPeltEditSeamsMode()
{
	return peltData.GetEditSeamsMode();
}
void UnwrapMod::fnSetPeltEditSeamsMode(BOOL mode)
{

	peltData.SetEditSeamsMode(this,mode);
}


BOOL UnwrapMod::GetPeltMapMode()
{
	return peltData.GetPeltMapMode();
}
void UnwrapMod::SetPeltMapMode(BOOL mode)
{
	peltData.SetPeltMapMode(this,mode);

	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	if (ip) ip->RedrawViews(ip->GetTime());
}

BOOL UnwrapMod::fnGetPeltPointToPointSeamsMode()
{
	return peltData.GetPointToPointSeamsMode();
}
void UnwrapMod::fnSetPeltPointToPointSeamsMode(BOOL mode)
{
	peltData.SetPointToPointSeamsMode(this, mode,TRUE);
	//	peltData.SetPeltMapMode(mode);
}

void UnwrapMod::fnPeltExpandSelectionToSeams()
{
	theHold.Begin();
	HoldSelection();
	theHold.Accept(GetString(IDS_PELT_EXPANDSELTOSEAM));

	peltData.ExpandSelectionToSeams(this);
	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	if (ip) ip->RedrawViews(ip->GetTime());
}

void UnwrapMod::fnPeltDialogResetRig()
{
	peltData.ResetRig(this);
}

void UnwrapMod::fnPeltDialogSelectRig()
{
	theHold.Begin();
	HoldSelection();
	theHold.Accept(GetString(IDS_PELTDIALOG_SELECTRIG));
	SHORT iret = GetAsyncKeyState (VK_CONTROL);
	if (iret==-32767)
		peltData.SelectRig(this,FALSE);
	else peltData.SelectRig(this,TRUE);
}

void UnwrapMod::fnPeltDialogSelectPelt()
{
	theHold.Begin();
	HoldSelection();
	theHold.Accept(GetString(IDS_PELTDIALOG_SELECTPELT));
	SHORT iret = GetAsyncKeyState (VK_CONTROL);
	if (iret==-32767)
		peltData.SelectPelt(this,FALSE);
	else peltData.SelectPelt(this,TRUE);
}

void UnwrapMod::fnPeltDialogSnapRig()
{
	theHold.Begin();
	HoldPoints();	
	theHold.Accept(GetString(IDS_PELTDIALOG_SNAPRIG));
	peltData.SnapRig(this);

}


BOOL UnwrapMod::fnGetPeltDialogStraightenSeamsMode()
{
	return peltData.peltDialog.GetStraightenMode();
}
void UnwrapMod::fnSetPeltDialogStraightenSeamsMode(BOOL mode)
{
	peltData.peltDialog.SetStraightenMode(mode);
}

void UnwrapMod::fnPeltDialogMirrorRig()
{
	theHold.Begin();
	HoldPoints();	
	theHold.Accept(GetString(IDS_PELTDIALOG_MIRRORRIG));

	peltData.MirrorRig(this);
}


void UnwrapMod::fnPeltDialogRun()
{
	theHold.Begin();
	HoldPoints();
	theHold.Put (new UnwrapPeltVertVelocityRestore (this));
	peltData.Run(this);
	theHold.Accept(GetString(IDS_PELTDIALOG_RUN));

}
void UnwrapMod::fnPeltDialogRelax1()
{
	theHold.Begin();
	HoldPoints();
	theHold.Put (new UnwrapPeltVertVelocityRestore (this));
	peltData.RunRelax(this,0);
	theHold.Accept(GetString(IDS_PELTDIALOG_RELAX1));

}
void UnwrapMod::fnPeltDialogRelax2()
{
	theHold.Begin();
	HoldPoints();
	theHold.Put (new UnwrapPeltVertVelocityRestore (this));
	peltData.RunRelax(this,1);
	theHold.Accept(GetString(IDS_PELTDIALOG_RELAX2));

}

BitArray* UnwrapMod::fnGetSeamSelection(INode *node)
{

		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			return &ld->mSeamEdges;
		}
	return NULL;

}

BitArray* UnwrapMod::fnGetSeamSelection()
{
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			return &ld->mSeamEdges;
		}
	}
	return NULL;
	
}

void UnwrapMod::fnSetSeamSelection(BitArray *sel, INode *node)
{


		MeshTopoData *ld = GetMeshTopoData(node);
		if (ld)
		{
			ld->mSeamEdges.ClearAll();
			for (int i = 0 ; i < (*sel).GetSize(); i++)
			{
				if ((i < ld->mSeamEdges.GetSize()) && ((*sel)[i]))
					ld->mSeamEdges.Set(i,TRUE);
			}
			NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
			if (ip) ip->RedrawViews(ip->GetTime());
		}

}

void UnwrapMod::fnSetSeamSelection(BitArray *sel)
{

	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];
		if (ld)
		{
			ld->mSeamEdges.ClearAll();
			for (int i = 0 ; i < (*sel).GetSize(); i++)
			{
				if ((i < ld->mSeamEdges.GetSize()) && ((*sel)[i]))
					ld->mSeamEdges.Set(i,TRUE);
			}
			NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
			if (ip) ip->RedrawViews(ip->GetTime());
		}
	}

}


void UnwrapMod::fnPeltDialogStraighten(int a, int b)
{
	peltData.StraightenSeam(this,a,b);

	InvalidateView();
	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	if (ip) ip->RedrawViews(ip->GetTime());
}

int UnwrapMod::fnGetPeltDialogFrames()
{
	return peltData.GetFrames();
}
void UnwrapMod::fnSetPeltDialogFrames(int frames)
{
	peltData.SetFrames(frames);

}

int UnwrapMod::fnGetPeltDialogSamples()
{
	return peltData.GetSamples();
}
void UnwrapMod::fnSetPeltDialogSamples(int samples)
{
	peltData.SetSamples(samples);
	peltData.peltDialog.UpdateSpinner(IDC_PELT_SAMPLESSPIN, samples);
}

float UnwrapMod::fnGetPeltDialogRigStrength()
{
	return peltData.GetRigStrength();
}
void UnwrapMod::fnSetPeltDialogRigStrength(float strength)
{
	peltData.SetRigStrength(strength);
	peltData.peltDialog.UpdateSpinner(IDC_PELT_RIGSTRENGTHSPIN, strength);
}

float UnwrapMod::fnGetPeltDialogStiffness()
{
	return peltData.GetStiffness();
}
void UnwrapMod::fnSetPeltDialogStiffness(float stiffness)
{
	peltData.SetStiffness(stiffness);
	peltData.peltDialog.UpdateSpinner(IDC_PELT_STIFFNESSSPIN, stiffness);
}

float UnwrapMod::fnGetPeltDialogDampening()
{
	return peltData.GetDampening();
}
void UnwrapMod::fnSetPeltDialogDampening(float dampening)
{
	peltData.SetDampening(dampening);
	peltData.peltDialog.UpdateSpinner(IDC_PELT_DAMPENINGSPIN, dampening);
}

float UnwrapMod::fnGetPeltDialogDecay()
{
	return peltData.GetDecay();
}
void UnwrapMod::fnSetPeltDialogDecay(float decay)
{
	peltData.SetDecay(decay);
	peltData.peltDialog.UpdateSpinner(IDC_PELT_DECAYSPIN, decay);
}

float UnwrapMod::fnGetPeltDialogMirrorAxis()
{
	return peltData.GetMirrorAngle();
}
void UnwrapMod::fnSetPeltDialogMirrorAxis(float axis)
{
	peltData.SetMirrorAngle(axis*PI/180.0f);
	peltData.peltDialog.UpdateSpinner(IDC_PELT_MIRRORAXISSPIN, axis);
	InvalidateView();

}

int UnwrapMod::fnGetMapMode()
{
	return mapMapMode;
}
void UnwrapMod::fnSetMapMode(int mode)
{
	//turn off the old mode

	if (mapMapMode == PELTMAP)
	{
		SetPeltMapMode(FALSE);
	}

	if (mapMapMode == SPLINEMAP)
	{
		peltData.EnablePeltButtons(this,hMapParams, TRUE);
		fnSplineMap_EndMapMode();
	}


	if (mapMapMode == UNFOLDMAP)
	{
		fnRegularMapEnd();
	}

	if (mapMapMode == LSCMMAP) //turn off the LSCM mapping
	{
		fnLSCMInteractive(FALSE);
	}




	


	if ((mode == mapMapMode) || (mode == NOMAP))
	{
		mapMapMode = NOMAP;
		EnableAlignButtons(FALSE);
		if (ip->GetSubObjectLevel() == 3)
			ip->SetSubObjectLevel(3,TRUE);
		mUIManager.UpdateCheckButtons();
		return;
	}
	else
	{

		fnSetTweakMode(FALSE);
		mapMapMode = mode;
		//press the buttons

		if (mapMapMode == PELTMAP)
			SetPeltMapMode(TRUE);
		if (mapMapMode == SPLINEMAP)
			fnSplineMap_StartMapMode();
		if (mapMapMode == UNFOLDMAP)
			fnRegularMapStart(mMeshTopoData.GetNode(0), TRUE);
		if (mapMapMode == LSCMMAP) //turn on the LSCM mapping
			fnLSCMInteractive(TRUE);
			
		if ((fnGetMapMode() == PLANARMAP) || (fnGetMapMode() == CYLINDRICALMAP) || (fnGetMapMode() == SPHERICALMAP) || (fnGetMapMode() == BOXMAP))
		{
			fnGizmoCenter();
			fnAlignAndFit(3);
			ApplyGizmo();
		}

		if (mapMapMode == SPLINEMAP) 
		{
			ip->SetSubObjectLevel(3,TRUE);
			EnableAlignButtons(FALSE);
			peltData.EnablePeltButtons(this,hMapParams, FALSE);
			peltData.SetPointToPointSeamsMode(this,FALSE,TRUE);
			peltData.SetEditSeamsMode(this,FALSE);

		}
		else if (mapMapMode == UNFOLDMAP) 
		{
			EnableAlignButtons(FALSE);
		}
		else if (mapMapMode == LSCMMAP)   //thse can be on when doing LSCM
		{
			EnableAlignButtons(FALSE);
		}
		else if (mapMapMode != PELTMAP) 
		{
			ip->SetSubObjectLevel(3,TRUE);
			EnableAlignButtons(TRUE);
		}
		mUIManager.UpdateCheckButtons();
	}

}
void UnwrapMod::fnQMap()
{
	ClearAFlag(A_HELD);
	ApplyQMap();
}

void UnwrapMod::ApplyQMap()
{
	int holdMap = mapMapMode;
	mapMapMode = PLANARMAP;
	TimeValue t = GetCOREInterface()->GetTime();
	HoldPointsAndFaces();

	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		Matrix3 tm(1);
		Matrix3 gizmoTM = mMapGizmoTM;
		if (!fnGetNormalizeMap())
		{
			for (int i = 0; i < 3; i++)
			{
				Point3 vec = gizmoTM.GetRow(i);
				vec = Normalize(vec);
				gizmoTM.SetRow(i,vec);
			}
		}
		tm = mMeshTopoData.GetNodeTM(t,ldID)* Inverse(gizmoTM);
		MeshTopoData *ld = mMeshTopoData[ldID];
		ld->ApplyMap(PLANARMAP, fnGetNormalizeMap(), tm, this);		
	}
	mapMapMode = holdMap;
	NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	if (ip) 
		ip->RedrawViews(ip->GetTime());
	InvalidateView();
}



void UnwrapMod::fnSetGizmoTM(Matrix3 tm)
{
	SetXFormPacket pckt(tm);
	TimeValue t = GetCOREInterface()->GetTime();
	tmControl->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
	if ((fnGetMapMode() == PLANARMAP) || (fnGetMapMode() == CYLINDRICALMAP) || (fnGetMapMode() == SPHERICALMAP) || (fnGetMapMode() == BOXMAP))
		ApplyGizmo();
}

Matrix3* UnwrapMod::fnGetGizmoTM()
{
	TimeValue t = GetCOREInterface()->GetTime();

	mGizmoTM.IdentityMatrix();
	tmControl->GetValue(t,&mGizmoTM,FOREVER,CTRL_RELATIVE);
	return &mGizmoTM;
}


void UnwrapMod::fnSetNormalizeMap(BOOL normalize)
{
	if (theHold.Holding())
	{
		theHold.Put(new UnwrapMapAttributesRestore(this));
	}
	normalizeMap = normalize;
	CheckDlgButton(hMapParams,IDC_NORMALIZEMAP_CHECK2,normalizeMap);

	if ((fnGetMapMode() == PLANARMAP) || (fnGetMapMode() == CYLINDRICALMAP) || (fnGetMapMode() == SPHERICALMAP) || (fnGetMapMode() == BOXMAP))
	{
		theHold.Begin();
		ApplyGizmo();

		theHold.Accept(GetString(IDS_MAPPING_ALIGN));

		NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
		if (ip) ip->RedrawViews(ip->GetTime());
		InvalidateView();


	}


}
BOOL UnwrapMod::fnGetNormalizeMap()
{
	return normalizeMap;
}


void UnwrapMod::fnSetShowEdgeDistortion(BOOL show)
{
	showEdgeDistortion = show;
	if (show)
		BuildEdgeDistortionData();
	InvalidateView();
	NotifyDependents(FOREVER,PART_GEOM,REFMSG_CHANGE);
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());

}
BOOL UnwrapMod::fnGetShowEdgeDistortion()
{
	return showEdgeDistortion;
}

void UnwrapMod::fnSetLockSpringEdges(BOOL lock)
{
	lockSpringEdges = lock;
}
BOOL UnwrapMod::fnGetLockSpringEdges()
{
	return lockSpringEdges;
}


void UnwrapMod::fnSetEdgeDistortionScale(float scale)
{
	edgeDistortionScale = scale;
	if (edgeDistortionScale <= 0.0f) edgeDistortionScale = 0.0001f;
	InvalidateView();
}
float UnwrapMod::fnGetEdgeDistortionScale()
{
	return edgeDistortionScale;
}

void UnwrapMod::fnSetShowCounter(BOOL show)
{
	showCounter = show;
	InvalidateView();
}
BOOL UnwrapMod::fnGetShowCounter()
{
	return showCounter;
}


void UnwrapMod::fnSetShowMapSeams(BOOL show)
{
	fnSetViewportOpenEdges(show);
	CheckDlgButton(hParams,IDC_SHOWMAPSEAMS_CHECK,show);

}
BOOL UnwrapMod::fnGetShowMapSeams()
{
	return fnGetViewportOpenEdges();
}

BOOL UnwrapMod::fnIsMesh()
{
	if (mMeshTopoData.Count())
	{
		MeshTopoData *ld = mMeshTopoData[0];		
		if (ld)
		{
			if (ld->GetMesh())
				return TRUE;
			else return FALSE;
		}
	}
	return TRUE;

}


int	UnwrapMod::fnGetSG()
{
//	if (iSG)
//		return iSG->GetIVal();

	return mUIManager.GetSpinIValue(ID_SMGRPSPIN);
}

void UnwrapMod::fnSetSG(int sg)
{
	mUIManager.SetSpinIValue(ID_SMGRPSPIN,sg);
//	if (iSG)
//		iSG->SetValue(sg,FALSE);
}


int		UnwrapMod::fnGetMatIDSelect()
{
//	if (iSG)
//		return iMatID->GetIVal();
	return mUIManager.GetSpinIValue(ID_MATIDSPIN);;
}

void	UnwrapMod::fnSetMatIDSelect(int matID)
{
	mUIManager.SetSpinIValue(ID_MATIDSPIN,matID);
//	if (iMatID)
//		iMatID->SetValue(matID,FALSE);

}


BOOL UnwrapMod::GetIsMeshUp()
{
	//first see if we have a selection if not use the whole mesh
	BOOL hasSelection = FALSE;
	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		MeshTopoData *ld = mMeshTopoData[i];
		if (ld->GetFaceSelection().NumberSet())
			hasSelection = TRUE;
	}
	TimeValue t = GetCOREInterface()->GetTime();

	float sideArea = 0.0f;
	float upArea = 0.0f;
	//loop through our meshes
	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		
		MeshTopoData *ld = mMeshTopoData[i];
		INode *node = GetMeshTopoDataNode(i);
		Matrix3 tm = node->GetObjectTM(t);
		BitArray upFaces;
		BitArray sideFaces;

		int numFaces = ld->GetNumberFaces();
		upFaces.SetSize(numFaces);
		upFaces.ClearAll();
		sideFaces.SetSize(numFaces);
		sideFaces.ClearAll();
		BitArray faceSel = ld->GetFaceSelection();
		if (!hasSelection)
			faceSel.SetAll();

		//loop through our faces
		for (int j = 0; j < numFaces; j++)
		{
			if (faceSel[j])
			{
				Point3 norm = ld->GetGeomFaceNormal(j);
				norm = VectorTransform(tm,norm);
				//if the vector is pointing up or down add it to our up list
				if ((norm.z > 0.5) || (norm.z < -0.5f))			
				{
					upFaces.Set(j,TRUE);
				}
				//otherwise add it to our sideways list
				else
				{
					sideFaces.Set(j,TRUE);
				}
			}
		}

		float x,y,width,height;
		float uvArea,geomArea;
		geomArea = 0.0f;
		uvArea = 0.0f;
		x = 0.0f;
		y = 0.0f;
		width = 0.0f;
		height = 0.0f;
		//compute our up area and store it off
		GetArea(&upFaces,x,y,width, height, uvArea, geomArea, ld);
		upArea += geomArea;
		//compute our side area and store it off
		geomArea = 0.0f;
		uvArea = 0.0f;
		GetArea(&sideFaces,x,y,width, height, uvArea, geomArea, ld);
		sideArea += geomArea;
	}

	if (sideArea > upArea)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}

BOOL	UnwrapMod::fnGetTweakMode()
{
	Interface *ip = GetCOREInterface();
	CommandMode *mode = ip->GetCommandMode();

	if (mode == tweakMode)
		return TRUE;
	else
		return FALSE;
}
void	UnwrapMod::fnSetTweakMode(BOOL mode)
{
	Interface *ip = GetCOREInterface();
	CommandMode *tmode = ip->GetCommandMode();
	if (mode)
	{
		if (tmode != tweakMode)
		{
			fnSetMapMode(NOMAP);
			fnSetPeltEditSeamsMode(FALSE);
			ip->SetCommandMode(tweakMode);
		}
	}
	else
	{
		ip->SetStdCommandMode  ( CID_OBJSELECT ) ;
	}
}


void	UnwrapMod::fnUVLoop(int mode)
{
//check if vert or face if so convert to edge
	
//save the old edge
	//loop
	BOOL holding = theHold.Holding();
	if (!holding)
		theHold.Begin();
	HoldSelection();
	if (!holding)
		theHold.Accept(GetString(IDS_DS_SELECT));

	
	theHold.Suspend();
	if (fnGetTVSubMode() == TVVERTMODE)
	{
		for (int i = 0; i < GetMeshTopoDataCount(); i++)
		{
			MeshTopoData *ld = GetMeshTopoData(i);
			BitArray esel;
			ld->GetEdgeSelFromVert(esel,FALSE);
			ld->SetTVEdgeSelection(esel);
		}
		
	}
	else if (fnGetTVSubMode() == TVFACEMODE)
	{
		for (int i = 0; i < GetMeshTopoDataCount(); i++)
		{
			MeshTopoData *ld = GetMeshTopoData(i);
			ld->ClearTVEdgeSelection();
			BitArray fsel = ld->GetFaceSelection();
			BitArray vsel;
			vsel.SetSize(ld->GetNumberTVVerts());
			vsel.ClearAll();
			BitArray esel;
			esel.SetSize(ld->GetNumberTVEdges());
			esel.ClearAll();

			BitArray skipsel;
			skipsel.SetSize(ld->GetNumberTVEdges());
			skipsel.ClearAll();


			for (int i = 0; i < ld->GetNumberTVEdges(); i++)
			{
				if (ld->GetTVEdgeHidden(i))
				{
					skipsel.Set(i,TRUE);
				}
				else
				{
					int ct = ld->GetTVEdgeNumberTVFaces(i);
					bool sel = false;
					bool unsel = false;
					for (int j = 0; j < ct; j++)
					{
						int fid = ld->GetTVEdgeConnectedTVFace(i,j);
						if (fsel[fid])
							sel = true;
						if (!fsel[fid])
							unsel = true;
					}
					if (sel && !unsel)
					{
						skipsel.Set(i,TRUE);
						int tid1 = ld->GetTVEdgeVert(i,0);
						int tid2 = ld->GetTVEdgeVert(i,1);
						vsel.Set(tid1,TRUE);
						vsel.Set(tid2,TRUE);
					}
					else if (sel && unsel)
						esel.Set(i,TRUE);
				}
			}
			for (int i = 0; i < ld->GetNumberTVEdges(); i++)
			{
				if (esel[i] && !skipsel[i])
				{
					int tid1 = ld->GetTVEdgeVert(i,0);
					int tid2 = ld->GetTVEdgeVert(i,1);
					if (!vsel[tid1] && !vsel[tid2])
					{
						esel.Clear(i);
					}
				}
			}
			ld->SetTVEdgeSelection(esel);
		}
	}

	if (mode == 1)
		GrowUVLoop(TRUE);
	else if (mode == -1)
		ShrinkUVLoop();
	else
	{
		SelectUVEdge(TRUE);
	}

	if (fnGetTVSubMode() == TVVERTMODE)
	{
		for (int i = 0; i < GetMeshTopoDataCount(); i++)
		{
			MeshTopoData *ld = GetMeshTopoData(i);
			BitArray vsel;
			ld->GetVertSelFromEdge(vsel);
			ld->SetTVVertSelection(vsel);
		}

	}
	else if (fnGetTVSubMode() == TVFACEMODE)
	{
		theHold.Suspend();
		fnEdgeToFaceSelect();
		theHold.Resume();
		for (int i = 0; i < GetMeshTopoDataCount(); i++)
		{
			MeshTopoData *ld = GetMeshTopoData(i);
			ld->ClearTVEdgeSelection();
			BitArray fsel = ld->GetFaceSelection();
			BitArray vsel;
			vsel.SetSize(ld->GetNumberTVVerts());
			vsel.ClearAll();
			BitArray esel;
			esel.SetSize(ld->GetNumberTVEdges());
			esel.ClearAll();
			for (int i = 0; i < ld->GetNumberTVEdges(); i++)
			{
				if (ld->GetTVEdgeHidden(i))
				{	
					int ct = ld->GetTVEdgeNumberTVFaces(i);
					bool sel = false;
					bool unsel = false;
					for (int j = 0; j < ct; j++)
					{
						int fid = ld->GetTVEdgeConnectedTVFace(i,j);
						if (fsel[fid])
							sel = true;
						if (!fsel[fid])
							unsel = true;
					}
					if (sel && unsel)
					{
						for (int j = 0; j < ct; j++)
						{
							int fid = ld->GetTVEdgeConnectedTVFace(i,j);
							ld->SetFaceSelected(fid,FALSE);
						}

					}
				}
			}

		}

	}


	if (fnGetSyncSelectionMode()) fnSyncGeomSelection();
	InvalidateView();			

	theHold.Resume();
//restore the selections
}
void	UnwrapMod::fnUVRing(int mode)
{
	BOOL holding = theHold.Holding();
	if (!holding)
		theHold.Begin();
	HoldSelection();
	if (!holding)
		theHold.Accept(GetString(IDS_DS_SELECT));

	theHold.Suspend();

	if (fnGetTVSubMode() == TVVERTMODE)
	{
		for (int i = 0; i < GetMeshTopoDataCount(); i++)
		{
			MeshTopoData *ld = GetMeshTopoData(i);
			BitArray esel;
			ld->GetEdgeSelFromVert(esel,FALSE);
			ld->SetTVEdgeSelection(esel);
		}

	}
	else if (fnGetTVSubMode() == TVFACEMODE)
	{
		for (int i = 0; i < GetMeshTopoDataCount(); i++)
		{
			MeshTopoData *ld = GetMeshTopoData(i);
			ld->ClearTVEdgeSelection();
			BitArray fsel = ld->GetFaceSelection();
			BitArray vsel;
			vsel.SetSize(ld->GetNumberTVVerts());
			vsel.ClearAll();
			BitArray esel;
			esel.SetSize(ld->GetNumberTVEdges());
			esel.ClearAll();
			for (int i = 0; i < ld->GetNumberTVEdges(); i++)
			{
				if (!ld->GetTVEdgeHidden(i))
				{
					int ct = ld->GetTVEdgeNumberTVFaces(i);				
					for (int j = 0; j < ct; j++)
					{
						int fid = ld->GetTVEdgeConnectedTVFace(i,j);
						if (fsel[fid])
							esel.Set(i,TRUE);
					}
				}
			}
			//select only ring edges
			Tab<int> edgesAtVertex;
			edgesAtVertex.SetCount(ld->GetNumberTVVerts());
			for (int i = 0; i < ld->GetNumberTVVerts(); i++)
				edgesAtVertex[i] = 0;
			for (int i =0; i < ld->GetNumberTVEdges(); i++)
			{
				if (esel[i])
				{
					int a = ld->GetTVEdgeVert(i, 0);
					int b = ld->GetTVEdgeVert(i, 1);
					edgesAtVertex[a] += 1;
					edgesAtVertex[b] += 1;
				}
			}

			//remove our interior edges
			for (int i =0; i < ld->GetNumberTVEdges(); i++)
			{

				if (esel[i])
				{
					int ct = ld->GetTVEdgeNumberTVFaces(i);
					if ( ct == 2 )
					{
						int a = ld->GetTVEdgeConnectedTVFace(i, 0);
						int b = ld->GetTVEdgeConnectedTVFace(i, 1);
						if (fsel[a] && fsel[b])
						{
							esel.Clear(i);
						}
					}
				}
			}

			BitArray ringVerts;
			ringVerts.SetSize(ld->GetNumberTVVerts());
			ringVerts.ClearAll();
			BitArray ringEdges;
			ringEdges.SetSize(ld->GetNumberTVEdges());
			ringEdges.ClearAll();
			for (int i =0; i < ld->GetNumberTVEdges(); i++)
			{
				if (esel[i])
				{
					int a = ld->GetTVEdgeVert(i, 0);
					int b = ld->GetTVEdgeVert(i, 1);
					if ( (edgesAtVertex[a] == 3) || (edgesAtVertex[b] == 3))
					{
						ringVerts.Set(a,TRUE);
						ringVerts.Set(b,TRUE);
						ringEdges.Set(i,TRUE);
					}
				}
			}

			//remove loop edges between rings
			for (int i =0; i < ld->GetNumberTVEdges(); i++)
			{
				if (esel[i] && (!ringEdges[i]))
				{
					int a = ld->GetTVEdgeVert(i, 0);
					int b = ld->GetTVEdgeVert(i, 1);
					if (ringVerts[a] && ringVerts[b])
					{
						esel.Clear(i);
					}
				}
			}

			ld->SetTVEdgeSelection(esel);
		}
	}

  	if (mode == 1)
		GrowUVRing(false);
	else if (mode == -1)
		ShrinkUVRing();
	else
	{
		GrowUVRing(true);
	}

	if (fnGetTVSubMode() == TVVERTMODE)
	{
		for (int i = 0; i < GetMeshTopoDataCount(); i++)
		{
			MeshTopoData *ld = GetMeshTopoData(i);
			BitArray vsel;
			ld->GetVertSelFromEdge(vsel);
			ld->SetTVVertSelection(vsel);
		}

	}
	else if (fnGetTVSubMode() == TVFACEMODE)
	{
		for (int i = 0; i < GetMeshTopoDataCount(); i++)
		{
			MeshTopoData *ld = GetMeshTopoData(i);
			BitArray vsel;
			ld->GetVertSelFromEdge(vsel);
			ld->SetTVVertSelection(vsel);
			BitArray fsel;
			ld->GetFaceSelFromVert(fsel,FALSE);
			
			//clean up faces that have a hidden edge with on side selected and the other not
			for (int i = 0; i < ld->GetNumberTVEdges(); i++)
			{
				if (ld->GetTVEdgeHidden(i))
				{				
					int ct = ld->GetTVEdgeNumberTVFaces(i);
					bool sel = false;
					bool unsel = false;
					for (int j = 0; j < ct; j++)
					{
						int fid = ld->GetTVEdgeConnectedTVFace(i,j);
						if (fsel[fid])
							sel = true;
						else if (!fsel[fid])
							unsel = true;
					}
					if (sel && unsel)
					{
						for (int j = 0; j < ct; j++)
						{
							int fid = ld->GetTVEdgeConnectedTVFace(i,j);
							fsel.Set(fid,FALSE);
						}
					}
				}
			}
			ld->SetFaceSelection(fsel);
		}
	}

	if (fnGetSyncSelectionMode()) fnSyncGeomSelection();
	InvalidateView();	

	theHold.Resume();

}



void	UnwrapMod::SpaceOrAlign(int align, BOOL horizontal)
{
//if face mode bail
	TimeValue t = GetCOREInterface()->GetTime();
	if  ( (fnGetTVSubMode() == TVFACEMODE) ||
		 (fnGetTVSubMode() == TVOBJECTMODE) )
	{
		return;
	}

//if vertex mode convert vertex selection to edge selection
	if (fnGetTVSubMode() == TVVERTMODE)
	{
		for (int i = 0; i < GetMeshTopoDataCount(); i++)
		{
			MeshTopoData *ld = GetMeshTopoData(i);
			ld->ClearTVEdgeSelection();
			BitArray esel;
			ld->GetEdgeSelFromVert(esel,FALSE);
			ld->SetTVEdgeSelection(esel);
		}
	}

//mark all selected vertices
	else if (fnGetTVSubMode() == TVEDGEMODE)
	{
		for (int i = 0; i < GetMeshTopoDataCount(); i++)
		{
			MeshTopoData *ld = GetMeshTopoData(i);
			ld->ClearTVVertSelection();
			BitArray vsel;
			ld->GetVertSelFromEdge(vsel);
			ld->SetTVVertSelection(vsel);
		}
	}

	for (int i = 0; i < GetMeshTopoDataCount(); i++)
	{
		MeshTopoData *ld = GetMeshTopoData(i);
		Tab<int> numberOfConnectedEdges;
		numberOfConnectedEdges.SetCount(ld->GetNumberTVVerts());
		for (int j = 0; j < numberOfConnectedEdges.Count(); j++)
			numberOfConnectedEdges[j] = 0;

		//build number of edges at each vert
		DWORDTab *vertConnectedToEdge;
		vertConnectedToEdge = new DWORDTab[ld->GetNumberTVVerts()];

		BitArray esel;
		esel = ld->GetTVEdgeSelection();

		for (DWORD j = 0; j < ld->GetNumberTVEdges(); j++)
		{
			if (esel[j])
			{			
				DWORD tid0 = ld->GetTVEdgeVert(j,0);
				DWORD tid1 = ld->GetTVEdgeVert(j,1);
				vertConnectedToEdge[tid0].Append(1,&j,10);
				vertConnectedToEdge[tid1].Append(1,&j,10);
			}
		}
		Tab<DWORD> processEdges;
		BitArray processedVerts;		
		processedVerts.SetSize(ld->GetNumberTVVerts());

		BitArray skipVerts;		
		skipVerts.SetSize(ld->GetNumberTVVerts());
		skipVerts.ClearAll();


		Tab<DWORD> currentVerts;
		Tab<DWORD> newVerts;

		for (DWORD j = 0; j < ld->GetNumberTVVerts(); j++)
		{

			if (vertConnectedToEdge[j].Count() > 0 && !skipVerts[j])
			{
//gather up all the connected verts
				processedVerts.ClearAll();
				processEdges.SetCount(0,FALSE);
				
				bool done = false;
				currentVerts.SetCount(0,FALSE);
				newVerts.SetCount(0,FALSE);;

				currentVerts.Append(1,&j,1000);
				skipVerts.Set(currentVerts[0]);
				while (!done)
				{
					BOOL hit = false;
					newVerts.SetCount(0,FALSE);
					for (int m = 0; m < currentVerts.Count(); m++)
					{
						for (int k = 0; k < vertConnectedToEdge[currentVerts[m]].Count(); k++)
					{
							DWORD eindex =  vertConnectedToEdge[currentVerts[m]][k];
						if (esel[eindex])
						{
							processEdges.Append(1,&eindex);

							DWORD tid0 = ld->GetTVEdgeVert(eindex,0);
							DWORD tid1 = ld->GetTVEdgeVert(eindex,1);

								DWORD nextVert = -1;
								if (tid0 != currentVerts[m])
									nextVert = tid0;
								else if (tid1 != currentVerts[m])
									nextVert = tid1;
								if (!skipVerts[nextVert])
									newVerts.Append(1,&nextVert);
							esel.Clear(eindex);
								skipVerts.Set(currentVerts[m]);								
							hit = true;
						}
					}
					}
					currentVerts = newVerts;
					if (!hit)
						done = true;
				}
				if (processEdges.Count() > 0)
				{
					int i1, i2;
					GetUVWIndices(i1,i2);

					if (align == 0)  //align h/v
					{					
						processedVerts.ClearAll();
						//compute the center
						float center = 0.0f;
						int centerCT = 0;

						if (horizontal)
							i1 = i2;
						for (int k = 0; k < processEdges.Count(); k++)
						{
							for (int m = 0; m < 2; m++)
							{
								int id = ld->GetTVEdgeVert(processEdges[k],m);
								if (!processedVerts[id])
								{
									Point3 p = ld->GetTVVert(id);
									center += p[i1];
									centerCT++;
									processedVerts.Set(id,TRUE);
								}
							}
						}

						if (centerCT)
						{
							center = center/(float)centerCT;
							processedVerts.ClearAll();
							for (int k = 0; k < processEdges.Count(); k++)
							{
								for (int m = 0; m < 2; m++)
								{
									int id = ld->GetTVEdgeVert(processEdges[k],m);
									if (!processedVerts[id])
									{
										Point3 p = ld->GetTVVert(id);
										p[i1] = center;
										ld->SetTVVert(t,id,p,this);
									}
								}
							}
						}
					}
//do space
					else //space or align.space to end points
					{
						processedVerts.ClearAll();
						//compute the center
						float l = 0.0f;
						int lCT = 0;

						if (!horizontal)
							i1 = i2;


						if (processEdges.Count())
						{
							
							processedVerts.ClearAll();
							//find the left most vert/edge
							int leftMostEdge = -1;
							int leftMostVert = -1;
							float leftMostValue = 0.0f;
							Box3 b;
							b.Init();

							Tab<int> vCount;
							vCount.SetCount(ld->GetNumberTVVerts());
							for (int k = 0; k < vCount.Count(); k++)
								vCount[k] = 0;
							for (int k = 0; k < processEdges.Count(); k++)
							{
								for (int m = 0; m < 2; m++)
								{
									int id = ld->GetTVEdgeVert(processEdges[k],m);
									vCount[id] += 1;
								}
							}


								

							for (int k = 0; k < processEdges.Count(); k++)
							{
								for (int m = 0; m < 2; m++)
								{
									int id = ld->GetTVEdgeVert(processEdges[k],m);
									Point3 p = ld->GetTVVert(id);
									if (vCount[id] == 1)
									{
										b += p;
										if ((p[i1] < leftMostValue) || (leftMostEdge==-1))
										{
											leftMostVert = id;
											leftMostValue = p[i1];
											leftMostEdge = k;
										}
									}
								}
							}

							if ((leftMostEdge == -1))
							{
								for (int k = 0; k < processEdges.Count(); k++)
								{
									for (int m = 0; m < 2; m++)
									{
										int id = ld->GetTVEdgeVert(processEdges[k],m);
										Point3 p = ld->GetTVVert(id);
										b += p;
										if ((p[i1] < leftMostValue) || (leftMostEdge==-1))
										{
											leftMostVert = id;
											leftMostValue = p[i1];
											leftMostEdge = k;
										}
									}
								}
							}

							int tempID = processEdges[0];
							processEdges[0] = processEdges[leftMostEdge];
							processEdges[leftMostEdge] = tempID;
							l = (b.Max()-b.Min())[i1]/(float)processEdges.Count();
							

							//start with that edge and work our way to the end
							for (int k = 0; k < processEdges.Count()-2; k++)
							{
								int anchor1 = ld->GetTVEdgeVert(processEdges[k],0);
								int anchor2 = ld->GetTVEdgeVert(processEdges[k],1);
								for (int m = k+1 ; m < processEdges.Count()-1; m++)
								{
									for (int n = 0; n < 2; n++)
									{
										int edgeID = processEdges[m];
										int testID = ld->GetTVEdgeVert(edgeID,n);
										if ((testID == anchor1) || (testID == anchor2))
										{
											int tempID = processEdges[k+1];
											processEdges[k+1] = processEdges[m];
											processEdges[m] = tempID;
											m = processEdges.Count();
											n = 2;
										}
									}
								}
							}
							//process the edges
							//space horizontal/vertical
							if (align == 1)
							{


								for (int k = 0; k < processEdges.Count(); k++)
								{
									int id0 = ld->GetTVEdgeVert(processEdges[k],0);
									int id1 = ld->GetTVEdgeVert(processEdges[k],1);
									int moveVert = id1;
									int baseVert  = id0;
									if (id1 == leftMostVert)
									{
										moveVert = id0;
										baseVert = id1;
									}

									Point3 pm = ld->GetTVVert(moveVert);
									Point3 pb = ld->GetTVVert(baseVert);


									pm[i1] =  pb[i1] + l;
									ld->SetTVVert(t,moveVert,pm,this);
									leftMostVert = moveVert;
								}
							}
							//align/space to end points
							else if (processEdges.Count() >= 2)
							{
								int id0 = ld->GetTVEdgeVert(processEdges[0],0);
								int id1 = ld->GetTVEdgeVert(processEdges[0],1);
								int id2 = ld->GetTVEdgeVert(processEdges[1],0);
								int id3 = ld->GetTVEdgeVert(processEdges[1],1);

								int startID = id0;
								if  ( (id0 == id2)  || (id0 == id3)  )
									startID = id1;

								int ct = processEdges.Count();
								id0 = ld->GetTVEdgeVert(processEdges[ct-2],0);
								id1 = ld->GetTVEdgeVert(processEdges[ct-2],1);
								id2 = ld->GetTVEdgeVert(processEdges[ct-1],0);
								id3 = ld->GetTVEdgeVert(processEdges[ct-1],1);

								int endID = id3;
								if  ( (id3 == id0)  || (id3 == id1)  )
									endID = id2;

								Point3 a = ld->GetTVVert(startID);
								Point3 b = ld->GetTVVert(endID);
								Point3 vec = (b-a);
								vec = vec/(float)ct;
								for (int k = 0; k < processEdges.Count(); k++)
								{
									int id0 = ld->GetTVEdgeVert(processEdges[k],0);
									int id1 = ld->GetTVEdgeVert(processEdges[k],1);
									int moveVert = id1;
									int baseVert  = id0;
									if (id1 == leftMostVert)
									{
										moveVert = id0;
										baseVert = id1;
									}

									Point3 pm = ld->GetTVVert(moveVert);
									Point3 pb = ld->GetTVVert(baseVert);
									a += vec;
									ld->SetTVVert(t,moveVert,a,this);
									leftMostVert = moveVert;
								}

							}
						}
					}
					
				}
			}
		}

		delete [] vertConnectedToEdge;
	}

	
}

void	UnwrapMod::fnAlign(BOOL horizontal)
{
	BOOL holding = theHold.Holding();
	if (!holding)
		theHold.Begin();
	HoldPoints();

	ClearAFlag(A_HELD);
	HoldSelection();

	if (!holding)
		theHold.Accept(GetString(IDS_DS_MOVE));

	theHold.Suspend();
	


	if (GetAsyncKeyState(VK_SHIFT) != 0)
		SelectUVEdge(FALSE);

	SpaceOrAlign(0,horizontal);
	

	if (fnGetSyncSelectionMode()) fnSyncGeomSelection();
	InvalidateView();

	theHold.Resume();
}

void	UnwrapMod::fnSpace(BOOL horizontal)
{
	BOOL holding = theHold.Holding();
	if (!holding)	
		theHold.Begin();
	HoldPoints();
	ClearAFlag(A_HELD);
	HoldSelection();
	if (!holding)
		theHold.Accept(GetString(IDS_DS_MOVE));

	theHold.Suspend();


	if (GetAsyncKeyState(VK_SHIFT) != 0)
		SelectUVEdge(FALSE);

	SpaceOrAlign(1,horizontal);
	

	if (fnGetSyncSelectionMode()) fnSyncGeomSelection();
	InvalidateView();
	theHold.Resume();
}

void    UnwrapMod::fnAlignLinear()
{
	BOOL holding = theHold.Holding();
	if (!holding)	
		theHold.Begin();
	HoldPoints();
	if (!holding)
		theHold.Accept(GetString(IDS_DS_MOVE));

	theHold.Suspend();
	SpaceOrAlign(2,TRUE);


	if (fnGetSyncSelectionMode()) fnSyncGeomSelection();
	InvalidateView();
	theHold.Resume();
}


BOOL UnwrapMod::fnIsPinned(int index, INode *node)
{
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
		return ld->IsTVVertPinned(index);
	return FALSE;
}
void UnwrapMod::fnPin(int index, INode *node)
{
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		ld->TVVertPin(index);
	}

	if (fnGetMapMode() == LSCMMAP)
		UpdateViewAndModifier();

}
void UnwrapMod::fnUnpin(int index, INode *node)
{
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		ld->TVVertUnpin(index);
	}
	if (fnGetMapMode() == LSCMMAP)
		UpdateViewAndModifier();
}

void UnwrapMod::fnPinSelected(INode *node)
{
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		for (int i = 0; i < ld->GetNumberTVVerts(); i++)
		{
			if (ld->GetTVVertSelected(i))
			{
				ld->TVVertPin(i);
			}
		}
	}
	if (fnGetMapMode() == LSCMMAP)
		UpdateViewAndModifier();

}
void UnwrapMod::fnUnpinSelected(INode *node)
{
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld)
	{
		for (int i = 0; i < ld->GetNumberTVVerts(); i++)
		{
			if (ld->GetTVVertSelected(i))
				ld->TVVertUnpin(i);
		}
	}
	if (fnGetMapMode() == LSCMMAP)
		UpdateViewAndModifier();

}

void UnwrapMod::RescaleSelectedCluster()
{
	ClearAFlag(A_HELD);
	theHold.Begin();
	HoldPoints();
	theHold.Accept(GetString(IDS_PW_RESCALECLUSTER));

	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		INode *node = mMeshTopoData.GetNode(ldID);
		BitArray faceSel = ld->GetFaceSelection();
		if (fnGetTVSubMode() == TVVERTMODE)
		{
			ld->GetFaceSelFromVert(faceSel, FALSE);
		}
		else if (fnGetTVSubMode() == TVEDGEMODE)
		{
			BitArray vselTemp = ld->GetTVVertSelection();
			BitArray vsel;
			ld->GetVertSelFromEdge(vsel);
			ld->SetTVVertSelection(vsel);
			ld->GetFaceSelFromVert(faceSel,FALSE);    		  			
			ld->SetTVVertSelection(vselTemp);
		}

		if (faceSel.NumberSet() == 0)
			faceSel.SetAll();


		if (faceSel.NumberSet())
			fnRescaleCluster(&faceSel, node);		
	}

	UpdateViewAndModifier();
	
}

void UnwrapMod::fnRescaleCluster(BitArray *sel, INode *node)
{
	MeshTopoData *ld = GetMeshTopoData(node);
	if (ld && sel)
	{
		Tab<int> clusterIDs;
		clusterIDs.SetCount(ld->GetNumberFaces());
		for (int i = 0; i < clusterIDs.Count(); i++)
		{
			clusterIDs[i] = -1;
		}

		BitArray usedFaces;
		usedFaces.SetSize(ld->GetNumberFaces());
		usedFaces.ClearAll();

		int clustCount = pblock->Count(unwrap_group_name);
		int currentClusterID = 0;


//add in any user defined clusters first
		Tab<int> cluster;
		for (int i = 0; i < clustCount; i++)
		{
			ld->GetToolGroupingData()->GetGroup(i,cluster);
			for (int j = 0; j < cluster.Count(); j++)
			{
				clusterIDs[cluster[j]] = i;
			}
			
		}
		currentClusterID = clustCount;


		BitArray holdFaceSel =  ld->GetFaceSelection();
		
		for (int i = 0; i < sel->GetSize(); i++)
		{
			if ((*sel)[i])
			{

				if ( ( usedFaces[i] == false) && (i < ld->GetNumberFaces()) )
				{
					ld->ClearFaceSelection();
					ld->SetFaceSelected(i,TRUE);
					ld->SelectElement(TVFACEMODE,TRUE);	
					BOOL clusterAdded = FALSE;
					for (int j = 0; j < ld->GetNumberFaces(); j++)
					{
						if (ld->GetFaceSelected(j) && (usedFaces[j] == false))
						{
//only add faces that dont belong to a a cluster
							if (clusterIDs[j] == -1)
							{
							clusterIDs[j] = currentClusterID;
							usedFaces.Set(j);
							clusterAdded = TRUE;
							}
						}
					}
					if (clusterAdded)
						currentClusterID++;
				}
			}
		}

//get our cluster density factor
		Tab<float> clustRescale;
		clustRescale.SetCount(currentClusterID);
		for (int i = 0; i < clustRescale.Count(); i++)
		{
			clustRescale[i] = 1.0f;
			if ( i < clustCount)
				clustRescale[i] = pblock->GetFloat(unwrap_group_density,0,i);
		}

		ld->SetFaceSelection(holdFaceSel);
		ld->RescaleClusters(clusterIDs,clustRescale,this);

	}
}

BOOL UnwrapMod::fnGetPackRescaleCluster()
{
	return packRescaleCluster;
}
void UnwrapMod::fnSetPackRescaleCluster(BOOL rescale)
{
	packRescaleCluster = rescale;
}


void	UnwrapMod::fnWeldAllShared()
{
	//hold tv dataa
	ClearAFlag(A_HELD);
	theHold.Begin();
	HoldPointsAndFaces();	
	theHold.Accept(GetString(IDS_PW_WELDSELECTED));

	//hold our selection
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		ld->HoldSelection();
		ld->BuildVertexClusterList();
	}

	//convert our sub selection type to vertex selection
	TransferSelectionStart();

	float weldThreshold = 1.0E+15;

	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];

		//expand our selection to include all matching
		BitArray geoVerts;
		geoVerts.SetSize(ld->GetNumberGeomVerts());
		geoVerts.ClearAll();
		for (int i = 0; i < ld->GetNumberTVVerts(); i++)
		{
			if (ld->GetTVVertSelected(i))
			{
				int geoIndex = ld->GetTVVertGeoIndex(i);
				if (geoIndex != -1)
					geoVerts.Set(geoIndex);
			}
		}
		for (int i = 0; i < ld->GetNumberTVVerts(); i++)
		{
			int geoIndex = ld->GetTVVertGeoIndex(i);
			if (geoIndex != -1 && geoVerts[geoIndex])
			{
				ld->SetTVVertSelected(i,TRUE);
			}
		}
		ld->WeldSelectedVerts(weldThreshold,this);
	}

	//put back our old vertex selection if need be
	TransferSelectionEnd(FALSE,TRUE);

	//restore our selection
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		ld->RestoreSelection();
	}

	UpdateViewAndModifier();
}
void	UnwrapMod::fnWeldSelectedShared()
{
	//hold tv dataa
	ClearAFlag(A_HELD);
	theHold.Begin();
	HoldPointsAndFaces();	
	theHold.Accept(GetString(IDS_PW_WELDSELECTED));

	//hold our selection
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		ld->HoldSelection();
		ld->BuildVertexClusterList();
	}

	//convert our sub selection type to vertex selection
	TransferSelectionStart();

	float weldThreshold = 1.0E+15;

	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];

		ld->WeldSelectedVerts(weldThreshold,this);
	}

	//put back our old vertex selection if need be
	TransferSelectionEnd(FALSE,TRUE);

	//restore our selection
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		MeshTopoData *ld = mMeshTopoData[ldID];
		ld->RestoreSelection();
	}

	UpdateViewAndModifier();

}


void UnwrapMod::fnRelaxOneClick()
{
	ClearAFlag(A_HELD);
	theHold.Begin();
	HoldPoints();	
	theHold.Accept(GetString(IDS_PW_RELAX));

	bool anythingSelected = AnyThingSelected();
	BOOL applyToAll = TRUE;
	if (anythingSelected)
		applyToAll = FALSE;


	
	BOOL cancel = FALSE;
	double timePassed = 0;
	double timeLimit = 15000;
	float rstretch = 0.0f;
	float rstrength = 0.5f;
	SetCursor(LoadCursor(NULL,IDC_WAIT));
	GetCOREInterface()->ReplacePrompt(GetString(IDS_RELAX_WAIT));
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		int ct = 0;
		
		while (timePassed < timeLimit)
		{
			MaxSDK::PerformanceTools::Timer timer;
			timer.StartTimer();
			mMeshTopoData[ldID]->TransferSelectionStart(fnGetTVSubMode());
			BitArray mVSelTemp = mMeshTopoData[ldID]->GetTVVertSelection();
			if (applyToAll)
			{
				mVSelTemp.ClearAll();
				for (int i = 0; i < mMeshTopoData[ldID]->GetNumberTVVerts(); i++)
				{
				if (mMeshTopoData[ldID]->IsTVVertVisible(i) && (mMeshTopoData[ldID]->GetTVVertDead(i) == FALSE) )
						mVSelTemp.Set(i,TRUE);
				}
			}		
			mMeshTopoData[ldID]->TransferSelectionEnd(fnGetTVSubMode(), TRUE,FALSE);

			if (ct %2 == 0)
				mMeshTopoData[ldID]->RelaxByFaceAngle(fnGetTVSubMode(), 20, rstretch, rstrength, 0, NULL, this,applyToAll);
			else
				mMeshTopoData[ldID]->RelaxByEdgeAngle(fnGetTVSubMode(), 20, rstretch, rstrength, 0, NULL, this,applyToAll);
			if (mMeshTopoData[ldID]->GetUserCancel())
			{
				
				mMeshTopoData[ldID]->SetUserCancel(FALSE);
				ldID = mMeshTopoData.Count();
				cancel = TRUE;
				timePassed = timeLimit;
			}
			else
			{
				ct++;
				for (int i = 0; i < mMeshTopoData.Count(); i++)
				{
					mMeshTopoData[i]->HoldFaceSel();
					mMeshTopoData[i]->ClearFaceSelection();
				}

				SelectOverlap();

				BitArray fsel = mMeshTopoData[ldID]->GetFaceSelection();
				int numberOverLap = fsel.NumberSet();
				BOOL overlap = false;
				if (numberOverLap > 0)
				{					
					for (int i = 0; i < mMeshTopoData[ldID]->GetNumberFaces(); i++)
					{
						if (fsel[i])
						{
							int degree = mMeshTopoData[ldID]->GetFaceDegree(i);
							for (int j = 0; j < degree; j++)
							{
								int tvIndex = mMeshTopoData[ldID]->GetFaceTVVert(i,j);
								if (mVSelTemp[tvIndex])
								{
									overlap = TRUE;
									j = degree;
									i = mMeshTopoData[ldID]->GetNumberFaces();
								}
							}

						}
					}
				}
				if (overlap == FALSE)
				{
					timePassed = timeLimit;
				}

				for (int i = 0; i < mMeshTopoData.Count(); i++)
				{
					mMeshTopoData[i]->RestoreFaceSel();
				}
			}
			
			timePassed += timer.EndTimer();
			if (timePassed > 5000)
			{
				rstretch = 0.15f;
				rstrength = 0.8f;
			}
			else if (timePassed > 10000)
			{
				rstretch = 0.3f;
				rstrength = 1.0f;
			}
			InvalidateView();
			TSTR prompt;
			prompt.printf(_T("%s  %3.2f"),GetString(IDS_RELAX_WAIT),timePassed/1000);
			GetCOREInterface()->ReplacePrompt(prompt);
		}
	}

	if (cancel == FALSE)
	{
		for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
		{
			mMeshTopoData[ldID]->RelaxByEdgeAngle(fnGetTVSubMode(), 300, 0.0, 0.1, 0, NULL, this,applyToAll);
			if (mMeshTopoData[ldID]->GetUserCancel())
			{
				ldID = mMeshTopoData.Count();
				cancel = TRUE;
			}	
		}
	}

	TSTR prompt;
	prompt.printf(_T(" "));
	GetCOREInterface()->ReplacePrompt(prompt);
	
	SetCursor(LoadCursor(NULL,IDC_ARROW));

	UpdateViewAndModifier();

	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		mMeshTopoData[ldID]->SetUserCancel(FALSE);
	}
}

void UnwrapMod::RotateAroundPivot(float angle)
{
	theHold.Begin();
	HoldPoints();	
	theHold.Accept(GetString(IDS_PW_ROTATE_UVW));

//mouse down
	
	center.x =  freeFormPivotScreenSpace.x;
	center.y =  freeFormPivotScreenSpace.y;

	tempCenter.x = center.x;
	tempCenter.y = center.y;
	centeron = TRUE;
	origSelCenter = selCenter;

//mouse move

	int i1,i2;
	GetUVWIndices(i1,i2);
	if ((i1==0) && (i2==2)) angle *= -1.0f;
	//convert our sub selection type to vertex selection
	TransferSelectionStart();

	RotatePoints(hView,angle);

//mouse up

//recompute pivot point
	Box3 bounds;
	bounds.Init();
	for (int ldID = 0; ldID < GetMeshTopoDataCount(); ldID++)
	{
		MeshTopoData *ld = GetMeshTopoData(ldID);
		if (ld==NULL)
		{
			DbgAssert(0);
		}
		else
		{
			int vselCount = ld->GetNumberTVVerts();

			int i1,i2;
			GetUVWIndices(i1,i2);
			for (int i = 0; i < vselCount; i++)
			{
				if (ld->GetTVVertSelected(i))
				{
					//get bounds
					Point3 p = Point3(0.0f,0.0f,0.0f);
					p[i1] = ld->GetTVVert(i)[i1];
					p[i2] = ld->GetTVVert(i)[i2];
					bounds += p;
				}
			}
		}
	}

	Point3 originalPt = (selCenter+freeFormPivotOffset);
	freeFormPivotOffset = originalPt - bounds.Center();

	//convert our sub selection type to current selection
	TransferSelectionEnd(FALSE,FALSE);


}


void    UnwrapMod::fnAlignElementToEdge()
{
	if (fnGetTVSubMode() != TVEDGEMODE)
		return;

	//holdpoints
	ClearAFlag(A_HELD);
	theHold.Begin();
	HoldPoints();
	theHold.Accept(GetString(IDS_DS_MOVE));

	MeshTopoData *md = NULL;
	//hold the selections
	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		MeshTopoData *ld = mMeshTopoData[i];
	//	ld->HoldSelection();		
	}

	//find and ld with an edge selection
	int eIndex = -1;
	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		MeshTopoData *ld = mMeshTopoData[i];
		BitArray esel = ld->GetTVEdgeSelection();
		if (esel.NumberSet() > 0)
		{
			if (md)
			{
				md->ClearTVEdgeSelection();
			}
			else
			{
				//get the first edge selection
				md = ld;
				for (int j = 0; j < esel.GetSize(); j++)
				{
					if (esel[j])
					{
						eIndex = j;
						j = esel.GetSize();						
					}
				}
			}
		}
	}

	if (md && eIndex != -1)
	{
		theHold.Suspend();

		//get the face cluster
//		int fa = md->GetTVEdgeConnectedTVFace(eIndex,0);
		//md->ClearFaceSelection();
//		md->SetFaceSelected(fa,TRUE);
//		md->SelectElement(TVFACEMODE,TRUE);
//		md->ConvertFaceToEdgeSel();
		md->ClearTVEdgeSelection();
		md->SetTVEdgeSelected(eIndex,TRUE);
		md->SelectElement(TVEDGEMODE,TRUE);

		int a = md->GetTVEdgeVert(eIndex,0);
		int b = md->GetTVEdgeVert(eIndex,1);
		Point3 pa = md->GetTVVert(a);
		Point3 pb = md->GetTVVert(b);
		Point3 vec = pb-pa;
		Point3 nVec = Normalize(vec);

		//find the rotation angle
		float angle = 100000.0f;
		for (int i = 0; i < 4; i++)
		{
			Point3 axis(0,0,0);
			if ( i == 0)
				axis.x = 1.0f;
			if ( i == 1)
				axis.y = 1.0f;
			if ( i == 2)
				axis.x = -1.0f;
			if ( i == 3)
				axis.y = -1.0f;

			float tang =md-> AngleFromVectors(axis,nVec);
			if (fabs(tang) < fabs(angle))
				angle = tang;
		}

		BitArray esel = md->GetTVEdgeSelection();
		int ns = esel.NumberSet();

		//rotate that cluster
		TransferSelectionStart();
		RotateAroundAxis(tempHwnd, angle,pa);
		TransferSelectionEnd(FALSE,FALSE);

		
		
		theHold.Resume();
	}

	//restore the selection
	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		MeshTopoData *ld = mMeshTopoData[i];
	//	ld->RestoreSelection();		
	}

}


void UnwrapMod::fnRegularMapFromEdge()
{
	//we need to be in edge mode for this tool to work.
	if (fnGetTVSubMode() != TVEDGEMODE)
		return;

	//hold or tv data on the undo stack
	ClearAFlag(A_HELD);	
	theHold.Begin();
	HoldPointsAndFaces();
	theHold.Accept(GetString(IDS_PW_UNFOLDMAP));

	MeshTopoData *md = NULL;
	//find a mesh topo data with an geom edge selection
	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		MeshTopoData *ld = mMeshTopoData[i];
		if (ld->GetGeomEdgeSelection().NumberSet())
		{

			INode *node = NULL;
			node = mMeshTopoData.GetNode(i);

			BitArray esel = ld->GetGeomEdgeSelection();
			BitArray fsel = ld->GetFaceSelection();

			//find a seed edge on the selection
			int seedEdge = -1;
			for (int j = 0 ; j < esel.GetSize(); j++)
			{
				if (esel[j])
				{
					seedEdge = j;
					j = esel.GetSize();
				}
			}

			//make sure we have a good edge
			if (seedEdge != -1)
			{
				//hold our selection
				ld->HoldSelection();

				//get a seed face attached to the seed edge
				int seedFace = -1;
				seedFace = ld->GetGeomEdgeConnectedFace(seedEdge,0);

				//get the ring edges
				//we need to suspend since these throw undo records on the stack and we already placed an undo
				//record there
				theHold.Suspend();

				//convert our selection into a ring to expand it to regular faces
				fnGeomRingSelect();
				//convert the edge selection now to face selection
				fnEdgeToFaceSelect();

				//start our regular map
				fnRegularMapStart(node,FALSE);
				//set the seed face
				fnRegularMapStartNewCluster(node,seedFace);
				ld->SetGeomEdgeSelection(esel);
				//expand the regular map using all the selected face
				fnRegularMapExpand(regularmap_expand_loop);
				//we are done
				fnRegularMapEnd();
				theHold.Resume();

				//end the loop
				i = mMeshTopoData.Count();

				//restore our selection
				ld->RestoreSelection();

			}
		}
	}
}



void UnwrapMod::fnStraighten()
{
	if (fnGetTVSubMode() != TVFACEMODE)
		return;

	//holdpoints
	ClearAFlag(A_HELD);
	theHold.Begin();
	HoldPoints();
	theHold.Accept(GetString(IDS_DS_MOVE));

	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		MeshTopoData *ld = mMeshTopoData[i];

		//get the face selection
		BitArray fSel = ld->GetFaceSelection();

		BitArray holdSel(fSel);
		//build the connection info
		TVConnectionInfo connectionData(ld);

		ld->ClearTVVertSelection();

		int lastCount = 0;
		while (fSel.NumberSet() != lastCount)
		{

			//convert our face sel to vsel
			BitArray vSel;
			vSel.SetSize(ld->GetNumberTVVerts());
			vSel.ClearAll();
			ld->GetVertSelFromFace(vSel);

			//build our border info based on the face selection
			connectionData.BuildBorders(fSel);


			int numTVEdges = ld->GetNumberTVEdges();
			int numTVVerts = ld->GetNumberTVVerts();
			BitArray borderVerts;
			borderVerts.SetSize(numTVVerts);
			borderVerts.ClearAll();


			//find a border vert
			int startVert = -1;
			for (int j = 0; j < numTVVerts; j++)
			{
				if (connectionData.mVertex[j]->mBorder)
				{
					startVert  = j;
					j = numTVVerts;
				}
			}


			Box3 bounds;
			bounds.Init();

			if (startVert != -1)
			{



				int originalVert = startVert;
				//find an edge to start from
				int numConnected = connectionData.mVertex[startVert]->mVisibleEdgeCount;//mConnectedTo.Count();
				int nextVert = -1;							
				for (int j = 0; j < numConnected; j++)
				{
					int ithNeighbor = connectionData.mVertex[startVert]->GetVisibleEdge(j);//mConnectedTo[j].mVert;					
					int neighbor = connectionData.mVertex[startVert]->mConnectedTo[ithNeighbor].mVert;
					if (connectionData.mVertex[neighbor]->mBorder && connectionData.mVertex[startVert]->mConnectedTo[ithNeighbor].mBorderEdge)
					{
						nextVert = neighbor;
						j = numConnected;
					}
				}

				if (nextVert < 0)
				{
					DbgAssert(0);
				}
				else
				{

					//determine if it is a u or V

					Point3 a = ld->GetTVVert(startVert);
					Point3 b = ld->GetTVVert(nextVert);
					bounds+= a;
					bounds+= b;

			

					connectionData.mVertex[startVert]->mX = 0;
					connectionData.mVertex[startVert]->mY = 0;

					borderVerts.Set(startVert,TRUE);

					int currentX = 0;
					int currentY = 0;

					Point3 dir(1.0f,0.0f,0.0f);

					//compute initial direction
					Point3 vec = (b-a);
					Point3 nvec = Normalize(vec);
					int currentDirection = 0;  // 0 -x, 1 +y, 2 +x , 3 -y

					if (fabs(vec.y) > fabs(vec.x))
					{
						if (vec.y > 0.0f)
						{
							currentDirection = 1;
							dir = Point3(0.0f,1.0f,0.0f);
						}
						else
						{
							currentDirection = 3;
							dir = Point3(0.0f,-1.0f,0.0f);					
						}
					}
					else
					{
						if (vec.x > 0.0f)
						{
							currentDirection = 2;
							dir = Point3(1.0f,0.0f,0.0f);
						}
						else
						{
							currentDirection = 0;
							dir = Point3(-1.0f,0.0f,0.0f);					
						}
					}

					float d = Length(a-b);
					Point3 offsetPoint = (dir * d);	
					connectionData.mVertex[startVert]->mU = offsetPoint.x;
					connectionData.mVertex[startVert]->mV = offsetPoint.y;	

					connectionData.mVertex[startVert]->mDir = currentDirection;
//					DebugPrint("BorderVert %d direction %f %f \n",startVert,dir.x,dir.y);

					bool done = false;


					//follow the edge one way till hit abnormal vert or get back to start
					bool hitStart = false;
					Point3 prevPoint(connectionData.mVertex[startVert]->mU,connectionData.mVertex[startVert]->mV,0.0f);
					while (!done)
					{
//						DebugPrint("BorderVert %d direction %f %f \n",nextVert,dir.x,dir.y);

						a = ld->GetTVVert(startVert);				
						b = ld->GetTVVert(nextVert);
						bounds+= b;

						Point3 vec = (b-a);
						Point3 nvec = Normalize(vec);

						float d = Length(vec);

						Point3 newPoint = prevPoint + (dir * d);	
						newPoint = (dir * d);	

						if (dir.x == 1.0f)
							currentX++;
						else if (dir.x == -1.0f)
							currentX--;
						if (dir.y == 1.0f)
							currentY++;
						else if (dir.y == -1.0f)
							currentY--;

						//set our info
						connectionData.mVertex[nextVert]->mU = newPoint.x;//b.x;
						connectionData.mVertex[nextVert]->mV = newPoint.y;	

						connectionData.mVertex[nextVert]->mX = currentX;
						connectionData.mVertex[nextVert]->mY = currentY;	
						connectionData.mVertex[nextVert]->mDir = currentDirection;
						borderVerts.Set(nextVert,TRUE);


						prevPoint = Point3(connectionData.mVertex[nextVert]->mU,connectionData.mVertex[nextVert]->mV,0.0f);
						//get our next vert
						int testVert = connectionData.mVertex[nextVert]->GetOpposingVert(startVert);

						//if we run into our start we are done
						if (testVert == originalVert)
						{
							hitStart = true;
							done = true;
						}
						//if we hit a 2 edge corner or the next opposing vert is not a mborder we need to change direction					
						else if (  (testVert < 0) && (connectionData.mVertex[nextVert]->mNumRealEdges == 2))  
						{
							int tempIth = -1;
							int newStart = connectionData.mVertex[nextVert]->GetNextVert(startVert,tempIth);

							if (newStart >= 0) //we have a valid vert
							{
								startVert = nextVert;
								nextVert = newStart;

								currentDirection--; //this counter clockwise turn so we go in the neg dir
								currentDirection = (currentDirection+4)%4;
							}
							else
							{
								int tempIth = -1;
								int temp = connectionData.mVertex[nextVert]->GetNextVert(newStart,tempIth);
								if (temp < 0)
								{
									DbgAssert(0);
								}
								startVert = nextVert;
								nextVert = temp;
								currentDirection++; //add one since we are changing direction
								currentDirection = currentDirection%4;
							}

						}
						//hit a 3 edge corner or more
						//we either shot off into nothing (testvert == -1) or into the inside by checking the border
						else if (  (testVert <= 0) || (connectionData.mVertex[nextVert]->IsBorderEdge(testVert)==false))//connectionData.mVertex[testVert]->mBorder == false))
						{
							//need to see if we can find a new direction
/*
							bool done = false;
							bool cw = true;
							while (!done)
							{
								int ith = -1;
								int newStart = connectionData.mVertex[nextVert]->GetNextVert(startVert,ith);
								if ( (newStart >= 0) && connectionData.mVertex[nextVert]->mConnectedTo[ith].mBorderEdge)//connectionData.mVertex[newStart]->mBorder == true)
								{
									done = true;
									startVert = nextVert;
									nextVert = newStart;
									if (cw)
									{
										currentDirection--; //this counter clockwise turn so we go in the neg dir
										currentDirection = (currentDirection+4)%4;
									}
									else
									{
										currentDirection++; //add one since we are changing direction
										currentDirection = currentDirection%4;
									}
								}
								else if (newStart == testVert) //we crossed the loop so we are going the other dir
								{
									cw = false;
								}
								else
								{
									startVert = newStart;
								}
							}
*/

							int oldStart = startVert;
							startVert = nextVert;
							int numConnected = connectionData.mVertex[startVert]->mVisibleEdgeCount;
							nextVert = -1;			

							int currentVert = 0;
							for (int j = 0; j < numConnected; j++)
							{
								int ithNeighbor = connectionData.mVertex[startVert]->GetVisibleEdge(j);
								int neighbor = connectionData.mVertex[startVert]->mConnectedTo[ithNeighbor].mVert;
								if (neighbor == oldStart)
								{
									currentVert = j;
									j = numConnected;
								}
							}

							//loop through all the connected verts at this point and find and out going border vert
							currentDirection = (currentDirection+2)%4;  //we need to flip the current direction since we are now looking back at the start
							currentDirection++; //add one since we are changing direction
							currentDirection = currentDirection%4;

							//loop through the connected verts looking for another border edge
							for (int j = 0; j < numConnected; j++)
							{
								currentVert++;
								if (currentVert >= numConnected)
									currentVert = 0;
								int ithNeighbor = connectionData.mVertex[startVert]->GetVisibleEdge(currentVert);
								int neighbor = connectionData.mVertex[startVert]->mConnectedTo[ithNeighbor].mVert;
								if (j == numConnected/2)
								{
									currentDirection += 2; //we crossed the border need to change dir again
									currentDirection = currentDirection%4;
								}

								if (neighbor >= 0)
								{
									if (   connectionData.mVertex[neighbor]->mBorder
										&&  connectionData.mVertex[startVert]->mConnectedTo[ithNeighbor].mBorderEdge)
									{
										nextVert = neighbor;								
										j = numConnected;
									}
								}
							}



						}
						//if the opposing vert is a border we can just reasign the start and next and continue
						else if (connectionData.mVertex[testVert]->mBorder)
						{
							startVert = nextVert;
							nextVert = testVert;
						}
						//see if the next loop vert is a border is so we can continue using the same dir vec
						//if we run into a bad vert end
						else // at something we dont understand surrender
						{
							bool good = false;
							if ( connectionData.mVertex[testVert]->mNumRealEdges == 3 ) 
								good = true;
							if ((connectionData.mVertex[testVert]->mNumRealEdges%2) == 0 )
								good = true;  
							done = good;
						}


						if (currentDirection == 0)
							dir = Point3(-1.0f,0.0f,0.0f);
						else if (currentDirection == 1)
							dir = Point3(0.0f,1.0f,0.0f);
						else if (currentDirection == 2)
							dir = Point3(1.0f,0.0f,0.0f);
						else if (currentDirection == 3)
							dir = Point3(0.0f,-1.0f,0.0f);

						//we need one last check since we could come off a turn into the end
						if (startVert == originalVert)
						{
							done = true;
						}
					}


					//now sort the borders
					int smallestX = -0xFFFF;
					int smallestY = -0xFFFF;
					int largestX = 0xFFFF;
					int largestY = 0xFFFF;
					for (int j = 0; j < connectionData.mVertex.Count(); j++)
					{
						if (borderVerts[j])
						{
							ld->SetTVVertSelected(j,TRUE);
							int x = connectionData.mVertex[j]->mX;
							int y = connectionData.mVertex[j]->mY;
							if (smallestX == -0xFFFF || x < smallestX)
								smallestX = x;
							if (smallestY == -0xFFFF || y < smallestY)
								smallestY = y;
							if (largestX == 0xFFFF || x > largestX)
								largestX = x;
							if (largestY == 0xFFFF || y > largestY)
								largestY = y;
						}					
					}

					//these hold all the offset size between cells
					Tab<float> yValues;
					Tab<float> xValues;
					Tab<int> yValuesCts;
					Tab<int> xValuesCts;

					xValues.SetCount(largestX-smallestX+1);
					xValuesCts.SetCount(largestX-smallestX+1);
					for (int j = 0; j < xValues.Count(); j++)
					{
						xValues[j] = 0.0f;
						xValuesCts[j] = 0;
					}

					yValues.SetCount(largestY-smallestY+1);
					yValuesCts.SetCount(largestY-smallestY+1);
					for (int j = 0; j < yValues.Count(); j++)
					{
						yValues[j] = 0.0f;
						yValuesCts[j] = 0;
					}

					//find the average offset
					for (int j = 0; j < connectionData.mVertex.Count(); j++)
					{
						if (borderVerts[j])
						{
							int x = connectionData.mVertex[j]->mX-smallestX;
							int y = connectionData.mVertex[j]->mY-smallestY;	
							float u = connectionData.mVertex[j]->mU;
							float v = connectionData.mVertex[j]->mV;

							if (u != 0.0f)
							{
								if ((u > 0.0) && (x > 0))
								{
									xValues[x-1] += u;
									xValuesCts[x-1] += 1;
								}
								else
								{
									xValues[x] -= u;
									xValuesCts[x] += 1;
								}
							}
							if (v != 0.0f)
							{
								if ((v > 0.0) && (y > 0))
								{
									yValues[y-1] += v;
									yValuesCts[y-1] += 1;
								}
								else
								{
									yValues[y] -= v;
									yValuesCts[y] += 1;
								}
							}
						}
					}

					float tempV = 0;
					int   tempCt = 0;
					for (int j = 0; j < xValues.Count(); j++)
					{
						if (xValuesCts[j] > 0)
						{
							xValues[j] = xValues[j] / (float)xValuesCts[j] ;
							tempV += xValues[j];
							tempCt++;
						}
					}

					if (tempCt>0)
					{
						tempV = tempV/(float)tempCt;
						for (int j = 0; j < xValues.Count(); j++)
						{
							if (xValuesCts[j] == 0)
								xValues[j] = tempV;
						}
					}

					tempV = 0;
					tempCt = 0;
					for (int j = 0; j < yValues.Count(); j++)
					{
						if (yValuesCts[j] > 0)
						{
							yValues[j] = yValues[j] / (float)yValuesCts[j] ;
							tempV += yValues[j];
							tempCt++;
						}
					}

					if (tempCt>0)
					{
						tempV = tempV/(float)tempCt;
						for (int j = 0; j < yValues.Count(); j++)
						{
							if (yValuesCts[j] == 0)
							{
								yValues[j] = tempV;
							}
						}
					}

					//now turn them into absolutes
					
					float tally = bounds.pmin.x;					
					for (int j = 0; j < xValues.Count(); j++)
					{
						float temp = xValues[j];
						xValues[j] = tally;
						tally += temp;
						
					}
					
					tally = bounds.pmin.y;					
					for (int j = 0; j < yValues.Count(); j++)
					{
						float temp = yValues[j];
						yValues[j] = tally;
						tally += temp;						
					}


					//make all the borders aligned
					for (int j = 0; j < connectionData.mVertex.Count(); j++)
					{
						if (borderVerts[j])
						{
							Vertex *v = connectionData.mVertex[j];
							int x = v->mX-smallestX;
							int y = v->mY-smallestY;
							v->mU = xValues[x];
							v->mUset = true;
							v->mV = yValues[y];
							v->mVset = true;
						}
					}

					//find all our corners
					BitArray isCorner;
					isCorner.SetSize(connectionData.mVertex.Count());
					isCorner.ClearAll();
					for (int j = 0; j < connectionData.mVertex.Count(); j++)
					{
						if (borderVerts[j] )
						{
							BOOL corner = TRUE;
							Vertex *v = connectionData.mVertex[j];

							for (int k = 0; k < v->mVisibleEdgeCount; k++)
							{
								int ith = v->GetVisibleEdge(k);
								if (v->mConnectedTo[ith].mBorderEdge)
								{
									int vindex = v->mConnectedTo[ith].mVert;
									int opposing = v->GetOpposingVert(vindex);
									BOOL isBorder = v->IsBorderEdge(opposing);
									if (isBorder) //(opposing >= 0)
										corner = FALSE;
								}
							}
							if (corner)
								isCorner.Set(j,TRUE);
						}
					}

					
					

					//now fill in the centers
					//this basically fires loop through the element aligning the loops
					for (int j = 0; j < connectionData.mVertex.Count(); j++)
					{
						Vertex *v = connectionData.mVertex[j];


						//corners launch 2 
						if (isCorner[j])
						{
							for (int k = 0; k < v->mVisibleEdgeCount; k++)
							{
								int ith = v->GetVisibleEdge(k);
								if (v->mConnectedTo[ith].mBorderEdge)
								{
									int startVert = j;
									int prevVert = v->mConnectedTo[ith].mVert;
									int nextVert = v->GetOpposingVert(prevVert);
									if (nextVert >= 0 && (isCorner[nextVert] == FALSE))
									{
										Vertex *v1 = connectionData.mVertex[prevVert];
										Vertex *v2 = connectionData.mVertex[startVert];
//										Vertex *v3 = connectionData.mVertex[nextVert];
										int dir = 0;
										int x =  v->mX-smallestX;
										float val =  xValues[x];
										if (v1->mY == v2->mY)
										{
											dir = 1;
											int y =  v->mY-smallestY;
											val = yValues[y];
										}

										int initVert = nextVert;
										while (!done)
										{
											if (borderVerts[nextVert] == FALSE)
											{							
												if (dir == 0)
												{
													if (connectionData.mVertex[nextVert]->mUset == false)
														connectionData.mVertex[nextVert]->mU = val;
													connectionData.mVertex[nextVert]->mUset = true;
												}
												else
												{
													if (connectionData.mVertex[nextVert]->mVset == false)
														connectionData.mVertex[nextVert]->mV = val;
													connectionData.mVertex[nextVert]->mVset = true;
												}
											}
											int testVert = connectionData.mVertex[nextVert]->GetOpposingVert(startVert);
											if ((testVert < 0) || (testVert == initVert) ||  (isCorner[testVert] == TRUE))
											{
												done = true;
											}
											else
											{
												startVert = nextVert;
												nextVert = testVert;
											}
										}
									}
								}
							}
						}
						else 
						{		
							int launchIndex = j;

							if (borderVerts[j] )
							{

								int dir = 0;;
								float val = 0.0f;
								if ( (v->mDir == 0) || (v->mDir == 2))
								{
									dir = 0; //align vertically
									int x =  v->mX-smallestX;
									val = xValues[x];
								}
								else
								{
									dir = 1; //align horizontally
									int y =  v->mY-smallestY;
									val = yValues[y];
								}


								//find our loop direction
								int nextVert = -1;
								{
									for (int k = 0; k < v->mVisibleEdgeCount; k++)
									{
										int ith = v->GetVisibleEdge(k);
										if (ith != -1)
										{
											if (v->mConnectedTo[ith].mBorderEdge == FALSE)
											{
												if (v->mConnectedTo[ith].mVert >= 0)
												{
													nextVert = v->mConnectedTo[ith].mVert;
													k = v->mVisibleEdgeCount;
												}
											}
										}
									}
								}

								//align all those along the loop
								startVert = launchIndex;
								bool done = false;
								if (nextVert < 0)
								{
									DbgAssert(0);
								}
								else
								{		
									int initVert = nextVert;
									if (borderVerts[nextVert] )
										done = true;
									while (!done)
									{
										if (borderVerts[nextVert] == FALSE)
										{							
											if (dir == 0)
											{
												connectionData.mVertex[nextVert]->mU = val;
												connectionData.mVertex[nextVert]->mUset = true;
											}
											else
											{
												connectionData.mVertex[nextVert]->mV = val;
												connectionData.mVertex[nextVert]->mVset = true;
											}
										}
										int testVert = connectionData.mVertex[nextVert]->GetOpposingVert(startVert);
										if ((testVert < 0) || (testVert == initVert) || borderVerts[testVert])
										{
											done = true;
										}
										else
										{
											startVert = nextVert;
											nextVert = testVert;
										}
									}		
								}
							}
						}
					}


					//copy the data back the modifier
					TimeValue t = GetCOREInterface()->GetTime();
					for (int j = 0; j < numTVVerts; j++)
					{
						if (vSel[j])
						{
							Point3 p(0,0,0);
							p = ld->GetTVVert(j);
							BOOL set = FALSE;
							//only send back data that was changed
							if (connectionData.mVertex[j]->mUset)
							{
								p.x = connectionData.mVertex[j]->mU;
								set = TRUE;
							}
							if (connectionData.mVertex[j]->mVset)
							{
								p.y = connectionData.mVertex[j]->mV;
								set = TRUE;
							}
							if (set)
							{
								ld->SetTVVert(t,j,p,this);
							}
							
						}
					}


				}
			}

			lastCount = fSel.NumberSet();

			for (int j = 0; j < ld->GetNumberFaces(); j++)
			{
				if (fSel[j])
				{
					int degree = ld->GetFaceDegree(j);
					for (int k = 0; k < degree; k++)
					{
						int tvIndex = ld->GetFaceTVVert(j,k);
						if (vSel[tvIndex] )
						{
							if (connectionData.mVertex[tvIndex]->mUset || connectionData.mVertex[tvIndex]->mVset)
							{
								ld->SetTVVertSelected(tvIndex,TRUE);
								fSel.Set(j,FALSE);
								k = degree;
							}
						}
					}
				}
			}


			ld->SetFaceSelection(fSel);

			if (fSel.NumberSet() == 0)
				lastCount = 0;


		}
		ld->SetFaceSelection(holdSel);
	}

	UpdateViewAndModifier();
}

void UnwrapMod::fnFlattenBySmoothingGroup(BOOL rotate, BOOL rescale, float padding) 
{
	pblock->SetValue(unwrap_flattenby,0,1);	

	BOOL temp = packRescaleCluster;
	packRescaleCluster = rescale;

	Tab<Point3*> normList;
	normList.SetCount(6);

	normList[0] = new Point3(1.0f,0.0f,0.0f);
	normList[1] = new Point3(-1.0f,0.0f,0.0f);
	normList[2] = new Point3(0.0f,1.0f,0.0f);
	normList[3] = new Point3(0.0f,-1.0f,0.0f);
	normList[4] = new Point3(0.0f,0.0f,1.0f);
	normList[5] = new Point3(0.0f,0.0f,-1.0f);


	fnFlattenMap(flattenAngleThreshold, &normList, padding, TRUE, 2, rotate, TRUE);
	for (int i = 0; i < 6; i++)
		delete normList[i];

	packRescaleCluster = temp;
	pblock->SetValue(unwrap_flattenby,0,0);
}
void UnwrapMod::fnFlattenByMaterialID(BOOL rotate, BOOL rescale, float padding) 
{

	BOOL temp = packRescaleCluster;
	packRescaleCluster = rescale;

	pblock->SetValue(unwrap_flattenby,0,2);

	Tab<Point3*> normList;
	normList.SetCount(6);

	normList[0] = new Point3(1.0f,0.0f,0.0f);
	normList[1] = new Point3(-1.0f,0.0f,0.0f);
	normList[2] = new Point3(0.0f,1.0f,0.0f);
	normList[3] = new Point3(0.0f,-1.0f,0.0f);
	normList[4] = new Point3(0.0f,0.0f,1.0f);
	normList[5] = new Point3(0.0f,0.0f,-1.0f);


	fnFlattenMap(flattenAngleThreshold, &normList, padding, TRUE, 2, rotate, TRUE);
	for (int i = 0; i < 6; i++)
		delete normList[i];

	packRescaleCluster = temp;
	pblock->SetValue(unwrap_flattenby,0,0);

}

void UnwrapMod::fnAlignByPivotVertical()
{
	//hold the points
	theHold.Begin();
	theHold.Put(new UnwrapPivotRestore(this));
	HoldPoints();	
	ClearAFlag(A_HELD);
	HoldSelection();	
	theHold.Accept(GetString(IDS_DS_MOVE));
	AlignByPivot(TRUE);
}

void UnwrapMod::fnAlignByPivotHorizontal()
{
	//hold the points
	theHold.Begin();
	theHold.Put(new UnwrapPivotRestore(this));
	HoldPoints();	
	ClearAFlag(A_HELD);
	HoldSelection();	
	theHold.Accept(GetString(IDS_DS_MOVE));	
	AlignByPivot(FALSE);
}


void UnwrapMod::AlignByPivot(BOOL vertical)
{


	if (GetAsyncKeyState(VK_SHIFT) != 0)
		SelectUVEdge(FALSE);

	TransferSelectionStart();

	

	float val = 0.0f;
	Point3 tempAlign(0,0,0);

	int id = 0;


	Point3 pivotCenter(0,0,0);
	pivotCenter.x =  freeFormPivotScreenSpace.x;
	pivotCenter.y =  freeFormPivotScreenSpace.y;

	float xzoom, yzoom;
	int width,height;

	ComputeZooms(hView,xzoom,yzoom,width,height);


	int tx = (width-int(xzoom))/2;
	int ty = (height-int(yzoom))/2;
	int i1, i2;
	GetUVWIndices(i1,i2);

	pivotCenter[i1] = (pivotCenter.x-tx-xscroll)/xzoom;
	pivotCenter[i2] = (pivotCenter.y+ty-yscroll - height)/-yzoom;

	
	
	tempAlign = pivotCenter;


	if (vertical)
		val = tempAlign.x;
	else
	{
		val = tempAlign.y;
		id = 1;
	}


	//align
	TimeValue t = GetCOREInterface()->GetTime();
	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		int numVerts = mMeshTopoData[i]->GetNumberTVVerts();
		BitArray vsel = mMeshTopoData[i]->GetTVVertSelection();
		for (int j = 0; j < numVerts; j++)
		{
			if (vsel[j])
			{
				Point3 p = mMeshTopoData[i]->GetTVVert(j);
				p[id] = val;
				mMeshTopoData[i]->SetTVVert(t,j,p,this);
			}
		}
	}

	//put back the selection
	TransferSelectionEnd(FALSE,FALSE);

	UpdateViewAndModifier();

}
