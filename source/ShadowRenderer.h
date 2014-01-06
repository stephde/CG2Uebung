#ifndef SHADOWRENDERER_H_INCLUDED
#define SHADOWRENDERER_H_INCLUDED


#include "AbstractRenderer.h"
#include "Camera.h"

/***
	remember setting light in painter
***/


class ShadowRenderer : public AbstractRenderer
{
public:
	ShadowRenderer(OpenGLFunctions & gl);
	~ShadowRenderer();

	QOpenGLShaderProgram* startShadowing(QVector3D light, OpenGLFunctions & gl);
	void endShadowing(Camera * camera, OpenGLFunctions & gl);
	void paintShadow(float timef, OpenGLFunctions & gl);

private:
	GLuint m_depthTex;
	GLuint m_depthFbo;
	int m_shadowMapSize;
};

#endif
