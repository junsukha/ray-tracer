#include <stdexcept>
#include "camera.h"


//Camera::Camera(SceneCameraData cameraData, int width , int height) {
//    pos = cameraData.pos;
//    look = cameraData.look;
//    up = cameraData.up;

//    heightAngle = cameraData.heightAngle;

//    aperture = cameraData.aperture;
//    focalLength = cameraData.focalLength;

//    imageWidth = width;
//    imageHeight = height;
//}

glm::vec4 Camera::getPos() const {
    return pos_;
}

void Camera::setViewMatrix() {
    // Optional TODO: implement the getter or make your own design
//    throw std::runtime_error("not implemented");
    glm::vec3 vec3Look = glm::vec3(look_);
    glm::vec3 vec3Up = glm::vec3(up_);

    glm::vec3 w = -glm::normalize(vec3Look);
    glm::vec3 v = glm::normalize(vec3Up - glm::dot(vec3Up, w)*w);

    glm::vec3 u = glm::cross(v, w);

    glm::mat4 R = glm::transpose(glm::mat4{glm::vec4{u,0.f}, glm::vec4{v, 0.f}, glm::vec4{w,0.f}, glm::vec4{0.f,0,0,1}});

    glm::mat4 T{glm::vec4{1.f,0,0,0},
                glm::vec4{0.f,1,0,0},
                glm::vec4{0.f,0,1,0},
                glm::vec4{glm::vec3{-pos_}, 1}};

//    glm::vec4 firstCol = glm::vec4(u, -pos_[0]);
//    glm::vec4 secondCol = glm::vec4(v, -pos_[1]);
//    glm::vec4 thirdCol = glm::vec4(w, -pos_[2]);
//    glm::vec4 fourthCol = glm::vec4(0.f, 0, 0, 1);

//    auto viewMatrix = glm::transpose(glm::mat4(firstCol, secondCol, thirdCol, fourthCol));

    viewMatrix = R * T;

}
glm::mat4 Camera::getViewMatrix() const {
//    // Optional TODO: implement the getter or make your own design
////    throw std::runtime_error("not implemented");
//    glm::vec3 vec3Look = glm::vec3(look_);
//    glm::vec3 vec3Up = glm::vec3(up_);

//    glm::vec3 w = -glm::normalize(vec3Look);
//    glm::vec3 v = glm::normalize(vec3Up - glm::dot(vec3Up, w)*w);

//    glm::vec3 u = glm::cross(v, w);

//    glm::mat4 R = glm::transpose(glm::mat4{glm::vec4{u,0.f}, glm::vec4{v, 0.f}, glm::vec4{w,0.f}, glm::vec4{0.f,0,0,1}});

//    glm::mat4 T{glm::vec4{1.f,0,0,0},
//                glm::vec4{0.f,1,0,0},
//                glm::vec4{0.f,0,1,0},
//                glm::vec4{glm::vec3{-pos_}, 1}};

////    glm::vec4 firstCol = glm::vec4(u, -pos_[0]);
////    glm::vec4 secondCol = glm::vec4(v, -pos_[1]);
////    glm::vec4 thirdCol = glm::vec4(w, -pos_[2]);
////    glm::vec4 fourthCol = glm::vec4(0.f, 0, 0, 1);

////    auto viewMatrix = glm::transpose(glm::mat4(firstCol, secondCol, thirdCol, fourthCol));

//    glm::mat4 viewMatrix = R * T;

    return viewMatrix;
}

glm::mat4 Camera::getInverseViewMatrix() const {
//    auto viewMatrix = getViewMatrix();
//    return glm::inverse(viewMatrix);
    return inverseViewMatrix;
}

void Camera::setInverseViewMatrix()
{
   inverseViewMatrix = glm::inverse(viewMatrix);
}

float Camera::getAspectRatio() const {
    // Optional TODO: implement the getter or make your own design
    return aspectRatio_;
//    throw std::runtime_error("not implemented");
}

float Camera::getHeightAngle() const {
    // Optional TODO: implement the getter or make your own design
    return heightAngle_;
    //    throw std::runtime_error("not implemented");
}

float Camera::getFocalLength() const {
    // Optional TODO: implement the getter or make your own design
    return focalLength_;
//    throw std::runtime_error("not implemented");
}

float Camera::getAperture() const {
    // Optional TODO: implement the getter or make your own design
    return aperture_;
//    throw std::runtime_error("not implemented");
}
