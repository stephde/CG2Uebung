#include "ObjectRenderer.h"

ObjectRenderer::ObjectRenderer(OpenGLFunctions & gl)
{
	m_program = createBasicShaderProgram("data/assimp.vert", "data/assimp.frag");
    m_program->bindAttributeLocation("a_vertex", 0);
    m_program->bindAttributeLocation("a_normal", 1);
    m_program->bindAttributeLocation("a_texc", 2);    
    m_program->link();

	m_useShadow = true;

	m_shadowRenderer = new ShadowRenderer(gl);
}

ObjectRenderer::~ObjectRenderer()
{
	qDeleteAll(m_sceneObjects);
}


int ObjectRenderer::addSceneObject(const QString & filePath, OpenGLFunctions & gl)
{
	int index = m_sceneObjects.size();

	m_sceneObjects.insert(index, new AssimpScene(gl, filePath, true));

	return index;
}

void ObjectRenderer::removeSceneObject(int index)
{
	if(0 <= index && index <= m_sceneObjects.size())
	{
		m_sceneObjects.remove(index);
	}
}

void ObjectRenderer::transformSceneObject(int index, 
								QVector3D translate, 
								float angle, QVector3D rotVec, 
								QVector3D scale)
{
	if(0 <= index && index < m_sceneObjects.size())
	{
		m_sceneObjects[index]->transform().translate(translate);
		m_sceneObjects[index]->transform().rotate(angle, rotVec);
		m_sceneObjects[index]->transform().scale(scale);
	}
}

void ObjectRenderer::paintSceneObjects(float timef, OpenGLFunctions & gl, Camera * camera, QVector3D light)
{
	m_program->bind();
    m_program->setUniformValue("light", light);
    m_program->setUniformValue("useshadow", false);
    m_program->setUniformValue("projection", camera->projection());
    m_program->setUniformValue("view", camera->view());
    m_program->release();

	for(auto i : m_sceneObjects.toStdMap())
	{
		i.second->draw(gl, *m_program);
	}

	if(m_useShadow)
	{
		QOpenGLShaderProgram * program = m_shadowRenderer->startShadowing(light, gl);

		paintSceneObjects(timef, gl, camera, program);

		m_shadowRenderer->endShadowing(camera, gl);
	}
}

void ObjectRenderer::paintSceneObjects(float timef, OpenGLFunctions & gl, Camera * camera,  QOpenGLShaderProgram * program, QVector3D light)
{
	program->bind();
    program->setUniformValue("light", light);
    program->setUniformValue("useshadow", true);
    program->setUniformValue("projection", camera->projection());
    program->setUniformValue("view", camera->view());
    program->release();

	for(auto i : m_sceneObjects.toStdMap())
	{
		i.second->draw(gl, *m_program);
	}
}