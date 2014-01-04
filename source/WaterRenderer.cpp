#include "WaterRenderer.h"

WaterRenderer::WaterRenderer(OpenGLFunctions &gl)
{
	
    //m_caustics  = FileAssociatedTexture::getOrCreate2D("data/caustics.png", gl);
	m_water		= FileAssociatedTexture::getOrCreate2D("data/water.png", gl, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
	m_waterheights = FileAssociatedTexture::getOrCreate2D("data/waterheights.png", gl, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    //m_waternormals = FileAssociatedTexture::getOrCreate2D("data/waternormals.png", gl);

	m_program = createBasicShaderProgram("data/water.vert", "data/water.frag");
    m_terrain = new Terrain(256, gl);

}

WaterRenderer::~WaterRenderer()
{
	delete m_terrain;
}

void WaterRenderer::update(Camera * camera)
{
	//set transforms?

	if(m_program->isLinked())
	{
		m_program->bind();

		m_program->setUniformValue("waterHeights", 4);
		m_program->setUniformValue("water",    2);
		//m_program->setUniformValue("caustics", 3);	
        m_program->setUniformValue("height", 0);

		m_program->setUniformValue("transform", camera->viewProjection());

		m_program->release();
	}
}

void WaterRenderer::paintWater(float timef, OpenGLFunctions &gl)
{
	if(m_program->isLinked())
	{
		gl.glEnable(GL_BLEND);
		gl.glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		gl.glActiveTexture(GL_TEXTURE2);
		gl.glEnable(GL_TEXTURE_2D);
		gl.glBindTexture(GL_TEXTURE_2D, m_water);

		gl.glActiveTexture(GL_TEXTURE4);
		gl.glEnable(GL_TEXTURE_2D);
		gl.glBindTexture(GL_TEXTURE_2D, m_waterheights);
		
		m_program->bind();
		m_program->setUniformValue("timef", timef);
        m_terrain->draw(gl);
        m_program->release();

        gl.glBindTexture(GL_TEXTURE_2D, 0);
        gl.glDisable(GL_TEXTURE_2D);

		gl.glActiveTexture(GL_TEXTURE2);		
        gl.glBindTexture(GL_TEXTURE_2D, 0);
        gl.glDisable(GL_TEXTURE_2D);

		gl.glDisable(GL_BLEND);
	}
}