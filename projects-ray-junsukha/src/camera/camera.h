#pragma once

#include <glm/glm.hpp>

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:
//    Camera(SceneCameraData cameraData, int imageWidth, int imageHeight);
    Camera(glm::vec4 pos, glm::vec4 look, glm::vec4 up, float heightAngle, float aperture, float focalLength, float aspectRatio) :
    pos_(pos),
    look_(look),
    up_(up),
    heightAngle_(heightAngle),
    aperture_(aperture),
    focalLength_(focalLength),
    aspectRatio_(aspectRatio){}

    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;

    glm::mat4 getInverseViewMatrix() const;

    void setInverseViewMatrix();
    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    // use this to get width of view plane. how? 1. get height of view plane using height angle. Then multiply height by aspect ratio (width/height) to get width
    float getHeightAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;

    glm::vec4 getPos() const;

    void setViewMatrix();
private:
    glm::vec4 pos_;
    glm::vec4 look_;
    glm::vec4 up_;
    float heightAngle_;
    float aperture_;
    float focalLength_;
    float aspectRatio_;// width / height
    glm::mat4 inverseViewMatrix;
    glm::mat4 viewMatrix;
};
