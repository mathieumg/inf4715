#ifndef GLES2_RENDERER_H
#define GLES2_RENDERER_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_GLES2)

#if defined(RENDERER_ANDROID)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>

extern PFNGLMAPBUFFEROESPROC   glMapBufferOES;
extern PFNGLUNMAPBUFFEROESPROC glUnmapBufferOES;

#elif defined(RENDERER_IOS)

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#endif

#define GL_TEXTURE0_ARB GL_TEXTURE0
#define glBindBufferARB glBindBuffer
#define glBufferDataARB glBufferData
#define GL_ELEMENT_ARRAY_BUFFER_ARB GL_ELEMENT_ARRAY_BUFFER
#define GL_ARRAY_BUFFER_ARB GL_ARRAY_BUFFER
#define GL_STATIC_DRAW_ARB GL_STATIC_DRAW
#define GL_DYNAMIC_DRAW_ARB GL_DYNAMIC_DRAW
#define glGenBuffersARB glGenBuffers
#define glDeleteBuffersARB glDeleteBuffers
#define GL_CLAMP GL_CLAMP_TO_EDGE
#define GL_WRITE_ONLY GL_WRITE_ONLY_OES
#define GL_READ_WRITE GL_WRITE_ONLY_OES

extern bool GLEW_ARB_vertex_buffer_object;

#define glActiveTextureARB glActiveTexture
#define glClientActiveTextureARB(a)

/* GL_EXT_texture_compression_dxt1 */
#ifndef GL_EXT_texture_compression_dxt1
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#endif

/* GL_EXT_texture_compression_s3tc */
#ifndef GL_EXT_texture_compression_s3tc
/* GL_COMPRESSED_RGB_S3TC_DXT1_EXT defined in GL_EXT_texture_compression_dxt1 already. */
/* GL_COMPRESSED_RGBA_S3TC_DXT1_EXT defined in GL_EXT_texture_compression_dxt1 already. */
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif

#define GL_RGBA8 GL_RGBA
#define GL_BGRA8 GL_BGRA

#include <Renderer.h>

namespace SampleFramework {
	class SamplePlatform;
}

namespace SampleRenderer
{

class RendererVertexBuffer;
class RendererIndexBuffer;
class RendererMaterial;

void PxToGL(GLfloat *gl44, const physx::PxMat44 &mat);
void PxToGLColumnMajor(GLfloat *gl44, const physx::PxMat44 &mat);
void RenToGL(GLfloat *gl44, const RendererProjection &proj);
	
class GLES2Renderer : public Renderer
{
	public:
		GLES2Renderer(const RendererDesc &desc, const char* assetDir);
		virtual ~GLES2Renderer(void);

	private:
		bool begin(void);
		void end(void);
		void checkResize(void);
	
	public:
		// clears the offscreen buffers.
		virtual void clearBuffers(void);
		
		// presents the current color buffer to the screen.
		virtual bool swapBuffers(void);
		// get the device pointer (void * abstraction)
		virtual void *getDevice(); 
		virtual bool captureScreen(const char* filename) { PX_ASSERT(0 && "implement captureScreen for GLES2renderer"); }

		// get the window size
		void getWindowSize(PxU32 &width, PxU32 &height) const;		
		// copy common renderer variables to the material (like g_MVP, g_modelMatrix, etc)
		virtual void setCommonRendererParameters();

		virtual RendererVertexBuffer   *createVertexBuffer(  const RendererVertexBufferDesc   &desc);
		virtual RendererIndexBuffer    *createIndexBuffer(   const RendererIndexBufferDesc    &desc);
		virtual RendererInstanceBuffer *createInstanceBuffer(const RendererInstanceBufferDesc &desc);
		virtual RendererTexture2D      *createTexture2D(     const RendererTexture2DDesc      &desc);
		virtual RendererTarget         *createTarget(        const RendererTargetDesc         &desc);
		virtual RendererMaterial       *createMaterial(      const RendererMaterialDesc       &desc);
		virtual RendererMesh           *createMesh(          const RendererMeshDesc           &desc);
		virtual RendererLight          *createLight(         const RendererLightDesc          &desc);

		virtual void finishRendering();
        virtual void finalizeTextRender(void);
	
	private:
		virtual void bindViewProj(const physx::PxMat44 &inveye, const PxMat44 &proj);
		virtual void bindFogState(const RendererColor &fogColor, float fogDistance);
		virtual void bindAmbientState(const RendererColor &ambientColor);
		virtual void bindDeferredState(void);
		virtual void bindMeshContext(const RendererMeshContext &context);
		virtual void beginMultiPass(void);
		virtual void endMultiPass(void);
		virtual void renderDeferredLight(const RendererLight &light);
		virtual PxU32 convertColor(const RendererColor& color) const;
		
		virtual bool isOk(void) const;
		
		virtual	bool initTexter();
		virtual	void setupTextRenderStates();
		virtual	void resetTextRenderStates();
		virtual	void renderTextBuffer(const void* vertices, PxU32 nbVerts, const PxU16* indices, PxU32 nbIndices, RendererMaterial* material);
		virtual	void renderLines2D(const void* vertices, PxU32 nbVerts);
		virtual	void setupScreenquadRenderStates();
		virtual	void resetScreenquadRenderStates();
        
        void renderQuad(const void* vertices, PxU32 nbVerts, const PxU16* indices, PxU32 nbIndices, RendererMaterial* material);
	private:
		SampleFramework::SamplePlatform*				m_platform;
		RendererVertexBuffer*							m_textVertexBuffer;
		RendererIndexBuffer*							m_textIndexBuffer;
        PxU32                                           m_textVertexBufferOffset; 
        PxU32                                           m_textIndexBufferOffset;
        RendererMaterial*                               m_textMaterial;
		RendererMesh*									m_textMesh;
        
        RendererVertexBuffer*							m_quadVertexBuffer;
		RendererIndexBuffer*							m_quadIndexBuffer;
		RendererMesh*									m_quadMesh;
        
		PxU32         m_displayWidth;
		PxU32         m_displayHeight;
		
		physx::PxMat44 m_viewMatrix;
		GLfloat		  m_glProjectionMatrix[16];
		PxVec3		  m_eyePosition;
		PxVec3		  m_eyeDirection;
		GLfloat		  m_ambientColor[3];
		GLfloat		  m_lightColor[3];
		GLfloat		  m_intensity;
		GLfloat		  m_lightDirection[3];
		GLfloat		  m_fogColorAndDistance[4];
        bool          m_quadRender;
};
}
#endif // #if defined(RENDERER_ENABLE_GLES2)
#endif
