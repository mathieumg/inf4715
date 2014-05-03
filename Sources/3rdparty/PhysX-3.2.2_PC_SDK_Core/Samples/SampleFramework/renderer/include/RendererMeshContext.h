/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
#ifndef RENDERER_MESH_CONTEXT_H
#define RENDERER_MESH_CONTEXT_H

#include <RendererConfig.h>

namespace SampleRenderer
{

	class Renderer;
	class RendererMesh;
	class RendererMaterial;
	class RendererMaterialInstance;

	class RendererMeshContext
	{
		friend class Renderer;
	public:
		const RendererMesh       *mesh;
		RendererMaterial         *material;
		RendererMaterialInstance *materialInstance;
		const physx::PxMat44	 *transform; // TODO: use a float4x3 instead of a 3x4. Which is basically a transposed 3x4 for better GPU packing.
		const physx::PxF32		 *shaderData;

		// TODO: this is kind of hacky, would prefer a more generalized
		//       solution via RendererMatrialInstance.
		const physx::PxMat44	 *boneMatrices; // TODO: use a float4x3 instead of a 3x4. Which is basically a transposed 3x4 for better GPU packing.
		PxU32                     numBones;

		enum CullMode
		{
			CLOCKWISE = 0,
			COUNTER_CLOCKWISE,
			NONE
		};

		CullMode				cullMode;
		bool					screenSpace;		//TODO: I am not sure if this is needed!

		enum FillMode
		{
			SOLID,
			LINE,
			POINT,
		};
		FillMode				fillMode;

	public:
		RendererMeshContext(void);
		~RendererMeshContext(void);

		bool isValid(void) const;
		bool isLocked(void) const;

	private:
		Renderer *m_renderer;
	};

} // namespace SampleRenderer

#endif
