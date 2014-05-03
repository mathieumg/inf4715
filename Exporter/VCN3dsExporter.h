///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Includes for plug-ins
///

#ifndef VCN3DSEXPORTER_H
#define VCN3DSEXPORTER_H

#pragma once

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
//SIMPLE TYPE

#include "utilapi.h"

//IGame Interface headers
#include "IGame.h"
#include "IGameObject.h"
#include "IGameProperty.h"
#include "IGameControl.h"
#include "IGameModifier.h"
#include "IConversionManager.h"
#include "IGameError.h" 
#include "IGameFX.h"

//Instance manager headers
#include "iInstanceMgr.h"

// MS XML
#include <atlcomcli.h>
#import <msxml6.dll>  raw_interfaces_only

//STL Headers
#include <vector>
#include <algorithm>
using namespace std;

//Vicuna Export Vertex Class
#include "VCNExportVertex.h"

#define VCN3dsExporter_CLASS_ID	Class_ID(0x6c937924, 0x8b24db98)

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;


#endif // VCN3DSEXPORTER_H
