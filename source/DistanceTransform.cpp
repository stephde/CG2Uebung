
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

    // Task_3_2 - ToDo Begin
    // m_sedt << should contain all scaled distances ;)

	/*
	uchar c;
	float dist;
	float closest = 255.0;

	//dist = anzahl der steps bis zum auftreffen der kontour
	
	for(int y=0; y < m_dstSize.height(); y++)
	{
		for(int x=0; x < m_dstSize.width(); x++)
		{
			//find closest pixel
			for(int y2=0; y2 < m_dstSize.height(); y++)
			{
				for(int x2=0; x2 < m_dstSize.width(); x++)
				{
					c = static_cast<unsigned char>(m_source.bits()[(y2 * w + x2) * step]);

					if(c >= threshold){
						//calc distance
						dist = sqrt((y-y2)*(y-y2) + (x-x2)*(x-x2));
						if(dist < closest)closest = dist;
					}
				}

				m_sedt[y * m_dstSize.width() + x] = closest;
				closest = 255.0;
			}
		}
	}
	*/
		
	// transform along columns
	for(int x = 0; x < w; x++)
	{	
		int *v = new int[h];
		float *z = new float[h+1];
		int k = 0;
		v[0] = 0;
		z[0] = -threshold;
		z[1] = +threshold;
		
		for(int y = 1; y < h; y++)
		{
			// s = ((pixelwert * position ^ 2) - (Vorgänger Pixel + Vorgänger position ^ 2)) durch (2 * position - 2 * 0);
			float s = ((source[(y * w) + x] + (y*y)) - (source[(v[k] * w) + x] + (v[k]*v[k]))) / (2 * y - 2 * v[k]);
			// solange s kleiner als z bei k (-INF) ...
			while(s <= z[k])
			{
				// k-1 und s neu berechnen mit dem Vorgängerpixel des Vorgängers
				k--;
				s = ((source[(y * w) + x] + (y*y)) - (source[(v[k] * w) + x] + (v[k]*v[k]))) / (2 * y - 2 * v[k]);
			}
			// k+1;
			k++;
			// v bekommt alten q wert bei nächstem wert
			v[k] = y;
			// z bekommt s wert
			z[k] = s;
			// Folgewert wird +INF
			z[k+1] = +threshold;
		}
		
		k = 0;
		
		for(int y = 0; y < h; y++)
		{
			// solange nächstes Z Feld kleiner als y Wert, k++
			while(z[k+1] < y)
				k++;
			// WTF ich versteh nichts mehr
			//printf("%f\n", (y - v[k]) * (y - v[k]) + source[(v[k] * w) + x]);
			m_sedt[(y * w) + x] = (y - v[k]) * (y - v[k]) + source[(v[k] * w) + x];
		}

		//m_sedt[(1024 * w) + x] = (1024 - v[k]) * (1024 - v[k]) + source[(v[k] * w) + x];
		
		delete [] v;
		delete [] z;
		
	}
	
	//repeat along rows
	for(int y = 0; y < h; y++)
	{
		int *v = new int[w];
		float *z = new float[w+1];
		int k = 0;
		v[0] = 0;
		z[0] = -threshold;
		z[1] = +threshold;
		
		for(int x = 1; x < w; x++)
		{
			// s = ((pixelwert * position ^ 2) - (Vorgänger Pixel + Vorgänger position ^ 2)) durch (2 * position - 2 * 0);
			float s = ((m_sedt[(y * w) + x] + (y*y)) - (m_sedt[(y * w) + v[k]] + (v[k]*v[k]))) / (2 * x - 2 * v[k]);
			// solange s kleiner als z bei k (-INF) ...
			while(s <= z[k])
			{
				// k-1 und s neu berechnen mit dem Vorgängerpixel des Vorgängers
				k--;
				s = ((m_sedt[(y * w) + x] + (y*y)) - (m_sedt[(y * w) + v[k]] + (v[k]*v[k]))) / (2 * x - 2 * v[k]);
			}
			// k+1;
			k++;
			// v bekommt alten q wert bei nächstem wert
			v[k] = y;
			// z bekommt s wert
			z[k] = s;
			// Folgewert wird +INF
			z[k+1] = +threshold;
		}
		
		k = 0;
		
		for(int x = 0; x < w; x++)
		{
			// solange nächstes Z Feld kleiner als y Wert, k++
			while(z[k+1] < x)
				k++;
			// WTF ich versteh nichts mehr	
			m_sedt[(y * w) + x] = (x-v[k])*(x-v[k]) + m_sedt[(y * w) + v[k]];
		}
		
		delete [] v;
		delete [] z;
	
	}

    // Task_3_2 - ToDo End
}
 
