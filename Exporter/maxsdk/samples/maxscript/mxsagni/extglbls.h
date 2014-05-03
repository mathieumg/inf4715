define_system_global_replace ( _T("rootNode"),				get_root_node,				set_root_node);
define_system_global ( _T("rendOutputFilename"),	get_rend_output_filename,	set_rend_output_filename);
define_system_global ( _T("rendUseActiveView"),		get_rend_useActiveView,		set_rend_useActiveView);
// renderViewIndex is deprecated in 3ds Max2013, use renderViewID instead.
define_system_global ( _T("rendViewIndex"),			get_rend_viewIndex,			set_rend_viewIndex);
define_system_global ( _T("rendViewID"),			get_rend_viewID,			set_rend_viewID);
define_system_global ( _T("rendCamNode"),			get_rend_camNode,			set_rend_camNode);
define_system_global ( _T("rendUseImgSeq"),			get_rend_useImgSeq,			set_rend_useImgSeq);
define_system_global ( _T("rendImgSeqType"),		get_rend_imgSeqType,		set_rend_imgSeqType);

define_system_global ( _T("preRendScript"),			get_rend_preRendScript,		set_rend_preRendScript);
define_system_global ( _T("usePreRendScript"),		get_rend_usePreRendScript,	set_rend_usePreRendScript);
define_system_global ( _T("localPreRendScript"),	get_rend_localPreRendScript,set_rend_localPreRendScript);
define_system_global ( _T("postRendScript"),		get_rend_postRendScript,	set_rend_postRendScript);
define_system_global ( _T("usePostRendScript"),		get_rend_usePostRendScript,	set_rend_usePostRendScript);

define_system_global ( _T("renderPresetMRUList"),	get_renderPresetMRUList,	NULL);
// JOHNSON RELEASE SDK
//define_system_global ( "rendUseIterative",		get_rend_useIterative,		set_rend_useIterative);

define_system_global ( _T("realTimePlayback"),		get_real_time_playback,		set_real_time_playback);
define_system_global ( _T("playActiveOnly"),		get_play_active_only,		set_play_active_only);
define_system_global ( _T("playbackLoop"),			get_playback_loop,			set_playback_loop);
define_system_global ( _T("animButtonEnabled"),		get_enable_animate_button,	set_enable_animate_button);
define_system_global ( _T("animButtonState"),		get_animate_button_state,	set_animate_button_state);
define_system_global ( _T("flyOffTime"),			get_fly_off_time,			set_fly_off_time);
//define_system_global( "sliderPrecision",		get_slider_precision,		set_slider_precision);

#ifndef WEBVERSION
define_system_global ( _T("renderDisplacements"),	get_renderDisplacements,	set_renderDisplacements);
define_system_global_replace ( _T("renderEffects"),			get_renderEffects,			set_renderEffects);
define_system_global ( _T("skipRenderedFrames"),	get_skipRenderedFrames,		set_skipRenderedFrames);
#endif // WEBVERSION

define_system_global ( _T("showEndResult"),			get_showEndResult,			set_showEndResult);

define_struct_global ( _T("useTransformGizmos"),	_T("preferences"),			get_xform_gizmos,     set_xform_gizmos);			
define_struct_global ( _T("constantReferenceSystem"), _T("preferences"),		get_constant_axis,	  set_constant_axis);
define_struct_global ( _T("useVertexDots"),			_T("preferences"),			get_useVertexDots,	  set_useVertexDots);
define_struct_global ( _T("useLargeVertexDots"),	_T("preferences"),			get_vertexDotType,	  set_vertexDotType);
define_struct_global ( _T("flyOffTime"),			_T("preferences"),			get_fly_off_time,	  set_fly_off_time);
define_struct_global ( _T("spinnerWrap"),			_T("preferences"),			get_spinner_wrap,	  set_spinner_wrap);

define_struct_global ( _T("spinnerPrecision"),		_T("preferences"),			get_spinner_precision,set_spinner_precision);
define_struct_global ( _T("spinnerSnap"),			_T("preferences"),			get_spinner_snap,	  set_spinner_snap);
define_struct_global ( _T("useSpinnerSnap"),		_T("preferences"),			get_spinner_useSnap,  set_spinner_useSnap);

#ifndef WEBVERSION	// russom - 04/16/02
define_struct_global ( _T("maximumGBufferLayers")	, _T("preferences"),		get_maxGBufferLayers, set_maxGBufferLayers);
#endif

// LAM - 8/29/03
#ifdef ECO1144
define_struct_global ( _T("DontRepeatRefMsg")	, _T("preferences"),			get_DontRepeatRefMsg, set_DontRepeatRefMsg);
#endif

define_struct_global ( _T("InvalidateTMOpt")	, _T("preferences"),	get_InvalidateTMOpt, set_InvalidateTMOpt);

define_struct_global ( _T("useTrackBar"),			_T("timeConfiguration"),	get_useTrackBar,			set_useTrackBar);		    
define_struct_global ( _T("playActiveOnly"),		_T("timeConfiguration"),	get_play_active_only,		set_play_active_only);
define_struct_global ( _T("realTimePlayback"),		_T("timeConfiguration"),	get_real_time_playback,		set_real_time_playback);
define_struct_global ( _T("playbackLoop"),			_T("timeConfiguration"),	get_playback_loop,			set_playback_loop);

// Log System
define_struct_global ( _T("quietMode"),				_T("logsystem"),			get_quiet_mode,				set_quiet_mode);

//mcr_global_def


