
#include <vector>
#include <cassert>

#include "OpenGLFunctions.h"

#include "Terrain.h"


Terrain::Terrain(
	unsigned short size
,	OpenGLFunctions & gl)
: m_vertices(QOpenGLBuffer::VertexBuffer)
, m_indices(QOpenGLBuffer::IndexBuffer  )
{
	m_vao.create();
	m_vao.bind();

	m_vertices.create();
	m_vertices.setUsagePattern(QOpenGLBuffer::StaticDraw);

	m_indices.create();
	m_indices.setUsagePattern(QOpenGLBuffer::StaticDraw);

    // Task_1_1 - ToDo Begin

    strip(size, m_vertices, m_indices, -1);

    // Configure your Vertex Attribute Pointer based on your vertex buffer (mainly number of coefficients ;)).

	gl.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);//
    gl.glEnableVertexAttribArray(0);

    // Task_1_1 - ToDo End

    m_vao.release();
}

void Terrain::draw(
	OpenGLFunctions & gl
,	const GLenum mode)
{
    // Task_1_1 - ToDo Begin

    // Setup the OpenGL state appropriate to the given and your personal drawing requirements.
    // You might take depth tests, primitive restart, and culling into account.

    // gl.glEnable(...
    // ...
	gl.glEnable(GL_PRIMITIVE_RESTART);
	//glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	gl.glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	gl.glPrimitiveRestartIndex(static_cast<unsigned short int>(-1));
    gl.glEnableVertexAttribArray(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    m_vao.bind();
	gl.glDrawElements(mode, m_indices.size() / sizeof(unsigned short int), GL_UNSIGNED_SHORT, 0);
    m_vao.release();

    // Remember to "clean up" the state after rendering.

    // gl.glDisable(...
    // ...
	gl.glDisableVertexAttribArray(0);
	//glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	gl.glDisable(GL_PRIMITIVE_RESTART);
	gl.glDisable(GL_CULL_FACE);
    // Task_1_1 - ToDo End
}

void Terrain::strip(
	const unsigned short size
,	QOpenGLBuffer & vertices
,	QOpenGLBuffer & indices
,	const GLuint primitiveRestartIndex)
{
    // Task_1_1 - ToDo Begin

    // perhaps, check for pointless parameter input

    // tips: probably use some for loops to fill up a std or qt container with 
    // floats or other structs, fitting the purpose. further, remember that 
    // gradually summing up lots of small floats might lead to major inaccuracies ...

	std::vector<float> vertex;

	for(int i=0;i < size;i++)
	{
		for (int k = 0; k < size; k++)
		{
			vertex.push_back((float)i);
			vertex.push_back(0.0);
			vertex.push_back((float)k); 
		}
	}
	vertices.bind();
	vertices.allocate(vertex.data(), (size+1) * size * 3 * sizeof(float));


	std::vector<unsigned short int> index;

	for(int y=0; y < size-1;y++)
	{
		for(int x=0; x <= size; x++)
		{
			if(x == size ){
				index.push_back(static_cast<unsigned short int>(primitiveRestartIndex));
			}else{
				index.push_back(y*size + x);
				index.push_back((y+1)*size + x);
			}
		}
	}
	indices.bind();
	indices.allocate(index.data(), ((size * 2) * (size) + size) * sizeof(unsigned short int));

    // Task_1_1 - ToDo End
}
