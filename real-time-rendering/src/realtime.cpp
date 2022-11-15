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
// ================== Project 5: Lights, Camera

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



void Realtime::initializeGL() { // only called once when run program.
    isInitializedRun = true;
    m_devicePixelRatio = this->devicePixelRatio();

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
//    bool success = SceneParser::parse(settings.sceneFilePath, metaData);

//    if (!success) {
//           std::cerr << "Error loading scene: \"" << settings.sceneFilePath << "\"" << std::endl;
//           exit(1);
//    }
    glClearColor(0,0,0,1);

    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

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
//    isInitializedRun = false;
}


void Realtime::setFragVert() {
//    // Task 6: pass in m_model as a uniform into the shader program
//    // uniformMatrixID is the location of uniform variable "modelMatrix"
//    GLuint uniformMatrixID = glGetUniformLocation(m_shader, "modelMatrix");
//    glUniformMatrix4fv(uniformMatrixID, 1, GL_FALSE, &m_model[0][0]);

//    // Task 7: pass in m_view and m_proj
//    GLuint uniformViewMatrixID = glGetUniformLocation(m_shader, "viewMatrix");
//    glUniformMatrix4fv(uniformViewMatrixID, 1, GL_FALSE, &m_view[0][0]);

//    GLuint uniformProjectMatrixID = glGetUniformLocation(m_shader, "projectionMatrix");
//    glUniformMatrix4fv(uniformProjectMatrixID, 1, GL_FALSE, &m_proj[0][0]);

    // Task 12: pass m_ka into the fragment shader as a uniform
    // GLuint uniformAmbient = glGetUniformLocation(m_shader, "k_a");
    // glUniform1f(uniformAmbient, m_ka); // pass m_ka into the shader from the CPU as a uniform

    // Task 13: pass light position and m_kd into the fragment shader as a uniform
    // GLuint uniformDiffuseCoefficient = glGetUniformLocation(m_shader, "k_d");
    // glUniform1f(uniformDiffuseCoefficient, m_kd);

//    GLuint uniformLightPosition = glGetUniformLocation(m_shader, "worldSpaceLightPosition");
//    glUniform4fv(uniformLightPosition, 1, &m_lightPos[0]);

    // Task 14: pass shininess, m_ks, and world-space camera position
    // GLuint uniformSpecularCoefficient = glGetUniformLocation(m_shader, "k_s");
    // glUniform1f(uniformSpecularCoefficient, m_ks);

    // GLuint uniformShiness = glGetUniformLocation(m_shader, "shiness");
    // glUniform1f(uniformShiness, m_shininess);

    // use inverse(m_view) to get world space camera position
//    glm::vec4 cameraPositionWorldSpace = inverse(m_view) * glm::vec4(0,0,0,1.f);
//    GLuint uniformWorldSpaceCameraPosition = glGetUniformLocation(m_shader, "worldSpaceCameraPosition");
//    glUniform4fv(uniformWorldSpaceCameraPosition, 1, &cameraPositionWorldSpace[0]);
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

void Realtime::paintGL() {
//    std::cout << settings.shapeParameter1 << std::endl;

    // Clear screen color and depth before painting
    // call this once. If called for each shape, only the last shape will appear on the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
    // these are the same for all shapes, i.e., global scene data
    GLint loc1 = glGetUniformLocation(m_shader, "globalSceneData.k_a"); // uniform variable in frag
    glUniform1f(loc1, metaData.globalData.ka); // variable in realtime.h

    GLint loc2 = glGetUniformLocation(m_shader, "globalSceneData.k_d");
    glUniform1f(loc2, metaData.globalData.kd);

    GLint loc3 = glGetUniformLocation(m_shader, "globalSceneData.k_s");
    glUniform1f(loc3, metaData.globalData.ks);
    */




    // get from surface to camera world space vector, which will be used in fragment shader to calculate specular component
//    int originalParam1 = settings.shapeParameter1;
//    int originalParam2 = settings.shapeParameter2;
//    if (settings.extraCredit1) {

//    }
    // extra credit 1.

//    if (!) {
//        originalParam1 = settings.shapeParameter1;
//        originalParam2 = settings.shapeParameter2;
//    }


    // extra credit 1
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

    for (int i = 0 ; i < 8; ++i) {
        GLint color = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].color").c_str());
        glUniform3f(color, 0,0,0);
    }
//    std::cout << metaData.lights.size() << std::endl;
//    GLint num = glGetUniformLocation(m_shader, "currentNumOfLights");
//    glUniform1f(num, metaData.lights.size());

    for (int i = 0; i < metaData.lights.size(); ++i) {
        GLint function = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "]function").c_str());
        glUniform3fv(function, 1, &metaData.lights[i].function[0]);


        GLint color = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].color").c_str());
        glUniform3fv(color, 1, &metaData.lights[i].color[i]);
        // if light is directional, send false to uniform isDirectionaLight
        GLuint directionalLight = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].isDirectional").c_str());

        if (metaData.lights[i].type != LightType::LIGHT_DIRECTIONAL){
            glUniform1f(directionalLight, 0.f);
//                    std::cout << "shouldn't appear" << std::endl;
            m_lightPos = metaData.lights[i].pos;
            GLuint uniformLightPosition = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].position").c_str());
            glUniform4fv(uniformLightPosition, 1, &m_lightPos[0]);

        } else if (metaData.lights[i].type == LightType::LIGHT_DIRECTIONAL){
//                    std::cout << metaData.lights[i].dir[0] << " " << metaData.lights[i].dir[1] << " " << metaData.lights[i].dir[2] << std::endl;
            glUniform1f(directionalLight, 1.f);
            // send direction of directional light to uniform variable
            GLuint loc0 = glGetUniformLocation(m_shader, ("lights["+std::to_string(i)+ "].direction").c_str());
            glUniform4fv(loc0, 1, &metaData.lights[i].dir[0]);
        }
    } // light

    GLint loc1 = glGetUniformLocation(m_shader, "k_a"); // uniform variable in frag
    glUniform1f(loc1, metaData.globalData.ka); // variable in realtime.h

    GLint loc2 = glGetUniformLocation(m_shader, "k_d");
    glUniform1f(loc2, metaData.globalData.kd);

    GLint loc3 = glGetUniformLocation(m_shader, "k_s");
    glUniform1f(loc3, metaData.globalData.ks);

    // loop over objects in the scene. and inside the loop, loop over lights
    // for each shape call paintGL___(). Inside paintGL__(), we bind specfic vao. If paintGLCube(), I bind cube_vao.
    // there's vbo that's linked to vao.Then in paintGLCube(), when I bind cube_vao, I have connection to specific vbo, which is cube_vbo because I linked them
    // in initializeGL.  There's no given or 정해진 coordinates info. m_sphereData = generateSphereData(settings.shapeParameter1, settings.shapeParameter2);
    // I get coordinates when I call generateSphereData with shapeParameter1 and 2.    
    for (auto &shape : metaData.shapes) {

            //extra credit 2
            if(settings.extraCredit2) {
                glm::vec4 objectOriginWorldSpace = shape.ctm * glm::vec4(0,0,0,1);
                float dist = glm::distance(cameraPositionWorldSpace, objectOriginWorldSpace);
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
            // send uniform lights to fragmetn shader and
            if (shape.primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
//                std::cout << "cube" << std::endl;
//                paintGLCube();
                // Task 2: activate the shader program by calling glUseProgram with `m_shader`

                glBindVertexArray(m_cube_vao);         

                //// these depend on objects
                GLint loc4 = glGetUniformLocation(m_shader, "shininess");
                glUniform1f(loc4, shape.primitive.material.shininess);

                GLint loc5 = glGetUniformLocation(m_shader, "o_a");
                glUniform3fv(loc5, 1, &shape.primitive.material.cAmbient[0]);

                GLint loc6 = glGetUniformLocation(m_shader, "o_d");
                glUniform3fv(loc6, 1, &shape.primitive.material.cDiffuse[0]);

                GLint loc7 = glGetUniformLocation(m_shader, "o_s");
                glUniform3fv(loc7, 1, &shape.primitive.material.cSpecular[0]);


                // calculate model matrix
                m_model = shape.ctm;

                // m_view and m_proj don't have to be in this loop?

                // Task 6: pass in m_model as a uniform into the shader program
                // uniformMatrixID is the location of uniform variable "modelMatrix"
                GLuint uniformMatrixID = glGetUniformLocation(m_shader, "modelMatrix");
                glUniformMatrix4fv(uniformMatrixID, 1, GL_FALSE, &m_model[0][0]);
                //// these depend on objects




                // Draw Command..  6 is because 3 for vertex.x, vertex.y, vertex.z and 3 for normal position
                glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 6); // check is it's 3 or 6. # of vertices to draw!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                // Unbind Vertex Array
                glBindVertexArray(0);
                // Task 3: deactivate the shader program by passing 0 into glUseProgram
//                glUseProgram(0);
            }

            else if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
//                paintGLSphere();
//                std::cout << "sphere" << std::endl;
                // Task 2: activate the shader program by calling glUseProgram with `m_shader`
//                glUseProgram(m_shader);

                // Bind Sphere Vertex Data
                glBindVertexArray(m_sphere_vao);

                GLint loc4 = glGetUniformLocation(m_shader, "shininess");
                glUniform1f(loc4, shape.primitive.material.shininess);

                GLint loc5 = glGetUniformLocation(m_shader, "o_a");
                glUniform3fv(loc5, 1, &shape.primitive.material.cAmbient[0]);

                GLint loc6 = glGetUniformLocation(m_shader, "o_d");
                glUniform3fv(loc6, 1, &shape.primitive.material.cDiffuse[0]);

                GLint loc7 = glGetUniformLocation(m_shader, "o_s");
                glUniform3fv(loc7, 1, &shape.primitive.material.cSpecular[0]);





        //        GLint objectShape = glGetUniformLocation(m_shader, )

                // calculate model matrix
                m_model = shape.ctm;

                // m_view and m_proj don't have to be in this loop?
                // calculate view matrix
                // m_view = generateViewMatrix();
                // calculate projection matrix
                // m_proj = generateProjectionMatrix();

                // Task 6: pass in m_model as a uniform into the shader program
                // uniformMatrixID is the location of uniform variable "modelMatrix"
                GLuint uniformMatrixID = glGetUniformLocation(m_shader, "modelMatrix");
                glUniformMatrix4fv(uniformMatrixID, 1, GL_FALSE, &m_model[0][0]);

                // Task 7: pass in m_view and m_proj
//                GLuint uniformViewMatrixID = glGetUniformLocation(m_shader, "viewMatrix");
//                glUniformMatrix4fv(uniformViewMatrixID, 1, GL_FALSE, &m_view[0][0]);

//                GLuint uniformProjectMatrixID = glGetUniformLocation(m_shader, "projectionMatrix");
//                glUniformMatrix4fv(uniformProjectMatrixID, 1, GL_FALSE, &m_proj[0][0]);







//                GLint loc1 = glGetUniformLocation(m_shader, "k_a"); // uniform variable in frag
//                glUniform1f(loc1, metaData.globalData.ka); // variable in realtime.h

//                GLint loc2 = glGetUniformLocation(m_shader, "k_d");
//                glUniform1f(loc2, metaData.globalData.kd);

//                GLint loc3 = glGetUniformLocation(m_shader, "k_s");
//                glUniform1f(loc3, metaData.globalData.ks);

//                // get world space camera position
//                glm::vec4 cameraPositionWorldSpace = inverse(m_view) * glm::vec4(0,0,0,1.f);
//                GLint loc8 = glGetUniformLocation(m_shader, "worldSpaceCameraPosition");
//                glUniform4fv(loc8, 1, &cameraPositionWorldSpace[0]);



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




                GLint loc4 = glGetUniformLocation(m_shader, "shininess");
                glUniform1f(loc4, shape.primitive.material.shininess);

                GLint loc5 = glGetUniformLocation(m_shader, "o_a");
                glUniform3fv(loc5, 1, &shape.primitive.material.cAmbient[0]);

                GLint loc6 = glGetUniformLocation(m_shader, "o_d");
                glUniform3fv(loc6, 1, &shape.primitive.material.cDiffuse[0]);

                GLint loc7 = glGetUniformLocation(m_shader, "o_s");
                glUniform3fv(loc7, 1, &shape.primitive.material.cSpecular[0]);





        //        GLint objectShape = glGetUniformLocation(m_shader, )

                // calculate model matrix
                m_model = shape.ctm;


                // m_view and m_proj don't have to be in this loop?
                // calculate view matrix
                // m_view = generateViewMatrix();
                // calculate projection matrix
                // m_proj = generateProjectionMatrix();

                // Task 6: pass in m_model as a uniform into the shader program
                // uniformMatrixID is the location of uniform variable "modelMatrix"
                GLuint uniformMatrixID = glGetUniformLocation(m_shader, "modelMatrix");
                glUniformMatrix4fv(uniformMatrixID, 1, GL_FALSE, &m_model[0][0]);

                // Task 7: pass in m_view and m_proj
//                GLuint uniformViewMatrixID = glGetUniformLocation(m_shader, "viewMatrix");
//                glUniformMatrix4fv(uniformViewMatrixID, 1, GL_FALSE, &m_view[0][0]);

//                GLuint uniformProjectMatrixID = glGetUniformLocation(m_shader, "projectionMatrix");
//                glUniformMatrix4fv(uniformProjectMatrixID, 1, GL_FALSE, &m_proj[0][0]);



//                GLint loc1 = glGetUniformLocation(m_shader, "k_a"); // uniform variable in frag
//                glUniform1f(loc1, metaData.globalData.ka); // variable in realtime.h

//                GLint loc2 = glGetUniformLocation(m_shader, "k_d");
//                glUniform1f(loc2, metaData.globalData.kd);

//                GLint loc3 = glGetUniformLocation(m_shader, "k_s");
//                glUniform1f(loc3, metaData.globalData.ks);

//                // get world space camera position
//                glm::vec4 cameraPositionWorldSpace = inverse(m_view) * glm::vec4(0,0,0,1.f);
//                GLint loc8 = glGetUniformLocation(m_shader, "worldSpaceCameraPosition");
//                glUniform4fv(loc8, 1, &cameraPositionWorldSpace[0]);

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




                GLint loc4 = glGetUniformLocation(m_shader, "shininess");
                glUniform1f(loc4, shape.primitive.material.shininess);

                GLint loc5 = glGetUniformLocation(m_shader, "o_a");
                glUniform3fv(loc5, 1, &shape.primitive.material.cAmbient[0]);

                GLint loc6 = glGetUniformLocation(m_shader, "o_d");
                glUniform3fv(loc6, 1, &shape.primitive.material.cDiffuse[0]);

                GLint loc7 = glGetUniformLocation(m_shader, "o_s");
                glUniform3fv(loc7, 1, &shape.primitive.material.cSpecular[0]);





        //        GLint objectShape = glGetUniformLocation(m_shader, )

                // calculate model matrix
                m_model = shape.ctm;



                // m_view and m_proj don't have to be in this loop?


                // Task 6: pass in m_model as a uniform into the shader program
                // uniformMatrixID is the location of uniform variable "modelMatrix"
                GLuint uniformMatrixID = glGetUniformLocation(m_shader, "modelMatrix");
                glUniformMatrix4fv(uniformMatrixID, 1, GL_FALSE, &m_model[0][0]);

                // Task 7: pass in m_view and m_proj
//                GLuint uniformViewMatrixID = glGetUniformLocation(m_shader, "viewMatrix");
//                glUniformMatrix4fv(uniformViewMatrixID, 1, GL_FALSE, &m_view[0][0]);

//                GLuint uniformProjectMatrixID = glGetUniformLocation(m_shader, "projectionMatrix");
//                glUniformMatrix4fv(uniformProjectMatrixID, 1, GL_FALSE, &m_proj[0][0]);



//                GLint loc1 = glGetUniformLocation(m_shader, "k_a"); // uniform variable in frag
//                glUniform1f(loc1, metaData.globalData.ka); // variable in realtime.h

//                GLint loc2 = glGetUniformLocation(m_shader, "k_d");
//                glUniform1f(loc2, metaData.globalData.kd);

//                GLint loc3 = glGetUniformLocation(m_shader, "k_s");
//                glUniform1f(loc3, metaData.globalData.ks);

//                // get world space camera position
//                glm::vec4 cameraPositionWorldSpace = inverse(m_view) * glm::vec4(0,0,0,1.f);
//                GLint loc8 = glGetUniformLocation(m_shader, "worldSpaceCameraPosition");
//                glUniform4fv(loc8, 1, &cameraPositionWorldSpace[0]);

                // Draw Command..  6 is because 3 for vertex.x, vertex.y, vertex.z and 3 for normal position
                glDrawArrays(GL_TRIANGLES, 0, m_cylinderData.size() / 6); // check is it's 3 or 6. # of vertices to draw!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                // Unbind Vertex Array
                glBindVertexArray(0);
                // Task 3: deactivate the shader program by passing 0 into glUseProgram
//                glUseProgram(0);
            }


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

//void Realtime::unbindProgram() {
//    glUseProgram(0);
//}

//void Realtime::unbindVBOVAO() {
//    glBindVertexArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//}

glm::mat4 Realtime::generateProjectionMatrix() {
    // need to use theta_width and theta height (or aspect ratio?)
    // aspect ratio = width / height
    float heightAngle = metaData.cameraData.heightAngle; // in radians
    float ratio = windowWidth / windowHeight;
    float widthAngle = heightAngle * (1/ratio);
    widthAngle = 2*atan(windowWidth / windowHeight * tan(heightAngle/2.f));
//    float widthAngle =
//    float widthAngle =
    // if I have width and height, I don't need angles
    glm::mat4 mR = glm::mat4(1.f/(settings.farPlane*(windowWidth/windowHeight * tan(heightAngle/2.f))), 0,0,0,//this is a column
                             0, 1.f/(settings.farPlane*tan(heightAngle/2.f)), 0, 0,
                             0, 0, 1.f/settings.farPlane, 0,
                             0, 0, 0, 1);


//    glm::mat4 mR(1.f/ (windowWidth/2.f), 0,0,0,//this is a column
//                  0, 1.f/windowHeight/2.f, 0, 0,
//                  0, 0, 1.f/settings.farPlane, 0,
//                  0, 0, 0, 1);

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

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    update(); // asks for a PaintGL() call to occur
}
