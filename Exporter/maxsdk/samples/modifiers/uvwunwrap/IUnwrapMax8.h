
//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: These are the max 8 and 9 interfaces for max script
// AUTHOR: Peter Watje
// DATE: 2006/10/07 
//***************************************************************************/

#ifndef __IUNWRAP8__H
#define __IUNWRAP8__H

#include "iFnPub.h"



#define UNWRAP_INTERFACE5 Interface_ID(0x53b3409b, 0x18ff7ad2)
#define UNWRAP_INTERFACE6 Interface_ID(0x53b3409b, 0x18ff7ad3)



enum {  unwrap_pelt_getmapmode,unwrap_pelt_setmapmode, 
		unwrap_pelt_geteditseamsmode,unwrap_pelt_seteditseamsmode, 
		unwrap_pelt_setseamselection,unwrap_pelt_getseamselection, 

		unwrap_pelt_getpointtopointseamsmode,unwrap_pelt_setpointtopointseamsmode, 

		unwrap_pelt_expandtoseams,
		unwrap_pelt_dialog,
		
		unwrap_peltdialog_resetrig,
		unwrap_peltdialog_selectrig,
		unwrap_peltdialog_selectpelt,
		unwrap_peltdialog_snaprigtoedges,
		unwrap_peltdialog_mirrorrig,

		unwrap_peltdialog_setstraightenmode,unwrap_peltdialog_getstraightenmode,
		unwrap_peltdialog_straighten,
		unwrap_peltdialog_run,
		unwrap_peltdialog_relax1,
		unwrap_peltdialog_relax2,

		unwrap_peltdialog_getframes,unwrap_peltdialog_setframes,
		unwrap_peltdialog_getsamples,unwrap_peltdialog_setsamples,

		unwrap_peltdialog_getrigstrength,unwrap_peltdialog_setrigstrength,
		unwrap_peltdialog_getstiffness,unwrap_peltdialog_setstiffness,

		unwrap_peltdialog_getdampening,unwrap_peltdialog_setdampening,
		unwrap_peltdialog_getdecay,unwrap_peltdialog_setdecay,
		unwrap_peltdialog_getmirroraxis,unwrap_peltdialog_setmirroraxis,

		unwrap_mapping_align,
		unwrap_mappingmode,
		unwrap_setgizmotm,unwrap_getgizmotm,

		unwrap_gizmofit,
		unwrap_gizmocenter,
		unwrap_gizmoaligntoview,

		unwrap_geomedgeselectionexpand,unwrap_geomedgeselectioncontract,

		unwrap_geomedgeloop,
		unwrap_geomedgering,

		unwrap_geomvertexselectionexpand,unwrap_geomvertexselectioncontract,

		unwrap_setgeomvertexselection,unwrap_getgeomvertexselection, 
		unwrap_setgeomedgeselection,unwrap_getgeomedgeselection, 

		unwrap_pelt_getshowseam,unwrap_pelt_setshowseam,

		unwrap_pelt_seamtosel,	unwrap_pelt_seltoseam,
		unwrap_getnormalizemap,unwrap_setnormalizemap,
		
		unwrap_getshowedgedistortion,unwrap_setshowedgedistortion,

		unwrap_getlockedgesprings,unwrap_setlockedgesprings,

		unwrap_selectoverlap,

		unwrap_gizmoreset,

		unwrap_getedgedistortionscale,unwrap_setedgedistortionscale,

		unwrap_relaxspring,
		unwrap_relaxspringdialog,
		unwrap_getrelaxspringiteration,unwrap_setrelaxspringiteration,
		unwrap_getrelaxspringstretch,unwrap_setrelaxspringstretch,

		unwrap_getrelaxspringvedges,unwrap_setrelaxspringvedges,


		unwrap_getrelaxstretch,unwrap_setrelaxstretch,
		unwrap_getrelaxtype,unwrap_setrelaxtype,

		unwrap_relaxbyfaceangle,unwrap_relaxbyedgeangle,

		unwrap_setwindowxoffset,
		unwrap_setwindowyoffset,
		unwrap_frameselectedelement,

		unwrap_setshowcounter,	unwrap_getshowcounter,
		
		unwrap_renderuv_dialog,
		unwrap_renderuv,
		unwrap_ismesh,
		unwrap_qmap,

		unwrap_qmap_setpreview,unwrap_qmap_getpreview,		

		unwrap_setshowmapseams,unwrap_getshowmapseams,	

		unwrap_getvertexpositionByNode,
		unwrap_numberverticesByNode,
		unwrap_getselectedpolygonsByNode,
		unwrap_selectpolygonsByNode, 
		unwrap_ispolygonselectedByNode,
		unwrap_numberpolygonsByNode,
		unwrap_markasdeadByNode,
		unwrap_numberpointsinfaceByNode,
		unwrap_getvertexindexfromfaceByNode,
		unwrap_gethandleindexfromfaceByNode,
		unwrap_getinteriorindexfromfaceByNode,

		unwrap_getvertexgindexfromfaceByNode,
		unwrap_gethandlegindexfromfaceByNode,
		unwrap_getinteriorgindexfromfaceByNode,

		unwrap_addpointtofaceByNode,
		unwrap_addpointtohandleByNode,
		unwrap_addpointtointeriorByNode,

		unwrap_setfacevertexindexByNode,
		unwrap_setfacehandleindexByNode,
		unwrap_setfaceinteriorindexByNode,

		unwrap_getareaByNode,
		unwrap_getboundsByNode,
		unwrap_pelt_setseamselectionByNode,unwrap_pelt_getseamselectionByNode, 
		unwrap_selectpolygonsupdateByNode,
		unwrap_sketchByNode,
		unwrap_getnormalByNode,
		unwrap_setvertexpositionByNode,
		unwrap_setvertexposition2ByNode,

		unwrap_getvertexweightByNode, unwrap_setvertexweightByNode,
		unwrap_isweightmodifiedByNode,unwrap_modifyweightByNode,
		unwrap_getselectedvertsByNode,unwrap_selectvertsByNode,
		unwrap_isvertexselectedByNode,

		unwrap_getselectedfacesByNode,unwrap_selectfacesByNode,
		unwrap_isfaceselectedByNode,

		unwrap_getselectededgesByNode,unwrap_selectedgesByNode,
		unwrap_isedgeselectedByNode,

		unwrap_setgeomvertexselectionByNode,unwrap_getgeomvertexselectionByNode, 
		unwrap_setgeomedgeselectionByNode,unwrap_getgeomedgeselectionByNode, 

		unwrap_setsg,unwrap_getsg,

		unwrap_setMatIDSelect,unwrap_getMatIDSelect,

	//2008 functions
		unwrap_splinemap,

		unwrap_splinemap_selectSpline,unwrap_splinemap_isSplineSelected,
		unwrap_splinemap_selectCrossSection,unwrap_splinemap_isCrossSectionSelected,
		unwrap_splinemap_clearSelectSpline,
		unwrap_splinemap_clearSelectCrossSection,

		unwrap_splinemap_numberOfSplines,
		unwrap_splinemap_numberOfCrossSections,

		unwrap_splinemap_getCrossSection_Pos,
		unwrap_splinemap_getCrossSection_ScaleX,
		unwrap_splinemap_getCrossSection_ScaleY,
		unwrap_splinemap_getCrossSection_Twist,


		unwrap_splinemap_setCrossSection_Pos,
		unwrap_splinemap_setCrossSection_ScaleX,
		unwrap_splinemap_setCrossSection_ScaleY,
		unwrap_splinemap_setCrossSection_Twist,

		unwrap_splinemap_recomputeCrossSections,

		unwrap_splinemap_fit,
		unwrap_splinemap_align,
		unwrap_splinemap_alignSelected,
		unwrap_splinemap_aligncommandmode,
		unwrap_splinemap_alignsectioncommandmode,
		unwrap_splinemap_copy,unwrap_splinemap_paste,unwrap_splinemap_pasteToSelected,
		unwrap_splinemap_delete,

		unwrap_splinemap_addCrossSectionCommandmode,
		unwrap_splinemap_insertCrossSection,

		unwrap_splinemap_startMapMode,
		unwrap_splinemap_endMapMode,

		unwrap_splinemap_moveSelectedCrossSection,
		unwrap_splinemap_rotateSelectedCrossSection,
		unwrap_splinemap_scaleSelectedCrossSection,


		unwrap_splinemap_dump,

		unwrap_splinemap_resample,
//2009 exposure
		unwrap_gettweakmode,unwrap_settweakmode,  //this allows you to turn on/off the tweak uvw mode
		
		unwrap_uvloop,unwrap_uvring,               //these allow you to do step ring loop selections

		unwrap_align,
		unwrap_space,               //these allow you align or space vertices horizontally or vertically

		unwrap_regularmap_start,
		unwrap_regularmap_reset,

		unwrap_regularmap_advanceUV,
		unwrap_regularmap_advanceSelected,

		unwrap_regularmap_end,
		unwrap_regularmap_getnormalize,
		unwrap_regularmap_setnormalize,

		unwrap_regularmap_getautoweld,
		unwrap_regularmap_setautoweld,

		unwrap_regularmap_getautoweldthreshold,
		unwrap_regularmap_setautoweldthreshold,

		unwrap_regularmap_geticonsize,
		unwrap_regularmap_seticonsize,

		unwrap_regularmap_getsinglestep,
		unwrap_regularmap_setsinglestep,

		unwrap_regularmap_getlimit,
		unwrap_regularmap_setlimit,

		unwrap_regularmap_getautofit,
		unwrap_regularmap_setautofit,

		unwrap_regularmap_resetfaces,


		unwrap_regularmap_startnewcluster,

		unwrap_regularmap_expand,


		unwrap_ispinned,
		unwrap_pin,
		unwrap_unpin,
		unwrap_pinselected,
		unwrap_unpinselected,


		unwrap_lscm_interactive,
		unwrap_lscm_solve,
		unwrap_lscm_reset,

		unwrap_rescalecluster,
		unwrap_getpackrescalecluster,unwrap_setpackrescalecluster,

		unwrap_showtoolbar,

		unwrap_addfloater, unwrap_deletefloater, unwrap_showfloater,


		unwrap_weldallshared, unwrap_weldselectedshared,

                unwrap_relaxoneclick,

		unwrap_group_create,unwrap_group_delete,unwrap_group_rename,unwrap_group_select,
		unwrap_group_create_byselection,unwrap_group_delete_byselection,unwrap_group_select_byselection,
		unwrap_group_gettexeldensity,unwrap_group_settexeldensity,


		unwrap_addtoolbar,

		unwrap_straighten,

		unwrap_flatten_bysmoothinggroup,
		unwrap_flatten_bymatid,


		unwrap_alignbypivot_horizontal,
		unwrap_alignbypivot_vertical,



};

//script enumIDs
enum
{
	regularmap_expansionenums
};
//expansion ids
enum RegularMapExpansions
{ 
	regularmap_expand_smgrp,
	regularmap_expand_planar,
	regularmap_expand_edgesel,
	regularmap_expand_facesel,
	regularmap_expand_outerborder,
	regularmap_expand_innerborder,
	regularmap_expand_ring,
	regularmap_expand_peltseams,
	regularmap_expand_loop,
};

class IUnwrapMod5 : public FPMixinInterface  //interface for R8
	{
	public:

		//Function Publishing System
		//Function Map For Mixin Interface
		//*************************************************
		BEGIN_FUNCTION_MAP


			VFN_1(unwrap_pelt_seteditseamsmode, fnSetPeltEditSeamsMode,TYPE_BOOL);
			FN_0(unwrap_pelt_geteditseamsmode, TYPE_BOOL, fnGetPeltEditSeamsMode);

			VFN_1(unwrap_pelt_setseamselection, fnSetSeamSelection,TYPE_BITARRAY);
			FN_0(unwrap_pelt_getseamselection,TYPE_BITARRAY, fnGetSeamSelection);

			VFN_1(unwrap_pelt_setpointtopointseamsmode, fnSetPeltPointToPointSeamsMode,TYPE_BOOL);
			FN_0(unwrap_pelt_getpointtopointseamsmode, TYPE_BOOL, fnGetPeltPointToPointSeamsMode);

			VFN_0(unwrap_pelt_expandtoseams, fnPeltExpandSelectionToSeams);
			VFN_0(unwrap_pelt_dialog, fnPeltDialog);

			VFN_0(unwrap_peltdialog_resetrig, fnPeltDialogResetRig);
			VFN_0(unwrap_peltdialog_selectrig, fnPeltDialogSelectRig);
			VFN_0(unwrap_peltdialog_selectpelt, fnPeltDialogSelectPelt);

			VFN_0(unwrap_peltdialog_snaprigtoedges, fnPeltDialogSnapRig);
			VFN_0(unwrap_peltdialog_mirrorrig, fnPeltDialogMirrorRig);

			VFN_1(unwrap_peltdialog_setstraightenmode, fnSetPeltDialogStraightenSeamsMode,TYPE_BOOL);
			FN_0(unwrap_peltdialog_getstraightenmode, TYPE_BOOL, fnGetPeltDialogStraightenSeamsMode);

			VFN_2(unwrap_peltdialog_straighten,fnPeltDialogStraighten,TYPE_INT,TYPE_INT);

			VFN_0(unwrap_peltdialog_run, fnPeltDialogRun);
			VFN_0(unwrap_peltdialog_relax1, fnPeltDialogRelax1);
			VFN_0(unwrap_peltdialog_relax2, fnPeltDialogRelax2);

			VFN_1(unwrap_peltdialog_setframes, fnSetPeltDialogFrames,TYPE_INT);
			FN_0(unwrap_peltdialog_getframes, TYPE_INT, fnGetPeltDialogFrames);

			VFN_1(unwrap_peltdialog_setsamples, fnSetPeltDialogSamples,TYPE_INT);
			FN_0(unwrap_peltdialog_getsamples, TYPE_INT, fnGetPeltDialogSamples);

			VFN_1(unwrap_peltdialog_setrigstrength, fnSetPeltDialogRigStrength,TYPE_FLOAT);
			FN_0(unwrap_peltdialog_getrigstrength, TYPE_FLOAT, fnGetPeltDialogRigStrength);

			VFN_1(unwrap_peltdialog_setstiffness, fnSetPeltDialogStiffness,TYPE_FLOAT);
			FN_0(unwrap_peltdialog_getstiffness, TYPE_FLOAT, fnGetPeltDialogStiffness);

			VFN_1(unwrap_peltdialog_setdampening, fnSetPeltDialogDampening,TYPE_FLOAT);
			FN_0(unwrap_peltdialog_getdampening, TYPE_FLOAT, fnGetPeltDialogDampening);

			VFN_1(unwrap_peltdialog_setdecay, fnSetPeltDialogDecay,TYPE_FLOAT);
			FN_0(unwrap_peltdialog_getdecay, TYPE_FLOAT, fnGetPeltDialogDecay);

			VFN_1(unwrap_peltdialog_setmirroraxis, fnSetPeltDialogMirrorAxis,TYPE_FLOAT);
			FN_0(unwrap_peltdialog_getmirroraxis, TYPE_FLOAT, fnGetPeltDialogMirrorAxis);


			VFN_1(unwrap_mapping_align, fnAlignAndFit,TYPE_INT);
			VFN_1(unwrap_mappingmode, fnSetMapMode,TYPE_INT);

			VFN_1(unwrap_setgizmotm, fnSetGizmoTM,TYPE_MATRIX3);
			FN_0(unwrap_getgizmotm, TYPE_MATRIX3, fnGetGizmoTM);
			
			VFN_0(unwrap_gizmofit, fnGizmoFit);
			VFN_0(unwrap_gizmocenter, fnGizmoCenter);
			
			VFN_0(unwrap_gizmoaligntoview, fnGizmoAlignToView);

			VFN_0(unwrap_geomedgeselectionexpand, fnGeomExpandEdgeSel);
			VFN_0(unwrap_geomedgeselectioncontract, fnGeomContractEdgeSel);

			VFN_0(unwrap_geomedgeloop, fnGeomLoopSelect);
			VFN_0(unwrap_geomedgering, fnGeomRingSelect);

			VFN_0(unwrap_geomvertexselectionexpand, fnGeomExpandVertexSel);
			VFN_0(unwrap_geomvertexselectioncontract, fnGeomContractVertexSel);

			VFN_1(unwrap_setgeomvertexselection, fnSetGeomVertexSelection,TYPE_BITARRAY);
			FN_0(unwrap_getgeomvertexselection,TYPE_BITARRAY, fnGetGeomVertexSelection);

			VFN_1(unwrap_setgeomedgeselection, fnSetGeomEdgeSelection,TYPE_BITARRAY);
			FN_0(unwrap_getgeomedgeselection,TYPE_BITARRAY, fnGetGeomEdgeSelection);


			FN_0(unwrap_pelt_getshowseam, TYPE_BOOL, fnGetAlwayShowPeltSeams);
			VFN_1(unwrap_pelt_setshowseam, fnSetAlwayShowPeltSeams,TYPE_BOOL);

			VFN_1(unwrap_pelt_seamtosel, fnPeltSeamsToSel,TYPE_BOOL);
			VFN_1(unwrap_pelt_seltoseam, fnPeltSelToSeams,TYPE_BOOL);

			VFN_1(unwrap_setnormalizemap, fnSetNormalizeMap,TYPE_BOOL);
			FN_0(unwrap_getnormalizemap, TYPE_BOOL, fnGetNormalizeMap);

			VFN_1(unwrap_setshowedgedistortion, fnSetShowEdgeDistortion,TYPE_BOOL);
			FN_0(unwrap_getshowedgedistortion, TYPE_BOOL, fnGetShowEdgeDistortion);

			VFN_1(unwrap_setlockedgesprings, fnSetLockSpringEdges,TYPE_BOOL);
			FN_0(unwrap_getlockedgesprings, TYPE_BOOL, fnGetLockSpringEdges);

			VFN_0(unwrap_selectoverlap, fnSelectOverlap);

			VFN_0(unwrap_gizmoreset, fnGizmoReset);
			
			VFN_1(unwrap_setedgedistortionscale, fnSetEdgeDistortionScale,TYPE_FLOAT);
			FN_0(unwrap_getedgedistortionscale, TYPE_FLOAT, fnGetEdgeDistortionScale);

			VFN_3(unwrap_relaxspring, fnRelaxBySprings,TYPE_INT,TYPE_FLOAT,TYPE_BOOL);
			VFN_0(unwrap_relaxspringdialog, fnRelaxBySpringsDialog);


			VFN_1(unwrap_setrelaxspringiteration, fnSetRelaxBySpringIteration,TYPE_INT);
			FN_0(unwrap_getrelaxspringiteration, TYPE_INT, fnGetRelaxBySpringIteration);

			VFN_1(unwrap_setrelaxspringstretch, fnSetRelaxBySpringStretch,TYPE_FLOAT);
			FN_0(unwrap_getrelaxspringstretch, TYPE_FLOAT, fnGetRelaxBySpringStretch);

			VFN_1(unwrap_setrelaxspringvedges, fnSetRelaxBySpringVEdges,TYPE_BOOL);
			FN_0(unwrap_getrelaxspringvedges, TYPE_BOOL, fnGetRelaxBySpringVEdges);

			VFN_1(unwrap_setrelaxstretch, fnSetRelaxStretch,TYPE_FLOAT);
			FN_0(unwrap_getrelaxstretch, TYPE_FLOAT, fnGetRelaxStretch);

			VFN_1(unwrap_setrelaxtype, fnSetRelaxType,TYPE_INT);
			FN_0(unwrap_getrelaxtype, TYPE_INT, fnGetRelaxType);

			VFN_4(unwrap_relaxbyfaceangle, fnRelaxByFaceAngleNoDialog,TYPE_INT,TYPE_FLOAT,TYPE_FLOAT,TYPE_BOOL);
			VFN_4(unwrap_relaxbyedgeangle, fnRelaxByEdgeAngleNoDialog,TYPE_INT,TYPE_FLOAT,TYPE_FLOAT,TYPE_BOOL);

			VFN_1(unwrap_setwindowxoffset, fnSetWindowXOffset,TYPE_INT);
			VFN_1(unwrap_setwindowyoffset, fnSetWindowYOffset,TYPE_INT);

			VFN_0(unwrap_frameselectedelement, fnFrameSelectedElement);
			
			VFN_1(unwrap_setshowcounter, fnSetShowCounter,TYPE_BOOL);
			FN_0(unwrap_getshowcounter, TYPE_BOOL, fnGetShowCounter);

			VFN_0(unwrap_renderuv_dialog, fnRenderUVDialog);

			VFN_1(unwrap_renderuv, fnRenderUV, TYPE_FILENAME);
			FN_0(unwrap_ismesh, TYPE_BOOL,fnIsMesh);

			VFN_0(unwrap_qmap, fnQMap);

			VFN_1(unwrap_qmap_setpreview, fnSetQMapPreview,TYPE_BOOL);
			FN_0(unwrap_qmap_getpreview, TYPE_BOOL, fnGetQMapPreview);

			VFN_1(unwrap_setshowmapseams, fnSetShowMapSeams,TYPE_BOOL);
			FN_0(unwrap_getshowmapseams, TYPE_BOOL, fnGetShowMapSeams);



		END_FUNCTION_MAP

		FPInterfaceDesc* GetDesc();    // <-- must implement 

				
		virtual BOOL fnGetPeltEditSeamsMode() = 0;
		virtual void fnSetPeltEditSeamsMode(BOOL mode) = 0;

		virtual BitArray* fnGetSeamSelection()=0;
		virtual void fnSetSeamSelection(BitArray *sel)=0;

		virtual BOOL fnGetPeltPointToPointSeamsMode() = 0;
		virtual void fnSetPeltPointToPointSeamsMode(BOOL mode) = 0;

		virtual void fnPeltExpandSelectionToSeams() = 0;
		virtual void fnPeltDialog() = 0;

		virtual void fnPeltDialogResetRig() = 0;

		virtual void fnPeltDialogSelectRig() = 0;
		virtual void fnPeltDialogSelectPelt() = 0;

		virtual void fnPeltDialogSnapRig() = 0;
		virtual void fnPeltDialogMirrorRig() = 0;

		virtual BOOL fnGetPeltDialogStraightenSeamsMode() = 0;
		virtual void fnSetPeltDialogStraightenSeamsMode(BOOL mode) = 0;

		virtual void fnPeltDialogStraighten(int a, int b) = 0;

		virtual void fnPeltDialogRun() = 0;
		virtual void fnPeltDialogRelax1() = 0;
		virtual void fnPeltDialogRelax2() = 0;

		virtual int fnGetPeltDialogFrames()=0;
		virtual void fnSetPeltDialogFrames(int frames)=0;

		virtual int fnGetPeltDialogSamples()=0;
		virtual void fnSetPeltDialogSamples(int samples)=0;

		virtual float fnGetPeltDialogRigStrength()=0;
		virtual void fnSetPeltDialogRigStrength(float strength)=0;

		virtual float fnGetPeltDialogStiffness()=0;
		virtual void fnSetPeltDialogStiffness(float stiffness)=0;

		virtual float fnGetPeltDialogDampening()=0;
		virtual void fnSetPeltDialogDampening(float dampening)=0;

		virtual float fnGetPeltDialogDecay()=0;
		virtual void fnSetPeltDialogDecay(float decay)=0;

		virtual float fnGetPeltDialogMirrorAxis()=0;
		virtual void fnSetPeltDialogMirrorAxis(float axis)=0;

		virtual void fnAlignAndFit(int axis) = 0;

		virtual void fnSetMapMode(int mode) = 0;


		virtual void fnSetGizmoTM(Matrix3 tm) = 0;
		virtual Matrix3 *fnGetGizmoTM() = 0;

		virtual void fnGizmoFit() = 0;
		virtual void fnGizmoCenter() = 0;

		virtual void fnGizmoAlignToView() = 0;
		
		virtual void	fnGeomExpandEdgeSel() = 0;
		virtual void	fnGeomContractEdgeSel() = 0;

		virtual void	fnGeomLoopSelect() = 0;
		virtual void	fnGeomRingSelect() = 0;

		virtual void	fnGeomExpandVertexSel() = 0;
		virtual void	fnGeomContractVertexSel() = 0;

		virtual BitArray* fnGetGeomVertexSelection()=0;
		virtual void fnSetGeomVertexSelection(BitArray *sel)=0;

		virtual BitArray* fnGetGeomEdgeSelection()=0;
		virtual void fnSetGeomEdgeSelection(BitArray *sel)=0;

		virtual BOOL fnGetAlwayShowPeltSeams()=0;
		virtual void fnSetAlwayShowPeltSeams(BOOL show)=0;

		virtual void fnPeltSeamsToSel(BOOL replace)= 0;
		virtual void fnPeltSelToSeams(BOOL replace)= 0;
		
		virtual void fnSetNormalizeMap(BOOL normalize) = 0;
		virtual BOOL fnGetNormalizeMap() = 0;


		virtual void fnSetShowEdgeDistortion(BOOL show) = 0;
		virtual BOOL fnGetShowEdgeDistortion() = 0;

		virtual void fnSetLockSpringEdges(BOOL lock) = 0;
		virtual BOOL fnGetLockSpringEdges() = 0;

		virtual void fnSelectOverlap() = 0;

		virtual void fnGizmoReset() = 0;

		
		virtual void fnSetEdgeDistortionScale(float scale) = 0;
		virtual float fnGetEdgeDistortionScale() = 0;

		virtual void fnRelaxBySprings(int frames, float stretch, BOOL vEdgesOnly) = 0;
		virtual void fnRelaxBySpringsDialog() = 0;
	
		virtual int fnGetRelaxBySpringIteration() = 0;
		virtual void fnSetRelaxBySpringIteration(int iteration) = 0;

		virtual float fnGetRelaxBySpringStretch() = 0;
		virtual void fnSetRelaxBySpringStretch(float stretch) = 0;


		virtual BOOL fnGetRelaxBySpringVEdges() = 0;
		virtual void fnSetRelaxBySpringVEdges(BOOL useVEdges) = 0;

		virtual float fnGetRelaxStretch() = 0;
		virtual void fnSetRelaxStretch(float stretch) = 0;

		virtual int fnGetRelaxType() = 0;
		virtual void fnSetRelaxType(int type) = 0;

		virtual void fnRelaxByFaceAngleNoDialog(int frames, float stretch, float str, BOOL lockOuterVerts) = 0;;
		virtual void fnRelaxByEdgeAngleNoDialog(int frames, float stretch,  float str, BOOL lockOuterVerts) = 0;;


		virtual void fnSetWindowXOffset(int offset) = 0;
		virtual void fnSetWindowYOffset(int offset) = 0;

		virtual void fnFrameSelectedElement() = 0;

		virtual void fnSetShowCounter(BOOL show) = 0;
		virtual BOOL fnGetShowCounter() = 0;

		virtual void fnRenderUVDialog() = 0;
		virtual void fnRenderUV(const MCHAR *name) = 0;
#pragma warning( push )
#pragma warning( disable : 4481 )
		//! \deprecated Deprecated method in terms of implementation as of 3ds Max 2013 - re-implement with const MCHAR* method signature
		virtual void fnRenderUV(MCHAR *name) sealed { fnRenderUV(const_cast<const MCHAR*>(name));}
#pragma warning( pop )
		virtual BOOL fnIsMesh() = 0;

		virtual void fnQMap() = 0;

		virtual void fnSetQMapPreview(BOOL preview) = 0;
		virtual BOOL fnGetQMapPreview() = 0;

		virtual void fnSetShowMapSeams(BOOL show) = 0;
		virtual BOOL fnGetShowMapSeams() = 0;

		
		
	};


class IUnwrapMod6 : public FPMixinInterface  //interface for R10 && R11
	{
	public:

		//Function Publishing System
		//Function Map For Mixin Interface
		//*************************************************
		BEGIN_FUNCTION_MAP


			FN_3(unwrap_getvertexpositionByNode,TYPE_POINT3, fnGetVertexPosition, TYPE_TIMEVALUE, TYPE_INT, TYPE_INODE);
			FN_1(unwrap_numberverticesByNode,TYPE_INT, fnNumberVertices,TYPE_INODE);
			FN_1(unwrap_getselectedpolygonsByNode,TYPE_BITARRAY, fnGetSelectedPolygons,TYPE_INODE);
			VFN_2(unwrap_selectpolygonsByNode, fnSelectPolygons,TYPE_BITARRAY,TYPE_INODE);
			FN_2(unwrap_ispolygonselectedByNode,TYPE_BOOL, fnIsPolygonSelected,TYPE_INT,TYPE_INODE);
			FN_1(unwrap_numberpolygonsByNode,TYPE_INT, fnNumberPolygons,TYPE_INODE);
			VFN_2(unwrap_markasdeadByNode, fnMarkAsDead,TYPE_INT,TYPE_INODE);
			FN_2(unwrap_numberpointsinfaceByNode,TYPE_INT,fnNumberPointsInFace,TYPE_INT,TYPE_INODE);
			FN_3(unwrap_getvertexindexfromfaceByNode,TYPE_INT,fnGetVertexIndexFromFace,TYPE_INT,TYPE_INT,TYPE_INODE);
			FN_3(unwrap_gethandleindexfromfaceByNode,TYPE_INT,fnGetHandleIndexFromFace,TYPE_INT,TYPE_INT,TYPE_INODE);
			FN_3(unwrap_getinteriorindexfromfaceByNode,TYPE_INT,fnGetInteriorIndexFromFace,TYPE_INT,TYPE_INT,TYPE_INODE);
			FN_3(unwrap_getvertexgindexfromfaceByNode,TYPE_INT,fnGetVertexGIndexFromFace,TYPE_INT,TYPE_INT,TYPE_INODE);
			FN_3(unwrap_gethandlegindexfromfaceByNode,TYPE_INT,fnGetHandleGIndexFromFace,TYPE_INT,TYPE_INT,TYPE_INODE);
			FN_3(unwrap_getinteriorgindexfromfaceByNode,TYPE_INT,fnGetInteriorGIndexFromFace,TYPE_INT,TYPE_INT,TYPE_INODE);

			VFN_5(unwrap_addpointtofaceByNode,fnAddPoint,TYPE_POINT3,TYPE_INT,TYPE_INT,TYPE_BOOL,TYPE_INODE);
			VFN_5(unwrap_addpointtohandleByNode,fnAddHandle,TYPE_POINT3,TYPE_INT,TYPE_INT,TYPE_BOOL,TYPE_INODE);
			VFN_5(unwrap_addpointtointeriorByNode,fnAddInterior,TYPE_POINT3,TYPE_INT,TYPE_INT,TYPE_BOOL,TYPE_INODE);

			VFN_4(unwrap_setfacevertexindexByNode,fnSetFaceVertexIndex,TYPE_INT,TYPE_INT,TYPE_INT,TYPE_INODE);
			VFN_4(unwrap_setfacehandleindexByNode,fnSetFaceHandleIndex,TYPE_INT,TYPE_INT,TYPE_INT,TYPE_INODE);
			VFN_4(unwrap_setfaceinteriorindexByNode,fnSetFaceInteriorIndex,TYPE_INT,TYPE_INT,TYPE_INT,TYPE_INODE);

			VFN_4(unwrap_getareaByNode,fnGetArea, TYPE_BITARRAY, 
							TYPE_FLOAT_BR,TYPE_FLOAT_BR, TYPE_INODE);
			VFN_6(unwrap_getboundsByNode,fnGetBounds, TYPE_BITARRAY, 
							TYPE_FLOAT_BR,TYPE_FLOAT_BR, 
							TYPE_FLOAT_BR,TYPE_FLOAT_BR, 
							TYPE_INODE);
			VFN_2(unwrap_pelt_setseamselectionByNode, fnSetSeamSelection,TYPE_BITARRAY,TYPE_INODE);
			FN_1(unwrap_pelt_getseamselectionByNode,TYPE_BITARRAY, fnGetSeamSelection,TYPE_INODE);
			VFN_3(unwrap_selectpolygonsupdateByNode, fnSelectPolygonsUpdate,TYPE_BITARRAY, TYPE_BOOL, TYPE_INODE);
			VFN_3(unwrap_sketchByNode, fnSketch,TYPE_INT_TAB,TYPE_POINT3_TAB,TYPE_INODE);
			FN_2(unwrap_getnormalByNode,TYPE_POINT3,fnGetNormal,TYPE_INT,TYPE_INODE);

			VFN_4(unwrap_setvertexpositionByNode, fnSetVertexPosition,TYPE_TIMEVALUE,TYPE_INT,TYPE_POINT3,TYPE_INODE);
			VFN_6(unwrap_setvertexposition2ByNode, fnSetVertexPosition2,TYPE_TIMEVALUE,TYPE_INT,TYPE_POINT3,TYPE_BOOL,TYPE_BOOL,TYPE_INODE);

			FN_2(unwrap_getvertexweightByNode,TYPE_FLOAT, fnGetVertexWeight,TYPE_INDEX,TYPE_INODE);
			VFN_3(unwrap_setvertexweightByNode,fnSetVertexWeight,TYPE_INT,TYPE_FLOAT,TYPE_INODE);
		
			FN_2(unwrap_isweightmodifiedByNode,TYPE_BOOL, fnIsWeightModified,TYPE_INT,TYPE_INODE);
			VFN_3(unwrap_modifyweightByNode,fnModifyWeight,TYPE_INT,TYPE_BOOL,TYPE_INODE);

			FN_1(unwrap_getselectedvertsByNode,TYPE_BITARRAY, fnGetSelectedVerts,TYPE_INODE);
			VFN_2(unwrap_selectvertsByNode, fnSelectVerts,TYPE_BITARRAY,TYPE_INODE);
			FN_2(unwrap_isvertexselectedByNode,TYPE_BOOL, fnIsVertexSelected,TYPE_INT,TYPE_INODE);

			FN_1(unwrap_getselectedfacesByNode,TYPE_BITARRAY, fnGetSelectedFaces,TYPE_INODE);
			VFN_2(unwrap_selectfacesByNode, fnSelectFaces,TYPE_BITARRAY,TYPE_INODE);
			FN_2(unwrap_isfaceselectedByNode,TYPE_BOOL, fnIsFaceSelected,TYPE_INT,TYPE_INODE);

			FN_1(unwrap_getselectededgesByNode,TYPE_BITARRAY, fnGetSelectedEdges,TYPE_INODE);
			VFN_2(unwrap_selectedgesByNode, fnSelectEdges,TYPE_BITARRAY,TYPE_INODE);
			FN_2(unwrap_isedgeselectedByNode,TYPE_BOOL, fnIsEdgeSelected,TYPE_INT,TYPE_INODE);

			VFN_2(unwrap_setgeomvertexselectionByNode, fnSetGeomVertexSelection,TYPE_BITARRAY,TYPE_INODE);
			FN_1(unwrap_getgeomvertexselectionByNode,TYPE_BITARRAY, fnGetGeomVertexSelection,TYPE_INODE);

			VFN_2(unwrap_setgeomedgeselectionByNode, fnSetGeomEdgeSelection,TYPE_BITARRAY,TYPE_INODE);
			FN_1(unwrap_getgeomedgeselectionByNode,TYPE_BITARRAY, fnGetGeomEdgeSelection,TYPE_INODE);


			VFN_1(unwrap_setsg,fnSetSG,TYPE_INT);
			FN_0(unwrap_getsg,TYPE_INT,fnGetSG);

			VFN_1(unwrap_setMatIDSelect,fnSetMatIDSelect,TYPE_INT);
			FN_0(unwrap_getMatIDSelect,TYPE_INT,fnGetMatIDSelect);

			VFN_0(unwrap_splinemap,fnSplineMap);

			VFN_2(unwrap_splinemap_selectSpline, fnSplineMap_SelectSpline,TYPE_INDEX,TYPE_BOOL);
			FN_1(unwrap_splinemap_isSplineSelected,TYPE_BOOL, fnSplineMap_IsSplineSelected,TYPE_INDEX);

			VFN_3(unwrap_splinemap_selectCrossSection, fnSplineMap_SelectCrossSection,TYPE_INDEX,TYPE_INDEX,TYPE_BOOL);
			FN_2(unwrap_splinemap_isCrossSectionSelected,TYPE_BOOL, fnSplineMap_IsCrossSectionSelected,TYPE_INDEX,TYPE_INDEX);


			VFN_0(unwrap_splinemap_clearSelectSpline, fnSplineMap_ClearSplineSelection);
			VFN_0(unwrap_splinemap_clearSelectCrossSection, fnSplineMap_ClearCrossSectionSelection);

			FN_0(unwrap_splinemap_numberOfSplines, TYPE_INT, fnSplineMap_NumberOfSplines);
			FN_1(unwrap_splinemap_numberOfCrossSections, TYPE_INT, fnSplineMap_NumberOfCrossSections,TYPE_INDEX);


			FN_2(unwrap_splinemap_getCrossSection_Pos, TYPE_POINT3, fnSplineMap_GetCrossSection_Pos,TYPE_INDEX,TYPE_INDEX);
			FN_2(unwrap_splinemap_getCrossSection_ScaleX, TYPE_FLOAT, fnSplineMap_GetCrossSection_ScaleX,TYPE_INDEX,TYPE_INDEX);
			FN_2(unwrap_splinemap_getCrossSection_ScaleY, TYPE_FLOAT, fnSplineMap_GetCrossSection_ScaleY,TYPE_INDEX,TYPE_INDEX);
			FN_2(unwrap_splinemap_getCrossSection_Twist, TYPE_QUAT, fnSplineMap_GetCrossSection_Twist,TYPE_INDEX,TYPE_INDEX);
			
			
			
			VFN_3(unwrap_splinemap_setCrossSection_Pos,  fnSplineMap_SetCrossSection_Pos,TYPE_INDEX,TYPE_INDEX,TYPE_POINT3);
			VFN_3(unwrap_splinemap_setCrossSection_ScaleX,  fnSplineMap_SetCrossSection_ScaleX,TYPE_INDEX,TYPE_INDEX,TYPE_FLOAT);
			VFN_3(unwrap_splinemap_setCrossSection_ScaleY,  fnSplineMap_SetCrossSection_ScaleY,TYPE_INDEX,TYPE_INDEX,TYPE_FLOAT);
			VFN_3(unwrap_splinemap_setCrossSection_Twist,  fnSplineMap_SetCrossSection_Twist,TYPE_INDEX,TYPE_INDEX,TYPE_QUAT);

			VFN_0(unwrap_splinemap_recomputeCrossSections,fnSplineMap_RecomputeCrossSections);

			VFN_2(unwrap_splinemap_fit,fnSplineMap_Fit,TYPE_BOOL,TYPE_FLOAT);			
			VFN_1(unwrap_splinemap_alignSelected,fnSplineMap_AlignSelected,TYPE_POINT3);
			VFN_3(unwrap_splinemap_align,fnSplineMap_Align,TYPE_INDEX,TYPE_INDEX,TYPE_POINT3);
			VFN_0(unwrap_splinemap_aligncommandmode, fnSplineMap_AlignFaceCommandMode);
			VFN_0(unwrap_splinemap_alignsectioncommandmode, fnSplineMap_AlignSectionCommandMode);
			


			VFN_0(unwrap_splinemap_copy, fnSplineMap_Copy);
			VFN_0(unwrap_splinemap_paste, fnSplineMap_Paste);
			VFN_2(unwrap_splinemap_pasteToSelected, fnSplineMap_PasteToSelected,TYPE_INDEX,TYPE_INDEX);
			

			VFN_0(unwrap_splinemap_delete, fnSplineMap_DeleteSelectedCrossSections);

			VFN_0(unwrap_splinemap_addCrossSectionCommandmode, fnSplineMap_AddCrossSectionCommandMode);

			VFN_2(unwrap_splinemap_insertCrossSection, fnSplineMap_InsertCrossSection,TYPE_INDEX,TYPE_FLOAT);


			VFN_0(unwrap_splinemap_startMapMode, fnSplineMap_StartMapMode);	
			VFN_0(unwrap_splinemap_endMapMode, fnSplineMap_EndMapMode);	
	
			VFN_1(unwrap_splinemap_moveSelectedCrossSection, fnSplineMap_MoveSelectedCrossSections,TYPE_POINT3);	
			VFN_1(unwrap_splinemap_rotateSelectedCrossSection, fnSplineMap_RotateSelectedCrossSections, TYPE_QUAT);	
			VFN_1(unwrap_splinemap_scaleSelectedCrossSection, fnSplineMap_ScaleSelectedCrossSections,TYPE_POINT2);	

			VFN_0(unwrap_splinemap_dump, fnSplineMap_Dump);	

			VFN_1(unwrap_splinemap_resample, fnSplineMap_Resample,TYPE_INT);	


			FN_0(unwrap_gettweakmode,TYPE_BOOL, fnGetTweakMode);
			VFN_1(unwrap_settweakmode, fnSetTweakMode,TYPE_BOOL);

			VFN_1(unwrap_uvloop, fnUVLoop,TYPE_INT);
			VFN_1(unwrap_uvring, fnUVRing,TYPE_INT);
			
			VFN_1(unwrap_align, fnAlign,TYPE_BOOL);
			VFN_1(unwrap_space, fnSpace,TYPE_BOOL);

			VFN_2(unwrap_regularmap_start, fnRegularMapStart,TYPE_INODE, TYPE_BOOL);
			VFN_0(unwrap_regularmap_reset, fnRegularMapReset);
			VFN_5(unwrap_regularmap_advanceUV, fnRegularMapAdvanceUV,TYPE_BOOL, TYPE_BOOL,TYPE_BOOL, TYPE_BOOL, TYPE_BOOL);
			VFN_1(unwrap_regularmap_advanceSelected, fnRegularMapAdvanceSelected, TYPE_BOOL);
			VFN_0(unwrap_regularmap_end, fnRegularMapEnd);
			FN_0(unwrap_regularmap_getnormalize, TYPE_BOOL, fnRegularMapGetNormalize);
			VFN_1(unwrap_regularmap_setnormalize, fnRegularMapSetNormalize,TYPE_BOOL);


			FN_0(unwrap_regularmap_getautoweld, TYPE_BOOL, fnRegularMapGetAutoWeld);
			VFN_1(unwrap_regularmap_setautoweld, fnRegularMapSetAutoWeld,TYPE_BOOL);

			FN_0(unwrap_regularmap_getautoweldthreshold, TYPE_FLOAT, fnRegularMapGetAutoWeldThreshold);
			VFN_1(unwrap_regularmap_setautoweldthreshold, fnRegularMapSetAutoWeldThreshold,TYPE_FLOAT);

			FN_0(unwrap_regularmap_geticonsize, TYPE_FLOAT, fnRegularMapGetIconSize);
			VFN_1(unwrap_regularmap_seticonsize, fnRegularMapSetIconSize,TYPE_FLOAT);

			FN_0(unwrap_regularmap_getsinglestep, TYPE_BOOL, fnRegularMapGetSingleStep);
			VFN_1(unwrap_regularmap_setsinglestep, fnRegularMapSetSingleStep,TYPE_BOOL);

			FN_0(unwrap_regularmap_getlimit, TYPE_INT, fnRegularMapGetLimit);
			VFN_1(unwrap_regularmap_setlimit, fnRegularMapSetLimit,TYPE_INT);

			FN_0(unwrap_regularmap_getautofit, TYPE_INT, fnRegularMapGetAutoFit);
			VFN_1(unwrap_regularmap_setautofit, fnRegularMapSetAutoFit,TYPE_INT);

			VFN_0(unwrap_regularmap_resetfaces, fnRegularMapResetFaces);
			VFN_2(unwrap_regularmap_startnewcluster, fnRegularMapStartNewCluster,TYPE_INODE, TYPE_INDEX);
			
			// smgrp, planar, edge selection, face selecion
			VFN_1(unwrap_regularmap_expand, fnRegularMapExpand, TYPE_ENUM );


			FN_2(unwrap_ispinned, TYPE_BOOL, fnIsPinned,TYPE_INDEX, TYPE_INODE);
			VFN_2(unwrap_pin, fnPin,TYPE_INDEX, TYPE_INODE);
			VFN_2(unwrap_unpin, fnUnpin,TYPE_INDEX, TYPE_INODE);

			VFN_1(unwrap_pinselected, fnPinSelected,TYPE_INODE);
			VFN_1(unwrap_unpinselected, fnUnpinSelected,TYPE_INODE);


			VFN_1(unwrap_lscm_interactive, fnLSCMInteractive,TYPE_BOOL);
			VFN_0(unwrap_lscm_solve, fnLSCMSolve);
			VFN_0(unwrap_lscm_reset, fnLSCMReset);

			VFN_2(unwrap_rescalecluster, fnRescaleCluster,TYPE_BITARRAY,TYPE_INODE);

			FN_0(unwrap_getpackrescalecluster, TYPE_BOOL, fnGetPackRescaleCluster);
			VFN_1(unwrap_setpackrescalecluster, fnSetPackRescaleCluster,TYPE_BOOL);

			VFN_1(unwrap_showtoolbar, fnShowToolBar,TYPE_BOOL);

			VFN_1(unwrap_addfloater,fnAddFloater,TYPE_STRING);
			VFN_1(unwrap_deletefloater,fnDeleteFloater,TYPE_STRING);
			VFN_2(unwrap_showfloater,fnShowFloater,TYPE_STRING, TYPE_BOOL);

			VFN_0(unwrap_weldallshared,fnWeldAllShared);
			VFN_0(unwrap_weldselectedshared,fnWeldSelectedShared);
			VFN_0(unwrap_relaxoneclick,fnRelaxOneClick);

			VFN_1(unwrap_group_create,fnGroupCreate,TYPE_STRING);
			VFN_1(unwrap_group_delete,fnGroupDelete,TYPE_STRING);
			VFN_2(unwrap_group_rename,fnGroupRename,TYPE_STRING,TYPE_STRING);
			VFN_1(unwrap_group_select,fnGroupSelect,TYPE_STRING);


			VFN_0(unwrap_group_create_byselection,fnGroupCreateBySelection);
			VFN_0(unwrap_group_delete_byselection,fnGroupDeleteBySelection);
			VFN_0(unwrap_group_select_byselection,fnGroupSelectBySelection);
			VFN_1(unwrap_group_settexeldensity,fnGroupSetTexelDensity,TYPE_FLOAT);
			FN_0(unwrap_group_gettexeldensity,TYPE_FLOAT,fnGroupGetTexelDensity);

			//where, name, pos, x,y, w, popup
			VFN_7(unwrap_addtoolbar, fnAddToolBar,TYPE_INT, TYPE_STRING,TYPE_INT, TYPE_INT, TYPE_INT,TYPE_INT, TYPE_BOOL);
			
			VFN_0(unwrap_straighten, fnStraighten);

			VFN_3(unwrap_flatten_bysmoothinggroup, fnFlattenBySmoothingGroup,TYPE_BOOL,TYPE_BOOL,TYPE_FLOAT);
			VFN_3(unwrap_flatten_bymatid, fnFlattenByMaterialID,TYPE_BOOL,TYPE_BOOL,TYPE_FLOAT);


		END_FUNCTION_MAP

		FPInterfaceDesc* GetDesc();    // <-- must implement 

		virtual Point3* fnGetVertexPosition(TimeValue t,  int index, INode *node)=0;	
		virtual int		fnNumberVertices(INode *node) = 0;
		virtual BitArray* fnGetSelectedPolygons(INode *node) = 0;
		virtual void fnSelectPolygons(BitArray *sel, INode *node) = 0;
		virtual BOOL fnIsPolygonSelected(int index, INode *node) = 0;
		virtual int	fnNumberPolygons(INode *node) = 0;
		virtual void fnMarkAsDead(int index, INode *node) = 0;
		virtual int fnNumberPointsInFace(int index, INode *node) = 0;			
		virtual int fnGetVertexIndexFromFace(int index,int vertexIndex, INode *node) = 0;
		virtual int fnGetHandleIndexFromFace(int index,int vertexIndex, INode *node) = 0;
		virtual int fnGetInteriorIndexFromFace(int index,int vertexIndex, INode *node) = 0;

		virtual int	fnGetVertexGIndexFromFace(int index,int vertexIndex, INode *node)=0;
		virtual int	fnGetHandleGIndexFromFace(int index,int vertexIndex, INode *node)=0;
		virtual int	fnGetInteriorGIndexFromFace(int index,int vertexIndex, INode *node)=0;

		virtual void fnAddPoint(Point3 pos, int fIndex,int ithV, BOOL sel, INode *node)=0;
		virtual void fnAddHandle(Point3 pos, int fIndex,int ithV, BOOL sel, INode *node)=0;
		virtual void fnAddInterior(Point3 pos, int fIndex,int ithV, BOOL sel, INode *node)=0;

		virtual void fnSetFaceVertexIndex(int fIndex,int ithV, int vIndex, INode *node)=0;
		virtual void fnSetFaceHandleIndex(int fIndex,int ithV, int vIndex, INode *node)=0;
		virtual void fnSetFaceInteriorIndex(int fIndex,int ithV, int vIndex, INode *node)=0;
		
		virtual void fnGetBounds(BitArray *faceSelection, 
						  float &x, float &y,
						  float &width, float &height,
						  INode *node) = 0;
		virtual void fnGetArea(BitArray *faceSelection, 
						  float &uvArea, float &geomArea,
						  INode *node) = 0;

		virtual void fnSetSeamSelection(BitArray *sel, INode *node) = 0;
		virtual BitArray* fnGetSeamSelection(INode *node) = 0;
		virtual void fnSelectPolygonsUpdate(BitArray *sel, BOOL update, INode *node) = 0;
		virtual void  fnSketch(Tab<int> *indexList,Tab<Point3*> *positionList,INode *node) = 0;
		virtual Point3*  fnGetNormal(int faceIndex, INode *node) = 0;
		virtual void fnSetVertexPosition2(TimeValue t, int index, Point3 pos, BOOL hold, BOOL update, INode *node) = 0;
		virtual void fnSetVertexPosition(TimeValue t, int index, Point3 pos, INode *node) = 0;
		virtual void fnSetVertexWeight(int index,float weight, INode *node) = 0;
		virtual float fnGetVertexWeight(int index, INode *node) = 0;
		virtual void fnModifyWeight(int index, BOOL modified, INode *node) = 0;
		virtual BOOL fnIsWeightModified(int index, INode *node) = 0;

		virtual BitArray* fnGetSelectedVerts(INode *node) = 0;
		virtual void fnSelectVerts(BitArray *sel, INode *node) = 0;
		virtual BOOL fnIsVertexSelected(int index, INode *node) = 0;

		virtual BitArray* fnGetSelectedFaces(INode *node) = 0;
		virtual void fnSelectFaces(BitArray *sel,INode *node) = 0;
		virtual BOOL fnIsFaceSelected(int index, INode *node) = 0;

		virtual BitArray* fnGetSelectedEdges(INode *node) = 0;
		virtual void fnSelectEdges(BitArray *sel, INode *node) = 0;
		virtual BOOL fnIsEdgeSelected(int index, INode *node) = 0;

		virtual void fnSetGeomVertexSelection(BitArray *sel, INode *node) = 0;
		virtual BitArray* fnGetGeomVertexSelection(INode *node) = 0;

		virtual BitArray* fnGetGeomEdgeSelection(INode *node) = 0;
		virtual void fnSetGeomEdgeSelection(BitArray *sel, INode *node) = 0;

		virtual int		fnGetSG()=0;
		virtual void	fnSetSG(int sg)=0;

		virtual int		fnGetMatIDSelect()=0;
		virtual void	fnSetMatIDSelect(int matID)=0;
//R11 function that mainly deal with spline mapping

		//this takes the existing and a face selection and applies a spline mapping
		//projection to the faces
		virtual void	fnSplineMap()=0;


		//these are used to handle spline element selection
		virtual void fnSplineMap_SelectSpline(int splineIndex, BOOL sel) = 0;
		virtual BOOL fnSplineMap_IsSplineSelected(int splineIndex) = 0;

		//this gets and set spline element selection
		virtual void fnSplineMap_SelectCrossSection(int splineIndex, int crossIndex, BOOL sel) = 0;
		virtual BOOL fnSplineMap_IsCrossSectionSelected(int splineIndex,int crossIndex) = 0;
		
		//this clears the spline selection
		virtual void fnSplineMap_ClearSplineSelection() = 0;
		//this clears the cross section selections
		virtual void fnSplineMap_ClearCrossSectionSelection() = 0;
		
		//returns the number of splines 
		virtual int fnSplineMap_NumberOfSplines() = 0;
		//returns the number of cross sections for a spline
		virtual int fnSplineMap_NumberOfCrossSections(int splineIndex) = 0;

		//these get and set the cross sections properties
		//these do not recompute the the acceleration structures
		//so fnSplineMap_RecomputeCrossSections should be called after calling the sets
		virtual Point3 fnSplineMap_GetCrossSection_Pos(int splineIndex, int crossSectionIndex) = 0;
		virtual float fnSplineMap_GetCrossSection_ScaleX(int splineIndex, int crossSectionIndex) = 0;
		virtual float fnSplineMap_GetCrossSection_ScaleY(int splineIndex, int crossSectionIndex) = 0;
		virtual Quat fnSplineMap_GetCrossSection_Twist(int splineIndex, int crossSectionIndex) = 0;

		virtual void fnSplineMap_SetCrossSection_Pos(int splineIndex, int crossSectionIndex, Point3 p) = 0;
		virtual void fnSplineMap_SetCrossSection_ScaleX(int splineIndex, int crossSectionIndex, float scaleX) = 0;
		virtual void fnSplineMap_SetCrossSection_ScaleY(int splineIndex, int crossSectionIndex, float scaleY) = 0;
		virtual void fnSplineMap_SetCrossSection_Twist(int splineIndex, int crossSectionIndex, Quat twist) = 0;

		//this recomputes acceleration structures, matrices, bounding boxes etc.
		//this needs to be called any time a cross section or spline chanhes
		virtual void fnSplineMap_RecomputeCrossSections() = 0;

		//this sill fit the cross sections to surround the nearest faces
		//	fitAll will fit all the sections, otherwise it will only fit selected cross sections
		//	extraScale is a fudge factor to expand the cross sections
		virtual void fnSplineMap_Fit(BOOL fitAll, float extraScale) = 0;
		//this aligns the selected cross sections to a vec, note the cross section is still
		//constrained to the splines path
		//	vec is in world space
		virtual void fnSplineMap_AlignSelected(Point3 vec) = 0;
		//this aligns a spec cross section to a vector
		//	vec is in world space
		virtual void fnSplineMap_Align(int splineIndex, int crossSectionIndex, Point3 vec) = 0;

		//these will start/stop the spline map modes
		//this is an align to face command mode where the selected cross sections are aligned to
		//a face
		virtual void fnSplineMap_AlignFaceCommandMode() = 0;
		//this command modes aligns the selected cross sections to the picked cross sections
		virtual void fnSplineMap_AlignSectionCommandMode() = 0;
		//this command mode allows the user to insert a cross section on a spline
		virtual void fnSplineMap_AddCrossSectionCommandMode() = 0;

		//this takes the selected cross section and stores it in a buffer
		virtual void fnSplineMap_Copy() = 0;
		//this copies the buffer cross section into the selected cross sections
		virtual void fnSplineMap_Paste() = 0;
		//this takes a cross section and pastes it onto the selected cross sections
		//	splineIndex, crossSectionIndex are the cross section to copy from
		virtual void fnSplineMap_PasteToSelected(int splineIndex, int crossSectionIndex) = 0;

		//this deletes the selected cross cross sections
		virtual void fnSplineMap_DeleteSelectedCrossSections() = 0;
		//this inserts a cross section into a spline
		//	splineIndex the index of the spline to insert at
		//	u where along the spline to insert the cross section at
		virtual void fnSplineMap_InsertCrossSection(int splineIndex, float u) = 0;

		//needs to be called when you start spline mapping
		//this brings up the dialog and builds some temp data
		virtual void fnSplineMap_StartMapMode() = 0;
		//needs to be called when the user is done with spline mapping
		virtual void fnSplineMap_EndMapMode() = 0;

		//moves the selected cross sections 
		//this distance is world space distance along the spline
		virtual void fnSplineMap_MoveSelectedCrossSections(Point3 v) = 0;
		//rotates the selected cross sections in radians
		virtual void fnSplineMap_RotateSelectedCrossSections(Quat val) = 0;
		//scales the selected cross sections 
		virtual void fnSplineMap_ScaleSelectedCrossSections(Point2 scale) = 0;
		virtual void fnSplineMap_Dump() = 0;

		//this resamples down all our spline cross sections
		virtual void fnSplineMap_Resample(int samples) = 0;

                //these let you get/set the UVW Tweak mode
		virtual BOOL	fnGetTweakMode() = 0;
		virtual void	fnSetTweakMode(BOOL mode) = 0;


		//these are step wise loop and ring methods, setting the mode to -1 shrinks to the loop/ring, 1 grows the loop/ring
		//0 does a full loop/ring
		virtual void	fnUVLoop(int mode) = 0;
		virtual void	fnUVRing(int mode) = 0;

		//function align or space the selection horizontally or vertically
		//they have no affect in face subobject mode.
		virtual void	fnAlign(BOOL horizontal) = 0;
		virtual void	fnSpace(BOOL horizontal) = 0;
		
		
		virtual void fnRegularMapStart(INode *node, BOOL bringUpUI)=0;	
		virtual void fnRegularMapReset()=0;	
		virtual void fnRegularMapAdvanceUV(BOOL uPosDir, BOOL vPosDir, BOOL uNegDir, BOOL vNegDir, BOOL singleStep)=0;	
		virtual void fnRegularMapAdvanceSelected(BOOL singleStep)=0;	
		virtual void fnRegularMapEnd()=0;	
		virtual BOOL fnRegularMapGetNormalize() = 0;
		virtual void fnRegularMapSetNormalize(BOOL normalize) = 0;

		virtual BOOL fnRegularMapGetAutoWeld() = 0;
		virtual void fnRegularMapSetAutoWeld(BOOL autoWeld) = 0;

		virtual float fnRegularMapGetAutoWeldThreshold() = 0;
		virtual void fnRegularMapSetAutoWeldThreshold(float autoWeldThreshold) = 0;

		virtual float fnRegularMapGetIconSize() = 0;
		virtual void fnRegularMapSetIconSize(float iconSize) = 0;

		virtual BOOL fnRegularMapGetSingleStep() = 0;
		virtual void fnRegularMapSetSingleStep(BOOL singleStep) = 0;

		virtual int fnRegularMapGetLimit() = 0;
		virtual void fnRegularMapSetLimit(int limit) = 0;
		

		virtual int fnRegularMapGetAutoFit() = 0;
		virtual void fnRegularMapSetAutoFit(int autofit) = 0;

		virtual void fnRegularMapResetFaces() = 0;

		virtual void fnRegularMapStartNewCluster(INode *node, int index) = 0;

		virtual void fnRegularMapExpand(int expandBy) = 0;



		virtual BOOL fnIsPinned(int index, INode *node) = 0;
		virtual void fnPin(int index, INode *node) = 0;
		virtual void fnUnpin(int index, INode *node) = 0;

		virtual void fnPinSelected(INode *node) = 0;
		virtual void fnUnpinSelected(INode *node) = 0;

		virtual void fnLSCMInteractive(BOOL start) = 0;
		virtual void fnLSCMSolve() = 0;
		virtual void fnLSCMReset() = 0;

	
		virtual void fnRescaleCluster(BitArray *sel, INode *node)=0;
		virtual BOOL	fnGetPackRescaleCluster() = 0;
		virtual void	fnSetPackRescaleCluster(BOOL rescale) = 0;

		virtual void	fnShowToolBar(BOOL visible)=0;

		virtual void	fnAddFloater(const MCHAR* name)=0;
		virtual void	fnDeleteFloater(const MCHAR* name)=0;
		virtual void	fnShowFloater(const MCHAR* name, BOOL show)=0;

		virtual void	fnWeldAllShared() = 0;
		virtual void	fnWeldSelectedShared() = 0;

		virtual	void	fnRelaxOneClick() = 0;

		//creates the group with the name that are part of the current face selection
		virtual void fnGroupCreate(const MCHAR *name) = 0;
		//deletes the group matching the name
		virtual void fnGroupDelete(const MCHAR *name) = 0;
		//renames a group
		virtual void fnGroupRename(const MCHAR *name,const MCHAR *newName) = 0;
		//selects the faces of the group matching the name
		virtual void fnGroupSelect(const MCHAR *name) = 0;

		//creates the group that are part of the current face slection
		virtual void fnGroupCreateBySelection() = 0;
		//deletes the groups that are part of the current face slection
		virtual void fnGroupDeleteBySelection() = 0;
		//selects the group based on the current face selection
		virtual void fnGroupSelectBySelection() = 0;
		//sets the texel density for the current face selection
		virtual void fnGroupSetTexelDensity(float val) = 0;
		//gets the texel density for the current face selection
		virtual float fnGroupGetTexelDensity() = 0;

		 virtual void   fnAddToolBar(int owner,const MCHAR* name, int pos, int x, int y, int width, BOOL popup) = 0;

		 virtual void	fnStraighten() = 0;
		 

		 virtual void fnFlattenBySmoothingGroup(BOOL rotate, BOOL rescale, float padding) = 0;
		 virtual void fnFlattenByMaterialID(BOOL rotate, BOOL rescale, float padding) = 0;

		 
		 virtual void fnAlignByPivotVertical() = 0;
		 virtual void fnAlignByPivotHorizontal() = 0;
		

};



#endif