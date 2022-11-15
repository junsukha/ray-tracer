#include "phong.h"
#include <cmath>
#include <iostream>

// This file contains the phong function you need to fill in.
// This should be the ONLY file you modify.

// Helper function to convert illumination to RGBA, applying some form of tone-mapping (e.g. clamping) in the process
RGBA toRGBA(const glm::vec4 &illumination) {
    // Task 1
    RGBA rgba{0,0,0};
    float a;
    std::vector<uint8_t> temp(4);
    for (int i = 0; i < 3; i++) {

        a = uint8_t(255*glm::min(glm::max(illumination[i], 0.f), 1.f));
        temp[i] = a;

     }
//     glm::min(glm::max(illumination, glm::vec4(0,0,0,0)),           glm::vec4(1,1,1,1));
    rgba = RGBA{temp[0], temp[1], temp[2]};
//    std::cout <<temp[0];
    return rgba;
//    return RGBA{0, 0, 0};
}

// Calculates the RGBA of a pixel from intersection infomation and globally-defined coefficients
RGBA phong(glm::vec3  position,
           glm::vec3  normal,
           glm::vec3  directionToCamera, // in world space?
           SceneMaterial  &material,
           std::vector<SceneLightData> &lights
           /*Sampler   &reflectionSampler*/) {
    // Normalizing directions
    normal            = glm::normalize(normal);
    directionToCamera = glm::normalize(directionToCamera);

    // Output illumination (we can ignore opacity)
    glm::vec4 illumination(0, 0, 0, 1);

    // Task 3: add the ambient term
    int rgb = 3;
    for (int i = 0; i < rgb; i++) {
        illumination[i] += ka * material.ambient[i]; // ambient is vec4
    }

    float fAtt; //was 0.f
    float dotProduct;
    glm::vec3 pointToLight; // normalized direction from point to light
    for (const LightInfo &light : lights) {
        // Task 6: compute the attenuation factor
        float distanceLightToPoint = glm::distance(position, light.source);
        fAtt = glm::min(1.0, 1.0 / (c1 + distanceLightToPoint * c2 + pow(distanceLightToPoint,2) * c3));

        // Task 4, task 6: add the diffuse term
        // normalize direction
        pointToLight = glm::normalize(light.source - position); // from position to light source vector

        dotProduct = glm::dot(normal, pointToLight);
        if (dotProduct < 0) { // normal vector is facing away from light source
            dotProduct = 0;
        }

        for (int i = 0; i < rgb; i++){
            illumination[i] += fAtt *
                    light.color[i] *
                    kd *
                    material.diffuse[i] *
                    dotProduct;

        }

        // Task 5, task 6: add the specular term
        // get reflecedDir: reflected direction of incoming light about surface normal

        // normalize direction
        glm::vec3 positionToLight = glm::normalize(light.source - position); // from light source to position vector
        float specDotProduct = glm::dot(positionToLight, normal);
        // if normal vector is facing away l, no light relfect
        if (specDotProduct < 0)
            specDotProduct = 0;

        glm::vec3 reflectedDir = positionToLight - 2 * specDotProduct * normal;

        glm::vec3 reflectedDirNormalized = glm::normalize(reflectedDir);

        float RV = glm::dot(reflectedDirNormalized,directionToCamera);
        for (int i = 0; i < rgb; i++) {
            illumination[i] += fAtt *
                    light.color[i] *
                    ks *
                    material.specular[i] *
                    pow(RV, material.shininess);
        }
    }

    // Task 7: add reflection

    // Use Sampler::getReflection(glm::vec3 startPosition, glm::vec3 lightDirection)
    // to get the reflection intensity when "recursively raytracing" in some direction from some position
    illumination +=  kr * reflectionSampler.getReflection(
                position,

                2 *
                glm::dot(normal,directionToCamera) *
                normal -
                directionToCamera);

    RGBA returnValue = toRGBA(illumination);
    return returnValue;
}
