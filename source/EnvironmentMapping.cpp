#include "EnvironmentMapping.h"

EnvironmentMapping::EnvironmentMapping(OpenGLFunctions &gl)
	: m_mapping(CubeMapping)
	, m_quad(nullptr)
{
	m_envmaps[CubeMapping]       = FileAssociatedTexture::getOrCreateCube("data/envMapping/env_cube_?.png", gl);
    m_envmaps[PolarMapping]      = FileAssociatedTexture::getOrCreate2D("data/envMapping/env_polar_half.png", gl);
    m_envmaps[ParaboloidMapping] = FileAssociatedTexture::getOrCreate2D("data/envMapping/env_paraboloid.png", gl);
    m_envmaps[SphereMapping]     = FileAssociatedTexture::getOrCreate2D("data/envMapping/env_sphere.png", gl);
	
    m_program = createBasicShaderProgram("data/envMapping/envmap.vert", "data/envMapping/envmap.frag");

	m_quad = new ScreenAlignedQuad(gl);  
}

EnvironmentMapping::~EnvironmentMapping()
{
	delete m_program;
	qDeleteAll(m_shaders);
	delete m_quad;
}

void EnvironmentMapping::bindEnvMaps(GLenum target, OpenGLFunctions &gl)
{
    // In the case of cube mapping another texture 
    // type is required to be bound (samplerCube instead of sampler2D).

    if (CubeMapping != m_mapping)
    {
        gl.glActiveTexture(target);
        gl.glEnable(GL_TEXTURE_2D);
        gl.glBindTexture(GL_TEXTURE_2D, m_envmaps[m_mapping]);
    }
    else
    {
        gl.glActiveTexture(target + 1);
        gl.glEnable(GL_TEXTURE_CUBE_MAP);
        gl.glBindTexture(GL_TEXTURE_CUBE_MAP, m_envmaps[m_mapping]);
    }
}

void EnvironmentMapping::unbindEnvMaps(GLenum target, OpenGLFunctions &gl)
{
    if (CubeMapping == m_mapping)
    {
        gl.glActiveTexture(target + 1);
        gl.glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        gl.glDisable(GL_TEXTURE_CUBE_MAP);
    }
    else
    {
        gl.glActiveTexture(target);
        gl.glBindTexture(GL_TEXTURE_2D, 0);
        gl.glDisable(GL_TEXTURE_2D);
    }
}

//ToDo: setUniforms
void EnvironmentMapping::update(Camera * camera)
{
	//set transforms?

	if(m_program->isLinked())
	{
		m_program->bind();

		m_program->setUniformValue("mapping", m_mapping);
		 
		m_program->setUniformValue("projectionTransformInv", camera->projection().inverted());
		m_program->setUniformValue("viewTransform", camera->view());

		m_program->setUniformValue("envmap", 0);
		m_program->setUniformValue("cubemap", 1);

		m_program->release();
	}
}

void EnvironmentMapping::nextMapping(Camera * camera)
{
	m_mapping = static_cast<EnvironmentMappingType>((m_mapping + 1) % 4);
    update(camera);
}

void EnvironmentMapping::paintEnvmap(float timef, OpenGLFunctions &gl)
{
	//ToDo change m_programms structure
	QOpenGLShaderProgram * program(m_program);

    if (!program->isLinked())
        return;

    bindEnvMaps(GL_TEXTURE0, gl);

    gl.glDepthMask(GL_FALSE);
    gl.glDepthFunc(GL_EQUAL);
   
    program->bind();
	m_quad->draw(gl);
    program->release();
        
    gl.glDepthFunc(GL_LESS);
    gl.glDepthMask(GL_TRUE);
	
    unbindEnvMaps(GL_TEXTURE0, gl);
}