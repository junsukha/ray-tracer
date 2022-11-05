#pragma once

#include <glm/glm.hpp>
#include "utils/rgba.h"
//#include "../utils/sceneparser.h"
#include "intersectionPoint.h"
#include <tuple>
#include "raytracescene.h"
//#include "../utils/bvhtree.h"
#include "texturemap.h"

// A forward declaration for the RaytraceScene class

class RayTraceScene;

// A class representing a ray-tracer

class RayTracer
{
public:
    struct Config {
        bool enableShadow        = false;
        bool enableReflection    = false;
        bool enableRefraction    = false;
        bool enableTextureMap    = false;
        bool enableTextureFilter = false;
        bool enableParallelism   = false;
        bool enableSuperSample   = false;
        bool enableAcceleration  = false;
        bool enableDepthOfField  = false;
        bool enableSoftShadow    = false;
    };

public:
    RayTracer(Config config);

    // Renders the scene synchronously.
    // The ray-tracer will render the scene and fill imageData in-place.
    // @param imageData The pointer to the imageData to be filled.
    // @param scene The scene to be rendered.
    void render(RGBA *imageData, const RayTraceScene &scene);

    /**
     * @brief getX calculates x coordinate in view plane corresponding to j coordinate in image
     * @param j
     * @param imageWidth
     * @return
     */
    float getX(auto j, auto imageWidth);

    /**
     * @brief getY
     * @param i
     * @param imageHeight
     * @return
     */
    float getY(auto i, auto imageHeight);

    glm::vec4 fromCameraToWorld(glm::vec3 rayDirInCameraSpace, glm::mat4 InverseViewMatrix);

    IntersectionPoint getIntersection(RenderShapeData &shape, glm::vec4 cameraPosition, glm::vec4 rayDirInWorldSpace);

    /**
     * @brief getIntersectionOfCube
     * @param shape
     * @param cameraPosition
     * @param rayDirInWorldSpace
     * @return intersection point, if any, in object space
     */
    IntersectionPoint getIntersectionOfCube(RenderShapeData &shape, glm::vec4 cameraPosition, glm::vec4 rayDirInWorldSpace);

    /**
     * @brief getIntersectionOfSphere
     * @param shape
     * @param cameraPosition
     * @param rayDirInWorldspace
     * @return intersection point, if any, in object space
     */
    IntersectionPoint getIntersectionOfSphere(RenderShapeData &shape, glm::vec4 cameraPosition, glm::vec4 rayDirInWorldspace);

    bool checkYRange(glm::vec4 intersection);

    /**
     * @brief getIntersectionOfCylinder
     * @param shape
     * @param cameraPosition
     * @param rayDirInWorldSpace
     * @return intersection point, if any, in object space
     */
    IntersectionPoint getIntersectionOfCylinder(RenderShapeData &shape, glm::vec4 cameraPosition, glm::vec4 rayDirInWorldSpace);

    /**
     * @brief loopShapes
     * @param rayDirInWorldSpace
     * @param shapes
     * @param cameraPosition
     * @return intersection points in this vector are in object space
     */
    std::vector<IntersectionPoint> loopShapes(glm::vec4 rayDirInWorldSpace, std::vector<RenderShapeData> shapes, glm::vec4 cameraPosition);

    /**
     * @brief getIntersectionOfCone
     * @param shape
     * @param cameraPosition
     * @param rayDirInWorldSpace
     * @return intersection point, if any, in object space
     */
    IntersectionPoint getIntersectionOfCone(RenderShapeData &shape, glm::vec4 cameraPosition, glm::vec4 rayDirInWorldSpace);

    /**
     * @brief chooseInterPoint
     * @param possibleTs
     * @param cameraPositionObjectSpace
     * @param rayDirInObjectSpace
     * @return
     */
    IntersectionPoint chooseInterPoint(std::vector<std::tuple<float, glm::vec4>> possibleTs, glm::vec4 cameraPositionObjectSpace, glm::vec4 rayDirInObjectSpace);
    std::tuple<int, IntersectionPoint> findTValueAmongAllObjects(std::vector<IntersectionPoint> intersectionPoints);

    /**
     * @brief phong
     * @param position
     * @param normal
     * @param directionToCamera
     * @param material
     * @param lights
     * @param globalData
     * @param shapes
     * @return
     */
    glm::vec4 phong(glm::vec3  position, // I think this is intersection position in world space
                           glm::vec3  normal,
                           glm::vec3  directionToCamera, // from intersection to camera
                           SceneMaterial  &material,
                           std::vector<SceneLightData> &lights,
                           const SceneGlobalData &globalData,
                           const std::vector<RenderShapeData> &shapes,
                           IntersectionPoint intersectionpoint,
                            int &maxRecursion
               /*Sampler   &reflectionSampler*/);


    float calculateFallOff(SceneLightData &light, float x);

    SceneColor calculateSpotLightIntensity(glm::vec3 position, SceneLightData light);

    float checkShadow(const SceneLightData &light, glm::vec3 intersectionPositionWorldSpace, const std::vector<RenderShapeData> &shapes, IntersectionPoint intersectionPoint);
    glm::vec4 findReflectionIllumination(int maxRecursion, std::tuple<int, IntersectionPoint> currentIntersectionpointTuple, glm::vec3 incomingRay);

    std::vector<TextureMap> saveTextureMaps(const std::vector<RenderShapeData> &shapes);

    /**
     * @brief findTextureColor finds a RGBA of pixel at corresponding location given intersectionPoint
     * @param intersectionPoint
     * @return RGBA value
     */
    glm::vec4 findTextureColor (IntersectionPoint intersectionPoint);

    /**
     * @brief calculateSphereUV calculates u,v coordinate of sphere given intersection point
     * @param intersection is intersection coordinate in object space
     * @return (u,v) coordinate
     */
    std::tuple<float, float> calculateSphereUV(glm::vec4 &intersection);

    /**
     * @brief calculateConeUV calculates u,v coordinate of cone given intersection point
     * @param intersection is intersection coordinate in object space
     * @return (u,v) coordinate
     */
    std::tuple<float, float> calculateConeUV(glm::vec4 &intersection);
    std::tuple<float, float> calculateCylinderUV(glm::vec4 &intersection);
    IntersectionPoint findNextIntersectionPoint(glm::vec4 incomingRayInWorldspace, glm::vec4 startingPositionInWorldSpace);

    /**
     * @brief calculateFristRefractedRay
     * @param incomingRay
     * @param intersectionpoint
     * @return
     */
    glm::vec4 calculateFristRefractedRay(glm::vec4 incomingRay, IntersectionPoint intersectionpoint);

    /**
     * @brief calculateSecondRefractedRay calculates outgoing refracted ray using opposite normal as it's inside the shape
     * @param incomingRay
     * @param intersectionpoint
     * @return
     */
    glm::vec4 calculateSecondRefractedRay(glm::vec4 incomingRay, IntersectionPoint intersectionpoint);

    glm::vec4 calculateRefractionIllumination(IntersectionPoint intersectionpoint, glm::vec3 directionToCamera, glm::vec3 position, glm::vec4 illumination, int &maxRecursion);
    glm::vec4 calculateReflectionIllumination(IntersectionPoint intersectionpoint, int &maxRecursion, glm::vec3 directionToCamera);
    SceneLightData makeMyLight();
    float softShadow(SceneLightData myLight, glm::vec3 intersectionPositionWorldSpace, const std::vector<RenderShapeData> &shapes, IntersectionPoint intersectionPoint);
    bool checkColorDifference(RGBA &color1, RGBA &color2);
private:
    const Config m_config;
    SceneGlobalData globalVarGlobalData;
    std::vector<RenderShapeData> globalVarShapes;
    glm::vec4 camersPosition;
    std::vector<SceneLightData> lights;
    std::vector<RenderShapeData> shapes;

    std::vector<TextureMap> textureMaps;
//    RayTraceScene scene_;
};

