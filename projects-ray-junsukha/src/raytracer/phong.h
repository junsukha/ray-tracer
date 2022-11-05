#pragma once

#include <glm/glm.hpp>
#include <vector>

//#include "param.h"
//#include "reflectionsampler.h"
#include "../utils/rgba.h"
#include "../utils/scenedata.h"


RGBA phong(glm::vec3  position,
           glm::vec3  normal,
           glm::vec3  directionToCamera,
           SceneMaterial  &material,
           std::vector<SceneLightData> &lights
           /*Sampler   &reflectionSampler*/);
