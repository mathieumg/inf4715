
#ifndef GLES2_RENDERER_MESH_H
#define GLES2_RENDERER_MESH_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_GLES2)

#include <RendererMesh.h>

#include "GLES2Renderer.h"

namespace SampleRenderer
{
class GLES2RendererMesh : public RendererMesh
{
	public:
		GLES2RendererMesh(const RendererMeshDesc &desc);
		virtual ~GLES2RendererMesh(void);
	
	public:
		virtual void renderIndices(PxU32 numVertices, PxU32 firstIndex, PxU32 numIndices, RendererIndexBuffer::Format indexFormat) const;
		virtual void renderVertices(PxU32 numVertices) const;
		
		virtual void renderIndicesInstanced(PxU32 numVertices, PxU32 firstIndex, PxU32 numIndices, RendererIndexBuffer::Format indexFormat, RendererMaterial *material) const;
		virtual void renderVerticesInstanced(PxU32 numVertices, RendererMaterial *material) const;
};
}
#endif // #if defined(RENDERER_ENABLE_OPENGL)
#endif
