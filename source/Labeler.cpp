#include "labeler.h"



Labeler::Labeler(){
	//m_labels = QList<Label *>();
	//m_labels << new Label();

	m_program = createBasicShaderProgram("data/label.vert", "data/labelAM.frag");
    m_program->bind();
    m_program->bindAttributeLocation("a_vertex", 0);
    m_program->setUniformValue("label", 0);
    m_program->release();
	m_program->link();
}
Labeler::~Labeler(){
	qDeleteAll(m_labels);
}

void Labeler::update(){}

int Labeler::createLabel(OpenGLFunctions & gl,	
						 const QString & fileName,
						 QVector3D pos	, 
						 QVector3D scale , 
						 float angle, QVector3D rot)
{
	int index = m_labels.size();
	m_labels << new Label();
	
	m_labels[index]->label = new ScreenAlignedQuad(gl, 0);
	m_labels[index]->texture = FileAssociatedTexture::getOrCreate2D(fileName, gl);
	m_labels[index]->transforms = QMatrix4x4();
	m_labels[index]->transforms.translate(pos);
	m_labels[index]->transforms.rotate(angle, rot);
	m_labels[index]->transforms.scale(scale);


	return index;
}

void Labeler::deleteLabel(int index){}
void Labeler::transformLabel(int index, QMatrix4x4 transform){}


void Labeler::paintLabels(float timef, OpenGLFunctions & gl, Camera * cam)
{
	gl.glEnable(GL_BLEND);
    gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gl.glActiveTexture(GL_TEXTURE0);
	
	m_program->bind();

	QList<Label *>::iterator i;
	for(i=m_labels.begin(); i != m_labels.end(); i++)
	{
		gl.glBindTexture(GL_TEXTURE_2D, (*i)->texture);
		m_program->setUniformValue("mvp", cam->viewProjection() * (*i)->transforms);
		(*i)->label->draw(gl);
	}
		
    m_program->release();

    gl.glBindTexture(GL_TEXTURE_2D, 0);
    gl.glDisable(GL_BLEND);
}