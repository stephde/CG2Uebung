
#include <cassert>

#include <QKeyEvent>

#include "Terrain.h"
#include "Icosahedron.h"
#include "FileAssociatedShader.h"
#include "FileAssociatedTexture.h"
#include "Camera.h"
#include "Canvas.h"

#include "ScreenAlignedQuad.h"

#include "Painter.h"

namespace
{
    const int CubeMapSize        = 256;

    const int EnvMapProgram      = AbstractPainter::PaintMode9 + 2;
    const int EnvMapCubeProgram  = AbstractPainter::PaintMode9 + 3;
                                 
    const int SphereProgram      = AbstractPainter::PaintMode9 + 4;
    const int SphereCubeProgram  = AbstractPainter::PaintMode9 + 5;

    const int TerrainProgram     = AbstractPainter::PaintMode9 + 6;
    const int TerrainCubeProgram = AbstractPainter::PaintMode9 + 7;

    // const int OtherProgram = AbstractPainter::PaintMode9 + 2;
    // ...
}


Painter::Painter()
: m_camera(nullptr)
, m_quad(nullptr)
, m_mapping(CubeMapping)
, m_icosa(nullptr)

, m_cubeFBO(-1)
, m_cubeTex(-1)
, m_cubeDepthRB(-1)
{
}

Painter::~Painter()
{
    qDeleteAll(m_terrains);
    qDeleteAll(m_programs);
    qDeleteAll(m_shaders);

    if (m_cubeFBO != -1)
        glDeleteFramebuffers(1, &m_cubeFBO);

    if (m_cubeTex != -1)
        glDeleteTextures(1, &m_cubeTex);
    if (m_cubeDepthRB != -1)
        glDeleteTextures(1, &m_cubeDepthRB);
	if(m_water != -1)
		glDeleteTextures(1, &m_water);
	if(m_caustics != -1)
		glDeleteTextures(1, &m_caustics);

    delete m_quad;
    delete m_icosa;
}

bool Painter::initialize()
{
    initializeOpenGLFunctions();


    // Note: As always, feel free to change/modify parameters .. as long as its possible to 
    // see the terrain and navigation basically works, everything is ok. 

    glClearColor(1.f, 1.f, 1.f, 0.f);

    m_transforms << QMatrix4x4();
    m_transforms[0].scale(16.f, 2.f, 16.f);
    m_transforms[0].translate(-.5f, 0.f, -.5f);

    m_terrains << new Terrain(256, *this);


    // Note: You can absolutely modify/paint/change these textures if you like.
    m_height    = FileAssociatedTexture::getOrCreate2D("data/height.png", *this); // e.g., there is a height2 or use L3DT (see moodle)
    m_ground    = FileAssociatedTexture::getOrCreate2D("data/ground.png", *this);
    m_caustics  = FileAssociatedTexture::getOrCreate2D("data/caustics.png", *this);
	m_water		= FileAssociatedTexture::getOrCreate2D("data/water.png", *this);


    // uebung 1_1
    //m_programs[PaintMode1] = createBasicShaderProgram("data/terrain_1_1.vert", "data/terrain_1_1.frag");

    // uebung 1_2
    //m_programs[PaintMode2] = createBasicShaderProgram("data/terrain_1_2.vert", "data/terrain_1_2.frag");

    // uebung 1_3
    //m_programs[PaintMode3] = createBasicShaderProgram("data/terrain_1_3.vert", "data/terrain_1_3.frag");

    // uebung 1_4 +
    m_programs[PaintMode4] = createBasicShaderProgram("data/terrain_1_4.vert", "data/terrain_1_4.frag"); 
    m_programs[PaintMode5] = createBasicShaderProgram("data/water.vert", "data/water.frag");

    // ...


    // uebung 2_1

    m_quad = new ScreenAlignedQuad(*this);


    // Task_2_1 - ToDo Begin

    // Adjust, if required, the clamp configuration for the following textures

    m_envmaps[CubeMapping]       = FileAssociatedTexture::getOrCreateCube("data/env_cube_?.png", *this);
    m_envmaps[PolarMapping]      = FileAssociatedTexture::getOrCreate2D("data/env_polar_half.png", *this);
    m_envmaps[ParaboloidMapping] = FileAssociatedTexture::getOrCreate2D("data/env_paraboloid.png", *this);
    m_envmaps[SphereMapping]     = FileAssociatedTexture::getOrCreate2D("data/env_sphere.png", *this);

    // Task_2_1 - ToDo End


    m_programs[EnvMapProgram] = createBasicShaderProgram("data/envmap.vert", "data/envmap.frag");
	
    // uebung 2_2

    m_icosa = new Icosahedron(*this, 4);

    m_programs[SphereProgram] = createBasicShaderProgram("data/sphere.vert", "data/sphere.frag");

    m_transforms << QMatrix4x4(); // 1

    // uebung 2_3

   m_programs[TerrainProgram] = createBasicShaderProgram("data/terrain_1_4.vert", "data/terrain_1_4.frag");
    
    m_waterheights = FileAssociatedTexture::getOrCreate2D("data/waterheights.png", *this);
    m_waternormals = FileAssociatedTexture::getOrCreate2D("data/waternormals.png", *this);

    m_programs[SphereCubeProgram] = createBasicShaderProgram("data/sphere_cube.vert", "data/sphere_cube.frag");
    m_programs[EnvMapCubeProgram] = createBasicShaderProgram("data/envmap_cube.vert", "data/envmap_cube.geom", "data/envmap_cube.frag");
    
    // Task_2_3 - ToDo Begin

    // Feel free to add shaders for your arbitrary geometry (e.g., terrain from the first assignment)

    //m_programs[TerrainCubeProgram] = createBasicShaderProgram("data/terrain_cube.vert", "data/terrain_cube.geom", "data/terrain_cube.frag");

    // Task_2_3 - ToDo End


    // Task_2_3 - ToDo Begin

    // ToDo: Add missing information for fbo initialization.
    // 

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
		
    return true;
}


void Painter::keyPressEvent(QKeyEvent * event)
{
    switch (event->key())
    {
    case Qt::Key_M:
        {
        // Note: Register all of your programs that use m_mapping and evn(), to
        // get their m_mapping value updated on changes appropriatelly...

        m_mapping = static_cast<EnvironmentMapping>((m_mapping + 1) % 4);
        QList<QOpenGLShaderProgram *> programs;
        programs << m_programs[EnvMapProgram] 
                 << m_programs[EnvMapCubeProgram]
                 << m_programs[SphereProgram];
               //<< m_programs[...];
        update(programs);
        }
        break;

    case Qt::Key_X:
        m_icosa_center += QVector3D(event->modifiers() && Qt::Key_Shift ? 0.01f : -0.01f, 0.f, 0.f);
        update();
        break;
    case Qt::Key_Y:
        m_icosa_center += QVector3D(0.f, event->modifiers() && Qt::Key_Shift ? 0.01f : -0.01f, 0.f);
        update();
        break;
    case Qt::Key_Z:
        m_icosa_center += QVector3D(0.f, 0.f, event->modifiers() && Qt::Key_Shift ? 0.01f : -0.01f);
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

    foreach(const int i, m_programs.keys())
    {
        QOpenGLShaderProgram * program(m_programs[i]);
        if (program->isLinked())
        {
            program->bind();
            program->setUniformValue("transform"
                , camera()->viewProjection() * m_transforms[i == SphereProgram || i == SphereCubeProgram ? 1 : 0]);
            program->release();
        }
    }
}

void Painter::update()
{
    update(m_programs.values());
}

void Painter::update(const QList<QOpenGLShaderProgram *> & programs)
{
    // This transform is used for the dynamic reflecting sphere in task 2_3

    m_transforms[1] = QMatrix4x4();
    m_transforms[1].translate(m_icosa_center);
    m_transforms[1].scale(0.33f, 0.33f, .33f);


    foreach(const int i, m_programs.keys())
    {
        QOpenGLShaderProgram * program(m_programs[i]);

        if (programs.contains(program) && program->isLinked())
        {
            program->bind();

            // Note: transform might be the modelview projection matrix, or similar.
            // the identifier was choosen to show that all transform is done before
            // GPU, to reduce vertex shader workload. So feel free to modify...
            program->setUniformValue("transform"
                , camera()->viewProjection() * m_transforms[i == SphereProgram || i == SphereCubeProgram ? 1 : 0]);

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
                program->setUniformValue("ground", 1);
            case PaintMode2:
                program->setUniformValue("height", 0);
            case PaintMode1:
                //  program->setUniformValue("offset", QVector3D(-0.5, 0.0, -0.5));
                break;
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

            //case TerrainCubeProgram:
            //    {
            //    // Task_2_3 - ToDo Begin

            //    // Provide the appropriate matrices to the geometry shader.

            //    /*QMatrix4x4 transforms[6] = 
            //    {
            //        ...
            //    };*/
            //    //program->setUniformValueArray("", transforms, 6);
            //    //...
            //    
            //    // Task_2_3 - ToDo End
            //    }
            //case TerrainProgram:
            //    program->setUniformValue("caustics", 2);
            //    program->setUniformValue("ground", 1);
            //    program->setUniformValue("height", 0);

            //    break;
            }

            program->release();
        }
    }
}

void Painter::paint(float timef)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (mode())
    {
    case PaintMode1:
        paint_2_1(timef); break;
    case PaintMode2:
        paint_2_2(timef); break;
    case PaintMode3:
        paint_2_3(timef); break;
    case PaintMode4:
        //paint_1_4(timef); break;
        //case PaintMode5:
        //    paint_1_5(timef); break;
        // ...
    default:
        break;
    }
}

// Note: Feel free to remove your old code and start on minor cleanups and refactorings....

void Painter::paint_1_1(float timef)
{
    QOpenGLShaderProgram * program(m_programs[PaintMode1]);
    Terrain * terrain(m_terrains[0]);

    if (program->isLinked())
    {
        // Note: you can use glLineWidth(...) to adjust ... :D - remember to setup default when done painting.
        program->bind();
        //      program->setUniformValue("color", QVector3D(0.4f, 0.4f, 0.4f)); // yes, you can change that
        //terrain->draw(*this, GL_LINE_STRIP);
        program->setUniformValue("color", QVector3D(0.8f, 0.8f, 0.8f)); // ... and that one too. if you like.
        terrain->draw(*this);
        program->release();
    }
}

void Painter::paint_1_2(float timef)
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

void Painter::paint_1_3(float timef)
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

void Painter::bindEnvMaps(GLenum target)
{
    // In the case of cube mapping another texture 
    // type is required to be bound (samplerCube instead of sampler2D).

    if (CubeMapping != m_mapping)
    {
        glActiveTexture(target);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_envmaps[m_mapping]);
    }
    else
    {
        glActiveTexture(target + 1);
        glEnable(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_envmaps[m_mapping]);
    }
}

void Painter::unbindEnvMaps(GLenum target)
{
    if (CubeMapping == m_mapping)
    {
        glActiveTexture(target + 1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDisable(GL_TEXTURE_CUBE_MAP);
    }
    else
    {
        glActiveTexture(target);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

void Painter::paint_2_1_envmap(
    const int programIndex
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

    unbindEnvMaps(GL_TEXTURE0);
}


void Painter::paint_2_2(float timef)
{
    paint_2_1_envmap(EnvMapProgram, timef);
    paint_2_2_sphere(SphereProgram, timef);
}

void Painter::paint_2_3(float timef)
{
    // Task_2_3 - ToDo Begin

    glBindFramebuffer(GL_FRAMEBUFFER, m_cubeFBO);
    
    // ToDO: set viewport, clear buffer
	glClear(GL_FRAMEBUFFER);
    glClearColor(0.0, 1.0, 0.0, 1.0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glViewport(0, 0, CubeMapSize, CubeMapSize);	


    paint_2_1_envmap(EnvMapCubeProgram, timef);
    paint_2_3_terrain(TerrainCubeProgram, timef);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ... set viewport, setup depth test, 
	glViewport(0, 0, camera()->viewport().width(), camera()->viewport().height()); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    // ...

    glActiveTexture(GL_TEXTURE2);
    glDisable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTex);

    paint_2_2_sphere(SphereCubeProgram, timef);

    glActiveTexture(GL_TEXTURE2);
    glDisable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


    // .. draw scene geometry 


    paint_2_3_terrain(TerrainProgram, timef);
    paint_2_1_envmap(EnvMapProgram, timef);

    // Task_2_3 - ToDo End
}


void Painter::paint_2_3_terrain(
    const int programIndex
,   float timef)
{
	paint_1_4(timef);

	/*QOpenGLShaderProgram * program(m_programs[PaintMode5]);
    Terrain * terrain(m_terrains[0]);

    if (!program->isLinked())
        return;

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_height);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_ground);

    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_caustics);

    program->bind();
    program->setUniformValue("timef", timef);
    terrain->draw(*this);
    program->release();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);*/
}
