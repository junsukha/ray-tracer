#pragma once

#include <glm/glm.hpp>
#include "utils/sceneparser.h"
#include <tuple>
class IntersectionPoint {
public:
    IntersectionPoint();
    IntersectionPoint(glm::vec4 newIntersectionPoint, bool doesIntersect);

    glm::vec4 getIntersectionPoint();
    bool getHasIntersectionPoint();
    glm::vec4 getNormalInObjectSpace();
    float getTValue();
    glm::mat4 getCTM();
    RenderShapeData getShape();
    int getShapeIndex();
    std::tuple<float,float> getUV();
    float getU();
    float getV();
    glm::mat4 getInverseCTM();

    void setIntersectionPoint(const glm::vec4 &newIntersectionPoint);
    void setHasIntersectionPoint(bool newHasIntersectionPoint);
    void setNormalInObjectSpace(const glm::vec4 &newNormal);    
    void setTValue(const float t);    
    void setCTM(glm::mat4 ctm);
    void setInverseCTM(glm::mat4 inverseCTM);
    void setShape(const RenderShapeData CurrentShape);
    void setShapeIndex(const int index);
    void setUV(const std::tuple<float,float> inputUV);
    void setUVFloat(float u, float v);
private:
    glm::vec4 intersectionPoint;
    bool hasIntersectionPoint;
    glm::vec4 normalInObjectSpace;
    float tValue;
    glm::mat4 ctm;
    glm::mat4 inverseCTM;
    RenderShapeData shape;
    int shapeIndex;
    std::tuple<float, float> uv;
    float u;
    float v;
};
