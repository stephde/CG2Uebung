#ifndef LABELER_H_INCLUDED
#define LABELER_H_INCLUDED

#include "AbstractRenderer.h"
#include "ScreenAlignedQuad.h"
#include "Camera.h"

struct Label{
	ScreenAlignedQuad * label;
	GLuint texture;
	QMatrix4x4 transforms;
};

class Labeler : public AbstractRenderer
{

public:
	Labeler();
	~Labeler();

	void update();
	void paintLabels(float timef, OpenGLFunctions & gl, Camera * cam);

	int createLabel(OpenGLFunctions & gl, const QString & fileName, 
						 QVector3D pos	= QVector3D(1.0, 1.0, 1.0), 
						 QVector3D scale = QVector3D(1.0, 1.0, 1.0), 
						 float angle = 0.0, QVector3D rot	= QVector3D(1.0, 1.0, 1.0));
	void deleteLabel(int index);
	void transformLabel(int index, QMatrix4x4 transform);

private:

	//labels
	QList<Label *> m_labels;
};

#endif
