
#pragma once

#include <QMatrix4x4>
#include <QMap>
#include <QVector3D>

#include "AbstractPainter.h"
#include "EnvironmentMapping.h"
#include "WaterRenderer.h"
#include "Labeler.h"
#include "ObjectRenderer.h"
#include "TreeNode.h"

class QOpenGLShader;
class QOpenGLShaderProgram;

class Camera;
class ScreenAlignedQuad;
class PatchedTerrain;


class Painter : public AbstractPainter
{
public:
    Painter();
    virtual ~Painter();

    virtual bool initialize();
    
    virtual void paint(float timef);
    virtual void resize(
        int width
    ,   int height);

    virtual void update();
    virtual void update(const QList<QOpenGLShaderProgram *> & programs);

    void keyPressEvent(QKeyEvent * event);

protected:

    void bindEnvMaps(GLenum target);
    void unbindEnvMaps(GLenum target);

    void paint_4_1(float timef);

protected:
    QOpenGLShaderProgram * createBasicShaderProgram(
        const QString & vertexShaderFileName
    ,   const QString & fragmentShaderFileName);

    QOpenGLShaderProgram * createBasicShaderProgram(
        const QString & vertexShaderFileName
    ,   const QString & geometryShaderFileName
    ,   const QString & fragmentShaderFileName);

    void patchify();
    void patchify(
        float extend
    ,   float x
    ,   float z
    ,   int level);
	void patchify(TreeNode * node, int lvl);
	void renderQuadtree(TreeNode * node);
	void checkTree(TreeNode * node);
   
    float height(
        const float x
    ,   const float z) const;

    bool cull(
        const QVector4D & v0
    ,   const QVector4D & v1
    ,   const QVector4D & v2);

    // ...

protected:
    Camera * m_camera;

    ScreenAlignedQuad * m_quad;

    QList<QMatrix4x4> m_transforms;

    QMap<int, QOpenGLShaderProgram *> m_programs;
    QList<QOpenGLShader *> m_shaders;
	
    PatchedTerrain * m_terrain;

    std::vector<unsigned short> m_heights;

    float m_yScale;
    float m_yOffset;

    GLuint m_height;
    GLuint m_normals;
    GLuint m_diffuse;
    GLuint m_detail;
    GLuint m_detailn;

    bool m_drawLineStrips;
    bool m_debug;

    float m_precission;
    int m_level;
    QVector3D m_cachedEye;

	EnvironmentMapping * m_envMap;
	WaterRenderer * m_waterRenderer;
	Labeler * m_labeler;
	ObjectRenderer * m_objectRenderer;
	TreeNode * m_quadtreeRoot;
};
