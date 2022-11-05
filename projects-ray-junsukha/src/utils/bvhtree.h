#pragma once
#include "sceneparser.h"


struct BVHNode
{
    int idx;
    float aabbMinX, aabbMaxX, aabbMinY, aabbMaxY, aabbMinZ, aabbMaxZ;
    int leftNode, firstTriIdx, triCount;
    bool isLeaf() { return triCount > 0; }
};

class BVHTree {

private:
    RenderData scene;
    std::vector<RenderShapeData> shapes;
//    glm::vec4 WorldSpaceminX;
//    glm::vec4 WorldSpaceminY;
//    glm::vec4 WorldSpaceminZ;

//    glm::vec4 WorldSpacemaxX;
//    glm::vec4 WorldSpacemaxY;
//    glm::vec4 WorldSpacemaxZ;
    std::vector<BVHNode> bvhNode;
public:
    BVHTree(RenderData& scene);
    void FindBBox();

};
