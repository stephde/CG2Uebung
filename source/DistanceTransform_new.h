
#pragma once

#include <QSize>

// Signed Euclidean Distance Transform

class DistanceTransform
{
public:
    DistanceTransform(
        QImage source
    ,   unsigned short dstWidth
    ,   unsigned short dstHeight
    ,   float distScale = 0.0625f);

    virtual ~DistanceTransform();
    float * data();

protected:

    void sedt(unsigned char threshold);
	inline void d(int x, int y, float value);
	inline float d(int x, int y);
	inline void p(int x, int y, QPoint p);
	inline QPoint p(int x, int y);
	inline bool I(int x, int y);
	inline float sqrtterm(int x, int y);

protected:
    const QImage m_source;
	
    const QSize m_dstSize;
    const float m_distScale;

	QPoint* m_points;
    float * m_sedt;
};
