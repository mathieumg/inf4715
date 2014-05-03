///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
///

#include "VCNExportVertex.h"

VCNExportVertex::VCNExportVertex(void)
{
}

VCNExportVertex::~VCNExportVertex(void)
{
}

bool operator==(const VCNExportVertex& vert1, const VCNExportVertex& vert2)
{
	if(vert1.Pos != vert2.Pos)
		return(false);
	if(vert1.Normal != vert2.Normal)
		return(false);
	if(vert1.UV != vert2.UV)
		return(false);
	if(vert1.Color != vert2.Color)
		return(false);
	return(true);
}
