
#include <QDebug>
#include <QImage>

#include "MathMacros.h"

#include "DistanceTransform.h"

DistanceTransform::DistanceTransform(
    QImage source
,   unsigned short dstWidth
,   unsigned short dstHeight
,   float distScale)
:   m_source(source)
,   m_dstSize(dstWidth, dstHeight)
,   m_distScale(distScale)
,   m_sedt(nullptr)
{
}

DistanceTransform::~DistanceTransform()
{
    delete m_sedt;
}

float * DistanceTransform::data()
{
    if (!m_sedt)
    {
        int size = m_source.height() * m_source.width();
        m_sedt = new float[size];

        // start distance transform

        sedt(255);

        // convert back to image 

        QImage dst(m_source);
        const int step = dst.bytesPerLine() / dst.width();

        for (int i = 0; i < size; ++i)
            dst.bits()[i * step] = clamp(0, 255, static_cast<unsigned char>(m_sedt[i] * 255.0));
            // ignore other 3 channels

        // rescale image

        dst = dst.scaled(m_dstSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        // blit data back to m_sedt

        size = dst.height() * dst.width();

        delete m_sedt;       
        m_sedt = new float[size];

        for (int i = 0; i < size; ++i)
            m_sedt[i] = static_cast<float>(dst.bits()[i * step]) / 255.f;
    }
    return m_sedt;
}


void DistanceTransform::sedt(const unsigned char threshold)
{
    const unsigned char * source = m_source.constBits();

    const int w = m_source.width();
    const int h = m_source.height();

    const int offset = 0;
    const int step = m_source.bytesPerLine() / m_source.width();

	const float DIST_MAX = 10.0 /*sqrt(w*w + h*h)*/;
	const float d1 = 3; //Distanz zweier benachbarter pixel
	const float d2 = 4; //Distanz zweier diagonal benachbarter pixel 

	float FIXME_MIN = 1.0;
	float FIXME_MAX = 0.0;
	float FIXME_ABS_MAX = 0.0;

	m_points = new QPoint[w*h];

	qDebug("gogo Dead Reckoning");

    // Task_3_2 - ToDo Begin
	int FIXME = 0;
	//Dead Reckoning algorithm by George Grevera [http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.102.7988&rep=rep1&type=pdf]

	// initialize stuff
	int x=0; 
	int y=0;
    for (y = 0; y < h; y++){
		for (x = 0; x < w; x++){
			d(x,y,DIST_MAX);
			p(x,y,QPoint(-1,-1));
		}
	}

	// initialize immediate interior & exterior elements
	for (y = 1; y < h-1; y++){
		for (x = 1; x < w-1; x++){
			if ((I(x-1,y) != I(x,y)) || (I(x+1,y) != I(x,y)) || (I(x,y-1) != I(x,y)) || (I(x,y+1) != I(x,y))){
				d(x,y,0.0);
				p(x,y,QPoint(x,y));
				FIXME++;
			}
		}
	}

	qDebug("%d on edge", FIXME);

	// first (forward) pass
	for (y = 1; y < h-1; y++){
		for (x = 1; x < w-1; x++){
			if ((d(x-1,y-1) + d2) < d(x,y)){
				p(x,y,p(x-1,y-1));
				d(x,y,sqrtterm(x,y));
			}
			if ((d(x,y-1) + d1) < d(x,y)){
				p(x,y,p(x,y-1));
				d(x,y,sqrtterm(x,y));
			}
			if ((d(x+1,y-1) +d2) < d(x,y)){
				p(x,y,p(x+1,y-1));
				d(x,y,sqrtterm(x,y));
			}
			if ((d(x-1,y) +d1) < d(x,y)){
				p(x,y,p(x-1,y));
				d(x,y,sqrtterm(x,y));
			}
		}
	}

	// final (backward) pass
	for (y = h-2; y > 1; y--) {
		for (x = w-2; x > 1; x--) {
			if ((d(x+1,y) + d1) < d(x,y)){
				p(x,y,p(x+1,y));
				d(x,y,sqrtterm(x,y));
			}
			if ((d(x-1,y+1) + d2) < d(x,y)){
				p(x,y,p(x-1,y+1));
				d(x,y,sqrtterm(x,y));
			}
			if ((d(x,y+1) +d1) < d(x,y)){
				p(x,y,p(x,y+1));
				d(x,y,sqrtterm(x,y));
			}
			if ((d(x+1,y+1) +d2) < d(x,y)){
				p(x,y,p(x+1,y+1));
				d(x,y,sqrtterm(x,y));
			}
		}
	}

	//Arthurs versuch an das dynamische scaling - die buchstaben sehen irgendwie nicht so zerfressen aus. gefällt mir persönlich besser
	for (y = h-1; y > 0; y--){
		for (x = w-1; x > 0; x--){
			if (I(x,y) == false){
				d(x,y,-1.0*d(x,y));
			}
			if (d(x,y) < FIXME_MIN){
				FIXME_MIN = d(x,y);
			}
			if (d(x,y) > FIXME_MAX){
				FIXME_MAX = d(x,y);
			}
		}
	}
	qDebug("Daten sind im Bereich von [%f,%f]",FIXME_MIN, FIXME_MAX);
	for (y = h-1; y > 0; y--){
		for (x = w-1; x > 0; x--){
			float val = d(x,y);
			if (d(x,y) > 0){
				val = val / FIXME_MAX; //Scale to [0;1]
				val = val * 0.5; //scale to [0;0.5]
				val = val + 0.5; //move to [0.5;1]
			} else {
				val = val / FIXME_MIN; //scale to [0;1]
				val = 0.5 - (val * 0.5); //scale to [0;0.5] and invert so that max distance is at 0 and min distance at 0.5
			}
			d(x,y,val);
		}
	}

	delete m_points;

	qDebug("...end.");
    // Task_3_2 - ToDo End
}

inline float DistanceTransform::sqrtterm(int x, int y){
	return sqrt((x-p(x,y).x())*(x-p(x,y).x()) + (y-p(x,y).y())*(y-p(x,y).y()));
}
 
//Sets the distance array at the position (x,y) with the given value
inline void DistanceTransform::d(int x, int y, float value){
	if (m_sedt == NULL){
		int size = m_source.height() * m_source.width();
        m_sedt = new float[size];
	}

	m_sedt[y*m_source.width() + x] = value;
}

inline float DistanceTransform::d(int x, int y){
	return m_sedt[y*m_source.width() + x];
}

//Returns true (pixel is set / white) or false (pixel is not set / black) by testing the red channel of the m_source image
inline bool DistanceTransform::I(int x, int y){
	QRgb color = m_source.pixel(x,y);
	if (qRed(color) == 0){
		return false;
	} else {
		return true;
	}
}

inline QPoint DistanceTransform::p(int x, int y){
	return m_points[y*m_source.width() + x];
}

inline void DistanceTransform::p(int x, int y, QPoint p){
	m_points[y*m_source.width() + x] = p;
}