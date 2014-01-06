#ifndef ABSTRACTRENDERER_H_INCLUDED
#define ABSTRACTRENDERER_H_INCLUDED

#include "FileAssociatedShader.h"
#include "FileAssociatedTexture.h"

class AbstractRenderer{
public:
	virtual ~AbstractRenderer();
		
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

};

#endif
