#ifndef ENVIRONMENTMAPPING_H_INCLUDED
#define ENVIRONMENTMAPPING_H_INCLUDED

#include <QMatrix4x4>
#include <QMap>
#include <QVector3D>
#include "AbstractPainter.h"
#include "FileAssociatedShader.h"
#include "FileAssociatedTexture.h"
#include "Canvas.h"
#include "Camera.h"
#include "Terrain.h"
#include "Icosahedron.h"
#include "ScreenAlignedQuad.h"


class ScreenAlignedQuad;
class QOpenGLShader;
class QOpenGLShaderProgram;


class EnvironmentMapping{

public:
	EnvironmentMapping(OpenGLFunctions &gl);
	~EnvironmentMapping();

    void bindEnvMaps(GLenum target, OpenGLFunctions &gl);
    void unbindEnvMaps(GLenum target, OpenGLFunctions &gl);

	void update(Camera * camera);

	void paintEnvmap(float timef, OpenGLFunctions &gl);

	enum EnvironmentMappingType
    {
        CubeMapping
    ,   PolarMapping
    ,   ParaboloidMapping
    ,   SphereMapping
    };

protected:
    QOpenGLShaderProgram * createBasicShaderProgram(
        const QString & vertexShaderFileName
    ,   const QString & fragmentShaderFileName);

    QOpenGLShaderProgram * createBasicShaderProgram(
        const QString & vertexShaderFileName
    ,   const QString & geometryShaderFileName
    ,   const QString & fragmentShaderFileName);

	
    QOpenGLShaderProgram * m_program;
    QList<QOpenGLShader *> m_shaders;
		
    ScreenAlignedQuad * m_quad;
    
    EnvironmentMappingType m_mapping;
    QMap<EnvironmentMappingType, GLuint> m_envmaps;
};

#endif
