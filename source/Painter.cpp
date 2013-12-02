
#include <cassert>

#include <QKeyEvent>

#include "Terrain.h"
#include "FileAssociatedShader.h"
#include "FileAssociatedTexture.h"
#include "Camera.h"
#include "Canvas.h"
#include "AssimpScene.h"
#include "DistanceTransform.h"

#include "ScreenAlignedQuad.h"

#include "Painter.h"

namespace
{
    const int ShadowMapSize = 2048;

    const int LabelAlphaMappingProgram  = AbstractPainter::PaintMode9 + 1;
    const int LabelDistanceMapProgram   = AbstractPainter::PaintMode9 + 2;
    const int ShadowMapProgram          = AbstractPainter::PaintMode9 + 3;
    const int ShadowMapDumpProgram      = AbstractPainter::PaintMode9 + 4;

    // const int OtherProgram = AbstractPainter::PaintMode9 + 2;
    // ...
}


Painter::Painter()
: m_camera(nullptr)
, m_quad(nullptr)

, m_hpicgs(nullptr)
, m_plane(nullptr)
, m_portcc(nullptr)

, m_hpicgsLabel(nullptr)
, m_portccLabel(nullptr)

, m_hpicgsLabelAM(-1)
, m_portccLabelAM(-1)
, m_hpicgsLabelDM(-1)
, m_portccLabelDM(-1)

, m_depthFbo(-1)
, m_depthTex(-1)
{
    setMode(PaintMode0);
}

Painter::~Painter()
{
    qDeleteAll(m_programs);
    qDeleteAll(m_shaders);

    delete m_quad;
    delete m_hpicgs;
    delete m_plane;
    delete m_portcc;

    delete m_hpicgsLabel;
    delete m_portccLabel;
	if(m_water != -1)
		glDeleteTextures(1, &m_water);
	if(m_caustics != -1)
		glDeleteTextures(1, &m_caustics);

    if (-1 != m_depthTex)
        glDeleteTextures(1, &m_depthTex);
    if (-1 != m_depthFbo)
        glDeleteFramebuffers(1, &m_depthFbo);

    FileAssociatedTexture::clean(*this);
}

bool Painter::initialize()
{
    initializeOpenGLFunctions();

    // Note: As always, feel free to change/modify parameters .. as long as its possible to 
    // see the terrain and navigation basically works, everything is ok. 

    glClearColor(1.f, 1.f, 1.f, 0.f);

    m_quad = new ScreenAlignedQuad(*this);

    m_terrains << new Terrain(256, *this);
    m_light = QVector3D(2.f, 3.f, 2.f);

    QOpenGLShaderProgram * program;

    // setup default assimp scene shader (rendering the meshes...)
	m_water		= FileAssociatedTexture::getOrCreate2D("data/water.png", *this);

    // ToDo: If you render your own scene (e.g., a terrain, keep in mind
    // to extend your shaders for supporting shadowmaps as well as in assimp shaders...

    program = createBasicShaderProgram("data/assimp.vert", "data/assimp.frag");
    m_programs[PaintMode1] = program;
    program->bindAttributeLocation("a_vertex", 0);
    program->bindAttributeLocation("a_normal", 1);
    program->bindAttributeLocation("a_texc", 2);    
    program->link();

    // load labeling shaders and resources

    m_programs[LabelAlphaMappingProgram]  = createBasicShaderProgram("data/label.vert", "data/labelAM.frag");
    m_programs[LabelDistanceMapProgram] = createBasicShaderProgram("data/label.vert", "data/labelDM.frag");
    
    m_hpicgsLabelAM = FileAssociatedTexture::getOrCreate2D("data/hpicgs_label.png", *this);
    m_portccLabelAM = FileAssociatedTexture::getOrCreate2D("data/companion_cube_label.png", *this);

    // uebung 1_4 +
    m_programs[PaintMode4] = createBasicShaderProgram("data/terrain_1_4.vert", "data/terrain_1_4.frag"); 
    m_programs[PaintMode5] = createBasicShaderProgram("data/water.vert", "data/water.frag");
    // load and position 3d models in the scene 
    // Note: feel free to modify / add / remove here... there should be at least 
    // a few scene objects visible ;)

    m_hpicgs = new AssimpScene(*this, "data/hpicgs.obj", true);
    m_hpicgs->transform().scale(0.7f, 0.7f, 0.7f);
    m_hpicgs->transform().translate(-0.9f, 0.8f, -0.3f);
    m_hpicgs->transform().rotate( 56.0f, 0.f, 1.0f, 0.f);
    m_hpicgs->transform().rotate(0.0f, 1.f, 0.0f, 0.f);
    m_plane = new AssimpScene(*this, "data/plane.obj", false);
    m_plane->transform().translate(0.f, -0.1f, 0.f);
    m_portcc = new AssimpScene(*this, "data/companion_cube.obj", true);
    m_portcc->transform().scale(0.5f, 0.5f, 0.5f);
    m_portcc->transform().translate(1.1f, 0.73f, 1.2f);
    m_portcc->transform().rotate(8.0f, 0.f, 1.0f, 0.f);

    // initialize label positions


    // Task_3_1 - ToDo Begin

    // Modify the transformation matrix T to position, scale, and orient 
    // the labels to appropriate places, based on the initial view! No
    // interaction should be required to be able to see both labels.

    m_hpicgsLabel = new ScreenAlignedQuad(*this, 0);
    QMatrix4x4 T;

    // ToDo: use T.translate/scale/rotate ...

    m_transforms << T;

    m_portccLabel = new ScreenAlignedQuad(*this, 0);

    // ToDo: use T.translate/scale/rotate ...

    m_transforms << T;
	
    // Task_3_1 - ToDo End


    // initialize shader program attribute input

    program = m_programs[LabelAlphaMappingProgram];
    program->bind();
    program->bindAttributeLocation("a_vertex", 0);
    program->release();

    program = m_programs[LabelDistanceMapProgram];
    program->bind();
    program->bindAttributeLocation("a_vertex", 0);
    program->release();

   m_programs[TerrainProgram] = createBasicShaderProgram("data/terrain_1_4.vert", "data/terrain_1_4.frag");
    
    m_waterheights = FileAssociatedTexture::getOrCreate2D("data/waterheights.png", *this);
    m_waternormals = FileAssociatedTexture::getOrCreate2D("data/waternormals.png", *this);
    // create distance transforms of label images

    glGenTextures(1, &m_hpicgsLabelDM);
    glBindTexture(GL_TEXTURE_2D, m_hpicgsLabelDM);
        
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    {
        QImage image("data/hpicgs_label_bitmask.png");

        // ToDo: pre resize?

        DistanceTransform DT(image, 512, 128, 0.0625f);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 512, 128, 0, GL_RED, GL_FLOAT, DT.data());
    }
    glGenerateMipmap(GL_TEXTURE_2D);


    glGenTextures(1, &m_portccLabelDM);
    glBindTexture(GL_TEXTURE_2D, m_portccLabelDM);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    {
        QImage image("data/companion_cube_label_bitmask.png");

        // ToDo: pre resize?
        // image = image.scaled(? , ? , Qt::FastTransformation);

    glGenTextures(1, &m_cubeTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTex);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for(int i=0; i<6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 
					CubeMapSize, CubeMapSize,         
					0, GL_RGBA, 
					GL_UNSIGNED_BYTE, nullptr);// for each face ;)*/
	}

    //same procedure again...

    glGenTextures(1, &m_cubeDepthRB); //glGenRenderbuffers
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeDepthRB);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Note: Be aware of multiple available DepthBufferComponent formats...

    for(int i=0; i<6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24, 
					CubeMapSize, CubeMapSize,        
					0, GL_DEPTH_COMPONENT, 
					GL_UNSIGNED_BYTE, nullptr);// for each face ;) 
	}

    // Task_2_3 - ToDo End


    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


    glGenFramebuffers(1, &m_cubeFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_cubeFBO);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_cubeTex, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_cubeDepthRB, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "cube fbo invalid";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
        DistanceTransform DT(image, 512, 128, 0.0625f);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 512, 128, 0, GL_RED, GL_FLOAT, DT.data());
    }        
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    // create fbo

    m_programs[ShadowMapProgram] = createBasicShaderProgram("data/shadowmap.vert", "data/shadowmap.frag");
    m_programs[ShadowMapDumpProgram] = createBasicShaderProgram("data/dump.vert", "data/dump.frag");

    glGenTextures(1, &m_depthTex);
    glBindTexture(GL_TEXTURE_2D, m_depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, ShadowMapSize, ShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &m_depthFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthFbo);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "depth fbo invalid";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
    return true;
}


void Painter::keyPressEvent(QKeyEvent * event)
{
    switch (event->key())
    {
    case Qt::Key_X:
        m_light += QVector3D(event->modifiers() && Qt::Key_Shift ? 0.05f : -0.05f, 0.f, 0.f);
        update();
        break;
    case Qt::Key_Y:
        m_light += QVector3D(0.f, event->modifiers() && Qt::Key_Shift ? 0.05f : -0.05f, 0.f);
        update();
        break;
    case Qt::Key_Z:
        m_light += QVector3D(0.f, 0.f, event->modifiers() && Qt::Key_Shift ? 0.05f : -0.05f);
        update();
        break;
    default:
        break;
    }
}


QOpenGLShaderProgram * Painter::createBasicShaderProgram(
    const QString & vertexShaderFileName
    , const QString & fragmentShaderFileName)
{
    QOpenGLShaderProgram * program = new QOpenGLShaderProgram();

    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Vertex, vertexShaderFileName, *program);
    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Fragment, fragmentShaderFileName, *program);
    program->bindAttributeLocation("a_vertex", 0);
    program->link();

    return program;
}

QOpenGLShaderProgram * Painter::createBasicShaderProgram(
    const QString & vertexShaderFileName
    , const QString & geometryShaderFileName
    , const QString & fragmentShaderFileName)
{
    QOpenGLShaderProgram * program = new QOpenGLShaderProgram();

    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Vertex, vertexShaderFileName, *program);
    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Geometry, geometryShaderFileName, *program);
    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Fragment, fragmentShaderFileName, *program);
    program->bindAttributeLocation("a_vertex", 0);
    program->link();

    return program;
}

void Painter::resize(
    int width
    , int height)
{
    glViewport(0, 0, width, height);
    update();
}

void Painter::update()
{
    update(m_programs.values());
}

void Painter::update(const QList<QOpenGLShaderProgram *> & programs)
{
    foreach(const int i, m_programs.keys())
    {
        QOpenGLShaderProgram * program(m_programs[i]);

        if (programs.contains(program) && program->isLinked())
        {
            program->bind();

            switch (i)
            {
            case PaintMode0:
            case PaintMode9:
            case PaintMode8:
            case PaintMode7:
            case PaintMode6:
            case PaintMode5:
            case PaintMode4:
				program->setUniformValue("waterHeights", 4);
				program->setUniformValue("water",    2);
				program->setUniformValue("caustics", 3);
            case PaintMode3:
            case PaintMode2:
            case PaintMode1:
                program->setUniformValue("projection", camera()->projection());
                program->setUniformValue("view", camera()->view());
                program->setUniformValue("shadowmap", 1);
                program->setUniformValue("shadow", false);
                break;
            case LabelAlphaMappingProgram:
            case LabelDistanceMapProgram:
                program->setUniformValue("label", 0);
                //case OtherProgram: // feel free to use more than one program per mode...
                //    break;

            case EnvMapCubeProgram:
                {
					QVector3D up = QVector3D(0.0, -1.0, 0.0);
                	Camera * cnx = new Camera(m_icosa_center, m_icosa_center + QVector3D(1.0, 0.0, 0.0), up); 
                    Camera * cnmx = new Camera(m_icosa_center, m_icosa_center - QVector3D(1.0, 0.0, 0.0), up); 
                    Camera * cny = new Camera(m_icosa_center, m_icosa_center + QVector3D(0.0, 1.0, 0.0), up); 
                    Camera * cnmy = new Camera(m_icosa_center, m_icosa_center - QVector3D(0.0, 1.0, 0.0), up); 
                    Camera * cnz = new Camera(m_icosa_center, m_icosa_center + QVector3D(0.0, 0.0, 1.0), up); 
                    Camera * cnmz = new Camera(m_icosa_center, m_icosa_center - QVector3D(0.0, 0.0, 1.0), up); 
  
                    cnx->setViewport(QSize(CubeMapSize, CubeMapSize)); 
                    cnmx->setViewport(QSize(CubeMapSize, CubeMapSize)); 
                    cny->setViewport(QSize(CubeMapSize, CubeMapSize)); 
                    cnmy->setViewport(QSize(CubeMapSize, CubeMapSize)); 
                    cnz->setViewport(QSize(CubeMapSize, CubeMapSize)); 
                    cnmz->setViewport(QSize(CubeMapSize, CubeMapSize)); 
  
                    cnx->setFovy(90.0); 
                    cnmx->setFovy(90.0); 
                    cny->setFovy(90.0); 
                    cnmy->setFovy(90.0); 
                    cnz->setFovy(90.0); 
                    cnmz->setFovy(90.0); 
  
                    QMatrix4x4 viewProjections[6] = 
                    { 
                        cnx->viewProjection(), cnmx->viewProjection(), 
                        cny->viewProjection(), cnmy->viewProjection(), 
                        cnz->viewProjection(), cnmz->viewProjection() 
                    };
               
                    QMatrix4x4 views[6] = 
                    { 
                        cnx->view(), cnmx->view(), 
                        cny->view(), cnmy->view(), 
                        cnz->view(), cnmz->view() 
                    }; 

                    program->setUniformValueArray("projectionsInverted", viewProjections, 6); 
                    program->setUniformValueArray("views", views, 6); 
              }

            case EnvMapProgram:
                program->setUniformValue("mapping", m_mapping);

                // Task_2_1 - ToDo Begin

                // Set required matrix/matrices of the vertex shader...
                // Note: use the camera()-> ... matrices here (e.g., view, projection, or inverted, etc..)

				program->setUniformValue("projectionTransformInv", camera()->projection().inverted());
				program->setUniformValue("viewTransformInv", camera()->view().inverted());
				program->setUniformValue("projectionTransform", camera()->projection());
				program->setUniformValue("viewTransform", camera()->view());
				program->setUniformValue("viewProjectionTransform", camera()->viewProjection());
				program->setUniformValue("viewProjectionTransformInv", camera()->viewProjection().inverted());
                //...

                // Task_2_1 - ToDo End

                program->setUniformValue("envmap", 0);
                program->setUniformValue("cubemap", 1);

                break;
            case ShadowMapProgram:
                program->setUniformValue("viewProjection", camera()->viewProjection());

            case SphereCubeProgram:
                program->setUniformValue("source", 2);

            case SphereProgram:
                program->setUniformValue("mapping", m_mapping);
				
				program->setUniformValue("projectionTransformInv", camera()->projection().inverted());
				program->setUniformValue("viewTransformInv", camera()->view().inverted());
				program->setUniformValue("projectionTransform", camera()->projection());
				program->setUniformValue("viewTransform", camera()->view());
				program->setUniformValue("viewProjectionTransform", camera()->viewProjection());
				program->setUniformValue("viewProjectionTransformInv", camera()->viewProjection().inverted());

                program->setUniformValue("envmap", 0);
                program->setUniformValue("cubemap", 1);

                // Task_2_2 - ToDo Begin

                // Set required matrices for eye vector retrieval.
                // Note: for the sphere you might need other matrices than
                // for the screen aligned quad...

                program->setUniformValue("transform", m_transforms[1]);
                //program->setUniformValue("...", camera()->...);

                // Task_2_2 - ToDo End

                break;
            case ShadowMapDumpProgram:
                program->setUniformValue("source", 0);
                break;
                //case OtherProgram: // feel free to use more than one program per mode...
                //    break;
            }

            program->release();
        }
    }
}

void Painter::paint(float timef)
{
    switch (mode())
    {
    case PaintMode0:
    case PaintMode1:
        paint_3_1(timef); break;
    case PaintMode2:
        paint_3_2(timef); break;
    case PaintMode3:
        paint_3_3(timef); break;
    case PaintMode4:
        paint_3_4(timef); break;
        //paint_1_4(timef); break;
        //case PaintMode5:
        //    paint_1_5(timef); break;
        // ...
    default:
        break;
    }
}

// Note: Feel free to remove your old code and start on minor cleanups and refactorings....

void Painter::paint_3_1_scene(const bool shadow, float timef)
{
    QOpenGLShaderProgram * program(m_programs[PaintMode1]);

    program->bind();
    program->setUniformValue("light", m_light);
    program->setUniformValue("useshadow", shadow);
    program->release();

    m_hpicgs->draw(*this, *program);
    m_portcc->draw(*this, *program);
    m_plane->draw(*this, *program);
}

{
    QOpenGLShaderProgram * program(m_programs[PaintMode2]);
    Terrain * terrain(m_terrains[0]);

    if (program->isLinked())
    {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_height);

        program->bind();
        terrain->draw(*this);
        program->release();

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}
void Painter::paint_3_1(float timef)
{
    paint_3_1_scene(false, timef);
    paint_3_1_label(camera()->viewProjection(), timef);
}

{
    QOpenGLShaderProgram * program(m_programs[PaintMode3]);
    Terrain * terrain(m_terrains[0]);

    if (program->isLinked())
    {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_height);

        glActiveTexture(GL_TEXTURE1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_ground);

        program->bind();
        terrain->draw(*this);
        program->release();

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}
void Painter::paint_3_1_label(
    const QMatrix4x4 & viewProjection
,   float timef)
{
    QOpenGLShaderProgram * program;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);

    program = m_programs[LabelAlphaMappingProgram];
    program->bind();

    glBindTexture(GL_TEXTURE_2D, m_hpicgsLabelAM);
    program->setUniformValue("mvp", viewProjection * m_transforms[0]);
    m_hpicgsLabel->draw(*this);

    glBindTexture(GL_TEXTURE_2D, m_portccLabelAM);
    program->setUniformValue("mvp", viewProjection * m_transforms[1]);
    m_portccLabel->draw(*this);

    program->release();
void Painter::paint_1_4(float timef)
{
    QOpenGLShaderProgram * program(m_programs[PaintMode4]);
	QOpenGLShaderProgram * program2(m_programs[PaintMode5]);
    Terrain * terrain(m_terrains[0]);
	Terrain * terWater(m_terrains[0]);



    if (program->isLinked())
    {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_height);

        glActiveTexture(GL_TEXTURE1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_ground);		

		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_caustics);

        program->bind();		
		program->setUniformValue("timef", timef);
		program->setUniformValue("cameraPos", camera()->eye().y());
        terrain->draw(*this);
        program->release();

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE1);		
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }

	if(program2->isLinked())
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_water);

		glActiveTexture(GL_TEXTURE4);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_waterheights);

		program2->bind();
		program2->setUniformValue("timef", timef);
        terWater->draw(*this);
        program2->release();

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE2);		
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

		glDisable(GL_BLEND);
	}
}

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void Painter::paint_3_2(float timef)
{
    paint_3_1_scene(false, timef);
    paint_3_2_label(camera()->viewProjection(), timef);
}

,   float timef)
{
    QOpenGLShaderProgram * program(m_programs[programIndex]);

    if (!program->isLinked())
        return;

    bindEnvMaps(GL_TEXTURE0);

    // Task_2_1 - ToDo Begin
    // Disable writing to the Depth Buffer using glDepthMask and only draw the
    // Quad, when its vertices z-components are equal to the far plane in NDC.

    // ToDo: configure depth state here
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_EQUAL);
   
	program->bind();
    m_quad->draw(*this);
    program->release();
	
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

    // ToDo: cleanup depth state here

    // Task_2_1 - ToDo End

void Painter::paint_3_2_label(
    const QMatrix4x4 & viewProjection
,   float timef)
{
    QOpenGLShaderProgram * program;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);

    program = m_programs[LabelDistanceMapProgram];
    program->bind();

    glBindTexture(GL_TEXTURE_2D, m_hpicgsLabelDM);
    program->setUniformValue("mvp", viewProjection * m_transforms[0]);
    m_hpicgsLabel->draw(*this);

    glBindTexture(GL_TEXTURE_2D, m_portccLabelDM);
    program->setUniformValue("mvp", viewProjection * m_transforms[1]);
    m_portccLabel->draw(*this);

    unbindEnvMaps(GL_TEXTURE0);
}

void Painter::paint_2_1(float timef)
{
    // Note: Feel free to add your own additional paint functions,
    // and if you might want to draw the scene with different programs used,
    // (e.g., when drawing into the dynamic cube map) provide a function
    // signature with program parameter... (e.g., the following can be drawn
    // either with EnvMapProgram or EnvMapCubeProgram).

    paint_2_1_envmap(EnvMapProgram, timef);
}

void Painter::paint_2_2_sphere(
    const int programIndex
,   float timef)
{
    QOpenGLShaderProgram * program = m_programs[programIndex];

    if (!program->isLinked())
        return;
    
    bindEnvMaps(GL_TEXTURE0);

    program->bind();
    program->setUniformValue("timef", timef);
	program->setUniformValue("camPos", camera()->eye());
    m_icosa->draw(*this);
    program->release();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void Painter::paint_3_3_shadowmap(float timef)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthFbo);
    glViewport(0, 0, ShadowMapSize, ShadowMapSize);

    glClearDepth(1.f);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, m_cubeFBO);
    
    // ToDO: set viewport, clear buffer
	glClear(GL_FRAMEBUFFER);
    glClearColor(0.0, 1.0, 0.0, 1.0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glViewport(0, 0, CubeMapSize, CubeMapSize);	
    QOpenGLShaderProgram * program(m_programs[ShadowMapProgram]);


    paint_2_1_envmap(EnvMapCubeProgram, timef);
    paint_2_3_terrain(TerrainCubeProgram, timef);

    // Task_3_3 - ToDo Begin

    QMatrix4x4 L;

    // L ...

    // Task_3_3 - ToDo End
	glViewport(0, 0, camera()->viewport().width(), camera()->viewport().height()); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    program->bind();
    program->setUniformValue("viewProjection", L);
    program->release();

    m_hpicgs->draw(*this, *program);
    m_portcc->draw(*this, *program);

    paint_3_2_label(L, timef);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, camera()->viewport().width(), camera()->viewport().height());
}

void Painter::paint_3_3(float timef)
{
    paint_3_3_shadowmap(timef);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_depthTex);

    paint_2_3_terrain(TerrainProgram, timef);
    paint_2_1_envmap(EnvMapProgram, timef);
    QOpenGLShaderProgram * program(m_programs[ShadowMapDumpProgram]);

    // Task_2_3 - ToDo End

    program->bind();
    m_quad->draw(*this);
    program->release();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Painter::paint_3_4(float timef)
{
	paint_1_4(timef);
    paint_3_3_shadowmap(timef);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_depthTex);
	/*QOpenGLShaderProgram * program(m_programs[PaintMode5]);
    Terrain * terrain(m_terrains[0]);

    glActiveTexture(GL_TEXTURE0);
    if (!program->isLinked())
        return;

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_height);
    QOpenGLShaderProgram * program(m_programs[PaintMode1]);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_ground);

    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_caustics);
    // Task_3_3 - ToDo Begin

    program->bind();
    program->setUniformValue("timef", timef);
    terrain->draw(*this);
    program->release();
    // QMatrix4x4 ...
    
    program->bind();
    program->setUniformValue("light", m_light);
    // program->setUniformValue("todo", ?);
    program->release();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    // Task_3_3 - ToDo End

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);


    paint_3_1_scene(true, timef);
    paint_3_2_label(camera()->viewProjection(), timef);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);*/
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}
