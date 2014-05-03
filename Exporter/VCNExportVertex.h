///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Exporter vertex definition
///

#ifndef VCNEXPORTVERTEX_H
#define VCNEXPORTVERTEX_H

#pragma once

#include "Max.h"

class VCNExportVertex
{
public:
	//Methods
	VCNExportVertex(void);
	~VCNExportVertex(void);
	friend bool operator==(const VCNExportVertex& vert1, const VCNExportVertex& vert2);

	//Properties
	Point3 Pos;
	Point3 Normal;
	Point2 UV;
	Point3 Color;
};

#endif // VCNEXPORTVERTEX_H
