#ifndef ENVIRONMENTMAPPING_H_INCLUDED
#define ENVIRONMENTMAPPING_H_INCLUDED

#include "Camera.h"
#include "ScreenAlignedQuad.h"
#include "AbstractRenderer.h"


class ScreenAlignedQuad;
class QOpenGLShader;
class QOpenGLShaderProgram;


class EnvironmentMapping: public AbstractRenderer {

public:
	EnvironmentMapping(OpenGLFunctions &gl);
	~EnvironmentMapping();

    void bindEnvMaps(GLenum target, OpenGLFunctions &gl);
    void unbindEnvMaps(GLenum target, OpenGLFunctions &gl);

	void update(Camera * camera);
	
	void nextMapping(Camera * camera);

	void paintEnvmap(float timef, OpenGLFunctions &gl);

	enum EnvironmentMappingType
    {
        CubeMapping
    ,   PolarMapping
    ,   ParaboloidMapping
    ,   SphereMapping
    };

private:
		
    ScreenAlignedQuad * m_quad;
    
    EnvironmentMappingType m_mapping;
    QMap<EnvironmentMappingType, GLuint> m_envmaps;
};

#endif
