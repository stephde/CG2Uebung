#include "ShadowRenderer.h"



ShadowRenderer::ShadowRenderer(OpenGLFunctions & gl)
{
	m_shadowMapSize = 2048;

	m_program = createBasicShaderProgram("data/shadowmap.vert", "data/shadowmap.frag");
	m_program->bind();
	m_program->setUniformValue("shadowmap", 1);
    //m_program->setUniformValue("light", m_light);
	m_program->release();

	gl.glGenTextures(1, &m_depthTex);
    gl.glBindTexture(GL_TEXTURE_2D, m_depthTex);
    gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, m_shadowMapSize, m_shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	GLfloat * borderColor = new GLfloat;
	*borderColor = 1.0;

	gl.glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    gl.glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    gl.glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl.glBindTexture(GL_TEXTURE_2D, 0);

    gl.glGenFramebuffers(1, &m_depthFbo);
    gl.glBindFramebuffer(GL_FRAMEBUFFER, m_depthFbo);

    gl.glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTex, 0);
    gl.glDrawBuffer(GL_NONE);
    gl.glReadBuffer(GL_NONE);

   if(gl.glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "depth fbo invalid";

    gl.glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
ShadowRenderer::~ShadowRenderer()
{

}


/*****
			Render scene Geometry beetwen the calls of start- and endShadowing 
			to render shadows
*****/

QOpenGLShaderProgram* ShadowRenderer::startShadowing(QVector3D light, OpenGLFunctions & gl)
{
	gl.glBindFramebuffer(GL_FRAMEBUFFER, m_depthFbo);
    gl.glViewport(0, 0, m_shadowMapSize, m_shadowMapSize);

    gl.glClearDepth(1.f);
    gl.glClear(GL_DEPTH_BUFFER_BIT);
	
	Camera * lcam = new Camera(light);

	QMatrix4x4 L = lcam->viewProjection();
	
    m_program->bind();
    m_program->setUniformValue("viewProjection", L);
    m_program->release();

	return m_program;
}

void ShadowRenderer::endShadowing(Camera * camera, OpenGLFunctions & gl)
{	
    gl.glBindFramebuffer(GL_FRAMEBUFFER, 0);
    gl.glViewport(0, 0, camera->viewport().width(), camera->viewport().height());
}

void ShadowRenderer::paintShadow(float timef, OpenGLFunctions & gl)
{

}