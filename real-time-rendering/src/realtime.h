#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
//#include "utils/shaderloader.h"
#include "utils/sceneparser.h"


class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();

    glm::mat4 generateViewMatrix();                     // my function
    glm::mat4 generateProjectionMatrix();

    // functions in paintGL()
    void paintGLSphere();
    void paintGLCube();
    void paintGLCone();
    void paintGLCylinder();


    void setFragVert();

    // functions in initializeGL()
    void initializeCone(int param1, int param2);
    void initializeCylinder(int param1, int param2);
    void initializeCube(int param1);
    void initializeSphere(int param1, int param2);

    std::vector<float> generateSphereData(int phiTesselations, int thetaTesselations);
    glm::vec4 sphericalToCartesian(float phi, float theta);
    void pushVec3(glm::vec4 vec, std::vector<float> *data);
public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    // my variables
    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view = glm::mat4(1);
    glm::mat4 m_proj = glm::mat4(1);

    GLuint m_shader;

    // std::vector<GLuint> vbos{m_sphere_vbo, m_cube_vbo, m_cone_vbo, m_cylinder_vbo};
    // std::vector<GLuint> vaos{m_sphere_vao, m_cube_vao, m_cone_vao, m_cylinder_vao};

    GLuint m_sphere_vbo;
    GLuint m_cube_vbo;
    GLuint m_cone_vbo;
    GLuint m_cylinder_vbo;

    GLuint m_sphere_vao;
    GLuint m_cube_vao;
    GLuint m_cone_vao;
    GLuint m_cylinder_vao;

    std::vector<float> m_sphereData;
    std::vector<float> m_cubeData;
    std::vector<float> m_cylinderData;
    std::vector<float> m_coneData;

    RenderData metaData;

    // light related
    glm::vec4 m_lightPos; // world space
    float m_ka;
    float m_kd;
    float m_ks;
    float m_shininess;

    float windowWidth;
    float windowHeight;

    bool isInitializedRun = false;

    float originalParam1;
    float originalParam2;
    bool click = false;

};
