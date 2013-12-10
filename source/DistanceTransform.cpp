
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

	uchar c;
	float closest=255.0, dist;
    // Task_3_2 - ToDo Begin
	//distance = anzahl der steps bis zum auftreffen der kontour
	//for(int y=0; y < m_dstsize.height();y++)
	//	for(int x =0; x < m_dstsize.width();x++)
	//	{

	//		//find closest pixel
	//		for(int y2=0; y2 < m_dstsize.height();y++)
	//			for(int x2 =0; x2 < m_dstsize.width();x++)
	//			{
	//				c = static_cast<unsigned char>(m_source.bits()[(y2 * w + x2) * step]);
	//				if( c >= threshold ){
	//					//calc distance
	//					dist = sqrt((y-y2)*(y-y2) + (x-x2)*(x-x2));
	//					if(dist < closest)closest = dist;
	//				}
	//			}
	//		m_sedt[y * m_dstsize.width() + x] = closest;
	//		closest = 255.0;
	//	}

    // m_sedt << should contain all scaled distances ;)


    // Task_3_2 - ToDo End
}
 
