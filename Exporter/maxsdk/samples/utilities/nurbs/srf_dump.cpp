/**********************************************************************
 *<
    FILE: srf_dump.cpp

    DESCRIPTION:  Test Utility for the API

    CREATED BY: Charlie Thaeler

    HISTORY: created 18 Feb, 1998

 *> Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/
#include "nutil.h"




void
APITestUtil::DumpMatrix3(int indent, Matrix3& mat)
{
	TCHAR indStr[80];

	for (int i = 0; i <= indent; i++)
		indStr[i] = _T(' ');
	indStr[indent+1] = _T('\0');

	for (int i = 0; i < 4; i++) {
		Point3 p = mat.GetRow(i);
		DumpPrint(_T("%s   %10.4f %10.4f %10.4f\n"), indStr, p.x, p.y, p.z);
	}
}

static TCHAR ybuf[256];
static TCHAR nbuf[256];
BOOL y_n_init = FALSE;
static TCHAR *
YorN(BOOL yorn)
{
    if (hInstance && !y_n_init) {
        LoadString(hInstance, IDS_YES, ybuf, _countof(ybuf));
        LoadString(hInstance, IDS_NO, nbuf, _countof(nbuf));
	}
	if (yorn)
		return ybuf;
	return nbuf;
}


void
APITestUtil::PrintPoint(int indent, Point3 pt)
{
	TCHAR indStr[80];

	for (int i = 0; i <= indent; i++)
		indStr[i] = _T(' ');
	indStr[indent+1] = _T('\0');
	DumpPrint(_T("%s   %s %g"), indStr, GetString(IDS_X), pt.x);
	DumpPrint(_T("   %s %g"), GetString(IDS_Y), pt.y);
	DumpPrint(_T("  %s %g\n"), GetString(IDS_Z), pt.z);
}

void
APITestUtil::DumpObject(int indent, NURBSObject* nobj, TimeValue t)
{
	TCHAR indStr[80];

	for (int i = 0; i <= indent; i++)
		indStr[i] = _T(' ');
	indStr[indent+1] = _T('\0');

	// now common object stuff
	DumpPrint(_T("%s%s \"%s\" "), indStr, GetString(IDS_NAME), nobj->GetName());
	DumpPrint(_T(" %s  %lx "), GetString(IDS_ID), nobj->GetId());
	DumpPrint(_T(" %s  "), GetString(IDS_TYPE));

	switch (nobj->GetType()) {
	case kNPoint:
		DumpPrint(_T("%s"), GetString(IDS_POINT));
		break;
	case kNPointCPoint:
		DumpPrint(_T("%s"), GetString(IDS_POINT_CONST_POINT));
		break;
	case kNCurveCPoint:
		DumpPrint(_T("%s"), GetString(IDS_CURVE_CONST_POINT));
		break;
	case kNCurveCurveIntersectionPoint:
		DumpPrint(_T("%s"), GetString(IDS_CURVE_CURVE_POINT));
		break;
	case kNSurfaceCPoint:
		DumpPrint(_T("%s"), GetString(IDS_SURF_CONST_POINT));
		break;
	case kNCurveSurfaceIntersectionPoint:
		DumpPrint(_T("%s"), GetString(IDS_CURV_SURF_INT_POINT));
		break;
	case kNTexturePoint:
		DumpPrint(_T("%s"), GetString(IDS_TEXTURE_POINT));
		break;

	case kNCV:
		DumpPrint(_T("%s"), GetString(IDS_CV));
		break;

	case kNCVCurve:
		DumpPrint(_T("%s"), GetString(IDS_CV_CURVE));
		break;
	case kNPointCurve:
		DumpPrint(_T("%s"), GetString(IDS_POINT_CURVE));
		break;
	case kNBlendCurve:
		DumpPrint(_T("%s"), GetString(IDS_BLEND_CURVE));
		break;
	case kNOffsetCurve:
		DumpPrint(_T("%s"), GetString(IDS_OFFSET_CURVE));
		break;
	case kNXFormCurve:
		DumpPrint(_T("%s"), GetString(IDS_XFORM_CURVE));
		break;
	case kNMirrorCurve:
		DumpPrint(_T("%s"), GetString(IDS_MIRROR_CURVE));
		break;
	case kNFilletCurve:
		DumpPrint(_T("%s"), GetString(IDS_FILLET_CURVE));
		break;
	case kNChamferCurve:
		DumpPrint(_T("%s"), GetString(IDS_CHAMFER_CURVE));
		break;
	case kNIsoCurve:
		DumpPrint(_T("%s"), GetString(IDS_ISO_CURVE));
		break;
	case kNProjectVectorCurve:
		DumpPrint(_T("%s"), GetString(IDS_PROJ_VECT_CURVE));
		break;
	case kNProjectNormalCurve:
		DumpPrint(_T("%s"), GetString(IDS_PROJ_NORM_CURVE));
		break;
	case kNSurfSurfIntersectionCurve:
		DumpPrint(_T("%s"), GetString(IDS_SURF_SURF_CURVE));
		break;
	case kNCurveOnSurface:
		DumpPrint(_T("%s"), GetString(IDS_COS_CURVE));
		break;
	case kNPointCurveOnSurface:
		DumpPrint(_T("%s"), GetString(IDS_POINT_COS_CURVE));
		break;
	case kNSurfaceNormalCurve:
		DumpPrint(_T("%s"), GetString(IDS_SURF_NORM_CURVE));
		break;



	case kNCVSurface:
		DumpPrint(_T("%s"), GetString(IDS_CV_SURFACE));
		break;
	case kNPointSurface:
		DumpPrint(_T("%s"), GetString(IDS_POINT_SURFACE));
		break;
	case kNBlendSurface:
		DumpPrint(_T("%s"), GetString(IDS_BLEND_SURFACE));
		break;
	case kNOffsetSurface:
		DumpPrint(_T("%s"), GetString(IDS_OFFSET_SURFACE));
		break;
	case kNXFormSurface:
		DumpPrint(_T("%s"), GetString(IDS_XFORM_SURFACE));
		break;
	case kNMirrorSurface:
		DumpPrint(_T("%s"), GetString(IDS_MIRROR_SURFACE));
		break;
	case kNRuledSurface:
		DumpPrint(_T("%s"), GetString(IDS_RULED_SURFACE));
		break;
	case kNULoftSurface:
		DumpPrint(_T("%s"), GetString(IDS_ULOFT_SURFACE));
		break;
	case kNUVLoftSurface:
		DumpPrint(_T("%s"), GetString(IDS_UVLOFT_SURFACE));
		break;
	case kNExtrudeSurface:
		DumpPrint(_T("%s"), GetString(IDS_EXTRUDE_SURFACE));
		break;
	case kNLatheSurface:
		DumpPrint(_T("%s"), GetString(IDS_LATHE_SURFACE));
		break;
	case kNNBlendSurface:
		DumpPrint(_T("%s"), GetString(IDS_NBLEND_SURFACE));
		break;
	case kN1RailSweepSurface:
		DumpPrint(_T("%s"), GetString(IDS_1RAIL_SURFACE));
		break;
	case kN2RailSweepSurface:
		DumpPrint(_T("%s"), GetString(IDS_2RAIL_SURFACE));
		break;
	case kNCapSurface:
		DumpPrint(_T("%s"), GetString(IDS_CAP_SURFACE));
		break;
	case kNMultiCurveTrimSurface:
		DumpPrint(_T("%s"), GetString(IDS_MULTI_TRIM_SURFACE));
		break;
	case kNFilletSurface:
		DumpPrint(_T("%s"), GetString(IDS_FILLET_SURFACE));
		break;
	}

	if (nobj->IsSelected())
		DumpPrint(_T("  %s\n"), GetString(IDS_SELECTED));
	else
		DumpPrint(_T("  %s\n"), GetString(IDS_NOT_SELECTED));


	// dump CV data since it doesn't otherwise exist in the database
	if (indent > 0 && nobj->GetType() != kNCV)
		return;

	// now stuff common amongst groups of types
	switch (nobj->GetKind()) {
	case kNURBSPoint:
		{
		NURBSPoint* pNPnt = (NURBSPoint*)nobj;
		double x, y, z;
		pNPnt->GetPosition(t, x, y, z);
		PrintPoint(indent, Point3(x, y, z));
		break; }

	case kNURBSTexturePoint:
		{
		NURBSTexturePoint* pNPnt = (NURBSTexturePoint*)nobj;
		double x, y;
		pNPnt->GetPosition(t, x, y);
		PrintPoint(indent, Point3(x, y, 0.0));
		break; }

	case kNURBSCurve:
		{
		NURBSCurve* pNCrv = (NURBSCurve*)nobj;

		DumpPrint(_T("%s   %s %d\n"),  indStr, GetString(IDS_MAT_ID),
			pNCrv->MatID());
		// Timming data.
        int numTrimPts = pNCrv->NumTrimPoints();
        DumpPrint(_T("%s %d\n"), GetString(IDS_NUM_TRIM_POINTS), numTrimPts);
        for(int i = 0; i < numTrimPts; i++) {
            NURBSTrimPoint trimPt = pNCrv->GetTrimPoint(t, i);
			DumpPrint(_T("%s %g "), GetString(IDS_PARAMETER),
								trimPt.GetParameter());
            switch(trimPt.GetDirection()) {
            case kNone:
                DumpPrint(GetString(IDS_DIRECTION_NONE));
                break;
            case kPositive:
                 DumpPrint(GetString(IDS_DIRECTION_POS));
                break;
            case kNegative:
                DumpPrint(GetString(IDS_DIRECTION_NEG));
                break;
            }
        }
		break; }

	case kNURBSSurface:
		{
		NURBSSurface* pNSrf = (NURBSSurface*)nobj;
		DumpPrint(_T("%s   %s  %s\n"), indStr, GetString(IDS_RENDERABLE),
			YorN(pNSrf->Renderable()));
		DumpPrint(_T("%s   %s %s\n"), indStr, GetString(IDS_FLIP_NORMALS),
			YorN(pNSrf->FlipNormals()));
		DumpPrint(_T("%s   %s %d\n"), indStr, GetString(IDS_MAT_ID),
			pNSrf->MatID());
		DumpPrint(_T("%s   %s %s\n"), indStr, GetString(IDS_CLOSED_U),
			YorN(pNSrf->IsClosedInU()));
		DumpPrint(_T("%s   %s %s\n"), indStr, GetString(IDS_CLOSED_V),
			YorN(pNSrf->IsClosedInV()));
		for (int i = 0; i < pNSrf->NumChannels(); i++) {
            int channel = pNSrf->GetChannelFromIndex(i);
            if (pNSrf->GenerateUVs(channel)) {
                DumpPrint(_T("%s   %s %d\n"), indStr,
                          GetString(IDS_UV_CHANNEL), channel);
                DumpPrint(_T("%s     %s:      %s\n"), indStr,
                          GetString(IDS_GENUV), YorN(pNSrf->GenerateUVs(channel)));
                for (int tex = 0; tex < 4; tex++) {
                    Point2 tuv = pNSrf->GetTextureUVs(t, tex, channel);
                    DumpPrint(_T("%s       %s %d = U: %g  V: %g\n"), indStr,
                              GetString(IDS_TEXUV), tex, tuv.x, tuv.y);
                }
                float ut, vt, uo, vo, a;
                pNSrf->GetTileOffset(t, ut, vt, uo, vo, a, channel);
                DumpPrint(_T("%s     %s %g"), indStr, GetString(IDS_UOFFSET), uo);
                DumpPrint(_T("  %s %g\n"), GetString(IDS_VOFFSET), vo);
                DumpPrint(_T("%s     %s   %g"), indStr, GetString(IDS_UTILE), ut);
                DumpPrint(_T("  %s   %g\n"), GetString(IDS_VTILE), vt);
                DumpPrint(_T("%s     %s   %g\n"), indStr, GetString(IDS_ANGLE), a);
                
                NURBSTextureSurface *pTexSurf = &pNSrf->GetTextureSurface(channel);
				switch (pTexSurf->MapperType()) {
				case kNMapDefault:
                    DumpPrint(_T("  %s\n"), GetString(IDS_CLP));
					break;
				case kNMapUserDefined: {
                    DumpPrint(_T("\n"));
                    int numU = pTexSurf->GetNumUPoints(),
                        numV = pTexSurf->GetNumVPoints();
                    DumpPrint(_T("%s  %s"), indStr, GetString(IDS_TEXTURE_POINT));
                    DumpPrint(_T(" %s  %d"), GetString(IDS_U), numU);
                    DumpPrint(_T(" %s  %d\n"), GetString(IDS_V), numV);
                    for (int u = 0; u < numU; u++)
                        for (int v = 0; v < numV; v++)
                            DumpObject(indent+4, pTexSurf->GetPoint(u, v), t);
						break; }
				case kNMapSufaceMapper:
                    DumpPrint(_T("  %s\n"), GetString(IDS_SURFACE_MAPPER));
					DumpPrint(_T("%s  %s %lx\n"), indStr, GetString(IDS_PARENT_ID), pTexSurf->GetParentId());
					break;
                }
            }
		}

		int numTrimLoops = pNSrf->NumTrimLoops(t);
		if (numTrimLoops > 0) {
			for (int loop = 0; loop < numTrimLoops; loop++) {
				int numCrvs = pNSrf->NumCurvesInLoop(t, loop);
				for (int crv = 0; crv < numCrvs; crv++) {
					int degree, numCVs, numKnots;
					NURBSCVTab cvs;
					NURBSKnotTab knots;
					if (pNSrf->Get2dTrimCurveData(t, loop, crv, degree,
										numCVs, cvs, numKnots, knots)) {
						DumpPrint(_T("      TRIM LOOP %d CURVE %d degree %d\n"), loop, crv, degree);
						for (int cv = 0; cv < numCVs; cv++) {
							double x, y, z;
							cvs[cv].GetPosition(t, x, y, z);
							DumpPrint(_T("        %s[%d]: %f %f %f\n"), GetString(IDS_CV), cv, x, y, z);
						}
					}
				}
			}
		}

		break; }
	}



	// now object type specific stuff
	switch (nobj->GetType()) {
	case kNPoint: {
		NURBSIndependentPoint* pNPnt = (NURBSIndependentPoint*)nobj;
		double x, y, z;
		pNPnt->GetPosition(t, x, y, z);
		PrintPoint(indent, Point3(x, y, z));
		break; }
	case kNTexturePoint: {
		NURBSTexturePoint* pNPnt = (NURBSTexturePoint*)nobj;
		double x, y;
		pNPnt->GetPosition(t, x, y);
		PrintPoint(indent, Point3(x, y, 0.0));
		break; }
	case kNPointCPoint: {
		NURBSPointConstPoint* pNPnt = (NURBSPointConstPoint*)nobj;
		DumpPrint(_T("%s  %s %lx\n"), indStr, GetString(IDS_PARENT_ID), pNPnt->GetParentId());
		switch(pNPnt->GetPointType()) {
		case kNConstOnObject:
			DumpPrint(_T("%s   %s\n"), indStr, GetString(IDS_ON_OBJECT));
			break;
		case kNConstOffset: {
			DumpPrint(_T("%s   %s"), indStr, GetString(IDS_OFFSET));
			Point3 p = pNPnt->GetOffset(t);
			PrintPoint(0, p);
			break; }
		}
		break; }
	case kNCurveCPoint: {
		NURBSCurveConstPoint* pNPnt = (NURBSCurveConstPoint*)nobj;
		DumpPrint(_T("%s  %s %lx\n"), indStr, GetString(IDS_PARENT_ID), pNPnt->GetParentId());
		DumpPrint(_T("%s   %s %g\n"), indStr, GetString(IDS_UPARAM), pNPnt->GetUParam(t));
		switch(pNPnt->GetPointType()) {
		case kNConstOnObject:
			DumpPrint(_T("%s   %s\n"), indStr, GetString(IDS_ON_OBJECT));
			break;
		case kNConstOffset: {
			DumpPrint(_T("%s   %s"), indStr, GetString(IDS_OFFSET));
			Point3 p = pNPnt->GetOffset(t);
			PrintPoint(0, p);
			break; }
		case kNConstNormal: {
			DumpPrint(_T("%s   %s %g\n"), indStr, GetString(IDS_NORMAL), pNPnt->GetNormal(t));
			break; }
		case kNConstTangent: {
			DumpPrint(_T("%s   %s %g\n"), indStr, GetString(IDS_UTANGENT), pNPnt->GetUTangent(t));
			break; }
		DumpPrint(_T("%s   %s  %s\n"), indStr, GetString(IDS_TRIM), YorN(pNPnt->GetTrimCurve()));
		DumpPrint(_T("%s   %s  %s\n"), indStr, GetString(IDS_FLIP_TRIM), YorN(pNPnt->GetFlipTrim()));
		}
		break; }
	case kNSurfaceCPoint: {
		NURBSSurfConstPoint* pNPnt = (NURBSSurfConstPoint*)nobj;
		DumpPrint(_T("%s  %s %lx\n"), indStr, GetString(IDS_PARENT_ID), pNPnt->GetParentId());
		DumpPrint(_T("%s   %s %g"), indStr, GetString(IDS_UPARAM), pNPnt->GetUParam(t));
		DumpPrint(_T("  %s %g\n"), GetString(IDS_VPARAM), pNPnt->GetVParam(t));
		switch(pNPnt->GetPointType()) {
		case kNConstOnObject:
			DumpPrint(_T("%s   %s\n"), indStr, GetString(IDS_ON_OBJECT));
			break;
		case kNConstOffset: {
			DumpPrint(_T("%s   %s"), indStr, GetString(IDS_OFFSET));
			Point3 p = pNPnt->GetOffset(t);
			PrintPoint(0, p);
			break; }
		case kNConstNormal: {
			DumpPrint(_T("%s   %s %g\n"), indStr, GetString(IDS_NORMAL), pNPnt->GetNormal(t));
			break; }
		case kNConstTangent: {
			DumpPrint(_T("%s   %s %g\n"), indStr, GetString(IDS_UTANGENT), pNPnt->GetUTangent(t));
			DumpPrint(_T("%s   %s %g\n"), indStr, GetString(IDS_VTANGENT), pNPnt->GetVTangent(t));
			break; }
		}
		break; }
	case kNCurveCurveIntersectionPoint: {
		NURBSCurveCurveIntersectionPoint* pNPnt = (NURBSCurveCurveIntersectionPoint*)nobj;
		for (int i = 0; i < 2; i++) {
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNPnt->GetParentId(i));
			DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_TRIM), YorN(pNPnt->GetTrimCurve(i)));
			DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_FLIP_TRIM), YorN(pNPnt->GetFlipTrim(i)));
		}
		break; }
	case kNCurveSurfaceIntersectionPoint: {
		NURBSCurveSurfaceIntersectionPoint* pNPnt = (NURBSCurveSurfaceIntersectionPoint*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNPnt->GetParentId(0));
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNPnt->GetParentId(1));
		DumpPrint(_T("%s  %s (%g %g)\n"),  indStr, GetString(IDS_SEED), pNPnt->GetSeed());
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_TRIM), YorN(pNPnt->GetTrimCurve()));
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_FLIP_TRIM), YorN(pNPnt->GetFlipTrim()));
		break; }




	case kNCV: {
		NURBSControlVertex* pNCV = (NURBSControlVertex*)nobj;
		double x, y, z, w;
		pNCV->GetPosition(t, x, y, z);
		w = pNCV->GetWeight(t);
		PrintPoint(indent, Point3(x, y, z));
		break; }




	case kNCVCurve: {
		NURBSCVCurve *pNCurve = (NURBSCVCurve*)nobj;
		DumpPrint(_T("%s   %s %d\n"),  indStr, GetString(IDS_ORDER), pNCurve->GetOrder());
		int numK = pNCurve->GetNumKnots();
		DumpPrint(_T("%s  %s %d\n%s    "), indStr, GetString(IDS_KNOTS), numK, indStr);
		for (int k = 0; k < numK; k++)
			DumpPrint(_T("%g "), pNCurve->GetKnot(k));
		DumpPrint(_T("\n"));
		int numCV = pNCurve->GetNumCVs();
		DumpPrint(_T("%s  %s\n"),  indStr, GetString(IDS_XFORM_MAT));
		DumpMatrix3(indent+4, pNCurve->GetTransformMatrix(t));
		DumpPrint(_T("%s  %s %d\n"),  indStr, GetString(IDS_CVS), numCV);
		for (int cv = 0; cv < numCV; cv++)
			DumpObject(indent+4, pNCurve->GetCV(cv), t);
		break; }
	case kNPointCurve: {
		NURBSPointCurve *pNCurve = (NURBSPointCurve*)nobj;
		int num = pNCurve->GetNumPts();
		DumpPrint(_T("%s  %s\n"),  indStr, GetString(IDS_XFORM_MAT));
		DumpMatrix3(indent+4, pNCurve->GetTransformMatrix(t));
		DumpPrint(_T("%s  %s %d\n"),  indStr, GetString(IDS_POINTS), num);
		for (int p = 0; p < num; p++)
			DumpObject(indent+4, pNCurve->GetPoint(p), t);
		break; }
	case kNOffsetCurve: {
		NURBSOffsetCurve *pNCurve = (NURBSOffsetCurve*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId());
		DumpPrint(_T("%s  %s %g\n"),  indStr, GetString(IDS_DISTANCE), pNCurve->GetDistance(t));
		break; }
	case kNXFormCurve: {
		NURBSXFormCurve *pNCurve = (NURBSXFormCurve*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId());
		DumpPrint(_T("%s  %s\n"),  indStr, GetString(IDS_TRANSFORM));
		DumpMatrix3(indent+4, pNCurve->GetXForm(t));
		break; }
	case kNMirrorCurve: {
		NURBSMirrorCurve *pNCurve = (NURBSMirrorCurve*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId());
		DumpPrint(_T("%s  %s %g\n"),  indStr, GetString(IDS_DISTANCE), pNCurve->GetDistance(t));
		DumpPrint(_T("%s  %s"), indStr, GetString(IDS_AXIS));
		switch (pNCurve->GetAxis()) {
		case kMirrorX:
			DumpPrint(_T("%s\n"), GetString(IDS_MIRRORX));
			break;
		case kMirrorY:
			DumpPrint(_T("%s\n"), GetString(IDS_MIRRORY));
			break;
		case kMirrorZ:
			DumpPrint(_T("%s\n"), GetString(IDS_MIRRORZ));
			break;
		case kMirrorXY:
			DumpPrint(_T("%s\n"), GetString(IDS_MIRRORXY));
			break;
		case kMirrorXZ:
			DumpPrint(_T("%s\n"), GetString(IDS_MIRRORXZ));
			break;
		case kMirrorYZ:
			DumpPrint(_T("%s\n"), GetString(IDS_MIRRORYZ));
			break;
		}
		DumpPrint(_T("%s  %s\n"),  indStr, GetString(IDS_TRANSFORM));
		DumpMatrix3(indent+4, pNCurve->GetXForm(t));
		break; }
	case kNBlendCurve: {
		NURBSBlendCurve *pNCurve = (NURBSBlendCurve*)nobj;
		for (int i = 0; i < 2; i++) {
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId(i));
			DumpPrint(_T("%s  %s %d  %g\n"),  indStr, GetString(IDS_TENSION), i, pNCurve->GetTension(t, i));
			DumpPrint(_T("%s  %s %d"), indStr, GetString(IDS_END), i);
			if (pNCurve->GetEnd(i))
				DumpPrint(_T(" %s\n"), GetString(IDS_HIGHU));
			else
				DumpPrint(_T(" %s\n"), GetString(IDS_LOWU));
		}
		break; }
	case kNFilletCurve: {
		NURBSFilletCurve *pNCurve = (NURBSFilletCurve*)nobj;
		DumpPrint(_T("%s  Radius: %g\n"),  indStr, pNCurve->GetRadius(t));
		for (int i = 0; i < 2; i++) {
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId(i));
			DumpPrint(_T("%s  %s %d"), indStr, GetString(IDS_END), i);
			if (pNCurve->GetEnd(i))
				DumpPrint(_T(" %s\n"), GetString(IDS_HIGHU));
			else
				DumpPrint(_T(" %s\n"), GetString(IDS_LOWU));
			DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_TRIM), YorN(pNCurve->GetTrimCurve(i)));
		}
		break; }
	case kNChamferCurve: {
		NURBSChamferCurve *pNCurve = (NURBSChamferCurve*)nobj;
		for (int i = 0; i < 2; i++) {
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId(i));
			DumpPrint(_T("%s  Length(%d): %g\n"),  indStr, i, pNCurve->GetLength(t, i));
			DumpPrint(_T("%s  %s %d"), indStr, GetString(IDS_END), i);
			if (pNCurve->GetEnd(i))
				DumpPrint(_T(" %s\n"), GetString(IDS_HIGHU));
			else
				DumpPrint(_T(" %s\n"), GetString(IDS_LOWU));
			DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_TRIM), YorN(pNCurve->GetTrimCurve(i)));
		}
		break; }
	case kNIsoCurve: {
		NURBSIsoCurve *pNCurve = (NURBSIsoCurve*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId());
		DumpPrint(_T("%s  %s"), indStr, GetString(IDS_DIRECTION));
		if (pNCurve->GetDirection())
			DumpPrint(_T(" %s %g\n"), GetString(IDS_U), pNCurve->GetParam(t));
		else
			DumpPrint(_T(" %s %g\n"), GetString(IDS_V), pNCurve->GetParam(t));
		Point2 seed = pNCurve->GetSeed();
		DumpPrint(_T("%s  %s (%g %g)\n"),  indStr, GetString(IDS_SEED), seed.x, seed.y);
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_TRIM_SURFACE), YorN(pNCurve->GetTrim()));
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_FLIP_TRIM), YorN(pNCurve->GetFlipTrim()));
		break; }
	case kNProjectVectorCurve: {
		NURBSProjectVectorCurve* pNCurve = (NURBSProjectVectorCurve*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId(0));
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId(1));
		Point3 p = pNCurve->GetPVec(t);
		DumpPrint(_T("%s  EVec "), indStr);
		PrintPoint(0, p);
		Point2 seed = pNCurve->GetSeed();
		DumpPrint(_T("%s  %s (%g %g)\n"),  indStr, GetString(IDS_SEED), seed.x, seed.y);
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_TRIM_SURFACE), YorN(pNCurve->GetTrim()));
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_FLIP_TRIM), YorN(pNCurve->GetFlipTrim()));
		break; }
	case kNProjectNormalCurve: {
		NURBSProjectNormalCurve* pNCurve = (NURBSProjectNormalCurve*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId(0));
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId(1));
		Point2 seed = pNCurve->GetSeed();
		DumpPrint(_T("%s  %s (%g %g)\n"),  indStr, GetString(IDS_SEED), seed.x, seed.y);
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_TRIM_SURFACE), YorN(pNCurve->GetTrim()));
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_FLIP_TRIM), YorN(pNCurve->GetFlipTrim()));
		break; }
	case kNSurfSurfIntersectionCurve: {
		NURBSSurfSurfIntersectionCurve* pNCurve = (NURBSSurfSurfIntersectionCurve*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId(0));
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId(1));
		Point2 seed = pNCurve->GetSeed();
		DumpPrint(_T("%s  %s (%g %g)\n"),  indStr, GetString(IDS_SEED), seed.x, seed.y);
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_TRIM_SURFACE), YorN(pNCurve->GetTrim(0)));
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_FLIP_TRIM), YorN(pNCurve->GetFlipTrim(0)));
		DumpPrint(_T("%s   %s 1 %s\n"),  indStr, GetString(IDS_TRIM_SURFACE), YorN(pNCurve->GetTrim(1)));
		DumpPrint(_T("%s   %s 2 %s\n"),  indStr, GetString(IDS_FLIP_TRIM), YorN(pNCurve->GetFlipTrim(1)));
		break; }
	case kNCurveOnSurface: {
		NURBSCurveOnSurface* pNCurve = (NURBSCurveOnSurface*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId());
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_TRIM_SURFACE), YorN(pNCurve->GetTrim()));
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_FLIP_TRIM), YorN(pNCurve->GetFlipTrim()));
		DumpPrint(_T("%s   %s %d\n"),  indStr, GetString(IDS_ORDER), pNCurve->GetOrder());
		int numK = pNCurve->GetNumKnots();
		DumpPrint(_T("%s  %s %d\n%s    "), indStr, GetString(IDS_KNOTS), numK, indStr);
		for (int k = 0; k < numK; k++)
			DumpPrint(_T("%g "), pNCurve->GetKnot(k));
		DumpPrint(_T("\n"));
		int numCV = pNCurve->GetNumCVs();
		DumpPrint(_T("%s  %s\n"),  indStr, GetString(IDS_XFORM_MAT));
		DumpMatrix3(indent+4, pNCurve->GetTransformMatrix(t));
		DumpPrint(_T("%s  %s %d\n"),  indStr, GetString(IDS_CVS), numCV);
		for (int cv = 0; cv < numCV; cv++)
			DumpObject(indent+4, pNCurve->GetCV(cv), t);
		break; }
	case kNPointCurveOnSurface: {
		NURBSPointCurveOnSurface* pNCurve = (NURBSPointCurveOnSurface*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId());
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_TRIM_SURFACE), YorN(pNCurve->GetTrim()));
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_FLIP_TRIM), YorN(pNCurve->GetFlipTrim()));
		DumpPrint(_T("%s  %s\n"),  indStr, GetString(IDS_XFORM_MAT));
		DumpMatrix3(indent+4, pNCurve->GetTransformMatrix(t));
		int numPnts = pNCurve->GetNumPts();
		DumpPrint(_T("%s  %s %d\n"),  indStr, GetString(IDS_POINT), numPnts);
		for (int i = 0; i < numPnts; i++)
			DumpObject(indent+4, pNCurve->GetPoint(i), t);
		break; }
	case kNSurfaceNormalCurve: {
		NURBSSurfaceNormalCurve* pNCurve = (NURBSSurfaceNormalCurve*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNCurve->GetParentId());
		DumpPrint(_T("%s  %s %g\n"),  indStr, GetString(IDS_DISTANCE), pNCurve->GetDistance(t));
		break; }




	case kNCVSurface: {
		NURBSCVSurface *pNSurface = (NURBSCVSurface*)nobj;
		if (pNSurface->IsRigid())
			DebugPrint(_T("%s   %s\n"), indStr, GetString(IDS_RIGID));
		DumpPrint(_T("%s   %s %d\n"),  indStr, GetString(IDS_UORDER), pNSurface->GetUOrder());
		DumpPrint(_T("%s   %s %d\n"),  indStr, GetString(IDS_VORDER), pNSurface->GetVOrder());
		DumpPrint(_T("%s  %s\n"),  indStr, GetString(IDS_XFORM_MAT));
		DumpMatrix3(indent+4, pNSurface->GetTransformMatrix(t));
		int numK = pNSurface->GetNumUKnots();
		DumpPrint(_T("%s  %s %d\n%s    "), indStr, GetString(IDS_UKNOTS),
						numK, indStr);
		for (int k = 0; k < numK; k++)
			DumpPrint(_T("%g "), pNSurface->GetUKnot(k));
		DumpPrint(_T("\n"));
		numK = pNSurface->GetNumVKnots();
		DumpPrint(_T("%s  %s %d\n%s    "), indStr, GetString(IDS_VKNOTS),
						numK, indStr);
		for (int k = 0; k < numK; k++)
			DumpPrint(_T("%g "), pNSurface->GetVKnot(k));
		DumpPrint(_T("\n"));
		int numU = pNSurface->GetNumUCVs(),
			numV = pNSurface->GetNumVCVs();
		DumpPrint(_T("%s  %s"), indStr, GetString(IDS_CVS));
		DumpPrint(_T(" %s  %d"), GetString(IDS_U), numU);
		DumpPrint(_T(" %s  %d\n"), GetString(IDS_V), numV);
		for (int u = 0; u < numU; u++)
			for (int v = 0; v < numV; v++)
				DumpObject(indent+4, pNSurface->GetCV(u, v), t);
		break; }
	case kNPointSurface: {
		NURBSPointSurface *pNSurface = (NURBSPointSurface*)nobj;
		DumpPrint(_T("%s  %s\n"),  indStr, GetString(IDS_XFORM_MAT));
		DumpMatrix3(indent+4, pNSurface->GetTransformMatrix(t));
		int numU = pNSurface->GetNumUPts(),
			numV = pNSurface->GetNumVPts();
		DumpPrint(_T("%s  %s"), indStr, GetString(IDS_POINTS));
		DumpPrint(_T(" %s  %d"), GetString(IDS_U), numU);
		DumpPrint(_T(" %s  %d\n"), GetString(IDS_V), numV);
		for (int u = 0; u < numU; u++)
			for (int v = 0; v < numV; v++)
				DumpObject(indent+4, pNSurface->GetPoint(u, v), t);
		break; }
	case kNBlendSurface: {
		NURBSBlendSurface *pNSurface = (NURBSBlendSurface*)nobj;
		for (int i = 0; i < 2; i++) {
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId(i));
			DumpPrint(_T("%s  %s %d   %g\n"),  indStr, GetString(IDS_TENSION), i, pNSurface->GetTension(t, i));
			DumpPrint(_T("%s  %s %d  "), indStr, GetString(IDS_EDGE), i);
			switch (pNSurface->GetEdge(i)) {
			case 0:
				DumpPrint(_T("%s\n"), GetString(IDS_LOWU));
				break;
			case 1:
				DumpPrint(_T("%s\n"), GetString(IDS_HIGHU));
				break;
			case 2:
				DumpPrint(_T("%s\n"), GetString(IDS_LOWV));
				break;
			case 3:
				DumpPrint(_T("%s\n"), GetString(IDS_HIGHV));
				break;
			}
			DumpPrint(_T("%s  %s %d  %s\n"),  indStr, GetString(IDS_FLIP), i, YorN(pNSurface->GetFlip(i)));
		}
		break; }
	case kNOffsetSurface: {
		NURBSOffsetSurface *pNSurface = (NURBSOffsetSurface*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId());
		DumpPrint(_T("%s  %s %g\n"),  indStr, GetString(IDS_DISTANCE), pNSurface->GetDistance(t));
		break; }
	case kNRuledSurface: {
		NURBSRuledSurface *pNSurface = (NURBSRuledSurface*)nobj;
		for (int i = 0; i < 2; i++) {
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId(i));
			DumpPrint(_T("%s  %s %d  %s\n"),  indStr, GetString(IDS_FLIP), i, YorN(pNSurface->GetFlip(i)));
		}
		break; }
	case kNULoftSurface: {
		NURBSULoftSurface *pNSurface = (NURBSULoftSurface*)nobj;
		int numC = pNSurface->GetNumCurves();
		for (int i = 0; i < numC; i++) {
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId(i));
			DumpPrint(_T("%s  %s %d  %s\n"),  indStr, GetString(IDS_FLIP), i, YorN(pNSurface->GetFlip(i)));
		}
		break; }
	case kNUVLoftSurface: {
		NURBSUVLoftSurface* pNSurface = (NURBSUVLoftSurface*)nobj;
		int numUC = pNSurface->GetNumUCurves();
		for (int i = 0; i < numUC; i++)
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetUParentId(i));
		int numVC = pNSurface->GetNumVCurves();
		for (int i = 0; i < numVC; i++)
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetVParentId(i));
		break; }
	case kNXFormSurface: {
		NURBSXFormSurface *pNSurface = (NURBSXFormSurface*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId());
		DumpPrint(_T("%s  %s\n"),  indStr, GetString(IDS_TRANSFORM));
		DumpMatrix3(indent+4, pNSurface->GetXForm(t));
		break; }
	case kNMirrorSurface: {
		NURBSMirrorSurface *pNSurface = (NURBSMirrorSurface*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId());
		DumpPrint(_T("%s  Distance: %g\n"),  indStr, pNSurface->GetDistance(t));
		DumpPrint(_T("%s  %s"), indStr, GetString(IDS_AXIS));
		switch (pNSurface->GetAxis()) {
		case kMirrorX:
			DumpPrint(_T("%s\n"), GetString(IDS_MIRRORX));
			break;
		case kMirrorY:
			DumpPrint(_T("%s\n"), GetString(IDS_MIRRORY));
			break;
		case kMirrorZ:
			DumpPrint(_T("%s\n"), GetString(IDS_MIRRORZ));
			break;
		case kMirrorXY:
			DumpPrint(_T("%s\n"), GetString(IDS_MIRRORXY));
			break;
		case kMirrorXZ:
			DumpPrint(_T("%s\n"), GetString(IDS_MIRRORXZ));
			break;
		case kMirrorYZ:
			DumpPrint(_T("%s\n"), GetString(IDS_MIRRORYZ));
			break;
		}
		DumpPrint(_T("%s  %s\n"),  indStr, GetString(IDS_TRANSFORM));
		DumpMatrix3(indent+4, pNSurface->GetXForm(t));
		break; }
	case kNExtrudeSurface: {
		NURBSExtrudeSurface *pNSurface = (NURBSExtrudeSurface*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId());
		DumpPrint(_T("%s  Distance: %g\n"),  indStr, pNSurface->GetDistance(t));
		DumpMatrix3(indent+4, pNSurface->GetAxis(t));
		break; }
	case kNLatheSurface: {
		NURBSLatheSurface *pNSurface = (NURBSLatheSurface*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId());
		DumpPrint(_T("%s  %s %g\n"),  indStr, GetString(IDS_DEGREES), pNSurface->GetRotation(t));
		DumpPrint(_T("%s  %s\n"),  indStr, GetString(IDS_AXIS));
		DumpMatrix3(indent+4, pNSurface->GetAxis(t));
		break; }
	case kN1RailSweepSurface: {
		NURBS1RailSweepSurface* pNSurface = (NURBS1RailSweepSurface*)nobj;
		int numC = pNSurface->GetNumCurves();
		for (int i = 0; i < numC; i++) {
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId(i));
			DumpPrint(_T("%s  %s %d  %s\n"),  indStr, GetString(IDS_FLIP), i, YorN(pNSurface->GetFlip(i)));
		}
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_RAIL_ID), pNSurface->GetParentRailId());
		DumpPrint(_T("%s  %s %d  %s\n"),  indStr, GetString(IDS_PARALLEL), numC, YorN(pNSurface->GetParallel()));
		DumpPrint(_T("%s  %s %d  %s\n"),  indStr, GetString(IDS_SNAPCS), numC, YorN(pNSurface->GetSnapCS()));
		DumpPrint(_T("%s  %s %d  %s\n"),  indStr, GetString(IDS_ROADLIKE), numC, YorN(pNSurface->GetRoadlike()));
		DumpMatrix3(indent+4, pNSurface->GetAxis(t));
		break; }
	case kN2RailSweepSurface: {
		NURBS2RailSweepSurface* pNSurface = (NURBS2RailSweepSurface*)nobj;
		int numC = pNSurface->GetNumCurves();
		for (int i = 0; i < numC; i++) {
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId(i));
			DumpPrint(_T("%s  %s %d  %s\n"),  indStr, GetString(IDS_FLIP), i, YorN(pNSurface->GetFlip(i)));
		}
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_RAIL_ID), pNSurface->GetRailParentId(0));
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_RAIL_ID), pNSurface->GetRailParentId(1));
		DumpPrint(_T("%s  %s %d  %s\n"),  indStr, GetString(IDS_PARALLEL), numC, YorN(pNSurface->GetParallel()));
		DumpPrint(_T("%s  %s %d  %s\n"),  indStr, GetString(IDS_SCALE), numC, YorN(pNSurface->GetScale()));
		DumpPrint(_T("%s  %s %d  %s\n"),  indStr, GetString(IDS_SNAPCS), numC, YorN(pNSurface->GetSnapCS()));
		break; }
	case kNNBlendSurface: {
		NURBSNBlendSurface* pNSurface = (NURBSNBlendSurface*)nobj;
		for (int i = 0; i < 4; i++) {
			NURBSId nid = pNSurface->GetParentId(i);
			if (nid == 0) continue;
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), nid);
			DumpPrint(_T("%s  %s %d "), indStr, GetString(IDS_EDGE), i);
			switch (pNSurface->GetEdge(i)) {
			case 0:
				DumpPrint(_T("%s\n"), GetString(IDS_LOWU));
				break;
			case 1:
				DumpPrint(_T("%s\n"), GetString(IDS_HIGHU));
				break;
			case 2:
				DumpPrint(_T("%s\n"), GetString(IDS_LOWV));
				break;
			case 3:
				DumpPrint(_T("%s\n"), GetString(IDS_HIGHV));
				break;
			}
		}
		break; }
	case kNCapSurface: {
		NURBSCapSurface* pNSurface = (NURBSCapSurface*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId());
		DumpPrint(_T("%s  %s  "), indStr, GetString(IDS_EDGE));
		switch (pNSurface->GetEdge()) {
		case 0:
			DumpPrint(_T("%s\n"), GetString(IDS_LOWU));
			break;
		case 1:
			DumpPrint(_T("%s\n"), GetString(IDS_HIGHU));
			break;
		case 2:
			DumpPrint(_T("%s\n"), GetString(IDS_LOWV));
			break;
		case 3:
			DumpPrint(_T("%s\n"), GetString(IDS_HIGHV));
			break;
		}
		break; }
	case kNMultiCurveTrimSurface: {
		NURBSMultiCurveTrimSurface* pNSurface = (NURBSMultiCurveTrimSurface*)nobj;
		DumpPrint(_T("%s  Surf %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetSurfaceParentId());
		int numC = pNSurface->GetNumCurves();
		for (int i = 0; i < numC; i++)
			DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId(i));
		break; }
	case kNFilletSurface: {
		NURBSFilletSurface* pNSurface = (NURBSFilletSurface*)nobj;
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId(0));
		DumpPrint(_T("%s  %s %lx\n"),  indStr, GetString(IDS_PARENT_ID), pNSurface->GetParentId(1));
		DumpPrint(_T("%s  %s %d %g\n"),  indStr, GetString(IDS_RADIUS), 0, pNSurface->GetRadius(t, 0));
		DumpPrint(_T("%s  %s %d %g\n"),  indStr, GetString(IDS_RADIUS), 1, pNSurface->GetRadius(t, 1));
		DumpPrint(_T("%s   %s  %s\n"),  indStr, GetString(IDS_CUBIC), YorN(pNSurface->IsCubic()));
		break; }
	}

	DumpPrint(_T("\n\n"));
}


void
APITestUtil::DumpPrint(const TCHAR *format, ...)
{
	TCHAR buf[512];
	va_list args;
	va_start(args,format);
	_vsntprintf(buf,512,format,args);
	va_end(args);
	if (mToFile && mFP && mFP->CanWrite()) {
		mFP->Write(buf);
		return;
	}

	DebugPrint(buf);
}

void
APITestUtil::DumpSelected()
{
	if (mToFile && _tcslen(mFilename) > 0) {
		mFP = new MaxSDK::Util::TextFile::Writer;
		Interface14 *iface = GetCOREInterface14();
		unsigned int codepage  = iface-> DefaultTextSaveCodePage(true);
		mFP->Open(mFilename, false, MaxSDK::Util::TextFile::Writer::WRITE_BOM | codepage );
	}
	for (int i=0; i < mpIp->GetSelNodeCount(); i++) {
		// Get a selected node
		INode* node = mpIp->GetSelNode(i);
		Object* obj = node->GetObjectRef();

		NURBSSet getSet;
		BOOL getres = GetNURBSSet(obj, mpIp->GetTime(), getSet, mRelational);
		int numObj = getSet.GetNumObjects();
		for (i = 0; i < numObj; i++)
			DumpObject(0, getSet.GetNURBSObject(i), mpIp->GetTime());

		getSet.DeleteObjects();
	}

	if (mFP) {
		delete mFP;
		mFP = NULL;
	}
}
