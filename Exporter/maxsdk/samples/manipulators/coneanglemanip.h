/**********************************************************************
 *<
	FILE: ConeAngleManip.h

	DESCRIPTION:  Manipulator for cone angles.

	CREATED BY: Scott Morrison

	HISTORY: created 5/18/00

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "Manipulator.h"

#pragma once

#define ConeAngleManip_CLASS_ID Class_ID(0x2581576e, 0x253a2a10)
#define PBLOCK_REF_NO	 0
ClassDesc* GetConeAngleManipDesc();

// Manipulator to specify a solid angle, such as a spotlight falloff angle
class ConeAngleManipulator: public SimpleManipulator 
{
public:
    ConeAngleManipulator();
    ConeAngleManipulator(INode* pNode);

    CreateMouseCallBack* GetCreateMouseCallBack();
    void BeginEditParams( IObjParam  *ip, ULONG flags, Animatable *prev);
    void EndEditParams( IObjParam *ip, ULONG flags, Animatable *next);
    RefTargetHandle Clone(RemapDir& remap);

    const TCHAR *GetObjectName() {
        return GetString(IDS_CONE_ANGLE_MANIP);
    }
    
    void DeleteThis() {
        delete this;
    }
    Class_ID ClassID() {
        return ConeAngleManip_CLASS_ID;
    }
    int	NumParamBlocks() {
        return 1;
    }
    IParamBlock2* GetParamBlock(int i) {
        return mpPblock;
    } 
    IParamBlock2* GetParamBlockByID(BlockID id) {
        return (mpPblock->ID() == id) ? mpPblock : NULL;
    } 
    
    void OnMouseMove(TimeValue t, ViewExp* pVpt, IPoint2& m, DWORD flags, ManipHitData* pHitData);
	void OnButtonDown(TimeValue t, ViewExp* pVpt, IPoint2& m, DWORD flags, ManipHitData* pHitData);

    virtual void SetAngle(float angle);

    void UpdateShapes(TimeValue t, TSTR& toolTip);
    void GenerateShapes(TimeValue t);

    void SetValues(Point3& origin, Point3& direction, float distance,
                   float angle, BOOL useSquare, float aspect);

    BOOL GetTargetPoint(TimeValue t, Point3& p);
};

// Parameter block description for cone angle manipulators

// block IDs
enum { kConeAngleParams };

// Parameter IDs
enum { kConeAngleOrigin,
       kConeAngleDirection,
       kConeAngleAngle,
       kConeAngleDistance,
       kConeAngleUseSquare,
       kConeAngleAspect,
};


#define HotspotManip_CLASS_ID Class_ID(0x2581576e, 0x25ea2a19)

class SpotLightHotSpotManipulator: public ConeAngleManipulator
{
public:
    SpotLightHotSpotManipulator(LightObject* pLight, INode* pNode);
    SpotLightHotSpotManipulator();
    
	Class_ID ClassID() {
		return HotspotManip_CLASS_ID;
	}

    void SetAngle(float angle);

    void UpdateShapes(TimeValue t, TSTR& toolTip);

    LightObject* GetLight() { return (LightObject*) GetManipTarget(); }
};

#define FalloffManip_CLASS_ID Class_ID(0x2581576e, 0x253a2a11)

class SpotLightFalloffManipulator: public ConeAngleManipulator
{
public:
    SpotLightFalloffManipulator(LightObject* pLight, INode* pNode);
    SpotLightFalloffManipulator();
    
	Class_ID ClassID() {
		return FalloffManip_CLASS_ID;
	}

    void SetAngle(float angle);

    void UpdateShapes(TimeValue t, TSTR& toolTip);

    LightObject* GetLight() { return (LightObject*) GetManipTarget(); }
};

