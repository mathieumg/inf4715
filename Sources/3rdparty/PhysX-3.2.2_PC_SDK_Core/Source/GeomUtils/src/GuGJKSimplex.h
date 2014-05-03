// This code contains NVIDIA Confidential Information and is disclosed to you 
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and 
// any modifications thereto. Any use, reproduction, disclosure, or 
// distribution of this software and related documentation without an express 
// license agreement from NVIDIA Corporation is strictly prohibited.
// 
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2012 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  


#ifndef PX_GJKSIMPLEX_H
#define PX_GJKSIMPLEX_H

#include "CmPhysXCommon.h"
#include "PsVecMath.h"

#if defined __SPU__ || (defined __GNUC__ && defined _DEBUG)
#define PX_GJK_INLINE PX_INLINE
#define PX_GJK_FORCE_INLINE PX_INLINE
#else
#define PX_GJK_INLINE PX_INLINE
#define PX_GJK_FORCE_INLINE PX_FORCE_INLINE
#endif


namespace physx
{
namespace Gu
{


	PX_NOALIAS Ps::aos::Vec3V closestPtPointTetrahedron(Ps::aos::Vec3V* PX_RESTRICT Q, Ps::aos::Vec3V* PX_RESTRICT A, Ps::aos::Vec3V* PX_RESTRICT B, PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB);

	//PX_NOALIAS Ps::aos::Vec3V closestPtPointTetrahedronSplit(Ps::aos::Vec3V* PX_RESTRICT Q, Ps::aos::Vec3V* PX_RESTRICT A, Ps::aos::Vec3V* PX_RESTRICT B, PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB);

	PX_NOALIAS Ps::aos::Vec3V closestPtPointTetrahedron(Ps::aos::Vec3V* PX_RESTRICT Q, Ps::aos::Vec3V* PX_RESTRICT A, Ps::aos::Vec3V* PX_RESTRICT B, PxI32* PX_RESTRICT aInd, PxI32* PX_RESTRICT bInd, const Ps::aos::Vec3VArg support, const Ps::aos::Vec3VArg supportA,
		const Ps::aos::Vec3VArg supportB,  PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB);

	PX_NOALIAS PX_FORCE_INLINE Ps::aos::BoolV PointOutsideOfPlane4(const Ps::aos::Vec3VArg _a, const Ps::aos::Vec3VArg _b, const Ps::aos::Vec3VArg _c, const Ps::aos::Vec3VArg _d)
	{
		using namespace Ps::aos;
		
		const Vec4V zero = Vec4V_From_F32(-1e-6f);

		//Vec4V a = Vec4V_From_Vec3V(_a);
		//Vec4V b = Vec4V_From_Vec3V(_b);
		//Vec4V c = Vec4V_From_Vec3V(_c);
		//Vec4V d = Vec4V_From_Vec3V(_d);

		//Vec4V dbcb0, dbcb1, dbcb2;
		//PX_TRANSPOSE_44_34(d, b, c, b, dbcb0, dbcb1, dbcb2);

		//const Vec3V ab = V3Sub(_b, _a);
		//const Vec3V ac = V3Sub(_c, _a);
		//const Vec3V ad = V3Sub(_d, _a);
		//const Vec3V bd = V3Sub(_d, _b);
		//const Vec3V bc = V3Sub(_c, _b);


		//Vec4V v0 = Vec4V_From_Vec3V(V3Cross(ab, ac));
		//Vec4V v1 = Vec4V_From_Vec3V(V3Cross(ac, ad));
		//Vec4V v2 = Vec4V_From_Vec3V(V3Cross(ad, ab));
		//Vec4V v3 = Vec4V_From_Vec3V(V3Cross(bd, bc));

		//Vec4V v00, v01, v02;
		//PX_TRANSPOSE_44_34(v0, v1, v2, v3, v00, v01, v02);

		//FloatV ax = V3GetX(_a);
		//FloatV ay = V3GetY(_a);
		//FloatV az = V3GetZ(_a);

		////dot0 = (signa0, signa1, signa2, signd3)
		//Vec4V dot0 = V4ScaleAdd(v02, az, V4ScaleAdd(v01, ay, V4Scale(v00, ax)));
		////dot1 = signd0, signd1, signd2, signa3)
		//Vec4V dot1 = V4MulAdd(v02, dbcb2, V4MulAdd(v01, dbcb1, V4Mul(v00, dbcb0)));		
		//return V4IsGrtrOrEq(V4Mul(dot0, dot1), zero);


		const Vec3V ab = V3Sub(_b, _a);
		const Vec3V ac = V3Sub(_c, _a);
		const Vec3V ad = V3Sub(_d, _a);
		const Vec3V bd = V3Sub(_d, _b);
		const Vec3V bc = V3Sub(_c, _b);

		const Vec3V v0 = V3Cross(ab, ac);
		const Vec3V v1 = V3Cross(ac, ad);
		const Vec3V v2 = V3Cross(ad, ab);
		const Vec3V v3 = V3Cross(bd, bc);

		const FloatV signa0 = V3Dot(v0, _a);
		const FloatV signa1 = V3Dot(v1, _a);
		const FloatV signa2 = V3Dot(v2, _a);
		const FloatV signd3 = V3Dot(v3, _a);

		const FloatV signd0 = V3Dot(v0, _d);
		const FloatV signd1 = V3Dot(v1, _b);
		const FloatV signd2 = V3Dot(v2, _c);
		const FloatV signa3 = V3Dot(v3, _b);

		const Vec4V signa = V4Merge(signa0, signa1, signa2, signa3);
		const Vec4V signd = V4Merge(signd0, signd1, signd2, signd3);
		return V4IsGrtrOrEq(V4Mul(signa, signd), zero);//same side, outside of the plane

		
	}


	PX_NOALIAS PX_FORCE_INLINE Ps::aos::Vec3V closestPtPointSegment(Ps::aos::Vec3V* PX_RESTRICT Q, Ps::aos::Vec3V* PX_RESTRICT A, Ps::aos::Vec3V* PX_RESTRICT B, PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB)
	{
		using namespace Ps::aos;
		const Vec3V a = Q[0];
		const Vec3V b = Q[1];

		const BoolV bTrue = BTTTT();
		//const Vec3V origin = V3Zero();
		const FloatV zero = FZero();
		const FloatV one = FOne();

		//Test degenerated case
		const Vec3V ab = V3Sub(b, a);
		const FloatV denom = V3Dot(ab, ab);
		const Vec3V ap = V3Neg(a);//V3Sub(origin, a);
		const FloatV nom = V3Dot(ap, ab);
		const BoolV con = FIsEq(denom, zero);
		const Vec3V v = V3Sub(A[1], A[0]);
		const Vec3V w = V3Sub(B[1], B[0]);
		const FloatV tValue = FClamp(FMul(nom, FRecip(denom)), zero, one);
		const FloatV t = FSel(con, zero, tValue);
		//TODO - can we get rid of this branch? The problem is size, which isn't a vector!
		if(BAllEq(con, bTrue))
		{
			size = 1;
			closestA = A[0];
			closestB = B[0];
			return Q[0];
		}

	/*	const PxU32 count = BAllEq(con, bTrue);
		size = 2 - count;*/
		
		const Vec3V tempClosestA = V3ScaleAdd(v, t, A[0]);
		const Vec3V tempClosestB = V3ScaleAdd(w, t, B[0]);
		closestA = tempClosestA;
		closestB = tempClosestB;
		return V3Sub(tempClosestA, tempClosestB);
	}

	PX_NOALIAS PX_FORCE_INLINE Ps::aos::Vec3V closestPtPointSegment(const Ps::aos::Vec3VArg a, const Ps::aos::Vec3VArg b)
	{
		using namespace Ps::aos;
	
		const FloatV zero = FZero();
		const FloatV one = FOne();

		//Test degenerated case
		const Vec3V ab = V3Sub(b, a);
		const FloatV denom = V3Dot(ab, ab);
		const Vec3V ap = V3Neg(a);//V3Sub(origin, a);
		const FloatV nom = V3Dot(ap, ab);
		const BoolV con = FIsEq(denom, zero);
		const FloatV tValue = FClamp(FMul(nom, FRecip(denom)), zero, one);
		const FloatV t = FSel(con, zero, tValue);

		return V3Sel(con, a, V3ScaleAdd(ab, t, a));
	}

	PX_NOALIAS PX_FORCE_INLINE Ps::aos::Vec3V closestPtPointSegment(const Ps::aos::Vec3VArg Q0, const Ps::aos::Vec3VArg Q1, const Ps::aos::Vec3VArg A0, const Ps::aos::Vec3VArg A1,
		const Ps::aos::Vec3VArg B0, const Ps::aos::Vec3VArg B1, PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB)
	{
		using namespace Ps::aos;
		const Vec3V a = Q0;
		const Vec3V b = Q1;

		const BoolV bTrue = BTTTT();
		const FloatV zero = FZero();
		const FloatV one = FOne();

		//Test degenerated case
		const Vec3V ab = V3Sub(b, a);
		const FloatV denom = V3Dot(ab, ab);
		const Vec3V ap = V3Neg(a);//V3Sub(origin, a);
		const FloatV nom = V3Dot(ap, ab);
		const BoolV con = FIsEq(denom, zero);
		const Vec3V v = V3Sub(A1, A0);
		const Vec3V w = V3Sub(B1, B0);
		const FloatV tValue = FClamp(FMul(nom, FRecip(denom)), zero, one);
		const FloatV t = FSel(con, zero, tValue);
		//TODO - can we get rid of this branch? The problem is size, which isn't a vector!
		if(BAllEq(con, bTrue))
		{
			size = 1;
			closestA = A0;
			closestB = B0;
			return Q0;
		}

	/*	const PxU32 count = BAllEq(con, bTrue);
		size = 2 - count;*/
		const Vec3V tempClosestA = V3ScaleAdd(v, t, A0);
		const Vec3V tempClosestB = V3ScaleAdd(w, t, B0);
		closestA = tempClosestA;
		closestB = tempClosestB;
		return V3Sub(tempClosestA, tempClosestB);
	}

	


	PX_NOALIAS PX_FORCE_INLINE Ps::aos::Vec3V closestPtPointSegmentBaryCentric(const Ps::aos::Vec3VArg Q0, const Ps::aos::Vec3VArg Q1, PxU32& size, Ps::aos::FloatV& t0)
	{
		using namespace Ps::aos;
		const Vec3V a = Q0;
		const Vec3V b = Q1;

		const BoolV bTrue = BTTTT();
		const FloatV zero = FZero();
		const FloatV one = FOne();

		//Test degenerated case
		const Vec3V ab = V3Sub(b, a);
		const FloatV denom = V3Dot(ab, ab);
		const Vec3V ap = V3Neg(a);//V3Sub(origin, a);
		const FloatV nom = V3Dot(ap, ab);
		const BoolV con = FIsEq(denom, zero);
		const FloatV tValue = FClamp(FMul(nom, FRecip(denom)), zero, one);
		const FloatV t = FSel(con, zero, tValue);
		const PxU32 count = BAllEq(con, bTrue);
		size = 2 - count;
		t0 = t;
		return V3ScaleAdd(ab, t, a);
	}


	PX_NOALIAS PX_GJK_FORCE_INLINE Ps::aos::Vec3V closestPtPointTriangle(const Ps::aos::Vec3VArg a, const Ps::aos::Vec3VArg b, const Ps::aos::Vec3VArg c, Ps::aos::Vec3V* PX_RESTRICT Q, Ps::aos::Vec3V* PX_RESTRICT A, Ps::aos::Vec3V* PX_RESTRICT B, PxI32* PX_RESTRICT aInd, PxI32* PX_RESTRICT bInd, PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB)
	{
		using namespace Ps::aos;

		size = 3;
		const FloatV zero = FZero();
		const FloatV eps = FEps();
		const BoolV bTrue = BTTTT();

		const Vec3V ab = V3Sub(b, a);
		const Vec3V ac = V3Sub(c, a);
		const Vec3V bc = V3Sub(b, c);

		const FloatV dac = V3Dot(ac, ac);
		const FloatV dbc = V3Dot(bc, bc);
		if(FAllGrtrOrEq(eps, FMin(dac, dbc)))
		{
			//degenerate
			size = 2;
			return closestPtPointSegment(Q, A, B, size, closestA, closestB);
		}
		
		const Vec3V ap = V3Neg(a);
		const Vec3V bp = V3Neg(b);
		const Vec3V cp = V3Neg(c);

		const FloatV d1 = V3Dot(ab, ap); //  snom
		const FloatV d2 = V3Dot(ac, ap); //  tnom
		const FloatV d3 = V3Dot(ab, bp); // -sdenom
		const FloatV d4 = V3Dot(ac, bp); //  unom = d4 - d3
		const FloatV d5 = V3Dot(ab, cp); //  udenom = d5 - d6
		const FloatV d6 = V3Dot(ac, cp); // -tdenom
		const FloatV unom = FSub(d4, d3);
		const FloatV udenom = FSub(d5, d6);

		//const FloatV va = FSub(FMul(d3, d6), FMul(d5, d4));//edge region of BC
		//const FloatV vb = FSub(FMul(d5, d2), FMul(d1, d6));//edge region of AC
		//const FloatV vc = FSub(FMul(d1, d4), FMul(d3, d2));//edge region of AB

		const FloatV va = FNegMulSub(d5, d4, FMul(d3, d6));//edge region of BC
		const FloatV vb = FNegMulSub(d1, d6, FMul(d5, d2));//edge region of AC
		const FloatV vc = FNegMulSub(d3, d2, FMul(d1, d4));//edge region of AB

		//check if p in vertex region outside a
		const BoolV con00 = FIsGrtrOrEq(zero, d1); // snom <= 0
		const BoolV con01 = FIsGrtrOrEq(zero, d2); // tnom <= 0
		const BoolV con0 = BAnd(con00, con01); // vertex region a
		if(BAllEq(con0, bTrue))
		{
			size = 1;
			closestA = A[0];
			closestB = B[0];
			return Q[0];
		}
		

		//check if p in vertex region outside b
		const BoolV con10 = FIsGrtrOrEq(d3, zero);
		const BoolV con11 = FIsGrtrOrEq(d3, d4);
		const BoolV con1 = BAnd(con10, con11); // vertex region b
		if(BAllEq(con1, bTrue))
		{
			size = 1;
			closestA = A[1];
			closestB = B[1];
			const Vec3V q = Q[1];
			Q[0] = Q[1];
			A[0] = A[1];
			B[0] = B[1];
			aInd[0] = aInd[1];
			bInd[0] = bInd[1];
			return q;
		}

		//check if p in vertex region outside of c
		const BoolV con20 = FIsGrtrOrEq(d6, zero);
		const BoolV con21 = FIsGrtrOrEq(d6, d5); 
		const BoolV con2 = BAnd(con20, con21); // vertex region c
		if(BAllEq(con2, bTrue))
		{
			size = 1;
			closestA = A[2];
			closestB = B[2];
			//const Vec3V v = V3Sub(A[2], B[2]);
			const Vec3V q = Q[2];
			Q[0] = Q[2];
			A[0] = A[2];
			B[0] = B[2];
			aInd[0] = aInd[2];
			bInd[0] = bInd[2];
			return q;
		}


		//check if p in edge region of AB
		const BoolV con30 = FIsGrtrOrEq(zero, vc);
		const BoolV con31 = FIsGrtrOrEq(d1, zero);
		const BoolV con32 = FIsGrtrOrEq(zero, d3);
		const BoolV con3 = BAnd(con30, BAnd(con31, con32));
		if(BAllEq(con3, bTrue))
		{
			//AB
			size = 2;
			const FloatV toRecipA = FSub(d1, d3);
			const FloatV denom = FSel(FIsGrtr(FAbs(toRecipA), eps), FRecip(toRecipA), zero);
			const Vec3V v = V3Sub(A[1], A[0]);
			const Vec3V w = V3Sub(B[1], B[0]);
			const FloatV scale = FMul(d1, denom);
			const Vec3V tempClosestA = V3ScaleAdd(v, scale, A[0]);
			const Vec3V tempClosestB = V3ScaleAdd(w, scale, B[0]);
			closestA = tempClosestA;
			closestB = tempClosestB;
			return V3Sub(tempClosestA, tempClosestB);
		}

		//check if p in edge region of BC
		const BoolV con40 = FIsGrtrOrEq(zero, va);
		const BoolV con41 = FIsGrtrOrEq(d4, d3);
		const BoolV con42 = FIsGrtrOrEq(d5, d6);
		const BoolV con4 = BAnd(con40, BAnd(con41, con42)); 
		if(BAllEq(con4, bTrue))
		{
			//BC
			size = 2;
			const FloatV toRecipB = FAdd(unom, udenom);
			const FloatV denom = FSel(FIsGrtr(FAbs(toRecipB), eps), FRecip(toRecipB), zero);
			const Vec3V v = V3Sub(A[2], A[1]);
			const Vec3V w = V3Sub(B[2], B[1]);
			const FloatV scale = FMul(unom, denom);
			const Vec3V tempClosestA = V3ScaleAdd(v, scale, A[1]);
			const Vec3V tempClosestB = V3ScaleAdd(w, scale, B[1]);
			closestA = tempClosestA;
			closestB = tempClosestB;
			Q[0] = Q[2];
			A[0] = A[2];
			B[0] = B[2];
			aInd[0] = aInd[2];
			bInd[0] = bInd[2];
			return V3Sub(tempClosestA, tempClosestB);

		}

		//check if p in edge region of AC
		const BoolV con50 = FIsGrtrOrEq(zero, vb);
		const BoolV con51 = FIsGrtrOrEq(d2, zero);
		const BoolV con52 = FIsGrtrOrEq(zero, d6);
		const BoolV con5 = BAnd(con50, BAnd(con51, con52));
		if(BAllEq(con5, bTrue))
		{
			//AC
			size = 2;
			const FloatV toRecipC = FSub(d2, d6);
			const FloatV denom = FSel(FIsGrtr(FAbs(toRecipC), eps), FRecip(toRecipC), zero);
			const Vec3V v = V3Sub(A[2], A[0]);
			const Vec3V w = V3Sub(B[2], B[0]);
			const FloatV scale = FMul(d2, denom);
			const Vec3V tempClosestA = V3ScaleAdd(v, scale, A[0]);
			const Vec3V tempClosestB = V3ScaleAdd(w, scale, B[0]);
			closestA = tempClosestA;
			closestB = tempClosestB;
			Q[1] = Q[2];
			A[1] = A[2];
			B[1] = B[2];
			aInd[1] = aInd[2];
			bInd[1] = bInd[2];
			return V3Sub(tempClosestA, tempClosestB);
		}
	
		//P must project inside face region. Compute Q using Barycentric coordinates
		const FloatV toRecipD = FAdd(va, FAdd(vb, vc));
		//const FloatV denom = FRecip(toRecipD);//V4GetW(recipTmp);
		const FloatV denom = FSel(FIsGrtr(FAbs(toRecipD), eps), FRecip(toRecipD), zero);
		//const FloatV denom = V4GetW(recipTmp);
		const Vec3V v0 = V3Sub(A[1], A[0]);
		const Vec3V v1 = V3Sub(A[2], A[0]);
		const Vec3V w0 = V3Sub(B[1], B[0]);
		const Vec3V w1 = V3Sub(B[2], B[0]);

		const FloatV t = FMul(vb, denom);
		const FloatV w = FMul(vc, denom);
		const Vec3V vA1 = V3Scale(v1, w);
		const Vec3V vB1 = V3Scale(w1, w);
		const Vec3V tempClosestA = V3Add(A[0], V3ScaleAdd(v0, t, vA1));
		const Vec3V tempClosestB = V3Add(B[0], V3ScaleAdd(w0, t, vB1));
		closestA = tempClosestA;
		closestB = tempClosestB;
		return V3Sub(tempClosestA, tempClosestB);
	}

	PX_NOALIAS PX_GJK_FORCE_INLINE Ps::aos::Vec3V closestPtPointTriangle(const Ps::aos::Vec3VArg a, const Ps::aos::Vec3VArg b, const Ps::aos::Vec3VArg c, Ps::aos::Vec3V* PX_RESTRICT Q, Ps::aos::Vec3V* PX_RESTRICT A, Ps::aos::Vec3V* PX_RESTRICT B, PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB)
	{
		using namespace Ps::aos;

		size = 3;
		const FloatV zero = FZero();
		const FloatV eps = FEps();
		const BoolV bTrue = BTTTT();

		const Vec3V ab = V3Sub(b, a);
		const Vec3V ac = V3Sub(c, a);
		const Vec3V bc = V3Sub(c, b);

		const FloatV dac = V3Dot(ac, ac);
		const FloatV dbc = V3Dot(bc, bc);
		if(FAllGrtrOrEq(eps, FMin(dac, dbc)))
		{
			//degenerate
			size = 2;
			return closestPtPointSegment(Q, A, B, size, closestA, closestB);
		}
		
		const Vec3V ap = V3Neg(a);
		const Vec3V bp = V3Neg(b);
		const Vec3V cp = V3Neg(c);

		const FloatV d1 = V3Dot(ab, ap); //  snom
		const FloatV d2 = V3Dot(ac, ap); //  tnom
		const FloatV d3 = V3Dot(ab, bp); // -sdenom
		const FloatV d4 = V3Dot(ac, bp); //  unom = d4 - d3
		const FloatV d5 = V3Dot(ab, cp); //  udenom = d5 - d6
		const FloatV d6 = V3Dot(ac, cp); // -tdenom
		const FloatV unom = FSub(d4, d3);
		const FloatV udenom = FSub(d5, d6);

		//const FloatV va = FSub(FMul(d3, d6), FMul(d5, d4));//edge region of BC
		//const FloatV vb = FSub(FMul(d5, d2), FMul(d1, d6));//edge region of AC
		//const FloatV vc = FSub(FMul(d1, d4), FMul(d3, d2));//edge region of AB

		//check if p in vertex region outside a
		const BoolV con00 = FIsGrtrOrEq(zero, d1); // snom <= 0
		const BoolV con01 = FIsGrtrOrEq(zero, d2); // tnom <= 0
		
		//check if p in vertex region outside b
		const BoolV con10 = FIsGrtrOrEq(d3, zero);
		const BoolV con11 = FIsGrtrOrEq(d3, d4);
		
		//check if p in vertex region outside of c
		const BoolV con20 = FIsGrtrOrEq(d6, zero);
		const BoolV con21 = FIsGrtrOrEq(d6, d5); 

		const BoolV con0 = BAnd(con00, con01); // vertex region a
		const BoolV con1 = BAnd(con10, con11); // vertex region b
		const BoolV con2 = BAnd(con20, con21); // vertex region c

		const BoolV bVertexRegion = BOr(con0, BOr(con1, con2));
		if(BAllEq(bVertexRegion, bTrue))
		{
			const Vec3V tempClosestA = V3Sel(con0, A[0], V3Sel(con1, A[1], A[2]));
			const Vec3V tempClosestB = V3Sel(con0, B[0], V3Sel(con1, B[1], B[2]));
			closestA = tempClosestA;
			closestB = tempClosestB;
			return  V3Sub(tempClosestA, tempClosestB);
		}

		const FloatV va = FNegMulSub(d5, d4, FMul(d3, d6));//edge region of BC
		const FloatV vb = FNegMulSub(d1, d6, FMul(d5, d2));//edge region of AC
		const FloatV vc = FNegMulSub(d3, d2, FMul(d1, d4));//edge region of AB

		//check if p in edge region of AB
		const BoolV con30 = FIsGrtrOrEq(zero, vc);
		const BoolV con31 = FIsGrtrOrEq(d1, zero);
		const BoolV con32 = FIsGrtrOrEq(zero, d3);
		
	
		//check if p in edge region of BC
		const BoolV con40 = FIsGrtrOrEq(zero, va);
		const BoolV con41 = FIsGrtrOrEq(d4, d3);
		const BoolV con42 = FIsGrtrOrEq(d5, d6);

		//check if p in edge region of AC
		const BoolV con50 = FIsGrtrOrEq(zero, vb);
		const BoolV con51 = FIsGrtrOrEq(d2, zero);
		const BoolV con52 = FIsGrtrOrEq(zero, d6);


		const BoolV con3 = BAnd(con30, BAnd(con31, con32));//edge AB region
		const BoolV con4 = BAnd(con40, BAnd(con41, con42));//edge BC region
		const BoolV con5 = BAnd(con50, BAnd(con51, con52));//edeg AC region
	
		const FloatV toRecipA = FSub(d1, d3);
		const FloatV toRecipB = FAdd(unom, udenom);
		const FloatV toRecipC = FSub(d2, d6);
		const FloatV toRecipD = FAdd(va, FAdd(vb, vc));

		const Vec4V tmp = V4Merge(toRecipA, toRecipB, toRecipC, toRecipD);
		//const Vec4V recipTmp = V4Recip(tmp);
		const Vec4V recipTmp = V4Sel(V4IsGrtr(V4Abs(tmp), V4Splat(eps)), V4Recip(tmp), V4Splat(zero));

		//TODO - can we roll these loops into 1???
		const BoolV bEdgeRegion = BOr(con3, BOr(con4, con5));

		if(BAllEq(bEdgeRegion, bTrue))
		{
			const FloatV sScale = FMul(d1, V4GetX(recipTmp));
			const FloatV uScale = FMul(unom, V4GetY(recipTmp));
			const FloatV tScale = FMul(d2, V4GetZ(recipTmp));

			const Vec3V A1 = V3Sel(con3, A[1], A[2]);
			const Vec3V B1 = V3Sel(con3, B[1], B[2]);
			const Vec3V A0 = V3Sel(con3, A[0], V3Sel(con4, A[1], A[0]));
			const Vec3V B0 = V3Sel(con3, B[0], V3Sel(con4, B[1], B[0]));

			const Vec3V v = V3Sub(A1, A0);
			const Vec3V w = V3Sub(B1, B0);

			const FloatV scale = FSel(con3, sScale, FSel(con4, uScale, tScale));

			const Vec3V tempClosestA = V3ScaleAdd(v, scale, A0);
			const Vec3V tempClosestB = V3ScaleAdd(w, scale, B0);
			closestA = tempClosestA;
			closestB = tempClosestB;
			return V3Sub(tempClosestA, tempClosestB);
		}

		//P must project inside face region. Compute Q using Barycentric coordinates
		const FloatV denom = V4GetW(recipTmp);
		const Vec3V v0 = V3Sub(A[1], A[0]);
		const Vec3V v1 = V3Sub(A[2], A[0]);
		const Vec3V w0 = V3Sub(B[1], B[0]);
		const Vec3V w1 = V3Sub(B[2], B[0]);

		const FloatV t = FMul(vb, denom);
		const FloatV w = FMul(vc, denom);
		const Vec3V vA1 = V3Scale(v1, w);
		const Vec3V vB1 = V3Scale(w1, w);
		const Vec3V tempClosestA = V3Add(A[0], V3ScaleAdd(v0, t, vA1));
		const Vec3V tempClosestB = V3Add(B[0], V3ScaleAdd(w0, t, vB1));
		closestA = tempClosestA;
		closestB = tempClosestB;
		return V3Sub(tempClosestA, tempClosestB);
	}

	
	PX_NOALIAS PX_GJK_FORCE_INLINE Ps::aos::Vec3V closestPtPointTriangleBaryCentric(const Ps::aos::Vec3VArg a, const Ps::aos::Vec3VArg b, const Ps::aos::Vec3VArg c, /*PxU32* PX_RESTRICT indices,*/ PxU32& size, Ps::aos::FloatV& t, Ps::aos::FloatV& w)
	{
		using namespace Ps::aos;

		size = 3;
		const FloatV zero = FZero();
		const FloatV one = FOne();
		const BoolV bTrue = BTTTT();
		const FloatV eps = FEps();


		//we have a degenerated case check in the tetraheron, so the triangle should not be a degenerated triangle, so we can get rid of this check
		//const FloatV eps = FEps();
		//const Vec3V ab = V3Sub(b, a);
		//const Vec3V ac = V3Sub(c, a);
		//const Vec3V bc = V3Sub(b, c);

		//const FloatV dac = V3Dot(ac, ac);
		//const FloatV dbc = V3Dot(bc, bc);
		//if(FAllGrtrOrEq(eps, FMin(dac, dbc)))
		//{
		//	__asm{int 3};
		//	//degenerate
		//	w = zero;
		//	return closestPtPointSegmentBaryCentric(a, b, size, t);
		//}
		
		const Vec3V ab = V3Sub(b, a);
		const Vec3V ac = V3Sub(c, a);
		const Vec3V ap = V3Neg(a);
		const Vec3V bp = V3Neg(b);
		const Vec3V cp = V3Neg(c);

		const FloatV d1 = V3Dot(ab, ap); //  snom
		const FloatV d2 = V3Dot(ac, ap); //  tnom
		const FloatV d3 = V3Dot(ab, bp); // -sdenom
		const FloatV d4 = V3Dot(ac, bp); //  unom = d4 - d3
		const FloatV d5 = V3Dot(ab, cp); //  udenom = d5 - d6
		const FloatV d6 = V3Dot(ac, cp); // -tdenom
		const FloatV unom = FSub(d4, d3);
		const FloatV udenom = FSub(d5, d6);


		//check if p in vertex region outside a
		const BoolV con00 = FIsGrtrOrEq(zero, d1); // snom <= 0
		const BoolV con01 = FIsGrtrOrEq(zero, d2); // tnom <= 0

		//check if p in vertex region outside b
		const BoolV con10 = FIsGrtrOrEq(d3, zero);
		const BoolV con11 = FIsGrtrOrEq(d3, d4);
		
		//check if p in vertex region outside c
		const BoolV con20 = FIsGrtrOrEq(d6, zero);
		const BoolV con21 = FIsGrtrOrEq(d6, d5); 

		const BoolV con0 = BAnd(con00, con01); // vertex region a
		const BoolV con1 = BAnd(con10, con11); // vertex region b
		const BoolV con2 = BAnd(con20, con21); // vertex region c

		const BoolV bVertexRegion = BOr(con0, BOr(con1, con2));
		if(BAllEq(bVertexRegion, bTrue))
		{
			t = FSel(con1, one, zero);
			w = FSel(con2, one, zero);
			return V3Sel(con0, a, V3Sel(con1, b, c));
		}

		const FloatV va = FNegMulSub(d5, d4, FMul(d3, d6));//edge region of BC
		const FloatV vb = FNegMulSub(d1, d6, FMul(d5, d2));//edge region of AC
		const FloatV vc = FNegMulSub(d3, d2, FMul(d1, d4));//edge region of AB

		//check if p in edge region of AB
		const BoolV con30 = FIsGrtrOrEq(zero, vc);
		const BoolV con31 = FIsGrtrOrEq(d1, zero);
		const BoolV con32 = FIsGrtrOrEq(zero, d3);
	
		//check if p in edge region of BC
		const BoolV con40 = FIsGrtrOrEq(zero, va);
		const BoolV con41 = FIsGrtrOrEq(d4, d3);
		const BoolV con42 = FIsGrtrOrEq(d5, d6);
		
		//check if p in edge region of AC
		const BoolV con50 = FIsGrtrOrEq(zero, vb);
		const BoolV con51 = FIsGrtrOrEq(d2, zero);
		const BoolV con52 = FIsGrtrOrEq(zero, d6);

		const BoolV con3 = BAnd(con30, BAnd(con31, con32));//edge AB region
		const BoolV con4 = BAnd(con40, BAnd(con41, con42)); //edge BC region
		const BoolV con5 = BAnd(con50, BAnd(con51, con52));//edge AC region

		//TODO - can we roll these loops into 1???
		const BoolV bEdgeRegion = BOr(con3, BOr(con4, con5));

		const FloatV toRecipA = FSub(d1, d3);
		const FloatV toRecipB = FAdd(unom, udenom);
		const FloatV toRecipC = FSub(d2, d6);
		const FloatV toRecipD = FAdd(va, FAdd(vb, vc));
		
		const Vec4V tmp = V4Merge(toRecipA, toRecipB, toRecipC, toRecipD);
		//const Vec4V recipTmp = V4Recip(tmp);
		const Vec4V recipTmp = V4Sel(V4IsGrtr(V4Abs(tmp), V4Splat(eps)), V4Recip(tmp), V4Splat(zero));

		if(BAllEq(bEdgeRegion, bTrue))
		{
			
			const FloatV sScale = FMul(d1, V4GetX(recipTmp));
			const FloatV uScale = FMul(unom, V4GetY(recipTmp));
			const FloatV tScale = FMul(d2, V4GetZ(recipTmp));

			const Vec3V Q1 = V3Sel(con3, b, c);
			const Vec3V Q0 = V3Sel(con3, a, V3Sel(con4, b, a));
			const Vec3V q01 = V3Sub(Q1, Q0);
			const FloatV scale = FSel(con3, sScale, FSel(con4, uScale, tScale));
			t = FSel(con3, sScale, FSel(con4, FSub(one, uScale), zero));
			w = FSel(con3, zero,   FSel(con4, uScale,			 tScale));
			return V3ScaleAdd(q01, scale, Q0);
		} 

		//P must project inside face region. Compute Q using Barycentric coordinates
		const FloatV denom = V4GetW(recipTmp);		
		const FloatV _t = FMul(vb, denom);
		const FloatV _w = FMul(vc, denom);
		t = _t;
		w = _w;
	
		return V3Add(a, V3ScaleAdd(ab, _t, V3Scale(ac, _w)));
	}

	PX_NOALIAS PX_FORCE_INLINE Ps::aos::Vec3V GJKCPairDoSimplex(Ps::aos::Vec3V* PX_RESTRICT Q, Ps::aos::Vec3V* PX_RESTRICT A, Ps::aos::Vec3V* PX_RESTRICT B, 
		const Ps::aos::Vec3VArg support, const Ps::aos::Vec3VArg supportA, const Ps::aos::Vec3VArg supportB, PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB)
	{
		using namespace Ps::aos;

		//const PxU32 tempSize = size;
		//calculate a closest from origin to the simplex
		switch(size)
		{
		case 1:
			{
				closestA = supportA;
				closestB = supportB;
				return support;
			}
		case 2:
			{
			return closestPtPointSegment(Q[0], support, A[0], supportA, B[0], supportB, size, closestA, closestB);
			}
		case 3:
			{
			return closestPtPointTriangle(Q[0], Q[1], support, Q, A, B, size, closestA, closestB);
			}
		case 4:
			return closestPtPointTetrahedron(Q, A, B, size, closestA, closestB);
		default:
			PX_ASSERT(0);
		}
		return support;
	}


	PX_NOALIAS PX_FORCE_INLINE Ps::aos::Vec3V GJKCPairDoSimplex(Ps::aos::Vec3V* PX_RESTRICT Q, Ps::aos::Vec3V* PX_RESTRICT A, Ps::aos::Vec3V* PX_RESTRICT B, PxI32* PX_RESTRICT aInd, PxI32* PX_RESTRICT bInd, 
		const Ps::aos::Vec3VArg support, const Ps::aos::Vec3VArg supportA, const Ps::aos::Vec3VArg supportB, PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB)
	{
		using namespace Ps::aos;

		//const PxU32 tempSize = size;
		//calculate a closest from origin to the simplex
		switch(size)
		{
		case 1:
			{
				closestA = supportA;
				closestB = supportB;
				return support;
			}
		case 2:
			{
			//return closestPtPointSegment(Q, A, B, size, closestA, closestB);
			return closestPtPointSegment(Q[0], support, A[0], supportA, B[0], supportB, size, closestA, closestB);
			}
		case 3:
			{
			return closestPtPointTriangle(Q[0], Q[1], support, Q, A, B, size, closestA, closestB);
			}
		case 4:
			return closestPtPointTetrahedron(Q, A, B, aInd, bInd, support, supportA, supportB, size, closestA, closestB);
		default:
			PX_ASSERT(0);
		}
		return support;
	}
}

}

#endif
