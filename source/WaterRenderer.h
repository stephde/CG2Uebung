#ifndef WATERPAINTER_INCLUDED
#define WATERPAINTER_INCLUDED

#include "Terrain.h"
#include "OpenGLFunctions.h"
#include "AbstractRenderer.h"
#include "Camera.h"

class QOpenGLShader;
class QOpenGLShaderProgram;

class WaterRenderer: public AbstractRenderer
{
public:
	WaterRenderer(OpenGLFunctions &gl);
	~WaterRenderer();

	void update(Camera * camera);

	void paintWater(float timef, GLuint height, OpenGLFunctions &gl);

protected:
	Terrain * m_terrain;

	GLuint m_water;
    //GLuint m_caustics;
	
    GLuint m_waterheights;
    //GLuint m_waternormals;
};

#endif
