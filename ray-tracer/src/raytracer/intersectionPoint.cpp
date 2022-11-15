#include "intersectionPoint.h"

IntersectionPoint::IntersectionPoint(){
    glm::vec4 intersectionPoint;
    bool hasIntersectionPoint;
    float tValue;
}

IntersectionPoint::IntersectionPoint(glm::vec4 newIntersectionPoint, bool doesIntersect) : intersectionPoint(newIntersectionPoint), hasIntersectionPoint(doesIntersect) {
//    intersectionPoint = newIntersectionPoint;
//    hasIntersectionPoint = doesIntersect;
}

glm::vec4 IntersectionPoint::getIntersectionPoint() {
    return intersectionPoint;
}

bool IntersectionPoint::getHasIntersectionPoint() {
    return hasIntersectionPoint;
}

void IntersectionPoint::setIntersectionPoint(const glm::vec4 &newIntersectionPoint)
{
    intersectionPoint = newIntersectionPoint;
}

void IntersectionPoint::setHasIntersectionPoint(bool newHasIntersectionPoint)
{
    hasIntersectionPoint = newHasIntersectionPoint;
}

void IntersectionPoint::setNormalInObjectSpace(const glm::vec4 &newNormal)
{
    normalInObjectSpace = newNormal;
}

glm::vec4 IntersectionPoint::getNormalInObjectSpace()
{
    return normalInObjectSpace;
}

void IntersectionPoint::setTValue(const float t)
{
    tValue = t;
}

float IntersectionPoint::getTValue()
{
    return tValue;
}

void IntersectionPoint::setCTM(glm::mat4 inputCtm)
{
    ctm = inputCtm;
}

void IntersectionPoint::setInverseCTM(glm::mat4 inputInverseCTM)
{
    inverseCTM = inputInverseCTM;
}

void IntersectionPoint::setShape(const RenderShapeData currentShape)
{
    shape = currentShape;
}

void IntersectionPoint::setShapeIndex(const int index)
{
    shapeIndex = index;
}

void IntersectionPoint::setUV(std::tuple<float, float> inputUV)
{
    uv = inputUV;
}

void IntersectionPoint::setUVFloat(float inputU, float inputV)
{
    u = inputU;
    v = inputV;
}



glm::mat4 IntersectionPoint::getCTM()
{
    return ctm;
}

RenderShapeData IntersectionPoint::getShape()
{
    return shape;
}

int IntersectionPoint::getShapeIndex()
{
    return shapeIndex;
}

std::tuple<float, float> IntersectionPoint::getUV()
{
    return uv;
}

float IntersectionPoint::getU()
{
    return u;
}

float IntersectionPoint::getV()
{
    return v;
}

glm::mat4 IntersectionPoint::getInverseCTM()
{
    return inverseCTM;
}
