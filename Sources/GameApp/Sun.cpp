///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Implements a sun light that change day/night environment. 
///

#include "Precompiled.h"
#include "Sun.h"

// Engine includes
#include "VCNRenderer/LightingCore.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNRenderer/XformCore.h"
#include "VCNUtils/Matrix.h"

//////////////////////////////////////////////////////////////////////////
Sun::Sun(const VCNNodeID nodeID)
  : VCNDirectionalLight( nodeID )
  , mPosition( -0.75f, 2.19f, -0.90f )
  , mDistance( 40 )
  , mFrustomWidth( 19 )
  , mFrustomHeight( 7 )
  , mFrustomNear( 25 )
  , mFrustomFar( 52 )
{
  SetTag( VCNTXT("Sun") );
  
   SetDiffuse( VCNColor(1.0f, 1.0f, 1.0f, 1.0f) );
   SetSpecular( VCNColor(1.0f, 1.0f, 1.0f, 1.0f) );
  SetDirection( Vector3(-0.47f, -0.1f, -0.8f).Normalized() );

  // Create a toolbar to set Sun parameters
  CreateToolbar();

  // Set ambient light
  VCNLightingCore::GetInstance()->SetAmbientLight( 0.25f, 0.25f, 0.25f );
  VCNLightingCore::GetInstance()->AddLight( nodeID );
}

//////////////////////////////////////////////////////////////////////////
Sun::~Sun(void)
{
  // Release light resources.
  if ( VCNLightingCore::GetInstance() )
  {
    VCNLightingCore::GetInstance()->RemoveLight( GetNodeID() );
  }
}



//////////////////////////////////////////////////////////////////////////
void Sun::CreateToolbar()
{
  std::stringstream ss;
  const VCNUInt32 barWidth = 250;
  const VCNUInt32 barHeight = 325;

  mBar = TwNewBar("SunBar");

  const VCNPoint& screenDimension = VCNRenderCore::GetInstance()->GetResolution();

  ss <<
    "SunBar "
    "label='[Sun Tool]' "
    "color='150 100 150 50' "
    "position='240 " << screenDimension.y - barHeight - 30 << "' "
    "size='" << barWidth << " " << barHeight << "' "
    "valuesWidth=100 "
    "refresh=1.0"
    << " iconified=true "
    ;

  TwDefine(ss.str().c_str());

  TwAddVarCB(mBar, "Dir",     TW_TYPE_DIR3F, SetFLDir, GetFLDir, this, "label='Dir'");
  TwAddVarRW(mBar, "At",      TW_TYPE_DIR3F, &mPosition,      "label='Sun look at' help='Change the position.'");
  TwAddVarRW(mBar, "Dist",    TW_TYPE_FLOAT, &mDistance,      "label='Sun distance' ");
  TwAddVarRW(mBar, "FWidth",  TW_TYPE_FLOAT, &mFrustomWidth,  "label='Frustum width' ");
  TwAddVarRW(mBar, "FHeight", TW_TYPE_FLOAT, &mFrustomHeight, "label='Frustum height' ");
  TwAddVarRW(mBar, "FNear",   TW_TYPE_FLOAT, &mFrustomNear,   "label='Frustum near' ");
  TwAddVarRW(mBar, "FFar",    TW_TYPE_FLOAT, &mFrustomFar,    "label='Frustum far' ");
}

//////////////////////////////////////////////////////////////////////////
void Sun::ReleaseToolbar()
{
  TwDeleteBar(mBar);
}

//////////////////////////////////////////////////////////////////////////
void TW_CALL Sun::SetFLDir( const void *value, void *clientData )
{
  Sun* tool = static_cast<Sun*>( clientData );
  VCN_ASSERT_MSG( tool, _T("Tool pointer is invalid") );

  const float* val = (const float*)value;
  Vector3 v(val[0], val[1], val[2]);

  tool->SetDirection( v );
}

//////////////////////////////////////////////////////////////////////////
void TW_CALL Sun::GetFLDir( void *value, void *clientData )
{
  Sun* tool = static_cast<Sun*>( clientData );
  VCN_ASSERT_MSG(tool, _T("Tool pointer is invalid"));

  float* out = (float*)value;
  const Vector3 dir = tool->GetDirection();
  out[0] = dir.x; out[1] = dir.y; out[2] = dir.z;
}


////////////////////////////////////////////////////////////////////////
///
/// Returns the view matrix of the directional light. We place the light
/// far from origin in opposite direction of the light direction.
/// 
/// @param view   [OUT]    computed view matrix
///
/// @return always true, cause always successful.
///
Matrix4 Sun::GetViewMatrix() const
{
  Matrix4 view;
  const Vector3 from = mPosition - GetDirection() * mDistance;
  view.SetLookAt(from, mPosition, Vector3(0, 1, 0));
  return view;
}


///////////////////////////////////////////////////////////////////////
///
/// Returns the orthographic box that the sun captures. We try to cover
/// the same frustum as the camera view. (we consider that the camera
/// view is always pointing forward the z)
///
/// @remark This projection is invalid for another kind of game.
/// 
/// @return the light projection matrix
///
Matrix4 Sun::GetProjectionMatrix() const 
{
  Matrix4 proj;
  proj.SetOrthogonality( mFrustomWidth, mFrustomHeight, mFrustomNear, mFrustomFar );
  return proj;
}
