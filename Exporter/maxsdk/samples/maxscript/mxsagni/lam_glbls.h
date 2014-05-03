// from avg_dlx
#include "avg_maxver.h" // defines MAGMA 

#include "mxs_units.h"

define_system_global( _T("listener"),			get_listener,			NULL);
define_system_global( _T("macroRecorder"),		get_macroRecorder,		NULL);
define_system_global( _T("superclasses"),		get_superclasses,		NULL);
#ifndef MAGMA
define_system_global( _T("avguard_dlx_ver"),	get_avguard_dlx_ver,	NULL);
#endif // MAGMA

define_struct_global (_T("fileName"),	_T("WAVsound"), getSoundFileName,	setSoundFileName);
define_struct_global (_T("start"),		_T("WAVsound"), getSoundStartTime,	setSoundStartTime);
define_struct_global (_T("end"),		_T("WAVsound"), getSoundEndTime,	NULL);
define_struct_global (_T("mute"),		_T("WAVsound"), getSoundMute,		setSoundMute);
define_struct_global (_T("isPlaying"),	_T("WAVsound"), getSoundIsPlaying,	NULL);

define_struct_global (_T("playbackSpeed"),	_T("timeConfiguration"), getPlaybackSpeed,	setPlaybackSpeed);

define_struct_global (_T("mode"),			_T("mouse"),	getMouseMode,			NULL);
define_struct_global (_T("pos"),			_T("mouse"),	getMouseMAXPos,			NULL);
define_struct_global (_T("screenpos"),		_T("mouse"),	getMouseScreenPos,		NULL);
define_struct_global (_T("buttonStates"),	_T("mouse"),	getMouseButtonStates,	NULL);
define_struct_global (_T("inAbort"),		_T("mouse"),	getInMouseAbort,		setInMouseAbort);

define_system_global( _T("rendTimeType"),	get_RendTimeType,	set_RendTimeType);
define_system_global( _T("rendStart"),		get_RendStart,		set_RendStart);
define_system_global( _T("rendEnd"),		get_RendEnd,		set_RendEnd);
define_system_global( _T("rendNThFrame"),	get_RendNThFrame,	set_RendNThFrame);
define_system_global( _T("rendHidden"),		get_RendHidden,		set_RendHidden);
define_system_global( _T("rendForce2Side"),	get_RendForce2Side,	set_RendForce2Side);

define_system_global( _T("rendSaveFile"),	get_RendSaveFile,	set_RendSaveFile);

#ifndef WEBVERSION
define_system_global( _T("rendShowVFB"),	get_RendShowVFB,	set_RendShowVFB);
define_system_global( _T("rendUseDevice"),	get_RendUseDevice,	set_RendUseDevice);
define_system_global( _T("rendUseNet"),		get_RendUseNet,		set_RendUseNet);
define_system_global( _T("rendFieldRender"),get_RendFieldRender,set_RendFieldRender);
define_system_global( _T("rendColorCheck"),	get_RendColorCheck,	set_RendColorCheck);
define_system_global( _T("rendSuperBlack"),	get_RendSuperBlack,	set_RendSuperBlack);
define_system_global( _T("rendSuperBlackThresh"),	get_RendSuperBlackThresh,	set_RendSuperBlackThresh);
define_system_global( _T("rendAtmosphere"),	get_RendAtmosphere,	set_RendAtmosphere);
 
define_system_global( _T("rendDitherTrue"),	get_RendDitherTrue,	set_RendDitherTrue);
define_system_global( _T("rendDither256"),	get_RendDither256,	set_RendDither256);
define_system_global( _T("rendMultiThread"),get_RendMultiThread,set_RendMultiThread);
define_system_global( _T("rendNThSerial"),	get_RendNThSerial,	set_RendNThSerial);
define_system_global( _T("rendVidCorrectMethod"),	get_RendVidCorrectMethod,	set_RendVidCorrectMethod);
 
define_system_global( _T("rendFieldOrder"),	get_RendFieldOrder,	set_RendFieldOrder);
define_system_global( _T("rendNTSC_PAL"),	get_RendNTSC_PAL,	set_RendNTSC_PAL);
define_system_global( _T("rendLockImageAspectRatio"),	get_LockImageAspRatio,	set_LockImageAspRatio);
define_system_global( _T("rendImageAspectRatio"),	get_ImageAspRatio,	set_ImageAspRatio);
define_system_global( _T("rendLockPixelAspectRatio"),	get_LockPixelAspRatio,	set_LockPixelAspRatio);
define_system_global( _T("rendPixelAspectRatio"),	get_PixelAspRatio,	set_PixelAspRatio);
define_system_global( _T("rendSimplifyAreaLights"),  get_RendSimplifyAreaLights,      set_RendSimplifyAreaLights);
#endif // WEBVERSION
define_system_global( _T("rendFileNumberBase"),		get_RendFileNumberBase,		set_RendFileNumberBase);
define_system_global( _T("rendPickupFrames"),		get_RendPickupFrames,		set_RendPickupFrames);

define_struct_global (_T("CommandMode"),	_T("toolMode"), getCommandMode,		setCommandMode);
define_struct_global (_T("CommandModeID"),	_T("toolMode"), getCommandModeID,	NULL);
define_struct_global (_T("AxisConstraints"),_T("toolMode"), getAxisConstraints,	setAxisConstraints);

define_struct_global (_T("DisplayType"),	_T("units"), getUnitDisplayType,	setUnitDisplayType);
#ifndef USE_HARDCODED_SYSTEM_UNIT
define_struct_global (_T("SystemType"),		_T("units"), getUnitSystemType,		setUnitSystemType);
define_struct_global (_T("SystemScale"),	_T("units"), getUnitSystemScale,	setUnitSystemScale);
#endif
define_struct_global (_T("MetricType"),		_T("units"), getMetricDisplay,		setMetricDisplay);
define_struct_global (_T("USType"),			_T("units"), getUSDisplay,			setUSDisplay);
define_struct_global (_T("USFrac"),			_T("units"), getUSFrac,				setUSFrac);

#ifndef WEBVERSION
define_struct_global (_T("CustomName"),		_T("units"), getCustomName,			setCustomName);
define_struct_global (_T("CustomValue"),	_T("units"), getCustomValue,		setCustomValue);
define_struct_global (_T("CustomUnit"),		_T("units"), getCustomUnit,			setCustomUnit);
#endif //WEBVERSION

define_struct_global (_T("enabled"),	_T("autoBackup"),	getAutoBackupEnabled,	setAutoBackupEnabled);
define_struct_global (_T("time"),		_T("autoBackup"),	getAutoBackupTime,		setAutoBackupTime);

define_system_global( _T("productAppID"),	get_productAppID,			NULL);

define_struct_global (_T("current"),	_T("renderers"), get_CurrentRenderer,		set_CurrentRenderer);
define_struct_global (_T("production"),	_T("renderers"), get_ProductionRenderer,	set_ProductionRenderer);
define_struct_global (_T("medit"),		_T("renderers"), get_MEditRenderer,			set_MEditRenderer);
#ifndef NO_DRAFT_RENDERER
define_struct_global (_T("draft"),		_T("renderers"), get_DraftRenderer,			set_DraftRenderer);
#endif // NO_DRAFT_RENDERER
#if !defined(NO_ACTIVESHADE)
define_struct_global (_T("activeShade"),_T("renderers"), get_ReshadeRenderer,		set_ReshadeRenderer);
#endif // !NO_ACTIVESHADE
define_struct_global (_T("medit_locked"),_T("renderers"), get_MEditRendererLocked,	set_MEditRendererLocked);

define_struct_global (_T("dualPlane"),		_T("gw"),		get_VptUseDualPlanes,	set_VptUseDualPlanes);

define_system_global( _T("manipulateMode"),	getManipulateMode,			setManipulateMode);

define_struct_global ( _T("coordSysNode"),			_T("toolMode"),				get_coordsys_node,			set_coordsys_node);

define_struct_global (_T("geometry"),	_T("hideByCategory"),	getHideByCategory_geometry,		setHideByCategory_geometry);
define_struct_global (_T("shapes"),		_T("hideByCategory"),	getHideByCategory_shapes,		setHideByCategory_shapes);
define_struct_global (_T("lights"),		_T("hideByCategory"),	getHideByCategory_lights,		setHideByCategory_lights);
define_struct_global (_T("cameras"),	_T("hideByCategory"),	getHideByCategory_cameras,		setHideByCategory_cameras);
define_struct_global (_T("helpers"),	_T("hideByCategory"),	getHideByCategory_helpers,		setHideByCategory_helpers);
define_struct_global (_T("spacewarps"),	_T("hideByCategory"),	getHideByCategory_spacewarps,	setHideByCategory_spacewarps);
define_struct_global (_T("particles"),	_T("hideByCategory"),	getHideByCategory_particles,	setHideByCategory_particles);
define_struct_global (_T("bones"),		_T("hideByCategory"),	getHideByCategory_bones,		setHideByCategory_bones);

define_struct_global (_T("hilite"),				_T("snapMode"),	getSnapHilite,		setSnapHilite);
define_struct_global (_T("markSize"),			_T("snapMode"),	getSnapMarkSize,	setSnapMarkSize);
define_struct_global (_T("toFrozen"),			_T("snapMode"),	getSnapToFrozen,	setSnapToFrozen);
define_struct_global (_T("axisConstraint"),		_T("snapMode"),	getSnapAxisConstraint,	setSnapAxisConstraint);
define_struct_global (_T("display"),			_T("snapMode"),	getSnapDisplay,		setSnapDisplay);
define_struct_global (_T("strength"),			_T("snapMode"),	getSnapStrength,	setSnapStrength);
define_struct_global (_T("hit"),				_T("snapMode"),	getSnapHit,			setSnapHit);
define_struct_global (_T("node"),				_T("snapMode"),	getSnapNode,		setSnapNode);
define_struct_global (_T("flags"),				_T("snapMode"),	getSnapFlags,		setSnapFlags);
define_struct_global (_T("hitPoint"),			_T("snapMode"),	getHitPoint,		setHitPoint);
define_struct_global (_T("worldHitpoint"),		_T("snapMode"),	getWorldHitpoint,	setWorldHitpoint);
define_struct_global (_T("screenHitPoint"),		_T("snapMode"),	getScreenHitpoint,	setScreenHitpoint);
define_struct_global (_T("OKForRelativeSnap"),	_T("snapMode"),	getSnapOKForRelativeSnap,	setSnapOKForRelativeSnap);
define_struct_global (_T("refPoint"),			_T("snapMode"),	getSnapRefPoint,	setSnapRefPoint);
define_struct_global (_T("topRefPoint"),		_T("snapMode"),	getSnapTopRefPoint,	setSnapTopRefPoint);
define_struct_global (_T("numOSnaps"),			_T("snapMode"),	getNumOSnaps,		setNumOSnaps);
define_struct_global (_T("snapRadius"),			_T("snapMode"),	getSnapRadius,		setSnapRadius);
define_struct_global (_T("snapPreviewRadius"),	_T("snapMode"),	getSnapPreviewRadius,		setSnapPreviewRadius);
define_struct_global (_T("displayRubberBand"),	_T("snapMode"),	getDisplaySnapRubberBand,	setDisplaySnapRubberBand);
define_struct_global (_T("useAxisCenterAsStartSnapPoint"),	_T("snapMode"),	getUseAxisCenterAsStartSnapPoint,	setUseAxisCenterAsStartSnapPoint);

define_system_global ( _T("timeDisplayMode"),							get_TimeDisplayMode,			set_TimeDisplayMode);

define_struct_global (_T("tension"),			_T("TCBDefaultParams"),		getTCBDefaultParam_t,			setTCBDefaultParam_t);
define_struct_global (_T("continuity"),			_T("TCBDefaultParams"),		getTCBDefaultParam_c,			setTCBDefaultParam_c);
define_struct_global (_T("bias"),				_T("TCBDefaultParams"),		getTCBDefaultParam_b,			setTCBDefaultParam_b);
define_struct_global (_T("easeTo"),				_T("TCBDefaultParams"),		getTCBDefaultParam_easeIn,		setTCBDefaultParam_easeIn);
define_struct_global (_T("easeFrom"),			_T("TCBDefaultParams"),		getTCBDefaultParam_easeOut,		setTCBDefaultParam_easeOut);

define_struct_global (_T("inTangentType"),		_T("BezierDefaultParams"),	getBezierTangentTypeIn,			setBezierTangentTypeIn);
define_struct_global (_T("outTangentType"),		_T("BezierDefaultParams"),	getBezierTangentTypeOut,		setBezierTangentTypeOut);

define_struct_global ( _T("mode"),				_T("MatEditor"),   MatEditor_GetMode, MatEditor_SetMode);

define_system_global ( _T("rootScene"),			get_scene_root,			NULL);


//EOF
