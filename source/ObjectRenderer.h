#ifndef OBJECTRENDER_H_INCLUDED
#define OBJECTRENDER_H_INCLUDED


#include "AbstractRenderer.h"
#include "AssimpScene.h"
#include "Camera.h"
#include "ShadowRenderer.h"


class ObjectRenderer : public AbstractRenderer
{
public:
	ObjectRenderer(OpenGLFunctions & gl);
	~ObjectRenderer();

	int addSceneObject(const QString & filePath, OpenGLFunctions & gl);
	void removeSceneObject(int index);

	void transformSceneObject(int index, 
								QVector3D translate, 
								float angle = 0.0, QVector3D rotate = QVector3D(1.0, 1.0, 1.0), 
								QVector3D scale = QVector3D(1.0, 1.0, 1.0));

	void paintSceneObjects(float timef, OpenGLFunctions & gl, Camera * camera, QVector3D light = QVector3D(0.0, 0.0, 0.0));
	void paintSceneObjects(float timef, OpenGLFunctions & gl, Camera * camera, QOpenGLShaderProgram * program, QVector3D light = QVector3D(0.0, 0.0, 0.0));

	void enableShadow(){m_useShadow = true;}
	void enableShadow(bool set){m_useShadow = set;}

private:
	QMap<int, AssimpScene *> m_sceneObjects;
	bool m_useShadow;

	ShadowRenderer * m_shadowRenderer;
};

#endif
