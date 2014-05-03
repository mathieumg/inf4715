
#ifndef GLES2_RENDERER_DIRECTIONAL_LIGHT_H
#define GLES2_RENDERER_DIRECTIONAL_LIGHT_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_GLES2)

#include <RendererDirectionalLight.h>

#include "GLES2Renderer.h"

namespace SampleRenderer
{
class RendererDirectionalLightDesc;

class GLES2RendererDirectionalLight : public RendererDirectionalLight
{
	public:
		GLES2RendererDirectionalLight(const RendererDirectionalLightDesc &desc, 
										GLES2Renderer &renderer, 
										GLfloat (&_lightColor)[3], GLfloat& _lightIntensity, GLfloat (&_lightDirection)[3]);
		virtual ~GLES2RendererDirectionalLight(void);
		
		virtual void bind(void) const;
	private:
		GLfloat (&m_lightColor)[3];
		GLfloat& m_lightIntensity;
		GLfloat (&m_lightDirection)[3];
};
}

#endif // #if defined(RENDERER_ENABLE_GLES2)
#endif
