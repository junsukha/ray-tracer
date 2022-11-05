#include "bvhtree.h"

BVHTree::BVHTree(RenderData& inputScene) {
    scene = inputScene;
    shapes =  scene.shapes;

    FindBBox();


}


void BVHTree::FindBBox() {
    int idx = 0;
    for (auto &shape : shapes) {
        glm::vec4 ObjectSpaceminX{-0.5, 0, 0, 1};
        glm::vec4 ObjectSpaceminY{0, -0.5, 0, 1};
        glm::vec4 ObjectSpaceminZ{0, 0, -0.5, 1};

        glm::vec4 ObjectSpacemaxX{0.5, 0, 0, 1};
        glm::vec4 ObjectSpacemaxY{0, 0.5, 0, 1};
        glm::vec4 ObjectSpacemaxZ{0, 0, 0.5, 1};

        glm::vec4 vertex1 = shape.ctm * ObjectSpaceminX;
        glm::vec4 vertex2 = shape.ctm * ObjectSpaceminY;
        glm::vec4 vertex3 = shape.ctm * ObjectSpaceminZ;

        glm::vec4 vertex4 = shape.ctm * ObjectSpacemaxX;
        glm::vec4 vertex5 = shape.ctm * ObjectSpacemaxY;
        glm::vec4 vertex6 = shape.ctm * ObjectSpacemaxZ;

        // find min and max of x, y, z from six vertices
        float minX = INFINITY;
        float maxX = -INFINITY;
        float minY = INFINITY;
        float maxY = -INFINITY;
        float minZ = INFINITY;
        float maxZ = -INFINITY;

        int numOfVertices = 6;
        int numOfCoord = 3;

        for (int j = 0; j < numOfCoord; j++) {
                if(vertex1[j] < minX)
                    minX = vertex1[j];
                if(vertex2[j] < minX)
                    minX = vertex2[j];
                if(vertex3[j] < minX)
                    minX = vertex3[j];
                if(vertex4[j] < minX)
                    minX = vertex4[j];
                if(vertex5[j] < minX)
                    minX = vertex5[j];
                if(vertex6[j] < minX)
                    minX = vertex6[j];

                if(vertex1[j] > maxX)
                    maxX = vertex1[j];
                if(vertex2[j] > maxX)
                    maxX = vertex2[j];
                if(vertex3[j] > maxX)
                    maxX = vertex3[j];
                if(vertex4[j] > maxX)
                    maxX = vertex4[j];
                if(vertex5[j] > maxX)
                    maxX = vertex5[j];
                if(vertex6[j] > maxX)
                    maxX = vertex6[j];

        }

        for (int j = 0; j < numOfCoord; j++) {
                if(vertex1[j] < minY)
                    minY = vertex1[j];
                if(vertex2[j] < minY)
                    minY = vertex2[j];
                if(vertex3[j] < minY)
                    minY = vertex3[j];
                if(vertex4[j] < minY)
                    minY = vertex4[j];
                if(vertex5[j] < minY)
                    minY = vertex5[j];
                if(vertex6[j] < minY)
                    minY = vertex6[j];

                if(vertex1[j] > maxY)
                    maxY = vertex1[j];
                if(vertex2[j] > maxY)
                    maxY = vertex2[j];
                if(vertex3[j] > maxY)
                    maxY = vertex3[j];
                if(vertex4[j] > maxY)
                    maxY = vertex4[j];
                if(vertex5[j] > maxY)
                    maxY = vertex5[j];
                if(vertex6[j] > maxY)
                    maxY = vertex6[j];

        }

        for (int j = 0; j < numOfCoord; j++) {
                if(vertex1[j] < minZ)
                    minZ = vertex1[j];
                if(vertex2[j] < minZ)
                    minZ = vertex2[j];
                if(vertex3[j] < minZ)
                    minZ = vertex3[j];
                if(vertex4[j] < minZ)
                    minZ = vertex4[j];
                if(vertex5[j] < minZ)
                    minZ = vertex5[j];
                if(vertex6[j] < minY)
                    minZ = vertex6[j];

                if(vertex1[j] > maxZ)
                    maxZ = vertex1[j];
                if(vertex2[j] > maxZ)
                    maxZ = vertex2[j];
                if(vertex3[j] > maxZ)
                    maxZ = vertex3[j];
                if(vertex4[j] > maxZ)
                    maxZ = vertex4[j];
                if(vertex5[j] > maxZ)
                    maxZ = vertex5[j];
                if(vertex6[j] > maxZ)
                    maxZ = vertex6[j];
        }

        glm::vec4 WorldSpaceminX = {minX, 0, 0, 1};
        glm::vec4 WorldSpaceminY = {0, minY, 0, 1};
        glm::vec4 WorldSpaceminZ = {0, 0, minZ, 1};

        glm::vec4 WorldSpacemaxX = {maxX, 0, 0, 1};
        glm::vec4 WorldSpacemaxY = {0, maxY, 0, 1};
        glm::vec4 WorldSpacemaxZ = {0, 0, maxZ, 1};

        BVHNode node{idx, minX, maxX, minY, maxY, minZ, maxZ};
        bvhNode.push_back(node);
        idx++;
    }

    for (int i = 0; i < bvhNode.size(); i++) {
        float boxminX = INFINITY;
        float boxminY = INFINITY;
        float boxminZ = INFINITY;
        float boxmaxX = -INFINITY;
        float boxmaxY = -INFINITY;
        float boxmaxZ = -INFINITY;

        if(bvhNode[i].aabbMinX < boxminX)
            boxminX = bvhNode[i].aabbMinX;
        if(bvhNode[i].aabbMinY < boxminY)
            boxminY = bvhNode[i].aabbMinY;
        if(bvhNode[i].aabbMinZ < boxminY)
            boxminZ = bvhNode[i].aabbMinZ;

        if(bvhNode[i].aabbMaxX < boxmaxX)
            boxmaxX = bvhNode[i].aabbMaxX;
        if(bvhNode[i].aabbMaxY < boxmaxY)
            boxmaxY = bvhNode[i].aabbMaxY;
        if(bvhNode[i].aabbMinY < boxminY)
            boxmaxZ = bvhNode[i].aabbMaxZ;
    }

}
