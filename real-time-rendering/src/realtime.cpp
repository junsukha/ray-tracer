#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "shapes/cube.h"
#include "shapes/cone.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"

#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "utils/shaderloader.h"
#include "shapes/sphereSolution.h"
#include "utils/objreader2.h"

//#include "utils/objreader.h"
// ================== Project 5: Lights, Camera

//extern objl::Loader loader;
Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();
    std::cout << "finish called" << std::endl;
    // Students: anything requiring OpenGL calls when the program exits should be done here
    //glDeleteBuffers(1, &m_vbo);
    //glDeleteVertexArrays(1, &m_vao);

    glDeleteBuffers(1, &m_sphere_vbo);
    glDeleteVertexArrays(1, &m_sphere_vao);

    glDeleteBuffers(1, &m_cube_vbo);
    glDeleteVertexArrays(1, &m_cube_vao);

    glDeleteBuffers(1, &m_cone_vbo);
    glDeleteVertexArrays(1, &m_cone_vao);

    glDeleteBuffers(1, &m_cylinder_vbo);
    glDeleteVertexArrays(1, &m_cylinder_vao);

    glDeleteProgram(m_shader);
    //// from lab 11
    // glDeleteProgram(m_texture_shader);
    // glDeleteProgram(m_phong_shader);
    // glDeleteVertexArrays(1, &m_sphere_vao);
    // glDeleteBuffers(1, &m_sphere_vbo);
    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteBuffers(1, &m_fullscreen_vbo);

    // Task 35: Delete OpenGL memory here
    // glDeleteTextures(1, &m_kitten_texture);
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteProgram(m_texture_shader);

    this->doneCurrent();
}


void Realtime::initializeSphere(int param1, int param2) {

    // Generate and bind VBO
    glGenBuffers(1, &m_sphere_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);

//     std::cout << settings.shapeParameter1 << " " << settings.shapeParameter2 << std::endl;
    Sphere sphere(param1, param2);
    m_sphereData = sphere.getSphereData();
//    std::cout << m_sphereData.size() << std::endl;
    // Generate sphere data
//    m_sphereData = generateSphereData(settings.shapeParameter1, settings.shapeParameter2);
    // Send data to VBO
    glBufferData(GL_ARRAY_BUFFER, m_sphereData.size() * sizeof(GLfloat), m_sphereData.data(), GL_STATIC_DRAW);
    // Generate, and bind vao
    glGenVertexArrays(1, &m_sphere_vao);
    glBindVertexArray(m_sphere_vao);

    // Enable and define attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void Realtime::initializeCube(int param1) {

    glGenBuffers(1, &m_cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);

    Cube cube(param1);
    m_cubeData = cube.getCubeData();
    // m_cubeData = generateCubeData(settings.shapeParameter1, settings.shapeParameter2);

    glBufferData(GL_ARRAY_BUFFER, m_cubeData.size() * sizeof(GLfloat), m_cubeData.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_cube_vao);
    glBindVertexArray(m_cube_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Realtime::initializeCylinder(int param1, int param2) {
    glGenBuffers(1, &m_cylinder_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cylinder_vbo);

//    m_cylinderData = generateCylinderData(settings.shapeParameter1, settings.shapeParameter2);
    Cylinder cylinder(param1, param2);
    m_cylinderData = cylinder.getCylinderData();

    glBufferData(GL_ARRAY_BUFFER, m_cylinderData.size() * sizeof(GLfloat), m_cylinderData.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_cylinder_vao);
    glBindVertexArray(m_cylinder_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Realtime::initializeCone(int param1, int param2) {
    glGenBuffers(1, &m_cone_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cone_vbo);

//    m_coneData = generateConeData(settings.shapeParameter1, settings.shapeParameter2);
    Cone cone(param1, param2);
    m_coneData = cone.getConeData();

    glBufferData(GL_ARRAY_BUFFER, m_coneData.size() * sizeof(GLfloat), m_coneData.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_cone_vao);
    glBindVertexArray(m_cone_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


//void Realtime::initializeMesh2() {
//    glGenBuffers(1, &m_mesh_vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, m_mesh_vbo);
//    std::string inputfile = "C:\\Users\\junsu\\Documents\\Brown\\2022Fall\\Computer-Graphics\\projects\\project5\\capsule.obj";
//    tinyobj::ObjReaderConfig reader_config;
//    reader_config.mtl_search_path="./";
//    tinyobj::ObjReader reader;
//    if (!reader.ParseFromFile(inputfile, reader_config)) {
//      if (!reader.Error().empty()) {
//          std::cerr << "TinyObjReader: " << reader.Error();
//      }
//      exit(1);
//    }
//    attrib = reader.GetAttrib();
//    shapes = reader.GetShapes();
//    materials = reader.GetMaterials();


//    // Loop over shapes
//    for (size_t s = 0; s < shapes.size(); s++) {
//      // Loop over faces(polygon)
//      size_t index_offset = 0;
//      for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
//        size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

//        // Loop over vertices in the face.
//        for (size_t v = 0; v < fv; v++) {
//          // access to vertex
//          tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
//          tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
//          tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
//          tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

//          // Check if `normal_index` is zero or positive. negative = no normal data
////          if (idx.normal_index >= 0) {
//            tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
//            tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
//            tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
////          }
//            mesh_data.push_back(vx);
//            mesh_data.push_back(vy);
//            mesh_data.push_back(vz);
//            mesh_data.push_back(nx);
//            mesh_data.push_back(ny);
//            mesh_data.push_back(nz);
//          // Check if `texcoord_index` is zero or positive. negative = no texcoord data
////          if (idx.texcoord_index >= 0) {
////            tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
////            tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
////          }

//        }
//        index_offset += fv;

//        // per-face material
//        shapes[s].mesh.material_ids[f];
//      }
//    }

//    glBufferData(GL_ARRAY_BUFFER, mesh_data.size() * sizeof(GLfloat), mesh_data.data(), GL_STATIC_DRAW);

//    glGenVertexArrays(1, &m_mesh_vao);
//    glBindVertexArray(m_mesh_vao);

//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));

//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

//    glBindVertexArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);

//}

//void Realtime::initializeMesh() {
//    glGenBuffers(1, &m_mesh_vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, m_mesh_vbo);


//    loader.LoadFile("C:\\Users\\junsu\\Documents\\Brown\\2022Fall\\Computer-Graphics\\projects\\project5\\capsule.obj");
//    std::cout <<  loader.LoadedVertices[0].Position.X << std::endl;
//    int numOfVertices = loader.LoadedVertices.size();

//    for (int i = 0; i < numOfVertices; ++i) {
//        mesh_data.push_back(loader.LoadedVertices[i].Position.X);
//        mesh_data.push_back(loader.LoadedVertices[i].Position.Y);
//        mesh_data.push_back(loader.LoadedVertices[i].Position.Z);
//        mesh_data.push_back(loader.LoadedVertices[i].Normal.X);
//        mesh_data.push_back(loader.LoadedVertices[i].Normal.Y);
//        mesh_data.push_back(loader.LoadedVertices[i].Normal.Z);
//    }

//    glBufferData(GL_ARRAY_BUFFER, mesh_data.size() * sizeof(GLfloat), mesh_data.data(), GL_STATIC_DRAW);

//    glGenVertexArrays(1, &m_mesh_vao);
//    glBindVertexArray(m_mesh_vao);

//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));

//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

//    glBindVertexArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//}

void Realtime::kitten() {
    QString kitten_filepath = QString(":/resources/images/kitten.png");
    //  std::cout << kitten_filepath << std::endl;
      // Task 1: Obtain image from filepath
      // Sanity check for fun
      QImage qImage;
      if(!qImage.load(kitten_filepath)) {
          std::cout << "Your Image Path: " << kitten_filepath.toStdString() << " is not valid." << std::endl;
          exit(1);
      }
      // loaded into CPU memory
      m_image = QImage(kitten_filepath);

      // Task 2: Format image to fit OpenGL
      m_image = m_image.convertToFormat(QImage::Format_RGBA8888).mirrored(); // vertically. topLeft <-> bottomLeft
      // Task 3: Generate kitten texture
      glGenTextures(0, &m_kitten_texture); // m_kitten_texture is an object that will hold texture data and texture object id. To be specific, it holds id and we can use that id to access texture data. m_kitten_texture is a variable that holds the id's of generated textures
      // Task 9: Set the active texture slot to texture slot 0
      // This call can be called after glBindTexture()
      glActiveTexture(GL_TEXTURE1);
      // Task 4: Bind kitten texture
      // We bind texture, m_kitten_texture, to GL_TEXTURE_2D
      // we have an empty texture sitting in the GL_TEXTURE_2D target in our state machine yet
      // Think of GL_TEXTURE_2D as a memory buffer in GPU
      glBindTexture(GL_TEXTURE_2D, m_kitten_texture); // to be specific, bind texture "id's" to our target GL_TEXTURE_2D. We will load our acutal texture data to GL_TEXTURE_2D and then we can use m_kitten_texture to get texture info in paintTexture function, like glBindTexture(GL_TEXTURE_2D, texture);. When a texture (e.g. m_kitten_texture) is bound to a target, the previous binding for that target is automatically broken. While a texture is bound, GL operations on the target to which it is bound affect the bound texture, and queries of the target to which it is bound return state from the bound texture. In effect, the texture targets become aliases for the textures currently bound to them, and the texture name zero refers to the default textures that were bound to them at initialization.
      // Task 5: Load image into kitten texture
      // Fill kitten texture with our QImage (m_image). GL_TEXTURE_2D is currently bound by m_kitten_texture, so any operations using GL_TEXTURE_2D acts on m_kitten_texture. This means at leanst one texture object/id should be bound to GL_TEXTURE_2D.
      // Transfer to GPU texture memory
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());
      // Task 6: Set min and mag filters' interpolation mode to linear
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // Task 7: Unbind kitten texture
      // This unbind our texture (m_kitten_texture) from GL_TEXTURE_2D target. We're done operating on m_kitten_texture, although the operations are acutally on GL_TEXTURE_2D
      glBindTexture(GL_TEXTURE_2D, 0);
      // Task 10: Set the texture.frag uniform for our texture
      // We need program ID (m_texture_shader) because it has access to shaders.
      glUseProgram(m_texture_shader); // can write this here? not earlier?
      GLuint textureIDd = glGetUniformLocation(m_texture_shader, "myTexture2");
      // 0 (or GL_TEXTURE0) is a texture slot we want to use and sample from
      // Set the uniform value for your sampler2D you created in your fragment shader to be the same texture slot number we bound our texture to
      glUniform1i(textureIDd, 0); // We're setting a sampler (in our case, GL_TEXTURE0) to uniform sample2D myTexture in texture.frag
      // Return to default state of program 0
      glUseProgram(0);
}

void Realtime::initializeGL() { // only called once when run program.
    isInitializedRun = true;
    m_devicePixelRatio = this->devicePixelRatio();

    // from lab 11
    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    glClearColor(0,0,0,1);

    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/postprocessing.vert", ":/resources/shaders/postprocessing.frag");

    /*
    // do the same thing for each type of shape?
    int numOfShapeTypes = 4;

    for (int i; i < numOfShapeTypes; ++i) {
        // generate and bind vbo
        glGenBuffers(1, &vbos[i]); // get index and assign it to vbos[i]
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]); // bind GL_ARRAY_BUFFER to vbos[i]

        switch(i) {
        case 0:
            // genereate sphere data
            SphereSolution sphereSolution;
            m_shapeData = sphereSolution.generateSphereData(settings.shapeParameter1, settings.shapeParameter2);
            break;

        case 1:
            Cone cone;
            m_shapeData = cone.generateConeData(settings.shapeParameter1, settings.shapeParameter2);
            break;
        case 2:
            Cube cube;;
            m_shapeData = cube.generateSphereData(settings.shapeParameter1, settings.shapeParameter2);
        case 3:
            Cylinder cylinder;
            m_shapeData = cylinder.generateCylinderData(settings.shapeParameter1, settings.shapeParameter2);
        }



        // send data to vbo
        glBufferData(GL_ARRAY_BUFFER, m_shapeData.size() * sizeof(GLfloat), m_shapeData.data(), GL_STATIC_DRAW);

        // generate, and bind vao
        glGenVertexArrays(1, &vaos[i]);
        glBindVertexArray(vaos[i]);

        // enable and define 0 to store vertex positions
        glEnableVertexAttribArray(0); // attribute position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), reinterpret_cast<void *>(0));

        // enable and define 1 to store normal positions
        glEnableVertexAttribArray(1); // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GL_FLOAT)));
    }
    */

    //extra credit 3
//    if(settings.extraCredit3) {
////        initializeMesh();
//        initializeMesh2();
//    }

    // for testing texture mapping
    //
    // kitten();

    int param1 = settings.shapeParameter1;
    int param2 = settings.shapeParameter2;

    initializeSphere(param1, param2);

    initializeCube(param1);

    initializeCone(param1, param2);

    initializeCylinder(param1, param2);

    /*
    // generate and bind vbo
    glGenBuffers(1, &m_sphere_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
    // genereate sphere data
    SphereSolution sphereSolution;

    m_sphereData = sphereSolution.generateSphereData(settings.shapeParameter1, settings.shapeParameter2);
    // send data to vbo
    glBufferData(GL_ARRAY_BUFFER, m_sphereData.size() * sizeof(GLfloat), m_sphereData.data(), GL_STATIC_DRAW);
    // generate, and bind vao
    glGenVertexArrays(1, &m_sphere_vao);
    glBindVertexArray(m_sphere_vao);

    // enable and define 0 to store vertex positions
    glEnableVertexAttribArray(0); // attribute position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), reinterpret_cast<void *>(0));
    */

    // clean up binding
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /***************** Project 6 **********************/
    if (settings.extraCredit4) {
        glUseProgram(m_shader);
        GLuint textureID = glGetUniformLocation(m_shader, "shapeTexture");
        glUniform1i(textureID, 0); // this is not unbind. bind GL_TEXTURE0
        glUseProgram(0);

    } else {
        glUseProgram(m_texture_shader); // can write this here? not earlier?
        GLuint textureID = glGetUniformLocation(m_texture_shader, "myTexture");
        // 0 (or GL_TEXTURE0) is a texture slot we want to use and sample from
        // Set the uniform value for your sampler2D you created in your fragment shader to be the same texture slot number we bound our texture to
        glUniform1i(textureID, 0); // We're setting a sampler (in our case, GL_TEXTURE0) to uniform sample2D myTexture in texture.frag
        // Return to default state of program 0
        glUseProgram(0);

        fullScreenQuad();
    }
    // In project 6, this is where I draw my scene on. For extra credit, I will draw my scenes with shape texture attached..
    makeFBO();
}

void Realtime::fullScreenQuad() {
    std::vector<GLfloat> fullscreen_quad_data = {
          -1.f,  1.0f, 0.0f,
           0.f,  1.f,

          -1.f, -1.f, 0.0f,
          0.0f, 0.0f,

           1.f, -1.f, 0.0f,
           1.f, 0.0f,

           1.f,  1.f, 0.0f,
           1.f,  1.f,

          -1.f,  1.f, 0.0f,
          0.0f,  1.f,

           1.f, -1.f, 0.0f,
           1.f, 0.0f
      };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);
    // Task 14: modify the code below to add a second attribute to the vertex attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::makeFBO(){// in initializeGL()
    // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    // Before we configure our FBO, we need to generate containers (texture object) for its attachments. As stated previously, these can either be Textures or Renderbuffers.
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0); // sampler
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture); // m_fbo_texture is a texture object. This call means that use that texture object for texture 2D data. glTexImage2D will uses whatever texture object was bound to GL_TEXTURE_2D
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // return to default state by unbinding our texture once we haveset all its parameters. "texture" here means GL_TEXTURE_2D I think.
    glBindTexture(GL_TEXTURE_2D, 0);
    // Task 20: Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer); //m_fbo_renderbuffer is a renderbuffer object
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height );
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // Task 18: Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); // m_fbo is a frame buffer object
    // Task 21: Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);
    // Task 22: Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

void Realtime::paintGLSphere() {
    // Bind Sphere Vertex Data
    glBindVertexArray(m_sphere_vao);
    // Task 2: activate the shader program by calling glUseProgram with `m_shader`
    glUseProgram(m_shader);

//    setFragVert();

    // Draw Command..  6 is because 3 for vertex.x, vertex.y, vertex.z and 3 for normal position
    glDrawArrays(GL_TRIANGLES, 0, m_sphereData.size() / 6); // check is it's 3 or 6. # of vertices to draw!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Unbind Vertex Array
    glBindVertexArray(0);
    // Task 3: deactivate the shader program by passing 0 into glUseProgram
    glUseProgram(0);
}

void Realtime::paintGLCube() {
    glBindVertexArray(m_cube_vao);
    glUseProgram(m_shader);

//    setFragVert();

    glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 6);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::paintGLCone() {
    glBindVertexArray(m_cone_vao);
    glUseProgram(m_shader);

//    setFragVert();

    glDrawArrays(GL_TRIANGLES, 0, m_coneData.size() / 6);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::paintGLCylinder() {
    glBindVertexArray(m_cylinder_vao);
    glUseProgram(m_shader);

//    setFragVert();

    glDrawArrays(GL_TRIANGLES, 0, m_cylinderData.size() / 6);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::extraCredit1() {
    int numOfObjects = metaData.shapes.size();
    if (settings.extraCredit1) {
        int param1 = 2.f * settings.shapeParameter1 / numOfObjects;
        int param2 = 2.f * settings.shapeParameter2 / numOfObjects;
        initializeSphere(param1, param2);

        initializeCube(param1);

        initializeCone(param1, param2);

        initializeCylinder(param1, param2);

    } else { // if click is true, turned on extrCredit at least once
        int param1 = settings.shapeParameter1;
        int param2 = settings.shapeParameter2;
        initializeSphere(param1, param2);

        initializeCube(param1);

        initializeCone(param1, param2);

        initializeCylinder(param1, param2);
    }
}

void Realtime::refreshLights() {
    for (int i = 0; i < 8; ++i) {
        GLuint isSpotId = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+"].isSpotLight").c_str());
        glUniform1f(isSpotId, false);

        GLuint directionalLight = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].isDirectional").c_str());
        glUniform1f(directionalLight, false);

        GLint color = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].color").c_str());
        glUniform4f(color, 0.f, 0, 0, 0);
    }
}

void Realtime::uniformLight() {
    // send number of colors to uniform
    int numOfLights = metaData.lights.size();
    GLint num = glGetUniformLocation(m_shader, "numOfLights");
    glUniform1i(num, numOfLights);

    refreshLights();

    // send colors to uniform
    for (int i = 0; i < metaData.lights.size(); ++i) {
//        if (metaData.lights[i].type == LightType::LIGHT_DIRECTIONAL)
//        std::cout << "directional" << std::endl;
//        if (metaData.lights[i].type == LightType::LIGHT_POINT)
//        std::cout << "point" << std::endl;

        GLint function = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].function").c_str());
        glUniform3fv(function, 1, &metaData.lights[i].function[0]);

        GLint color = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].color").c_str());
        glUniform4fv(color, 1, &metaData.lights[i].color[0]);

        // if light is directional, send false to uniform isDirectionaLight
        GLuint directionalLight = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].isDirectional").c_str());
        if (metaData.lights[i].type != LightType::LIGHT_DIRECTIONAL){
            glUniform1f(directionalLight, false);
            m_lightPos = metaData.lights[i].pos;
            GLuint uniformLightPosition = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].position").c_str());
            glUniform4fv(uniformLightPosition, 1, &m_lightPos[0]);

            if (metaData.lights[i].type == LightType::LIGHT_SPOT) {
                GLuint isSpotId = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+"].isSpotLight").c_str());
                glUniform1f(isSpotId, true);
                // set angle
                GLuint angleID = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].angle").c_str());
                glUniform1f(angleID, metaData.lights[i].angle);

                // set penumbra
                GLuint penumbra = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].penumbra").c_str());
                glUniform1f(penumbra, metaData.lights[i].penumbra);

                // set direction
                GLuint dirID = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+"].direction").c_str());
                glUniform4fv(dirID, 1, &metaData.lights[i].dir[0]);
            }

        } else { // (metaData.lights[i].type == LightType::LIGHT_DIRECTIONAL){
//                    std::cout << metaData.lights[i].dir[0] << " " << metaData.lights[i].dir[1] << " " << metaData.lights[i].dir[2] << std::endl;
            glUniform1f(directionalLight, true);
            // send direction of directional light to uniform variable
            GLuint loc0 = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+"].direction").c_str());
            glUniform4fv(loc0, 1, &metaData.lights[i].dir[0]);
//            glUniform4f(loc0, metaData.lights[i].dir[0], metaData.lights[i].dir[1], metaData.lights[i].dir[2], metaData.lights[i].dir[3]);
        }
    } // light

    GLint loc1 = glGetUniformLocation(m_shader, "k_a"); // uniform variable in frag
    glUniform1f(loc1, metaData.globalData.ka); // variable in realtime.h

    GLint loc2 = glGetUniformLocation(m_shader, "k_d");
    glUniform1f(loc2, metaData.globalData.kd);

    GLint loc3 = glGetUniformLocation(m_shader, "k_s");
    glUniform1f(loc3, metaData.globalData.ks);
}

void Realtime::extraCredit2(RenderShapeData &shape, glm::vec4 cameraPositionWorldSpace) {
    if(settings.extraCredit2) {
        glm::vec4 objectOriginWorldSpace = shape.ctm * glm::vec4(0,0,0,1);
        float dist = glm::distance(cameraPositionWorldSpace, objectOriginWorldSpace);
//        int remain = dist % 10;
//        switch(dist %)

        int param1 = settings.shapeParameter1 / (dist * 5 / 100);
        int param2 = settings.shapeParameter2 / (dist * 5 / 100);
//                if(dist <= 12.f) {
//                    param1 = settings.shapeParameter1;
//                    param2 = settings.shapeParameter2;
//                }
        initializeSphere(param1, param2);

        initializeCube(param1);

        initializeCone(param1, param2);

        initializeCylinder(param1, param2);

    }
}

void Realtime::extraCredit4(RenderShapeData &shape) {
    //texture mapping ec
    // extra credit Texture Mapping
    if(settings.extraCredit4) {
        std::string textureFileName = shape.primitive.material.textureMap.filename;
        QString qstrTextureFileName = QString::fromStdString(textureFileName);

        QImage qImage;
        if(!qImage.load(qstrTextureFileName)) {
            std::cout << "Your Image Path: " << qstrTextureFileName.toStdString() << " is not valid." << std::endl;
            // exit(1);
        }

        m_image = QImage(qstrTextureFileName);
        m_image = m_image.convertToFormat(QImage::Format_RGBA8888).mirrored();

        glGenTextures(1, &m_shape_texture);
        glActiveTexture(GL_TEXTURE0);
        // basically, this is binding texture (m_fbo_texture) to GL_TEXTURE0, which means telling sampler GL_TEXTURE0 where to sample from.
        glBindTexture(GL_TEXTURE_2D, m_shape_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

//        // to set uniform variable because m_shader has connection to default.frag and default.vert
//        glUseProgram(m_shader);

//        glBindVertexArray(m_fullscreen_vao);
//        glBindTexture(GL_TEXTURE_2D, m_shape_texture);

//        // send GL_TEXTURE0 to default.frag
//        GLuint textureID = glGetUniformLocation(m_shader, "myTexture");
//        glUniform1i(textureID, 0);

        glUseProgram(0);
//        m_fullscreen_vao를 사용해서 m_shape_texture에 있는 textureㄹ를 샘플링하겟다.. 그럼 m_fullscreen_vao를 default.frag에서 써서 uv coordniate 얻어서 frag 에서 m_shape_texture를 샘플링..
//        이전에는 m_fullscreen_vao랑 m_fbo (에 그려진 texture)를 써서 샘플링.
    } // texture mapping ec
}

void Realtime::uniformShape(RenderShapeData &shape) {
    //// these depend on objects
    // for texture mapping extra credit
    extraCredit4(shape);

    GLint loc4 = glGetUniformLocation(m_shader, "shininess");
    float shininess = shape.primitive.material.shininess;
//    if (shininess <= 0)
//        shininess = 1e-10;
    glUniform1f(loc4, shininess);

    GLint loc5 = glGetUniformLocation(m_shader, "o_a");
                glUniform4fv(loc5, 1, &shape.primitive.material.cAmbient[0]);
//    glUniform4f(loc5, shape.primitive.material.cAmbient[0], shape.primitive.material.cAmbient[1], shape.primitive.material.cAmbient[2], shape.primitive.material.cAmbient[3]);
//                std::cout << "cube's cAmbient: " << shape.primitive.material.cAmbient[0] << " " <<shape.primitive.material.cAmbient[1] << " " <<shape.primitive.material.cAmbient[2] << " " << shape.primitive.material.cAmbient[3] << std::endl;

    GLint loc6 = glGetUniformLocation(m_shader, "o_d");
                glUniform4fv(loc6, 1, &shape.primitive.material.cDiffuse[0]);
//    glUniform4f(loc6, shape.primitive.material.cDiffuse[0], shape.primitive.material.cDiffuse[1], shape.primitive.material.cDiffuse[2], shape.primitive.material.cDiffuse[3]);

    GLint loc7 = glGetUniformLocation(m_shader, "o_s");
                glUniform4fv(loc7, 1, &shape.primitive.material.cSpecular[0]);
//    glUniform4f(loc7, shape.primitive.material.cSpecular[0], shape.primitive.material.cSpecular[1], shape.primitive.material.cSpecular[2], shape.primitive.material.cSpecular[3]);

    // calculate model matrix
    m_model = shape.ctm;

    // m_view and m_proj don't have to be in this loop?

    // Task 6: pass in m_model as a uniform into the shader program
    // uniformMatrixID is the location of uniform variable "modelMatrix"
    GLuint uniformMatrixID = glGetUniformLocation(m_shader, "modelMatrix");
    glUniformMatrix4fv(uniformMatrixID, 1, GL_FALSE, &m_model[0][0]);
    //// these depend on objects
}

void Realtime::checkShapeTypeAndDraw(RenderShapeData &shape) {
    if (shape.primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
        // Task 2: activate the shader program by calling glUseProgram with `m_shader`
        glBindVertexArray(m_cube_vao);
        // Draw Command..  6 is because 3 for vertex.x, vertex.y, vertex.z and 3 for normal position

        if (settings.extraCredit4) {
            glBindTexture(GL_TEXTURE_2D, m_shape_texture);
            glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 8);

        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 6);
        }
        // Unbind Vertex Array
        glBindVertexArray(0);
    }

    else if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
        // Task 2: activate the shader program by calling glUseProgram with `m_shader`
    //                glUseProgram(m_shader);

        // Bind Sphere Vertex Data
        glBindVertexArray(m_sphere_vao);
        // Draw Command..  6 is because 3 for vertex.x, vertex.y, vertex.z and 3 for normal position
        glDrawArrays(GL_TRIANGLES, 0, m_sphereData.size() / 6); // check is it's 3 or 6. # of vertices to draw!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // Unbind Vertex Array
        glBindVertexArray(0);
        // Task 3: deactivate the shader program by passing 0 into glUseProgram
    //                glUseProgram(0);
    }

    else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CONE) {
//                paintGLCone();
//                glUseProgram(m_shader);
        glBindVertexArray(m_cone_vao);

/*
    //    setFragVert();
//                GLint function = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "]function").c_str());
//                glUniform3fv(function, 1, &metaData.lights[i].function[0]);


//                GLint color = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].color").c_str());
//                glUniform3fv(color, 1, &metaData.lights[i].color[i]);
//                // if light is directional, send false to uniform isDirectionaLight
//                GLuint directionalLight = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].isDirectional").c_str());

//                if (metaData.lights[i].type != LightType::LIGHT_DIRECTIONAL){
//                    glUniform1f(directionalLight, 0.f);
////                    std::cout << "shouldn't appear" << std::endl;
//                    m_lightPos = metaData.lights[i].pos;
//                    GLuint uniformLightPosition = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].position").c_str());
//                    glUniform4fv(uniformLightPosition, 1, &m_lightPos[0]);

//                } else if (metaData.lights[i].type == LightType::LIGHT_DIRECTIONAL){
////                    std::cout << metaData.lights[i].dir[0] << " " << metaData.lights[i].dir[1] << " " << metaData.lights[i].dir[2] << std::endl;
//                    glUniform1f(directionalLight, 1.f);
//                    // send direction of directional light to uniform variable
//                    GLuint loc0 = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].direction").c_str());
//                    glUniform4fv(loc0, 1, &metaData.lights[i].dir[0]);
//                }
*/

        // Draw Command..  6 is because 3 for vertex.x, vertex.y, vertex.z and 3 for normal position
        glDrawArrays(GL_TRIANGLES, 0, m_coneData.size() / 6); // check is it's 3 or 6. # of vertices to draw!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        // Unbind Vertex Array
        glBindVertexArray(0);
        // Task 3: deactivate the shader program by passing 0 into glUseProgram
//                glUseProgram(0);

    }

    else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER) {
//                paintGLCylinder();
//                glUseProgram(m_shader);
        glBindVertexArray(m_cylinder_vao);

//                uniformShape(shape);


        // Draw Command..  6 is because 3 for vertex.x, vertex.y, vertex.z and 3 for normal position
        glDrawArrays(GL_TRIANGLES, 0, m_cylinderData.size() / 6); // check is it's 3 or 6. # of vertices to draw!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        // Unbind Vertex Array
        glBindVertexArray(0);
        // Task 3: deactivate the shader program by passing 0 into glUseProgram
//                glUseProgram(0);

    }
    /*else { // extra credit 3
        if (settings.extraCredit3) {
            glBindVertexArray(m_mesh_vao);
            GLint loc4 = glGetUniformLocation(m_shader, "shininess");
            glUniform1f(loc4, materials[0].shininess);

            GLint loc5 = glGetUniformLocation(m_shader, "o_a");
            glUniform3f(loc5,
                        materials[0].ambient[0],
                        materials[0].ambient[1],
                        materials[0].ambient[2]
                        );

            GLint loc6 = glGetUniformLocation(m_shader, "o_d");
            glUniform3f(loc6,
                        materials[0].diffuse[0],
                        materials[0].diffuse[1],
                        materials[0].diffuse[2]
                        );

            GLint loc7 = glGetUniformLocation(m_shader, "o_s");
            glUniform3f(loc7,
                        materials[0].specular[0],
                        materials[0].specular[1],
                        materials[0].specular[2]);


            m_model =  glm::mat4(1);



            // Task 6: pass in m_model as a uniform into the shader program
            // uniformMatrixID is the location of uniform variable "modelMatrix"
            GLuint uniformMatrixID = glGetUniformLocation(m_shader, "modelMatrix");
            glUniformMatrix4fv(uniformMatrixID, 1, GL_FALSE, &m_model[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, mesh_data.size() / 6); // check

            // Unbind Vertex Array
            glBindVertexArray(0);
            */

            /*
            glBindVertexArray(m_mesh_vao);
            GLint loc4 = glGetUniformLocation(m_shader, "shininess");
            glUniform1f(loc4, loader.LoadedMaterials[0].Ns);

            GLint loc5 = glGetUniformLocation(m_shader, "o_a");
            glUniform3f(loc5,
                        loader.LoadedMaterials[0].Ka.X,
                        loader.LoadedMaterials[0].Ka.Y,
                        loader.LoadedMaterials[0].Ka.Z);

            GLint loc6 = glGetUniformLocation(m_shader, "o_d");
            glUniform3f(loc6,
                        loader.LoadedMaterials[0].Kd.X,
                        loader.LoadedMaterials[0].Kd.Y,
                        loader.LoadedMaterials[0].Kd.Z);

            GLint loc7 = glGetUniformLocation(m_shader, "o_s");
            glUniform3f(loc7,
                        loader.LoadedMaterials[0].Ks.X,
                        loader.LoadedMaterials[0].Ks.Y,
                        loader.LoadedMaterials[0].Ks.Z);


            m_model =  glm::mat4(1);



            // Task 6: pass in m_model as a uniform into the shader program
            // uniformMatrixID is the location of uniform variable "modelMatrix"
            GLuint uniformMatrixID = glGetUniformLocation(m_shader, "modelMatrix");
            glUniformMatrix4fv(uniformMatrixID, 1, GL_FALSE, &m_model[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, mesh_data.size() / 6); // check

            // Unbind Vertex Array
            glBindVertexArray(0);

        }
    }*/
}

void Realtime::paintShapes() {
    glUseProgram(m_shader);
    // calculate view matrix
    m_view = generateViewMatrix();
    // calculate projection matrix
    m_proj = generateProjectionMatrix();
    // Task 7: pass in m_view and m_proj
    GLuint uniformViewMatrixID = glGetUniformLocation(m_shader, "viewMatrix");
    glUniformMatrix4fv(uniformViewMatrixID, 1, GL_FALSE, &m_view[0][0]);

    GLuint uniformProjectMatrixID = glGetUniformLocation(m_shader, "projectionMatrix");
    glUniformMatrix4fv(uniformProjectMatrixID, 1, GL_FALSE, &m_proj[0][0]);
    // Students: anything requiring OpenGL calls every frame should be done here

    // get world space camera position
    glm::vec4 cameraPositionWorldSpace = inverse(m_view) * glm::vec4(0,0,0,1.f);
    GLint loc8 = glGetUniformLocation(m_shader, "worldSpaceCameraPosition");
    glUniform4fv(loc8, 1, &cameraPositionWorldSpace[0]);

    // texture mapping extra credit
    /*
    GLuint ec = glGetUniformLocation(m_shader, "ec4");
    if(settings.extraCredit4) {
        glUniform1i(ec, settings.extraCredit4);
        glBindVertexArray(m_fullscreen_vao);
        // Task 10: Bind "texture" to slot 0
        // Bind the input parameter texture to texture slot 0; GL_TEXTURE_2D is bound to GL_TEXTURE0 in initializeGL.
        // basically, this is binding texture (m_fbo_texture) to GL_TEXTURE0, which means telling sampler GL_TEXTURE0 where to sample from.
        glBindTexture(GL_TEXTURE_2D, m_shape_texture);
    } else {
        glUniform1i(ec, settings.extraCredit4);
    }
    */

    uniformLight();

    // loop over objects in the scene. and inside the loop, loop over lights
    // for each shape call paintGL___(). Inside paintGL__(), we bind specfic vao. If paintGLCube(), I bind cube_vao.
    // there's vbo that's linked to vao.Then in paintGLCube(), when I bind cube_vao, I have connection to specific vbo, which is cube_vbo because I linked them
    // in initializeGL.  There's no given or 정해진 coordinates info. m_sphereData = generateSphereData(settings.shapeParameter1, settings.shapeParameter2);
    // I get coordinates when I call generateSphereData with shapeParameter1 and 2.
    for (auto &shape : metaData.shapes) {
/*
//        std::vector<TextureMap> RayTracer::saveTextureMaps(const std::vector<RenderShapeData> &shapes)
//        {
//            for (int i = 0; i < shapes.size(); i++) {
//                TextureMap textureMap;
//                QString filePath = QString::fromStdString(shapes[i].primitive.material.textureMap.filename);
//                textureMap.loadImageFromFile(filePath);
//                textureMaps.push_back(textureMap);
//            }
//            return textureMaps;
//        }
*/
        //extra credit 2
        extraCredit2(shape, cameraPositionWorldSpace);

        uniformShape(shape);

        checkShapeTypeAndDraw(shape);

        /*
        // I included thses in each paintGL function
        // unibind vbo vao glprogram for each shape?
        // Unbind Vertex Array
        glBindVertexArray(0);
        glUseProgram(0);
        */

    } // shapes
    glUseProgram(0);
}

void Realtime::paintGL() {
    // Clear screen color and depth before painting
    // call this once. If called for each shape, only the last shape will appear on the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Project6. Currently, when inverse is on, only shapes get inverted. This is because background is default FBO. I need to, although I can invert shapes already, send shapes and background to my FBO and invert color.
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_fbo_width, m_fbo_height);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // extra credit 1
    extraCredit1();

//    if(settings.extraCredit4) {
//    /************************texture mapping extra credit********************************/
//        // m_fullscreen_vao has data of full quad
//        glBindVertexArray(m_fullscreen_vao);
//        // Task 10: Bind "texture" to slot 0
//        // Bind the input parameter texture to texture slot 0; GL_TEXTURE_2D is bound to GL_TEXTURE0 in initializeGL.
//        // basically, this is binding texture (m_fbo_texture) to GL_TEXTURE0, which means telling sampler GL_TEXTURE0 where to sample from.
//        glBindTexture(GL_TEXTURE_2D, m_shape_texture);

//        // since m_fullscreen_vao is bound, six uv coordinates in quad data will be sent
//        glDrawArrays(GL_TRIANGLES, 0, 6);// 6 or fullscreen_quad_data.size() / 5

//        glBindTexture(GL_TEXTURE_2D, 0);
//        glBindVertexArray(0);
//    /************************texture mapping extra credit********************************/
//    }






    // uses defualt.frag and default.vert and m_shader...
    // doesn't matter which shader/vert/frag is used.. as long as FBO is used..
    // shapes will be drawn to m_fbo_texture. why? because m_fbo_texture is attached to m_fbo in makeFBO(). m_fbo_texture will be drawn to default FBO by calling paintTexture(m_fbo_texture)
    paintShapes(); // == paintExampleGeometry();

    /**************** project 6 ***************/
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_screen_width*1.25f, m_screen_height*1.25f);

//    glClearColor(0,0,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // uses m_texture_shader, texture.frag, texture.vert
    // paint texture that's drawn on FBO? to default?
    paintTexture(m_fbo_texture);
    // paintTexture(m_shape_texture);

}

void Realtime::uniformSharpenFilter() {
    std::vector<float> filter1{0.f,0.f,0.f,0,2.f,0,0,0,0};
    std::vector<float> filter2{1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f};
    for (int i = 0; i < 9; ++i) {
        GLuint ele1 = glGetUniformLocation(m_texture_shader, ("filter1["+ std::to_string(i) +"]").c_str());
        glUniform1f(ele1, filter1[i]);

        GLuint ele2 = glGetUniformLocation(m_texture_shader, ("filter2["+ std::to_string(i) +"]").c_str());
        glUniform1f(ele2, filter2[i]);
    }
}

void Realtime::uniformEdgeFilter() {
    GLuint edge = glGetUniformLocation(m_texture_shader, "isEdgeFilterOn");
    glUniform1i(edge, true);

    std::vector<float> filter1{1.f,1.f,1.f,1,-8.f,1,1,1,1};

    for (int i = 0; i < 9; ++i) {
        GLuint ele1 = glGetUniformLocation(m_texture_shader, ("filter1["+ std::to_string(i) +"]").c_str());
        glUniform1f(ele1, filter1[i]);
    }
}

// paint texture drawn to myFBO back to default FBO
void Realtime::paintTexture(GLuint texture){
    glUseProgram(m_texture_shader);

    // save filter1 and filter2 data as uniform in texture.frag
     uniformSharpenFilter();
    // kernel filter extra credit
    // uniformEdgeFilter();

    GLuint textureWidthID = glGetUniformLocation(m_texture_shader, "textureWidth");
    glUniform1i(textureWidthID, m_fbo_width);

    GLuint textureHeightID = glGetUniformLocation(m_texture_shader, "textureHeight");
    glUniform1i(textureHeightID, m_fbo_height);
    // Task 32: Set your bool uniform on whether or not to filter the texture drawn
    GLuint togglePixelID = glGetUniformLocation(m_texture_shader, "isPixelPostProcessingOn");
    glUniform1i (togglePixelID, settings.perPixelFilter);

    GLuint toggleKernelID = glGetUniformLocation(m_texture_shader, "isKernelPostProcessingOn");
    glUniform1i (toggleKernelID, settings.kernelBasedFilter);

    if (settings.extraCredit4) {
        // glBindVertexArray(m_cube_vao);
    } else {
    // m_fullscreen_vao has data of full quad
        glBindVertexArray(m_fullscreen_vao);
    }
    // Task 10: Bind "texture" to slot 0
    // Bind the input parameter texture to texture slot 0; GL_TEXTURE_2D is bound to GL_TEXTURE0 in initializeGL.
    // basically, this is binding texture (m_fbo_texture) to GL_TEXTURE0, which means telling sampler GL_TEXTURE0 where to sample from.
    glBindTexture(GL_TEXTURE_2D, texture);

    if(settings.extraCredit4) {
//        glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size()/8);
    } else {
        // since m_fullscreen_vao is bound, six uv coordinates in quad data will be sent
        glDrawArrays(GL_TRIANGLES, 0, 6);// 6 or fullscreen_quad_data.size() / 5
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


glm::mat4 Realtime::generateProjectionMatrix() {
    // need to use theta_width and theta height (or aspect ratio?)
    // aspect ratio = width / height
    float heightAngle = metaData.cameraData.heightAngle; // in radians
    float ratio = windowWidth / windowHeight;
    float widthAngle = heightAngle * (1/ratio);
    widthAngle = 2*atan(windowWidth / windowHeight * tan(heightAngle/2.f));

    // if I have width and height, I don't need angles
    glm::mat4 mR = glm::mat4(1.f/(settings.farPlane*(windowWidth/windowHeight * tan(heightAngle/2.f))), 0,0,0,//this is a column
                             0, 1.f/(settings.farPlane*tan(heightAngle/2.f)), 0, 0,
                             0, 0, 1.f/settings.farPlane, 0,
                             0, 0, 0, 1);

    float c = -settings.nearPlane / settings.farPlane;
    glm::mat4 mM = glm::mat4(1.f,0,0,0,
                            0,1,0,0,
                            0,0,1/(1+c), -1,
                            0, 0, -c/(1.f+c), 0);

    glm::mat4 mL = glm::mat4(1.f,0,0,0,
                             0,1.f,0,0,
                             0,0,-2.f,0,
                             0,0,-1.f,1.f);

//    std::cout << settings.nearPlane << " " << settings.farPlane << std::endl;
//    m_proj = glm::perspective(glm::radians(45.0),1.0 * windowWidth / windowHeight, double(settings.nearPlane), double(settings.farPlane));
//    return m_proj;
    return mL * mM * mR;
}

glm::mat4 Realtime::generateViewMatrix() {
    glm::vec3 vec3Look = glm::vec3(metaData.cameraData.look);
    glm::vec3 vec3Up = glm::vec3(metaData.cameraData.up);

    glm::vec3 w = -glm::normalize(vec3Look);
    glm::vec3 v = glm::normalize(vec3Up - glm::dot(vec3Up, w)*w);

    glm::vec3 u = glm::cross(v, w);

    glm::mat4 R = glm::transpose(glm::mat4{glm::vec4{u,0.f}, glm::vec4{v, 0.f}, glm::vec4{w,0.f}, glm::vec4{0.f,0,0,1}});

    glm::mat4 T{glm::vec4{1.f,0,0,0},
                glm::vec4{0.f,1,0,0},
                glm::vec4{0.f,0,1,0},
                glm::vec4{glm::vec3{-metaData.cameraData.pos}, 1}};

    glm::mat4 viewMatrix = R * T;

    return viewMatrix;
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    // use w and h to correctly update project matrix?
    // can't use perspective
    // m_proj = glm::perspective(glm::radians(45.0),1.0 * w / h,0.01,100.0); // copied from lab10
    windowWidth = w;
    windowHeight = h;

    /** Project 6**/
    // Task 34: Delete Texture, Renderbuffer, and Framebuffer memory
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    // Task 34: Regenerate your FBOs
    makeFBO();

    m_proj = generateProjectionMatrix();


//    m_proj = generateProjectionMatrix();

    // near plane , far plane are in settings.cpp.
    // update
    // where do i get width and height of window size?

    // view matrix stays the same? yes
    // m_view = generateViewMatrix();
    // also camera? view matrix?

//    initializeSphere();

//    initializeCube();

//    initializeCone();

//    initializeCylinder();

    // clean up binding
//    glBindVertexArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);


}

void Realtime::sceneChanged() {

//    finish();
    // parse here. yes load a scene here
    bool success = SceneParser::parse(settings.sceneFilePath, metaData);

    if (!success) {
           std::cerr << "Error loading scene: \"" << settings.sceneFilePath << "\"" << std::endl;
           exit(1);
    } else {
        std::cout << "Successfully parsed" << std::endl;
    }
    makeCurrent();
    if (isInitializedRun) {
        glDeleteBuffers(1, &m_sphere_vbo);
        glDeleteVertexArrays(1, &m_sphere_vao);

        glDeleteBuffers(1, &m_cube_vbo);
        glDeleteVertexArrays(1, &m_cube_vao);

        glDeleteBuffers(1, &m_cone_vbo);
        glDeleteVertexArrays(1, &m_cone_vao);

        glDeleteBuffers(1, &m_cylinder_vbo);
        glDeleteVertexArrays(1, &m_cylinder_vao);

//        glDeleteProgram(m_shader);
    }


//    glDeleteBuffers(1, &m_cube_vbo);
//    glDeleteVertexArrays(1, &m_cube_vao);

//    glDeleteBuffers(1, &m_sphere_vbo);
//    glDeleteVertexArrays(1, &m_sphere_vao);

//    glDeleteBuffers(1, &m_cone_vbo);
//    glDeleteVertexArrays(1, &m_cone_vao);

//    glDeleteBuffers(1, &m_cylinder_vbo);
//    glDeleteVertexArrays(1, &m_cylinder_vao);

     //call codes in initializeGL

    int param1 = settings.shapeParameter1;
    int param2 = settings.shapeParameter2;

    initializeSphere(param1, param2);

    initializeCube(param1);

    initializeCone(param1, param2);

    initializeCylinder(param1, param2);

    // clean up binding
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    update(); // asks for a PaintGL() call to occur

}

void Realtime::settingsChanged() { // since settings are changed, update my variables accordingly
    // reset camera?
//    initializeGL(); // to update vao and vbo?


    if (isInitializedRun) {
        // if I check Extra credit 1, then settings.extracredit1 = true. ?  Yes
//        std::cout << settings.extraCredit1 << std::endl;
        makeCurrent();
//        glDeleteBuffers(1, &m_cube_vbo);
//        glDeleteVertexArrays(1, &m_cube_vao);

        int param1 = settings.shapeParameter1;
        int param2 = settings.shapeParameter2;

        // since param1 and 2 are changed, regenerate objects using updated params
        initializeCube(param1);

//        glBindVertexArray(0);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);

//        glDeleteBuffers(1, &m_sphere_vbo);
//        glDeleteVertexArrays(1, &m_sphere_vao);

    //call codes in initializeGL
        initializeSphere(param1, param2);

        initializeCone(param1, param2);

        initializeCylinder(param1, param2);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        update(); // asks for a PaintGL() call to occur;

    }


}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        // rotate horizontally
        float c = glm::cos(deltaX/1000.f);
        float s = glm::sin(deltaX/1000.f);
        float x = 0.f;
        float y = 1.f;
        float z = 0.f;

        glm::mat4 rotate((1-c)*pow(x,2)+c, (1-c)*x*y+s*z, (1-c)*x*z-s*y,0,
                         (1-c)*x*y-s*z, (1-c)*pow(y,2)+c, (1-c)*y*z+s*x,0,
                         (1-c)*x*z+s*y, (1-c)*y*z-s*x, (1-c)*pow(z,2)+c, 0,
                         0,0,0,1);

        metaData.cameraData.look = rotate * metaData.cameraData.look;

        // rotate vertically
        glm::vec3 perpLookAndUp = glm::cross(glm::vec3(metaData.cameraData.look), glm::vec3(metaData.cameraData.up));

        c = glm::cos(deltaY/10000.f);
        s = glm::sin(deltaY/10000.f);
        x = perpLookAndUp.x;
        y = perpLookAndUp.y;
        z = perpLookAndUp.z;

        rotate = glm::mat4((1-c)*pow(x,2)+c, (1-c)*x*y+s*z, (1-c)*x*z-s*y,0,
                         (1-c)*x*y-s*z, (1-c)*pow(y,2)+c, (1-c)*y*z+s*x,0,
                         (1-c)*x*z+s*y, (1-c)*y*z-s*x, (1-c)*pow(z,2)+c, 0,
                         0,0,0,1);

        metaData.cameraData.look = rotate * metaData.cameraData.look;
        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    if (m_keyMap[Qt::Key_W] == true) {
        metaData.cameraData.pos += deltaTime*metaData.cameraData.look;
//        m_view = generateViewMatrix();
    } else if (m_keyMap[Qt::Key_S] == true) {
        metaData.cameraData.pos -= deltaTime*metaData.cameraData.look;
    } else if (m_keyMap[Qt::Key_A] == true) {
        glm::vec3 dir = glm::cross(glm::vec3(metaData.cameraData.up), glm::vec3(metaData.cameraData.look));
        metaData.cameraData.pos += deltaTime * glm::vec4(dir, 0.f);
    } else if (m_keyMap[Qt::Key_D] == true) {
        glm::vec3 dir = glm::cross(glm::vec3(metaData.cameraData.look), glm::vec3(metaData.cameraData.up));
        metaData.cameraData.pos += deltaTime * glm::vec4(dir, 0.f);
    } else if (m_keyMap[Qt::Key_Space] == true) {
        metaData.cameraData.pos += 5*deltaTime * glm::vec4(0,1.f,0,0);
    } else if (m_keyMap[Qt::Key_Control] == true) {
        metaData.cameraData.pos += 5*deltaTime * glm::vec4(0,-1.f,0,0);
    }
    update(); // asks for a PaintGL() call to occur
}
