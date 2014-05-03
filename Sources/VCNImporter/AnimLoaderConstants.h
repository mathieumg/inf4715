///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Constants only used while loading models
///

#ifndef VICUNA_ANIMATION_LOADER_CONSTANTS
#define VICUNA_ANIMATION_LOADER_CONSTANTS

// File extensions
const VCNTChar* kXMLAnimExtension            = VCNTXT("ANM.xml");

// Root node
const VCNTChar* kAttrAnimNode                = VCNTXT("VCNAnim");
const VCNTChar* kAttrAnimTarget              = VCNTXT("target");

// Joints
const VCNTChar* kNodeAnimJoint               = VCNTXT("Joint");
const VCNTChar* kAttrAnimJointTarget         = VCNTXT("Name");
const VCNTChar* kAttrAnimJointPosX           = VCNTXT("posx");
const VCNTChar* kAttrAnimJointPosY           = VCNTXT("posy");
const VCNTChar* kAttrAnimJointPosZ           = VCNTXT("posz");
const VCNTChar* kAttrAnimJointRotX           = VCNTXT("rotx");
const VCNTChar* kAttrAnimJointRotY           = VCNTXT("roty");
const VCNTChar* kAttrAnimJointRotZ           = VCNTXT("rotz");

// Frame positions
const VCNTChar* kAttrAnimJointPosCount       = VCNTXT("NbKFPos");
const VCNTChar* kNodeAnimJointPosition       = VCNTXT("KF_Pos");
const VCNTChar* kAttrAnimJointPosFrame       = VCNTXT("frame");
const VCNTChar* kAttrAnimJointPosFrameTime   = VCNTXT("time");
const VCNTChar* kAttrAnimJointFramePosX      = VCNTXT("x");
const VCNTChar* kAttrAnimJointFramePosY      = VCNTXT("y");
const VCNTChar* kAttrAnimJointFramePosZ      = VCNTXT("z");

// Frame rotations
const VCNTChar* kNodeAnimJointRotation       = VCNTXT("KF_Rot");
const VCNTChar* kAttrAnimJointRotCount       = VCNTXT("NbKFRot");
const VCNTChar* kAttrAnimJointRotFrame       = VCNTXT("frame");
const VCNTChar* kAttrAnimJointRotFrameTime   = VCNTXT("time");
const VCNTChar* kAttrAnimJointFrameRotX      = VCNTXT("x");
const VCNTChar* kAttrAnimJointFrameRotY      = VCNTXT("y");
const VCNTChar* kAttrAnimJointFrameRotZ      = VCNTXT("z");
const VCNTChar* kAttrAnimJointFrameRotW      = VCNTXT("w");

// Frame rotations
const VCNTChar* kNodeAnimJointScale          = VCNTXT("KF_Scl");
const VCNTChar* kAttrAnimJointScaleCount     = VCNTXT("NbKFScl");
const VCNTChar* kAttrAnimJointScaleFrame     = VCNTXT("frame");
const VCNTChar* kAttrAnimJointScaleFrameTime = VCNTXT("time");
const VCNTChar* kAttrAnimJointFrameScaleX    = VCNTXT("x");
const VCNTChar* kAttrAnimJointFrameScaleY    = VCNTXT("y");
const VCNTChar* kAttrAnimJointFrameScaleZ    = VCNTXT("z");

#endif
