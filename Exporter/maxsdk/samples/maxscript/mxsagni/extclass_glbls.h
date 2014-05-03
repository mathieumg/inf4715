// Some render globals

// RK: 5/17/99 Not valid in Shiva anymore
//define_struct_global ( "maxRayDepth",			"scanlineRender",		get_max_ray_depth,			set_max_ray_depth);

#ifndef WEBVERSION
define_struct_global ( _T("antiAliasFilter"),		_T("scanlineRender"),		get_anti_alias_filter,		set_anti_alias_filter);
define_struct_global ( _T("antiAliasFilterSize"),	_T("scanlineRender"),	get_scanRenderer_antiAliasFilterSz,		set_scanRenderer_antiAliasFilterSz);
define_struct_global ( _T("enablePixelSampler"),	    _T("scanlineRender"),	get_scanRenderer_pixelSamplerEnable,	set_scanRenderer_pixelSamplerEnable);

// LAM: 9/10/01 More scanline renderer exposure

define_struct_global ( _T("mapping"),				_T("scanlineRender"),	get_scanRenderer_mapping,				set_scanRenderer_mapping);
define_struct_global ( _T("shadows"), 				_T("scanlineRender"),	get_scanRenderer_shadows,				set_scanRenderer_shadows);
define_struct_global ( _T("autoReflect"), 			_T("scanlineRender"),	get_scanRenderer_autoReflect,			set_scanRenderer_autoReflect);
define_struct_global ( _T("forceWireframe"), 		_T("scanlineRender"),	get_scanRenderer_forceWireframe,		set_scanRenderer_forceWireframe);
define_struct_global ( _T("wireThickness"), 		_T("scanlineRender"),	get_scanRenderer_wireThickness,			set_scanRenderer_wireThickness);
define_struct_global ( _T("antiAliasing"), 			_T("scanlineRender"),	get_scanRenderer_antiAliasing,			set_scanRenderer_antiAliasing);
define_struct_global ( _T("filterMaps"), 			_T("scanlineRender"),	get_scanRenderer_filterMaps,			set_scanRenderer_filterMaps);
define_struct_global ( _T("objectMotionBlur"), 		_T("scanlineRender"),	get_scanRenderer_objectMotionBlur,		set_scanRenderer_objectMotionBlur);
define_struct_global ( _T("objectBlurDuration"), 	_T("scanlineRender"),	get_scanRenderer_objectBlurDuration,	set_scanRenderer_objectBlurDuration); 
define_struct_global ( _T("objectBlurSamples"), 	_T("scanlineRender"),	get_scanRenderer_objectBlurSamples,		set_scanRenderer_objectBlurSamples);
define_struct_global ( _T("objectBlurSubdivisions"),_T("scanlineRender"),	get_scanRenderer_objectBlurSubdivisions,set_scanRenderer_objectBlurSubdivisions);
define_struct_global ( _T("imageMotionBlur"), 		_T("scanlineRender"),	get_scanRenderer_imageMotionBlur,		set_scanRenderer_imageMotionBlur);
define_struct_global ( _T("imageBlurDuration"), 	_T("scanlineRender"),	get_scanRenderer_imageBlurDuration,		set_scanRenderer_imageBlurDuration);
define_struct_global ( _T("autoReflectLevels"), 	_T("scanlineRender"),	get_scanRenderer_autoReflectLevels,		set_scanRenderer_autoReflectLevels);
define_struct_global ( _T("colorClampType"), 		_T("scanlineRender"),	get_scanRenderer_colorClampType,		set_scanRenderer_colorClampType);
define_struct_global ( _T("imageBlurEnv"), 			_T("scanlineRender"),	get_scanRenderer_imageBlurEnv,			set_scanRenderer_imageBlurEnv);
define_struct_global ( _T("imageBlurTrans"), 		_T("scanlineRender"),	get_scanRenderer_imageBlurTrans,		set_scanRenderer_imageBlurTrans);
define_struct_global ( _T("conserveMemory"), 		_T("scanlineRender"),	get_scanRenderer_conserveMemory,		set_scanRenderer_conserveMemory);
define_struct_global ( _T("enableSSE"),				_T("scanlineRender"),	get_scanRenderer_enableSSE,				set_scanRenderer_enableSSE);
#ifdef	SINGLE_SUPERSAMPLE_IN_RENDER
define_struct_global ( _T("samplerQuality"),		_T("scanlineRender"),	get_scanRenderer_samplerQuality,		set_scanRenderer_samplerQuality);
#endif	// SINGLE_SUPERSAMPLE_IN_RENDER
#endif // WEBVERSION

