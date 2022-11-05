#include "raytracer.h"
#include <iostream>

RayTracer::RayTracer(Config config) :
    m_config(config)
{}

// this is used when sorting possibleTs vector
bool mycompare (const std::tuple<int, glm::vec4> &lhs, const std::tuple<int, glm::vec4> &rhs){
  return get<0>(lhs) < get<0>(rhs);
}

// this calculates normal vector of conical Top equation at intersectionPoint
auto conicalTopNormal = [](glm::vec4 intersectionPoint) {
    float norX = 2 * intersectionPoint.x;
    float norY = (0.5 - intersectionPoint.y) / 2.f;
    float norZ = 2 * intersectionPoint.z;

    // normalize normal vector
    return glm::normalize(glm::vec4{norX, norY, norZ, 0});
};

auto finiteCylinderNormal = [](glm::vec4 intersectionPoint) {
    float norX = 2 * intersectionPoint.x;
    float norY = 0;
    float norZ = 2 * intersectionPoint.z;

    return glm::normalize(glm::vec4{norX, norY, norZ, 0});
};

auto sphereNormal = [](glm::vec4 intersectionPoint) {
    float norX = 2 * intersectionPoint.x;
    float norY = 2 * intersectionPoint.y;
    float norZ = 2 * intersectionPoint.z;

    return glm::normalize(glm::vec4{norX, norY, norZ, 0});
};

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

    return rgba;

}


std::vector<TextureMap> RayTracer::saveTextureMaps(const std::vector<RenderShapeData> &shapes)
{
    for (int i = 0; i < shapes.size(); i++) {
        TextureMap textureMap;
        QString filePath = QString::fromStdString(shapes[i].primitive.material.textureMap.filename);
        textureMap.loadImageFromFile(filePath);
        textureMaps.push_back(textureMap);
    }
    return textureMaps;
}

void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {
    // Note that we're passing `data` as a pointer (to its first element)
    // Recall from Lab 1 that you can access its elements like this: `data[i]`
//    BVHTree bvhTree(scene);


    // TODO: Implement the ray tracing algorithm. Good luck!
    auto k = 1.f;
    SceneCameraData cameraData = scene.getCameraData();

    Camera camera(cameraData.pos,
                  cameraData.look,
                  cameraData.up,
                  cameraData.heightAngle,
                  cameraData.aperture,
                  cameraData.focalLength,
                  scene.width()/float(scene.height()));

    camera.setViewMatrix();
    camera.setInverseViewMatrix();
    // global variables. will be used for recursive reflection
    globalVarGlobalData = scene.getGlobalData();
    globalVarShapes = scene.getShapes();
    camersPosition = camera.getPos();
    lights = scene.getLights();
    shapes = scene.getShapes();

    if(m_config.enableTextureMap) {
        // std::vector<TextureMap> textureMaps; is a global variable
        textureMaps = saveTextureMaps(shapes);
    }
    auto cameraHeightAngleRadian = camera.getHeightAngle();
    auto aspectRatio = camera.getAspectRatio();
    auto scaleFactorV = 2 * k * tan(cameraHeightAngleRadian / // convert to degree
                                    2.0);
    auto scaleFactorU = scaleFactorV * aspectRatio; //aspectRatio = width / height

    auto x = 0.f;
    auto y = 0.f;

    glm::vec3 uvk; // coordinate on view plane
    glm::vec3 eye;
    glm::vec3 cameraToPlaneDir;

    int imageHeight = scene.height();
    int imageWidth = scene.width();

    // for super sampling
    std::vector<RGBA> colors;
    std::vector<std::tuple<float,float>> coords;
    bool isDifferenceBig = false;

    glm::vec3 rayDirInCameraSpace;
    glm::vec4 rayDirInWorldSpace;
    // for each pixel on image, find corresponding position in view plane,
    // and shoot ray through that position. Find ray direction of course.
    // Then find an intersection point.
    for (int j = 0; j < imageWidth; j++) {
        for (int i = 0; i < imageHeight; i++) {            
again:
            coords.clear();
            float xCenter = getX(j, float(imageWidth));
            float yCenter = getY(i, float(imageHeight));

            coords.push_back(std::tuple<float,float>(xCenter,yCenter));

            RGBA color;

            if(m_config.enableSuperSample) {
                // pick four corners of current (u,v). And shoot ray. If those four point colors are significantly diffrent,
                // divide the pixel into four quadrants and should 4 rays for each small box. and average the color of those 4 rays
                // and then average the colors from each small box to make one original pixel color.
                float xLeftTop = getX(j-0.5, float(imageWidth));
                float yLeftTop = getY(i-0.5, float(imageHeight));

                float xRightTop = getX(j+0.5, float(imageWidth));
                float yRightTop = getY(i-0.5, float(imageHeight));

                float xLeftBot = getX(j-0.5, float(imageWidth));
                float yLeftBot = getY(i+0.5, float(imageHeight));

                float xRightBot = getX(j+0.5, float(imageWidth));
                float yRightBot = getY(i+0.5, float(imageHeight));

                coords.push_back(std::tuple<float,float>(xLeftTop,yLeftTop));
                coords.push_back(std::tuple<float,float>(xRightTop,yRightTop));
                coords.push_back(std::tuple<float,float>(xLeftBot,yLeftBot));
                coords.push_back(std::tuple<float,float>(xRightBot,yRightBot));

                if (isDifferenceBig) {
                    coords.push_back(std::tuple<float, float>(xCenter, yLeftTop)); // yLefttop == yCenterTop
                    coords.push_back(std::tuple<float, float>(xLeftTop, yCenter));
                    coords.push_back(std::tuple<float, float>(xRightTop, yCenter));
                    coords.push_back(std::tuple<float, float>(xCenter, yLeftBot));

                    isDifferenceBig = false;
                }
            }// if m_config.enableSuperSample is true

            RGBA totalColors{0,0,0,0};
            for (int c = 0; c < coords.size(); ++c) {
                x = std::get<0>(coords[c]);
                y = std::get<1>(coords[c]);
                // (u,v,k) is the sample point on the view plane.
                uvk = glm::vec3{scaleFactorU*x, scaleFactorV*y, -k};
                eye = glm::vec3{0.f,0,0};
                /**Converting ray (which is 'd') from camera space to world space to object space, to get t **/
                // we have ray in camera space. this is vec3
                rayDirInCameraSpace = uvk - eye;
                rayDirInCameraSpace = glm::normalize(rayDirInCameraSpace);
                // now we have ray in world space. this is vec4
                rayDirInWorldSpace = fromCameraToWorld(rayDirInCameraSpace, camera.getInverseViewMatrix());
                // but again, we want to use ray in object space to calculate t

                // for this speicific ray, need to check all objects (shapes) and find intersection point if exists
                // should return vector of intersection points of length scene.getShapes().size()
                // scene 에 포함된 모든 object 마다 intersectionPoint 있는지 확인해서 있으면 IntersectionPoint(true) return. 아니면 false.
                // 지금 여기에는 현재 ray 가 관통하는 모든 object 들의 intersectionPoint가 있음. 내가 계산했던 t 는 한 object내에서 가장 작은 t고. 그러므로
                // 이 vector 안에 있는 intersection Points 중에서 가장 작은 t 값을 가진 intersection point를 구해야됨.
                std::vector<IntersectionPoint> intersectionPoints = loopShapes(rayDirInWorldSpace, scene.getShapes(), camera.getPos()); //camera.getPos is world space poistion

                /**Computing normal
                // for each intersectionPoint in a vector "intersectionPoints" calculate normal in object space
                // scene.getShapes 가 shapes vector 인데. 여기 index랑 intersectionPoints index랑 순서는 일치. 그럼
                //  invsers(M_3.T) intersectionPoints.getNormal

                // need to transfer each normal in object space to world space.. material property 같은건 intersectionPoints[i] 랑 scene.getShapes()[i] 랑 같은 object이니
                **/


                /** Among all t's (or intersection points) from all objects, choose the smallest t and corresponding intersection point and return it.**/
                // this intersectionPoint has the non-neg smallest t among all t's from all objects that ray intersect
                std::tuple<int, IntersectionPoint> IdxAndintersectionPoint = findTValueAmongAllObjects(intersectionPoints);
                int index = std::get<0>(IdxAndintersectionPoint);
                IntersectionPoint intersectionPoint = std::get<1>(IdxAndintersectionPoint);

                // if intersectionPoint is false.. then intersectionPoint here is just first element of
                // vector intersectionPoints. So color black and
                if (!intersectionPoint.getHasIntersectionPoint()) {
                    RGBA black{0,0,0,1};
                    imageData[imageWidth*i + j] = black;
                    continue;
                }

                /** transform object space normal and intersectionpoint to world space **/
                glm::vec4 objectSpaceNormal = intersectionPoint.getNormalInObjectSpace();
                glm::vec4 intersectionPosition = intersectionPoint.getIntersectionPoint(); // this is also in object space

                // get ctm matrix which will be used to transfer from obj to world space
                glm::mat3 TransFromObjToWorldMat3 = intersectionPoint.getCTM();

                // transform normal from object space to world's
                glm::vec3 worldSpaceNormalVec3 = glm::inverse(glm::transpose(TransFromObjToWorldMat3)) * objectSpaceNormal;

                glm::vec4 worldspaceNormalVec4{worldSpaceNormalVec3, 0};
                worldspaceNormalVec4 = glm::normalize(worldspaceNormalVec4);

                // transform intersectionPosition from object to world
                glm::mat4 TransFromObjToWorldMat4 = intersectionPoint.getCTM();
                glm::vec4 intersectionPositionWorldSpace = TransFromObjToWorldMat4 * intersectionPosition;


                SceneMaterial material = scene.getShapes()[index].primitive.material;
                glm::vec3 directionFromPositionToCamera = camera.getPos() - intersectionPositionWorldSpace;
                std::vector<SceneLightData> lights = scene.getLights();

                /** get texture color**/
    //            int indexShape = intersectionPoint.getShapeIndex();
    //            intersectionPoint.setShapeIndex(index);
    //            RGBA textureColor = findTextureColor(intersectionPoint);
    //            glm::vec4 textureColorFloat = glm::vec4(textureColor.r / 255.f, textureColor.g / 255.f, textureColor.b / 255.f, 1.f);
                glm::vec4 textureColorFloat = glm::vec4(1,1,1,1);

                int maxRecursion = 0;
                glm::vec4 illumination = phong(intersectionPositionWorldSpace,
                       worldspaceNormalVec4,
                       directionFromPositionToCamera,
                       material,
                       lights,
                       scene.getGlobalData(),
                       scene.getShapes(),
                       intersectionPoint,
                       maxRecursion
                       /*Sampler   &reflectionSampler*/);

    //            RGBA color = toRGBA(illumination);
                //    return returnValue;


                /******************* reflection   start   ***************************/
                // for each intersection point that camera views,
                // for each light (already inside the loop)
                // using current intersection Position(intial one is "camera position"position)
                // and using reflected vector at normal of current positoin ( normal is already calculated at the very top)
                // glm::reflect(-positionToLight, normal) // 여기서 light은 다른 object에서 reflect 돼서 들어오는 빛


                // I think this one should be run outside phong. We want reflection phong at initial intersectoin point. so should add all recursive phong values. Pretend it plays separately from previous Iout equstion. Image Iout + Irefl

                // with given interatcion, find reflection ray. (use glm::reflect(incoming ray, normal). interaction point (P) and ray (d) are in object's space. convert to world. and for each shape in scene,
                // convert point and ray to each shape's space, and shoot it and find another closest intersection. Add theses intensities (need shape's reflection vector cReflective. keep going.




    //            phong(..., maxRecursion);

                // findReflectionIntensity will pass in reflectIllumination as reference.

                // reflectIllumination inclues ks and cReflectance
                // needs cReflectance here because
    //            SceneColor cReflective = intersectionPoint.getShape().primitive.material.cReflective;
    //            glm::vec4 reflectIllumination =
    //                    globalVarGlobalData.ks *
    //                    cReflective *
    //                    findReflectionIllumination(maxRecursion,
    //                                               IdxAndintersectionPoint,
    //                                               -directionFromPositionToCamera);

                // uncomment this to apply reflection!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! .. //
    //            RGBA color = toRGBA(illumination + reflectIllumination);
                color = toRGBA(illumination);

                totalColors.r+=color.r;
                totalColors.g+=color.g;
                totalColors.b+=color.b;
                totalColors.a+=color.a;
    //            texture();

                colors.push_back(color);

                if (colors.size() > 1 && colors.size() < 9) {
                    isDifferenceBig = checkColorDifference(colors[colors.size()-1], colors[colors.size()-2]);
                    if (isDifferenceBig)
                        goto again;
                }
                // after implementing reflection, should do color + reflectcolor.  color is just light/shadow at current intersection. reflectcolor is the ones that are coming from other 'light' sources ( 'light' that 다른 objects에서 반사되어 팅겨온것들)
            } // looping over corners

            totalColors.r /= float(coords.size());
            totalColors.g /= float(coords.size());
            totalColors.b /= float(coords.size());
            totalColors.a /= float(coords.size());

//            imageData[imageWidth*i + j] = color;
            imageData[imageWidth*i + j] = totalColors;

         }//for i
    }//for j
}

bool RayTracer::checkColorDifference(RGBA &color1, RGBA &color2) {
    float sum = 0;
    sum += pow(color1.r - color2.r,2);
    sum += pow(color1.g - color2.g,2);
    sum += pow(color1.b - color2.b,2);

    float distance = sqrt(sum);
    if (distance > 300.f)
        return true;
    return false;
}

// return illumination, i.e., [0,1]**4 not color RGBA
//glm::vec4 RayTracer::findReflectionIllumination(int maxRecursion,
//                                           std::tuple<int, IntersectionPoint>  currentIntersectionpointTuple,
//                                           glm::vec3 incomingRay) {//  incoming ray toward intersection point

//    IntersectionPoint currentIntersectionpoint = std::get<1>(currentIntersectionpointTuple);

//    // we only run findReflectionIllumination with valid intersectionpoint. If no intersectionpoint exits, we don't run the function.
//    glm::vec4 currentPointReflectMaterialValue = currentIntersectionpoint.getShape().primitive.material.cReflective;
//    // base cases
//    // base case 1. check if current shape's material is shiny
//    if (currentPointReflectMaterialValue.r == 0.0 &&
//            currentPointReflectMaterialValue.g == 0.0 &&
//            currentPointReflectMaterialValue.b == 0.0) { // for ex, if red sphere isn't shiny surface, no reflection from other objects
////        std::cout << "check vec4" << std::endl;
//        return glm::vec4(0,0,0,1.f);
//    }


//    glm::vec4 reflectIllumination{0.f,0,0,1};
//    /** These are for parameters of phong Start **/
//    glm::vec4 currentIntersectionPosition = currentIntersectionpoint.getIntersectionPoint();
//    glm::mat4 TransFromObjToWorldMat4 = currentIntersectionpoint.getCTM();
//    glm::vec4 intersectionPositionWorldSpace = TransFromObjToWorldMat4 * currentIntersectionPosition;

//    glm::vec4 objectSpaceNormal = currentIntersectionpoint.getNormalInObjectSpace();
//    glm::mat3 TransFromObjToWorldMat3 = currentIntersectionpoint.getCTM();
//    // transform normal from object space to world's
//    glm::vec3 worldSpaceNormalVec3 = glm::inverse(glm::transpose(TransFromObjToWorldMat3)) * objectSpaceNormal;
//    glm::vec4 worldspaceNormalVec4{worldSpaceNormalVec3, 0};
//    worldspaceNormalVec4 = glm::normalize(worldspaceNormalVec4);

//    glm::vec3 directionFromPositionToCamera = camersPosition - intersectionPositionWorldSpace;

//    int index = std::get<0>(currentIntersectionpointTuple);
//    SceneMaterial material = shapes[index].primitive.material;
//    glm::vec4 textureColorFloat = glm::vec4(1,1,1,1);
////    RGBA textureColor = findTextureColor(currentIntersectionpoint);
////    glm::vec4 textureColorFloat = glm::vec4(textureColor.r / 255.f, textureColor.g / 255.f, textureColor.b / 255.f, 1.f);

////    std::vector<SceneLightData> lights = scene_.getLights();
//    /** These are for parameters of phong End **/

//    // 해당 intersectoinpoint 에서 color 즉 intensity 를 구해야되는데. [0,1]버전으로 구해야됨.
//    glm::vec4 currentPointIllumination = phong(intersectionPositionWorldSpace,
//                                               worldspaceNormalVec4,
//                                               directionFromPositionToCamera,
//                                               material,
//                                               lights,
//                                               globalVarGlobalData,
//                                               shapes,
//                                               currentIntersectionpoint
//                                               );
////            SceneColor cReflective = currentIntersectionpoint.getShape().primitive.material.cReflective;
//    // globalVarGlobalData.ks * cReflective  should be multiplied to next intersectionpoint's reflectionIllumination
//    // since maxRecursion is already 4, stop using further reflection and us

//    reflectIllumination.r += currentPointIllumination.r; /*+ globalVarGlobalData.ks * cReflective * findReflectionIntensity(0, currentIntersectionpoint);*/
//    reflectIllumination.g += currentPointIllumination.g;
//    reflectIllumination.b += currentPointIllumination.b;
//    // base case 2.
//    if (maxRecursion == 4) {

//        // this case happens when reached at maximum recursion count and has intersection point
//        if (currentIntersectionpoint.getHasIntersectionPoint()) { // don't need this condition because I only call "findReflectionIllumination" when intersection point is valid
//            return currentPointReflectMaterialValue * currentPointIllumination;
////            return reflectIllumination;
//        // this case happens when reached at maximum recursion count but has no intersection point, which will never happen.
//        } else {
////            RGBA currentPointIllumination = phong();
////            reflectIllumination += currentPointIllumination;

//            // think when red sphere is the only one. I want no reflection illumination added.
//            return glm::vec4{0,0,0, 1.f};
//        }
//    }

//    // base case 3.. but this case won't happen b/c I don't run this function in the beginning if there's no intersection point
//    // another base case. no matter what maxRecursion values is, no reflection from other objects
////    if (!currentIntersectionpoint.getHasIntersectionPoint()) {
////        return glm::vec4{0,0,0,1.f};
////    }

//    /******** keep doing recursive reflection ****/
//    //// need to update P and d
//    glm::vec4 fromPositionToLightInWorldSpace;

//    // get intersectionPosition in object space
//    glm::vec4 intersectionPositionObjectSpace = currentIntersectionpoint.getIntersectionPoint();

//    // transform intersectionPosition from object to world
////    glm::mat4 TransFromObjToWorldMat4 = currentIntersectionpoint.getCTM();
////    glm::vec4 intersectionPositionWorldSpace = TransFromObjToWorldMat4 * intersectionPositionObjectSpace;

////    // transform normal from object space to world's and normalize it
////    glm::vec4 objectSpaceNormal = currentIntersectionpoint.getNormalInObjectSpace();
////    glm::mat3 TransFromObjToWorldMat3 = currentIntersectionpoint.getCTM();
////    glm::vec3 worldSpaceNormalVec3 =
///
///
/// (glm::transpose(TransFromObjToWorldMat3)) * objectSpaceNormal;
////    glm::vec4 worldspaceNormalVec4{worldSpaceNormalVec3, 0};


//    worldspaceNormalVec4 = glm::normalize(worldspaceNormalVec4);

//    // get reflected direction. from current position to outgoing reflected ray
//    // will use this as d.
//    glm::vec4 reflectedRayAtCurrentPositionObjectSpace = glm::reflect(glm::vec4(incomingRay,0), objectSpaceNormal);
//    glm::vec3 reflectedRayAtCurrentPositionWorldSpaceVec3 = glm::inverse(glm::transpose(TransFromObjToWorldMat3)) * reflectedRayAtCurrentPositionObjectSpace;
//    glm::vec4 reflectedRayAtCurrentPositionWorldSpaceVec4{reflectedRayAtCurrentPositionWorldSpaceVec3, 0};
//    reflectedRayAtCurrentPositionWorldSpaceVec4 = glm::normalize(reflectedRayAtCurrentPositionWorldSpaceVec4);


//    currentIntersectionPosition = currentIntersectionPosition + reflectedRayAtCurrentPositionObjectSpace * glm::vec4(0.001, 0.001, 0.001, 1);
//    intersectionPositionWorldSpace = TransFromObjToWorldMat4 * currentIntersectionPosition; // this is P + epsilon*d

//    // Next step is to convert d to d~. and, by using P~ td~, will find intersection point which reflacecd ray will hits. loopShapes will take care of it
//    std::vector<IntersectionPoint> intersectionPoints = loopShapes(reflectedRayAtCurrentPositionWorldSpaceVec4, globalVarShapes, intersectionPositionWorldSpace);// fromPositionToLightInWorldspace is my d and intersectionPositionWorldspace is my P

//    std::tuple<int, IntersectionPoint> nextIdxAndintersectionPoint = findTValueAmongAllObjects(intersectionPoints);
////    int index = std::get<0>(nextIdxAndintersectionPoint);
//    IntersectionPoint closestIntersectionPoint = std::get<1>(nextIdxAndintersectionPoint);

//    // from current intersectionpoint, found a next intersection point (don't know if exists or not).
//    // if no further intersection exists, return 0 because no reflection
//    if (!closestIntersectionPoint.getHasIntersectionPoint()) {
//        return glm::vec4(0,0,0,1.f);
//    }

//    // closestIntersectionPoint is where reflected ray hits.
//    SceneColor cReflective = currentIntersectionpoint.getShape().primitive.material.cReflective;
//    return  currentPointReflectMaterialValue * currentPointIllumination + globalVarGlobalData.ks * findReflectionIllumination(maxRecursion+1, nextIdxAndintersectionPoint, reflectedRayAtCurrentPositionWorldSpaceVec4);

//}






std::tuple<int, IntersectionPoint> RayTracer::findTValueAmongAllObjects(std::vector<IntersectionPoint> intersectionPoints) {
    int indexOfSmallestT = 0;
    float temp = INFINITY;
    for (auto i = 0; i < intersectionPoints.size(); ++i) {
        if ((intersectionPoints[i].getHasIntersectionPoint() == true) && (intersectionPoints[i].getTValue() <= temp)) {
            indexOfSmallestT = i;
            temp = intersectionPoints[i].getTValue();
        }
    }

    return std::tuple<int, IntersectionPoint>(indexOfSmallestT, intersectionPoints[indexOfSmallestT]);
    //    return intersectionPoints[indexOfSmallestT];
}



// what do we return? vector of intersection points
// we loop over all the shapes in the scene and find
std::vector<IntersectionPoint> RayTracer::loopShapes(glm::vec4 rayDirInWorldSpace, std::vector<RenderShapeData> shapes, glm::vec4 cameraPosition) { // cameraPosition here is world space
    std::vector<IntersectionPoint> intersectionPoints; // use t for each shape if any. The smallest t .. shapes.size()

    // for each shape in shapes
    for (int idx = 0; idx < shapes.size(); ++idx) {
        /** shapes[i] 의 intersection point 찾아서 t 계산 후 tValues[i]에 저장. 나중에 ith element in tValues 가 ith shape in shapes 의 t 값. **/
        IntersectionPoint intersection;
        // for each object, find intersection of non negative smallest t
        // intersectionPoints pick one intersection point for each object. if no intersection, still returns intersection with false value.
        intersection = getIntersection(shapes[idx], cameraPosition, rayDirInWorldSpace);
        // save idx of shapes to intersection so that to use it for texture mapping?

        intersection.setShapeIndex(idx);

        intersectionPoints.push_back(intersection); // what if IntersectionPoint also has normal vector. we need ctm to transform normal in object to world
    }

    /**
    // this is repetitve process. need to find smallest t using intersectionPoint. P + td = intersection . -> t = (intersection - P) / t
    sort(intersectionPoints.begin(), intersectionPoints.end(), mycompare);
    **/

    // 지금 intersectionPoints vector 에는 모든 shapes을 loop하면서 intersect 하는지 여부랑 하면 position 어딘지 저장됨.
    // 이 중에서 t 가 가장 작은 값을 가지는 position을 return 하면되는거지. 그럼 return type 은 그냥 IntersectionPoint 하나 되는거고. vector가 아니고.
    return intersectionPoints;
}


IntersectionPoint RayTracer::getIntersection(RenderShapeData &shape, glm::vec4 cameraPosition, glm::vec4 rayDirInWorldSpace) {
    IntersectionPoint intersectionPoint;
    switch(shape.primitive.type){
        case PrimitiveType::PRIMITIVE_CUBE:
            intersectionPoint = getIntersectionOfCube(shape, cameraPosition, rayDirInWorldSpace);
//            intersectionPoints.push_back(intersectionPoint);
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            intersectionPoint = getIntersectionOfCone(shape, cameraPosition, rayDirInWorldSpace);
           break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            intersectionPoint = getIntersectionOfSphere(shape, cameraPosition, rayDirInWorldSpace);
//            intersectionPoints.push_back(intersectionPoint);
           break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            intersectionPoint = getIntersectionOfCylinder(shape, cameraPosition, rayDirInWorldSpace);
           break;

   }
    return intersectionPoint;
}

std::tuple<float, float> RayTracer::calculateConeUV(glm::vec4 &intersection) {
    float u, v;
    // get u
    float theta = atan2(intersection.z, intersection.x);
    if (theta <= 0) {
        u = -theta/(2*M_PI);
    } else {
        u = 1 - theta / (2*M_PI);
    }
    // get v
    float phi = asin(intersection.y / 0.5);
    v = intersection.y + 0.5;

    if (v == 1)
        u = 0.5;

    return std::tuple<float, float>(u,v);
}

IntersectionPoint RayTracer::getIntersectionOfCone(RenderShapeData &shape, glm::vec4 cameraPosition, glm::vec4 rayDirInWorldSpace) {

    std::vector<std::tuple<float, glm::vec4>> possibleTs;
    glm::mat4 inverseCTM = glm::inverse(shape.ctm);
    // get camera position in object space. 'P tilde"
    glm::vec4 cameraPositionObjectSpace = inverseCTM * cameraPosition;
    // get 'd' in object space. Don't normalize. 'd tilde'
    glm::vec4 rayDirInObjectSpace = inverseCTM * rayDirInWorldSpace;

//    IntersectionPoint intersectionPoint;
    float Px = cameraPositionObjectSpace.x;
    float Py = cameraPositionObjectSpace.y;
    float Pz = cameraPositionObjectSpace.z;

    float dx = rayDirInObjectSpace.x;
    float dy = rayDirInObjectSpace.y;
    float dz = rayDirInObjectSpace.z;

    /** Check intersections with conical top**/
    float A = pow(dx,2) + pow(dz,2) - pow(dy, 2) / 4.f;
    float B = 2 * Px * dx + 2 * Pz * dz - (Py * dy) / 2 + dy / 4;
    float C = pow(Px, 2) + pow(Pz, 2) - pow(Py, 2) / 4.f + Py / 4 - 1.f/16;

    // B^2 - 4AC
    float D = pow(B, 2) - 4 * A * C;


    float t = INFINITY;
    glm::vec4 normal;
    glm::vec4 intersection;
    glm::mat4 ctm;

    // for texture
    float u,v;

    if (D < 0.f){
        // don't add any t's

    }

    else if (D == 0){ // tangent intersection
        float t0 = -B / (2*A);
        intersection = cameraPositionObjectSpace + t * rayDirInObjectSpace;

        // check validation
        if ((intersection.y >= -0.5 && intersection.y <= 0.5) &&
                (t0 < t ) && (t0 >= 0.f)) {

            t = t0;
            normal = conicalTopNormal(intersection);

            std::tuple<float,float> uv = calculateConeUV(intersection);
            u = std::get<0>(uv);
            v = std::get<1>(uv);
        }
    }
    else { // two intersections.
        float t1 = (-B + sqrt(pow(B,2) - 4 * A * C)) / (2 * A);
        float t2 = (-B - sqrt(pow(B,2) - 4 * A * C)) / (2 * A);

        intersection = cameraPositionObjectSpace + t1 * rayDirInObjectSpace;
        if ((intersection.y >= -0.5 && intersection.y <= 0.5) &&
                (t1 < t) && (t1 >= 0.f)) {
            t = t1;
            normal = conicalTopNormal(intersection);

            // calculate u,v
            std::tuple<float,float> uv = calculateConeUV(intersection);
            u = std::get<0>(uv);
            v = std::get<1>(uv);
        }

        intersection = cameraPositionObjectSpace + t2 * rayDirInObjectSpace;
        if ((intersection.y >= -0.5 && intersection.y <= 0.5) &&
                (t2 < t) && (t2 >= 0.f)) {
            t = t2;
            normal = conicalTopNormal(intersection);

            // calculate u,v
            std::tuple<float,float> uv = calculateConeUV(intersection);
            u = std::get<0>(uv);
            v = std::get<1>(uv);
        }

    }

    /** Check intersections with flat base**/
    float tFlat = (-0.5 - Py) / dy;
    intersection = cameraPositionObjectSpace + tFlat * rayDirInObjectSpace;
    if (pow(intersection.x,2) + pow(intersection.z,2) <= 0.25){
        if ((tFlat < t ) && (tFlat >= 0.f)){
            t = tFlat;
            normal = glm::vec4{0,-1,0,0};

            u = intersection.x + 0.5;
            v = intersection.z + 0.5;
        }
    }

    IntersectionPoint intersectionPoint;
    if (t < INFINITY){
        intersection = cameraPositionObjectSpace + t * rayDirInObjectSpace;
        intersectionPoint.setIntersectionPoint(intersection);
        intersectionPoint.setHasIntersectionPoint(true);
        intersectionPoint.setNormalInObjectSpace(normal);
        intersectionPoint.setTValue(t);
        intersectionPoint.setCTM(shape.ctm);
        intersectionPoint.setInverseCTM(glm::inverse(shape.ctm));
        intersectionPoint.setShape(shape);
        intersectionPoint.setUVFloat(u,v);
        return intersectionPoint;
    } else {
        intersectionPoint.setHasIntersectionPoint(false);
        return intersectionPoint;
    }
}

IntersectionPoint RayTracer::chooseInterPoint(std::vector<std::tuple<float, glm::vec4>> possibleTs, glm::vec4 cameraPositionObjectSpace, glm::vec4 rayDirInObjectSpace) {
    IntersectionPoint intersectionPoint;

    sort(possibleTs.begin(), possibleTs.end(), mycompare);

    // check all possible T's and pick non negative smallest one
    for (const auto &tuple : possibleTs) {

        // if 0th element of tuple, which is t value, is non negative, that's the valid t
        if (std::get<0>(tuple) >= 0){
            intersectionPoint.setHasIntersectionPoint(true);
            intersectionPoint.setIntersectionPoint(cameraPositionObjectSpace + std::get<0>(tuple) * rayDirInObjectSpace);
            // save normal vector, which is 1st element of tuple, to valid intersectionPoint
            intersectionPoint.setNormalInObjectSpace(std::get<1>(tuple));
            intersectionPoint.setTValue(std::get<0>(tuple));
            return intersectionPoint;
        }
    }
    // looped possible T's but the vector is empty or all T's are negative.
    // so this shape/object has no intersection point with given ray
    // no intersection means no normal vector
    intersectionPoint.setHasIntersectionPoint(false);
    return intersectionPoint;
}


std::tuple<float, float> RayTracer::calculateCylinderUV(glm::vec4 &intersection) {
    float u, v;
    // get u
    float theta = atan2(intersection.z, intersection.x);
    if (theta <= 0) {
        u = -theta/(2*M_PI);
    } else {
        u = 1 - theta / (2*M_PI);
    }
    // get v
    float phi = asin(intersection.y / 0.5);
    v = intersection.y + 0.5;

    return std::tuple<float, float>(u,v);
}

IntersectionPoint RayTracer::getIntersectionOfCylinder(RenderShapeData &shape, glm::vec4 cameraPosition, glm::vec4 rayDirInWorldSpace) {
    // don't need in this case
    std::vector<std::tuple<float, glm::vec4>> possibleTs;
    glm::mat4 inverseCTM = glm::inverse(shape.ctm);
    // get camera position in object space. 'P tilde"
    glm::vec4 cameraPositionObjectSpace = inverseCTM * cameraPosition;
    // get 'd' in object space. Don't normalize. 'd tilde'
    glm::vec4 rayDirInObjectSpace = inverseCTM * rayDirInWorldSpace;

    float Px = cameraPositionObjectSpace.x;
    float Py = cameraPositionObjectSpace.y;
    float Pz = cameraPositionObjectSpace.z;

    float dx = rayDirInObjectSpace.x;
    float dy = rayDirInObjectSpace.y;
    float dz = rayDirInObjectSpace.z;

    float A = pow(dx,2) + pow(dz,2);
    float B = 2*Px*dx + 2*Pz*dz;
    float C = pow(Px,2) + pow(Pz,2) - 0.25;

    float D = pow(B,2) - 4*A*C;


    float t = INFINITY;
    glm::vec4 normal;
    glm::vec4 intersection;

    float u, v;
    /*** checking if ray intersects finite cylinder  ***/
    if (D < 0) {// no intersection. so no t. don't add anything to possibleTs. just pass
//        intersectionPoint.setHasIntersectionPoint(false);
//        return intersectionPoint;


    }

    else if (D == 0){ // tangent intersection
        float t1 = -B / (2*A);

//        if (t < 0){
//            intersectionPoint.setHasIntersectionPoint(false);
//            return intersectionPoint;
//        }

        intersection = cameraPositionObjectSpace + t1 * rayDirInObjectSpace;

//        IntersectionPoint intersectionPoint;
        if (checkYRange(intersection) && (t1 < t) && (t1 >= 0)){
//            intersectionPoint.setHasIntersectionPoint(true);
//            intersectionPoint.setIntersectionPoint(intersection);
//            return intersectionPoint;
            t = t1;
            normal = finiteCylinderNormal(intersection);
//            possibleTs.push_back(std::tuple<float, glm::vec4>(t, normal)); // this t could be negative. will check negativity at the end

            std::tuple<float,float> uv = calculateCylinderUV(intersection);
            u = std::get<0>(uv);
            v = std::get<1>(uv);

        }
        else { // do nothing
//            intersectionPoint.setHasIntersectionPoint(false);
//            return intersectionPoint;
        }
    }
    else{ // D > 0 i.e., two intersections
        float t1 = (-B + sqrt(pow(B,2)-4*A*C)) / (2*A);
        float t2 = (-B - sqrt(pow(B,2)-4*A*C)) / (2*A);

//        glm::vec4 intersection1 = cameraPositionObjectSpace + t1 * rayDirInObjectSpace;
//        glm::vec4 intersection2 = cameraPositionObjectSpace + t2 * rayDirInObjectSpace;
        intersection = cameraPositionObjectSpace + t1 * rayDirInObjectSpace;
        if (checkYRange(intersection) && (t1 < t) && (t1 >= 0)) {
            t = t1;
            normal = finiteCylinderNormal(intersection);
            // texture
            std::tuple<float,float> uv = calculateCylinderUV(intersection);
            u = std::get<0>(uv);
            v = std::get<1>(uv);
//            float theta = atan2(intersection.z, intersection.x);
//            if (theta <= 0) {
//                u = -theta/(2*M_PI);
//            } else {
//                u = 1 - theta / (2*M_PI);
//            }
//            v = intersection.y + 0.5;
        }

        intersection = cameraPositionObjectSpace + t2 * rayDirInObjectSpace;
        if (checkYRange(intersection) && (t2 < t) && (t2 >= 0)) {
            t = t2;
            normal = finiteCylinderNormal(intersection);
            // texture
            std::tuple<float,float> uv = calculateCylinderUV(intersection);
            u = std::get<0>(uv);
            v = std::get<1>(uv);
//            float theta = atan2(intersection.z, intersection.x);
//            if (theta <= 0) {
//                u = -theta/(2*M_PI);
//            } else {
//                u = 1 - theta / (2*M_PI);
//            }
//            v = intersection.y + 0.5;
        }




//        if (checkYRange(intersection1)){
//            glm::vec4 normal1 = finiteCylinderNormal(intersection1);
//            possibleTs.push_back(std::tuple<float, glm::vec4>(t1, normal1));
//        }
//        if (checkYRange(intersection2)){
//            glm::vec4 normal2 = finiteCylinderNormal(intersection2);
//            possibleTs.push_back(std::tuple<float, glm::vec4>(t2, normal2));
//        }

        /**
        if ((t1 < t2) && (t1 > 0)) {
            glm::vec4 intersection = cameraPositionObjectSpace + t1 * rayDirInObjectSpace;
            if (checkYRange(intersection)){
                intersectionPoint.setHasIntersectionPoint(true);
                intersectionPoint.setIntersectionPoint(intersection);
                return intersectionPoint;
            }
            else {
                intersectionPoint.setHasIntersectionPoint(false);
                return intersectionPoint;
//                return glm::vec4{INFINITY, INFINITY, INFINITY, 1};
            }
        } else if ((t2 < t1) && (t2 > 0)){ // t2 < t1
            glm::vec4 intersection = cameraPositionObjectSpace + t2 * rayDirInObjectSpace;
            if (checkYRange(intersection)){
                intersectionPoint.setHasIntersectionPoint(true);
                intersectionPoint.setIntersectionPoint(intersection);
                return intersectionPoint;
            }
            else {
                intersectionPoint.setHasIntersectionPoint(false);
                return intersectionPoint;
//                return glm::vec4{INFINITY, INFINITY, INFINITY, 1};
            }
        }

        else { //t1 < t2 but t1 < 0  or  t2 < t1 but t2 < 0

        }
        **/
    }

    /*** Now check if ray intersects top plane***/
    float t3 = (0.5 - Py) / dy;
    intersection = cameraPositionObjectSpace + t3 * rayDirInObjectSpace;
    if ((pow(intersection.x,2) + pow(intersection.z,2) <= 0.25) &&
            (t3 < t) && (t3 >= 0)) {
        t = t3;
        normal = glm::vec4{0,1,0,0};

        u = intersection.x + 0.5;
        v = -intersection.z + 0.5;
    }


    /*** Now check if ray intersects bottom plane***/
    float t4 = (-0.5 - Py) / dy;
    intersection = cameraPositionObjectSpace + t4 * rayDirInObjectSpace;
    if ((pow(intersection.x,2) + pow(intersection.z,2) <= 0.25) &&
        (t4 < t) && (t4 >= 0)){
        t = t4;
        normal = glm::vec4{0,-1,0,0};

        u = intersection.x + 0.5;
        v = intersection.z + 0.5;
    }

    IntersectionPoint intersectionPoint;
    if (t < INFINITY){
        intersection = cameraPositionObjectSpace + t * rayDirInObjectSpace;
        intersectionPoint.setIntersectionPoint(intersection);
        intersectionPoint.setHasIntersectionPoint(true);
        intersectionPoint.setNormalInObjectSpace(normal);
        intersectionPoint.setTValue(t);
        intersectionPoint.setCTM(shape.ctm);
        intersectionPoint.setInverseCTM(glm::inverse(shape.ctm));
        intersectionPoint.setShape(shape);
        intersectionPoint.setUVFloat(u, v);
        return intersectionPoint;
    } else {
        intersectionPoint.setHasIntersectionPoint(false);
        return intersectionPoint;
    }
}

// check y range for cylinder shape
bool RayTracer::checkYRange(glm::vec4 intersection) {
    if (intersection.y >= -0.5 && intersection.y <= 0.5)
        return true;
    else return false;
}

std::tuple<float, float> RayTracer::calculateSphereUV(glm::vec4 &intersection) {
    float u, v;
    // get u
    float theta = atan2(intersection.z, intersection.x);
    if (theta <= 0) {
        u = -theta/(2*M_PI);
    } else {
        u = 1 - theta / (2*M_PI);
    }
    // get v
    float phi = asin(intersection.y / 0.5);
    v = phi / M_PI + 1 / 2.f;

    if (v == 0.f || v == 1.f)
        u = 0.5;

    return std::tuple<float, float>(u,v);
}

IntersectionPoint RayTracer::getIntersectionOfSphere(RenderShapeData &shape, glm::vec4 cameraPosition, glm::vec4 rayDirInWorldSpace) {
    std::vector<std::tuple<float, glm::vec4>> possibleTs;
    glm::mat4 inverseCTM = glm::inverse(shape.ctm);
    // get camera position in object space. 'P tilde"
    glm::vec4 cameraPositionObjectSpace = inverseCTM * cameraPosition;
    // get 'd' in object space. Don't normalize. 'd tilde'
    glm::vec4 rayDirInObjectSpace = inverseCTM * rayDirInWorldSpace;

    float Px = cameraPositionObjectSpace.x;
    float Py = cameraPositionObjectSpace.y;
    float Pz = cameraPositionObjectSpace.z;

    float dx = rayDirInObjectSpace.x;
    float dy = rayDirInObjectSpace.y;
    float dz = rayDirInObjectSpace.z;

    float A = pow(dx,2) + pow(dy,2) + pow(dz,2);
    float B = 2 * (Px*dx + Py*dy + Pz*dz);
    float C = pow(Px,2) + pow(Py,2) + pow(Pz,2) - 0.25;

    float D = pow(B,2) - 4*A*C;

    float t = INFINITY;
    glm::vec4 intersection;
    glm::vec4 normal;

    // for texture
    float u, v;

    if (D < 0){
        // Do nothing
        // current ray doesn't intersect current object
    }
    else if (D == 0) {
        float t1 = - B / (2*A);
        // no need to check any validity
        intersection = cameraPositionObjectSpace + t * rayDirInObjectSpace;
        if ((t1 < t) && (t1 >= 0)) {
            t = t1;
            normal = sphereNormal(intersection);
        }

        std::tuple<float,float> uv = calculateSphereUV(intersection);
        u = std::get<0>(uv);
        v = std::get<1>(uv);
    }
    else {
        float t1 = (-B + sqrt(pow(B,2)-4*A*C)) / (2*A);
        float t2 = (-B - sqrt(pow(B,2)-4*A*C)) / (2*A);

        intersection = cameraPositionObjectSpace + t1 * rayDirInObjectSpace;
        if ((t1 < t) && (t1 >=0)) {
            t = t1;
            normal = sphereNormal(intersection);

            std::tuple<float,float> uv = calculateSphereUV(intersection);
            u = std::get<0>(uv);
            v = std::get<1>(uv);
        }

        intersection = cameraPositionObjectSpace + t2 * rayDirInObjectSpace;
        if ((t2 < t) && (t2 >=0)) {
            t = t2;
            normal = sphereNormal(intersection);

            std::tuple<float,float> uv = calculateSphereUV(intersection);
            u = std::get<0>(uv);
            v = std::get<1>(uv);
        }
    }

    IntersectionPoint intersectionPoint;
    if (t < INFINITY){
        intersection = cameraPositionObjectSpace + t * rayDirInObjectSpace;
        intersectionPoint.setIntersectionPoint(intersection);
        intersectionPoint.setHasIntersectionPoint(true);
        intersectionPoint.setNormalInObjectSpace(normal);
        intersectionPoint.setTValue(t);
        intersectionPoint.setCTM(shape.ctm);
        intersectionPoint.setInverseCTM(glm::inverse(shape.ctm));
        intersectionPoint.setShape(shape);
        intersectionPoint.setUVFloat(u,v);
        return intersectionPoint;
    } else {
        intersectionPoint.setHasIntersectionPoint(false);
        return intersectionPoint;
    }
}


// need to find six t's. need to find smallest t but > 0.
IntersectionPoint RayTracer::getIntersectionOfCube(RenderShapeData &shape, glm::vec4 cameraPosition, glm::vec4 rayDirInWorldSpace) {

    std::vector<std::tuple<float, glm::vec4>> possibleTs;
    glm::mat4 inverseCTM = glm::inverse(shape.ctm);
    // get camera position in object space. 'P tilde"
    glm::vec4 cameraPositionObjectSpace = inverseCTM * cameraPosition;
    // get 'd' in object space. Don't normalize. 'd tilde'
    glm::vec4 rayDirInObjectSpace = inverseCTM * rayDirInWorldSpace;

    float Px = cameraPositionObjectSpace.x;
    float Py = cameraPositionObjectSpace.y;
    float Pz = cameraPositionObjectSpace.z;

    float dx = rayDirInObjectSpace.x;
    float dy = rayDirInObjectSpace.y;
    float dz = rayDirInObjectSpace.z;


    float t = INFINITY;

    glm::vec4 normal;

    float u;
    float v;
    /** front face **/
    float tFront = (0.5 - Pz) / float(dz);
    glm::vec4 intersect = cameraPositionObjectSpace + tFront * rayDirInObjectSpace;
    // sanity check    
    if ((intersect.x <= 0.5) &&
            (intersect.x >= -0.5) &&
            (intersect.y <= 0.5) &&
            (intersect.y >= -0.5) &&
            (tFront < t) &&
            (tFront >= 0)) {
        t = tFront;
        normal = glm::vec4{0,0,1.f,0};
        u = intersect.x + 0.5;
        v = intersect.y + 0.5;
    }

    /** back face **/
    float tBack = (-0.5 - Pz) / float(dz);
    glm::vec4 intersect2 = cameraPositionObjectSpace + tBack * rayDirInObjectSpace;
    // sanity check
    if ((intersect2.x <= 0.5) &&
            (intersect2.x >= -0.5) &&
            (intersect2.y <= 0.5) &&
            (intersect2.y >= -0.5) &&
            (tBack < t) &&
            (tBack >= 0)) {
        t = tBack;
        normal= glm::vec4{0,0,-1.f,0};

        u = -intersect2.x + 0.5;
        v = intersect2.y + 0.5;
    }

    /** left side face **/
    float tLeftSide = (-0.5 - Px) / float(dx);
    glm::vec4 intersect3 = cameraPositionObjectSpace + tLeftSide * rayDirInObjectSpace;
    // sanity check
    if ((intersect3.z <= 0.5) &&
            (intersect3.z >= -0.5) &&
            (intersect3.y <= 0.5) &&
            (intersect3.y >= -0.5) &&
            (tLeftSide < t) &&
            (tLeftSide >= 0)) {
        t = tLeftSide;
        normal = glm::vec4{-1,0,0,0};

        u = intersect3.z + 0.5;
        v = intersect3.y + 0.5;
    }

    /** right side face **/
    float tRightSide = (0.5 - Px) / float(dx);
    glm::vec4 intersect4 = cameraPositionObjectSpace + tRightSide * rayDirInObjectSpace;
    // sanity check
    if ((intersect4.z <= 0.5) &&
            (intersect4.z >= -0.5) &&
            (intersect4.y <= 0.5) &&
            (intersect4.y >= -0.5) &&
            (tRightSide < t) &&
            (tRightSide >= 0)){
        normal = glm::vec4{1, 0,0,0};
        t = tRightSide;

        u = -intersect4.z + 0.5;
        v = intersect4.y + 0.5;
    }

    /** top side face **/
    float tTopSide = (0.5 - Py) / float(dy);
    glm::vec4 intersect5 = cameraPositionObjectSpace + tTopSide * rayDirInObjectSpace;
    // sanity check
    if ((intersect5.z <= 0.5) &&
            (intersect5.z >= -0.5) &&
            (intersect5.x <= 0.5) &&
            (intersect5.x >= -0.5)&&
            (tTopSide < t) &&
            (tTopSide >= 0)) {
        normal = glm::vec4{0, 1,0,0};
        t = tTopSide;

        u = intersect5.x + 0.5;
        v = -intersect5.z + 0.5;
    }

    /** bottom side face **/
    float tBottomSide = (-0.5 - Py) / float(dy);
    glm::vec4 intersect6 = cameraPositionObjectSpace + tBottomSide * rayDirInObjectSpace;
    // sanity check
    if ((intersect6.z <= 0.5) &&
            (intersect6.z >= -0.5) &&
            (intersect6.x <= 0.5) &&
            (intersect6.x >= -0.5)&&
            (tBottomSide < t) &&
            (tBottomSide >= 0)) {
        t = tBottomSide;
        normal = glm::vec4{0,-1,0,0};

        u = intersect6.x + 0.5;
        v = intersect6.z + 0.5;
    }

    IntersectionPoint intersectionPoint;
    if (t < INFINITY){
        intersect = cameraPositionObjectSpace + t * rayDirInObjectSpace;
        intersectionPoint.setIntersectionPoint(intersect);
        intersectionPoint.setHasIntersectionPoint(true);
        intersectionPoint.setNormalInObjectSpace(normal);
        intersectionPoint.setTValue(t);
        intersectionPoint.setCTM(shape.ctm);
        intersectionPoint.setInverseCTM(glm::inverse(shape.ctm));
        intersectionPoint.setShape(shape);
        intersectionPoint.setUV(std::tuple<float, float> (u,v));
        intersectionPoint.setUVFloat(u,v);
        return intersectionPoint;
    } else {
        intersectionPoint.setHasIntersectionPoint(false);
        return intersectionPoint;
    }

}

float RayTracer::getX(auto j, auto imageWidth) {
    return (((j + 0.5) / imageWidth) - 0.5);
}

float RayTracer::getY(auto i, auto imageHeight) {
    return ((imageHeight - 1 - i + 0.5) / imageHeight - 0.5);
}

glm::vec4 RayTracer::fromCameraToWorld(glm::vec3 rayDirInCameraSpace, glm::mat4 InverseViewMatrix) {
    return InverseViewMatrix * glm::vec4{rayDirInCameraSpace, 0};
}

float RayTracer::calculateFallOff(SceneLightData &light, float x) {
    float outer = light.angle;
    float inner =  light.angle -light.penumbra;
    return -2 * pow((x - inner)/(outer-inner),3) + 3 * pow((x - inner) / (outer - inner),2);
}

SceneColor RayTracer::calculateSpotLightIntensity(glm::vec3 position, SceneLightData light) {
    glm::vec4 currentDirectionFromLightToPos = glm::vec4{position,1}-light.pos;
    float dotProd = glm::dot(glm::normalize(currentDirectionFromLightToPos),
                             glm::normalize(light.dir));
    float x = acos(dotProd);// angle (radian) between current direction and spotlight direction

    float inner = light.angle - light.penumbra;
    float outer = light.angle;
    if (x <= inner) {
        // return full light intensity
        return light.color;
    } else if (x > inner && x < outer){
        return light.color * (1.f - calculateFallOff(light, x));
    } else {
        return light.color * 0.f;
    }
}

/*
// For reference
//RGBA color = phong(intersectionPositionWorldSpace,
//       worldspaceNormalVec4,
//       directionFromPositionToCamera,
//       material,
//       lights,
//       scene.getGlobalData(),
//       scene.getShapes()
//       /*Sampler   &reflectionSampler);

//For each ray from camera to view plane:
//    at an intersection 'p' of smallest t(if any):
//        convert 'p' to current object's world by using inverse CTM
//        call object space 'p' as 'p~'
//        Find t that satisfies p~ + t*d~ = light source (in object) where d~ is from p~ to light source.  We can find t here just using p+ t*d = light source in world. Also if light is directional, don't need this. Just check if there's any valid t of objects
//        for every object in scene:
//            convert 'p' to object's world and call it 'p~~'
//            Find if there exists t such that p~~ + t*d~~ = object's implicit equation. d~~ is vector from p~~ to this object's space's light position.
//            If t is found, then 'p' is shadow
*/

float RayTracer::checkShadow(const SceneLightData &light,
                             glm::vec3 intersectionPositionWorldSpace,
                             const std::vector<RenderShapeData> &shapes,
                             IntersectionPoint intersectionPoint) { // info in this variable is not world space
    // if light type is directional, source is infinitely far away. Just check if p+td has any valid t's with other objects
    // get camera position in object space. 'P tilde"
//    glm::vec4 cameraPositionObjectSpace = glm::inverse(shape.ctm) * cameraPosition;
    // get 'd' in object space. Don't normalize. 'd tilde'
//    glm::vec4 rayDirInObjectSpace = glm::inverse(shape.ctm) * rayDirInWorldSpace;

    glm::vec4 fromPositionToLightInWorldSpace;
    if (light.type != LightType::LIGHT_DIRECTIONAL) {
            fromPositionToLightInWorldSpace = glm::normalize(light.pos - glm::vec4(intersectionPositionWorldSpace,1));
    } else fromPositionToLightInWorldSpace = -light.dir;

    glm::mat4 currentShapeCTM = intersectionPoint.getCTM();
    glm::vec4 fromPositionToLightInObjectSpace = intersectionPoint.getInverseCTM() * fromPositionToLightInWorldSpace;
    glm::vec4 intersectionPositionObjectSpace = intersectionPoint.getInverseCTM() * glm::vec4(intersectionPositionWorldSpace,1);

    float tForLight;    

    if (light.type != LightType::LIGHT_DIRECTIONAL) {


        tForLight = (light.pos.x - intersectionPositionObjectSpace.x) / fromPositionToLightInObjectSpace.x;

        // this variable is to avoid self-shadowing
        glm::vec4 intersectionPositionWorldSpaceEpsilon(glm::vec4(intersectionPositionWorldSpace,1) + fromPositionToLightInWorldSpace*0.0001f);// check this line

        std::vector<IntersectionPoint> intersectionPoints = loopShapes(fromPositionToLightInWorldSpace, shapes, intersectionPositionWorldSpaceEpsilon);// fromPositionToLightInWorldspace is my d and intersectionPositionWorldspace is my P

        std::tuple<int, IntersectionPoint> IdxAndintersectionPoint = findTValueAmongAllObjects(intersectionPoints);
        int index = std::get<0>(IdxAndintersectionPoint);
        IntersectionPoint closestIntersectionPoint = std::get<1>(IdxAndintersectionPoint);

        // if intersectionPoint is false.. then intersectionPoint here is just first element of
        // vector intersectionPoints.
        // this means no other objects inbetween given intersectionPoint and light source. So not shadow
        if (!closestIntersectionPoint.getHasIntersectionPoint()) {
            return 1.f;
        }
        /**Version 1**/
        else { // has other objects inbetween. but need to compare tObject with tLight
            // here I'm assuming that no objects are beyond light source
//            return 0.f;

            float tObject = closestIntersectionPoint.getTValue();

            float pointLightDis = glm::distance(light.pos,glm::vec4(intersectionPositionWorldSpace,1));

            glm::vec4 objPosInWorld = closestIntersectionPoint.getCTM() * closestIntersectionPoint.getIntersectionPoint();
            float pointLObjectDis = glm::distance(objPosInWorld, glm::vec4(intersectionPositionWorldSpace,1));
//            std::cout << "tForLight: " << tForLight << " " << "tForObject" << tObject << std::endl;
            if (pointLightDis <= pointLObjectDis) {

                return 1.f; // need to check this part. when I used 1.f, some shadows are missing
            } else return 0.f;

        }
        /**Version 2**/
        // this version assumes that no objects are beyond light source. So if there's intersecition existed with other objects, that object must be before light source. So original intersection point is in shadow
        /*
        else {
            return 0.f;
        }
        */


        /*
        for (auto &shape : shapes) {
            // get given intersectionPositionWorldSpace's position in shape/object's space
            glm::mat4 objectCTM = shape.ctm;
            glm::vec4 intersectionPositionAnotherObjectSpace = objectCTM * glm::vec4(intersectionPositionWorldSpace,1);


        }
        */
    } // light.type != directional
    else { // light.type == LightType::LIGHT_DIRECTIONAL
        glm::vec4 intersectionPositionWorldSpaceEpsilon(glm::vec4(intersectionPositionWorldSpace,1) + fromPositionToLightInWorldSpace*0.0001f);// check this line

        std::vector<IntersectionPoint> intersectionPoints = loopShapes(fromPositionToLightInWorldSpace, shapes, intersectionPositionWorldSpaceEpsilon);// fromPositionToLightInWorldspace is my d and intersectionPositionWorldspace is my P

        std::tuple<int, IntersectionPoint> IdxAndintersectionPoint = findTValueAmongAllObjects(intersectionPoints);
        int index = std::get<0>(IdxAndintersectionPoint);
        IntersectionPoint closestIntersectionPoint = std::get<1>(IdxAndintersectionPoint);

        if (!closestIntersectionPoint.getHasIntersectionPoint()) {
            return 1.f;
        }
        else {
            return 0.f;
        }
    }

    // for directional light, just check if there's any t s.t p+td has instersection of other objects


    // otherwise, check find t value for the light, and compare with any other v alid t's of objects. If any object's t is smaller than light's t, position is shadow. so return 0.
}

glm::vec4 RayTracer::phong(glm::vec3 position, // world space
                      glm::vec3 normal, // word space
                      glm::vec3 directionToCamera,
                      SceneMaterial &material,
                      std::vector<SceneLightData> &lights, // these are in world space
                      const SceneGlobalData &globalData,
                      const std::vector<RenderShapeData> &shapes,
                      IntersectionPoint intersectionpoint,
                      int &maxRecursion
                      )
{

    // texture
    glm::vec4 textureColorFloatNew;
    if(m_config.enableTextureMap) {
        textureColorFloatNew = findTextureColor(intersectionpoint);
    }
    // Normalizing directions
    normal            = glm::normalize(normal);
    directionToCamera = glm::normalize(directionToCamera);

    // Output illumination (we can ignore opacity)
    glm::vec4 illumination(0, 0, 0, 1);

    // for spot light
    SceneColor intensity(0,0,0,1);

    // Task 3: add the ambient term
    int rgb = 3;
    for (int i = 0; i < rgb; i++) {
        illumination[i] += globalData.ka * material.cAmbient[i]; // ambient is vec4
    }

    float fAtt; //was 0.f
    float dotProduct;
    glm::vec3 pointToLight; // should be "normalized" direction from point to light


    // for soft shadow
    if (m_config.enableSoftShadow) {
        SceneLightData myLight = makeMyLight();
        lights.push_back(myLight);
    }

    for (const SceneLightData &light : lights) {

        float distanceLightToPoint = glm::distance(glm::vec4{position,1.f}, light.pos);


        if (light.type == LightType::LIGHT_DIRECTIONAL)
            fAtt = 1.f;

        else {
            fAtt = glm::min(1.0, 1.0 / (light.function[0] + distanceLightToPoint * light.function[1] + pow(distanceLightToPoint,2) * light.function[2]));}

        if (light.type == LightType::LIGHT_POINT)
            pointToLight = glm::normalize(light.pos - glm::vec4{position, 1.f}); // from position to light source vector

        if (light.type == LightType::LIGHT_DIRECTIONAL)
            pointToLight = -light.dir;

        if (light.type == LightType::LIGHT_SPOT){
            pointToLight = glm::normalize(light.pos - glm::vec4{position, 1.f});
            intensity = calculateSpotLightIntensity(position, light);
        } else { // otherwise, use given light's color.
            intensity = light.color;
        }


        /***********************SHADOW***************************/
        // check that if a ray from "position" to light.pos intersect any other objects before the ray hits the light
        // do I have to loop over all other objects?. yees and find if there's any valid t's ,i.e, t's that satisfy impllicit equation. If so. intersection point in in shadow
        //IntersectionPoint intersectionpoint has the info of current shape. Current shape here is the one that has intersection with current ray
        float V;
        /** Soft Shadow start **/
        if (m_config.enableSoftShadow) {
            if(light.type!= LightType::LIGHT_AREA)
                 V = checkShadow(light, position, shapes, intersectionpoint); // if position is not in shadow, V = 1. Otherwise 0
            else {

                 // added myLight to light vector before the loop
                 V = softShadow(light, position, shapes, intersectionpoint);

            }
         /** Soft Shadow end **/

        } else {
            if(m_config.enableShadow)
                V = checkShadow(light, position, shapes, intersectionpoint);
            else V = 1.f;
        }


        // get pointToLight vector and
        pointToLight = glm::normalize(pointToLight);
        dotProduct = glm::dot(normal, pointToLight); // remember to normalize before doing dot product
        if (dotProduct < 0) { // normal vector is facing away from light source
            dotProduct = 0;
        }

        // for texture
        float blend = intersectionpoint.getShape().primitive.material.blend;
        if(!intersectionpoint.getShape().primitive.material.textureMap.isUsed) {
            blend = 0.f;
        }

        // diffuse term
        if (m_config.enableTextureMap) {
            for (int i = 0; i < rgb; i++){
                illumination[i] += V*fAtt *
                        intensity[i] *
                        ((blend * textureColorFloatNew[i]) +
                        ((1.0 - blend) * (globalData.kd * material.cDiffuse[i]))) *
                        dotProduct;
            }
        } else {
            for (int i = 0; i < rgb; i++){
                illumination[i] += V*fAtt *
                        intensity[i] *
                        (globalData.kd * material.cDiffuse[i]) *
                        dotProduct;
            }
        }
        /****************************Calculate specular term*******************************/
        glm::vec3 positionToLight;
        // Task 5, task 6: add the specular term
        // get reflecedDir: reflected direction of incoming light about surface normal
        if (light.type == LightType::LIGHT_SPOT) {
            positionToLight = glm::normalize(-light.dir);
        }


        // when light is point light
        if (light.type == LightType::LIGHT_POINT)
        // normalize direction
           positionToLight = glm::normalize(light.pos - glm::vec4{position,1}); // from light source to position vector


        // when light is directional light
        if (light.type == LightType::LIGHT_DIRECTIONAL)
           positionToLight = glm::normalize(-light.dir);

        float specDotProduct = glm::dot(positionToLight, normal);
        // if normal vector is facing away l, no light relfect
        if (specDotProduct < 0)
            specDotProduct = 0; //  was used to calculate reflectedDir but I used glm::reflect instead

        glm::vec3 reflectedDir = glm::normalize(glm::reflect(-positionToLight, normal));

        // specular term
        float RV = glm::dot(reflectedDir,directionToCamera);
        if (RV < 0)
            RV = 0.f;

        for (int i = 0; i < rgb; i++) {
            illumination[i] += V*fAtt *
                    intensity[i] *
                    globalData.ks *
                    material.cSpecular[i] *
                    pow(RV, material.shininess);
        }
    }// for all lights

    /** Refraction  Start **/
    glm::vec4 refractIllumination;
    if (m_config.enableRefraction){
        if(intersectionpoint.getShape().primitive.material.ior == 0.0) {
            refractIllumination = glm::vec4(0.f, 0, 0, 1);
        }
        refractIllumination = calculateRefractionIllumination(intersectionpoint, directionToCamera, position, illumination, maxRecursion);

        illumination += refractIllumination;    }
    /** Refraction  End **/

    if (m_config.enableReflection) {
        // get reflection illumination term
        glm::vec4 reflectIllumination = calculateReflectionIllumination(intersectionpoint,
                                        maxRecursion,
                                        directionToCamera);

        illumination += globalVarGlobalData.ks * reflectIllumination;
    }


    return illumination;

} // phong

float RayTracer::softShadow(SceneLightData myLight,
                      glm::vec3 intersectionPositionWorldSpace,
                      const std::vector<RenderShapeData> &shapes,
                      IntersectionPoint intersectionPoint) {


    glm::vec4 fromPositionToLightInWorldSpace;


    srand(time(NULL));
    int width = (rand() % int(myLight.width));
    int height = (rand() % int(myLight.height));
    myLight.pos.x += width;
    myLight.pos.z += height;

    fromPositionToLightInWorldSpace = glm::normalize(myLight.pos - glm::vec4(intersectionPositionWorldSpace,1));
    glm::mat4 currentShapeCTM = intersectionPoint.getCTM();
    glm::vec4 fromPositionToLightInObjectSpace = intersectionPoint.getInverseCTM() * fromPositionToLightInWorldSpace;
    glm::vec4 intersectionPositionObjectSpace = intersectionPoint.getInverseCTM() * glm::vec4(intersectionPositionWorldSpace,1);

    float tForLight;

    tForLight = (myLight.pos.x - intersectionPositionObjectSpace.x) / fromPositionToLightInObjectSpace.x;

    // this variable is to avoid self-shadowing
    glm::vec4 intersectionPositionWorldSpaceEpsilon(glm::vec4(intersectionPositionWorldSpace,1) + fromPositionToLightInWorldSpace*0.0001f);// check this line

    std::vector<IntersectionPoint> intersectionPoints = loopShapes(fromPositionToLightInWorldSpace, shapes, intersectionPositionWorldSpaceEpsilon);// fromPositionToLightInWorldspace is my d and intersectionPositionWorldspace is my P

    std::tuple<int, IntersectionPoint> IdxAndintersectionPoint = findTValueAmongAllObjects(intersectionPoints);
    int index = std::get<0>(IdxAndintersectionPoint);
    IntersectionPoint closestIntersectionPoint = std::get<1>(IdxAndintersectionPoint);

    // if intersectionPoint is false.. then intersectionPoint here is just first element of
    // vector intersectionPoints.
    // this means no other objects inbetween given intersectionPoint and light source. So not shadow
    if (!closestIntersectionPoint.getHasIntersectionPoint()) {
        return 1.f;
    }
    /**Version 1**/
    else { // has other objects inbetween. but need to compare tObject with tLight
        // here I'm assuming that no objects are beyond light source
//            return 0.f;

        float tObject = closestIntersectionPoint.getTValue();

        float pointLightDis = glm::distance(myLight.pos,glm::vec4(intersectionPositionWorldSpace,1));

        glm::vec4 objPosInWorld = closestIntersectionPoint.getCTM() * closestIntersectionPoint.getIntersectionPoint();
        float pointLObjectDis = glm::distance(objPosInWorld, glm::vec4(intersectionPositionWorldSpace,1));
//            std::cout << "tForLight: " << tForLight << " " << "tForObject" << tObject << std::endl;
        if (pointLightDis <= pointLObjectDis) {

            return 1.f; // need to check this part. when I used 1.f, some shadows are missing
        } else return 0.f;
    }
}

SceneLightData RayTracer::makeMyLight() {
    SceneLightData myLight{-1,
                   LightType::LIGHT_AREA,
                   SceneColor{0.7, 0.7, 0.7, 1},
                   glm::vec3{0.8, 0.5, 0},
                   glm::vec4{0.f, 4, 0, 1},//position
                   glm::vec4{0,0,0,0},
                   0.f,
                   0.f,
                   10.f,
                   10.f
                  };
    return myLight;
}

glm::vec4 RayTracer::calculateReflectionIllumination(IntersectionPoint intersectionpoint,
                                    int &maxRecursion,
                                    glm::vec3 directionToCamera
                                    ) {
    glm::vec4 reflectionIllumination;
    // we only run findReflectionIllumination with valid intersectionpoint. If no intersectionpoint exits, we don't run the function.
    glm::vec4 currentPointReflectMaterialValue = intersectionpoint.getShape().primitive.material.cReflective;
    // base cases
    // base case 1. check if current shape's material is shiny
    // if current material is not shiny , just return current shape's color
    if (currentPointReflectMaterialValue.r == 0.0 &&
            currentPointReflectMaterialValue.g == 0.0 &&
            currentPointReflectMaterialValue.b == 0.0) { // for ex, if red sphere isn't shiny surface, no reflection from other objects
         return glm::vec4(0,0,0.f,1);
    }

    if (maxRecursion < 4) {
        // find parameters for phong of next intersection point
        // find a reflected ray (P from P + td) that starts from current intersection.
        glm::vec4 incomingRayTowardIntersectionObjectSpace = intersectionpoint.getInverseCTM() * glm::vec4(-directionToCamera,0);
        glm::vec4 objectSpaceNormal = intersectionpoint.getNormalInObjectSpace();
        glm::mat4 TransFromObjToWorldMat4 = intersectionpoint.getCTM();
        // get reflected direction. from current position to outgoing reflected ray
            // will use this as d.
        glm::vec4 reflectedRayAtCurrentPositionObjectSpace = glm::reflect(incomingRayTowardIntersectionObjectSpace, objectSpaceNormal);
        glm::vec4 reflectedRayAtCurrentPositionWorldSpaceVec4 = TransFromObjToWorldMat4 * reflectedRayAtCurrentPositionObjectSpace;

        glm::vec4 currentIntersectionPositionInObjectEpsilon = intersectionpoint.getIntersectionPoint() + reflectedRayAtCurrentPositionObjectSpace * glm::vec4(0.001, 0.001, 0.001, 1);
        glm::vec4 currentIntersectionPositionInWorldEpsilon = TransFromObjToWorldMat4 * currentIntersectionPositionInObjectEpsilon; // this is P + epsilon*d
        // use world spcae variables. loopshapes will convert into object space.
        std::vector<IntersectionPoint> intersectionPoints = loopShapes(reflectedRayAtCurrentPositionWorldSpaceVec4, shapes, currentIntersectionPositionInWorldEpsilon); //position is world space
        std::tuple<int, IntersectionPoint> IdxAndintersectionPoint = findTValueAmongAllObjects(intersectionPoints);
        IntersectionPoint nextIntersectionPoint = std::get<1>(IdxAndintersectionPoint);

        if (!nextIntersectionPoint.getHasIntersectionPoint()) {
            return glm::vec4(0,0,0,1.f);
        }

        glm::vec4 nextPosition = nextIntersectionPoint.getIntersectionPoint();

        // transform normal from object space to world's
        glm::vec3 nextWorldSpaceNormalVec3 = glm::inverse(glm::transpose(nextIntersectionPoint.getCTM())) * nextIntersectionPoint.getNormalInObjectSpace();;
        glm::vec4 nextWorldspaceNormalVec4{nextWorldSpaceNormalVec3, 0};
        nextWorldspaceNormalVec4 = glm::normalize(nextWorldspaceNormalVec4);

        maxRecursion++;

        SceneMaterial nextMaterial = nextIntersectionPoint.getShape().primitive.material;
        reflectionIllumination = currentPointReflectMaterialValue /*(현재 material의))*/ * phong(nextPosition,
                                                                                                nextWorldspaceNormalVec4,
                                                                                                -reflectedRayAtCurrentPositionWorldSpaceVec4,
                                                                                                nextMaterial,
                                                                                                lights,
                                                                                                globalVarGlobalData,
                                                                                                shapes,
                                                                                                nextIntersectionPoint,
                                                                                                maxRecursion);
        return reflectionIllumination;
    }
    else // maxRecursion exceeded
        return glm::vec4(0,0,0,1.f);
}


glm::vec4 RayTracer::calculateRefractionIllumination(IntersectionPoint intersectionpoint,
                                                     glm::vec3 directionToCamera,
                                                     glm::vec3 position,
                                                     glm::vec4 illumination,
                                                     int &maxRecursion){


    glm::vec4 refractIllumination(0.f,0,0,1);
    SceneColor cTransparent = intersectionpoint.getShape().primitive.material.cTransparent;

    glm::vec4 firstRefractedRayObjectSpace = calculateFristRefractedRay(-(glm::vec4(directionToCamera,0)), intersectionpoint); // -directionToCamera is used for incomingRay
    glm::vec4 firstRefractedRayWorlSpace = intersectionpoint.getCTM() * firstRefractedRayObjectSpace;

    // find next intersection point
    // use world spcae variables. loopshapes will convert into object space.

    IntersectionPoint nextIntersectionpoint = findNextIntersectionPoint(firstRefractedRayWorlSpace, glm::vec4(position,0));
    // this happens when current intersectionpoint (before nextIntersectionpoint) is at tangent of shape
    if (!nextIntersectionpoint.getHasIntersectionPoint()) {
        refractIllumination = glm::vec4(0.f,0,0,1);
    } else {
        glm::vec4 secondRefractedRayObjectSpace = calculateSecondRefractedRay(firstRefractedRayObjectSpace, nextIntersectionpoint);

        glm::vec4 secondRefractedRayWorldSpace = nextIntersectionpoint.getCTM() * secondRefractedRayObjectSpace;

        IntersectionPoint nextNextIntersectionpoint = findNextIntersectionPoint(secondRefractedRayWorldSpace, nextIntersectionpoint.getIntersectionPoint());
        if (!nextNextIntersectionpoint.getHasIntersectionPoint()) {
            refractIllumination = glm::vec4(0.f,0,0,1);
        } else {

            // find illumination of object that reflected ray intersects
            glm::vec4 nextNextIntersectionPositionWorldSpace =  nextNextIntersectionpoint.getCTM()*nextNextIntersectionpoint.getIntersectionPoint();

            // transform normal from object space to world's
            glm::vec3 nextNextWorldSpaceNormalVec3 = glm::inverse(glm::transpose(nextNextIntersectionpoint.getCTM())) * nextNextIntersectionpoint.getNormalInObjectSpace();;
            glm::vec4 nextNextWorldspaceNormalVec4{nextNextWorldSpaceNormalVec3, 0};
            nextNextWorldspaceNormalVec4 = glm::normalize(nextNextWorldspaceNormalVec4);

            SceneMaterial material = nextNextIntersectionpoint.getShape().primitive.material;
            glm::vec4 illuminationOfAnotherObject = phong(nextNextIntersectionPositionWorldSpace,
                                                nextNextWorldspaceNormalVec4,
                                                secondRefractedRayWorldSpace,
                                                material,
                                                lights,
                                                globalVarGlobalData,
                                                shapes,
                                                nextNextIntersectionpoint,
                                                maxRecursion);

            refractIllumination = globalVarGlobalData.kt * ((glm::vec4(1.f,1,1,1) - cTransparent) * illumination + cTransparent * illuminationOfAnotherObject);

        }
    }
    return refractIllumination;
}


IntersectionPoint RayTracer::findNextIntersectionPoint(glm::vec4 incomingRayInWorldspace, glm::vec4 startingPositionInWorldSpace) {
    std::vector<IntersectionPoint> intersectionPoints = loopShapes(incomingRayInWorldspace, shapes, startingPositionInWorldSpace); //position is world space
    std::tuple<int, IntersectionPoint> IdxAndintersectionPoint = findTValueAmongAllObjects(intersectionPoints);
    IntersectionPoint nextIntersectionPoint = std::get<1>(IdxAndintersectionPoint);
    return nextIntersectionPoint;
}


glm::vec4 RayTracer::calculateFristRefractedRay(glm::vec4 incomingRay, IntersectionPoint intersectionpoint) {
    // find an incident vector
    glm::vec4 incomingRayTowardIntersectionObjectSpace = intersectionpoint.getInverseCTM() * glm::vec4(incomingRay);
    // find surface normal at refraction point
    glm::vec4 objectSpaceNormal = intersectionpoint.getNormalInObjectSpace();
    // find shape's refraction index
    float ior = intersectionpoint.getShape().primitive.material.ior;
    // get first refracted ray
    glm::vec4 firstRefractedRayObjectSpace = glm::refract(incomingRayTowardIntersectionObjectSpace, objectSpaceNormal, 1.0f/ior); // source material index of refraction (IOR)' / 'destination material IOR
    return firstRefractedRayObjectSpace;
}

glm::vec4 RayTracer::calculateSecondRefractedRay(glm::vec4 incomingRay, IntersectionPoint intersectionpoint) {
    // find an incident vector
    glm::vec4 incomingRayTowardIntersectionObjectSpace = intersectionpoint.getInverseCTM() * glm::vec4(incomingRay);
    // find surface normal at refraction point but opposite direction as the ray is inside the shape
    glm::vec4 objectSpaceNormal = -intersectionpoint.getNormalInObjectSpace();
    // find shape's refraction index
    float ior = intersectionpoint.getShape().primitive.material.ior;
    // get first refracted ray
    glm::vec4 firstRefractedRayObjectSpace = glm::refract(incomingRayTowardIntersectionObjectSpace, objectSpaceNormal, 1.0f/ior); // source material index of refraction (IOR)' / 'destination material IOR
    return firstRefractedRayObjectSpace;
}


// using intersectionPoint in object space, find u,v in uv coord and using (u,v) find (c, r) in texture map
glm::vec4 RayTracer::findTextureColor(IntersectionPoint intersectionPoint) {
//    std::tuple<float, float> uv = findUVCoord(intersectionPoint);
//    std::tuple<float, float> uv = intersectionPoint.getUV();

//    float u = std::get<0>(uv);
//    float v = std::get<1>(uv);

    float u = intersectionPoint.getU();
    float v = intersectionPoint.getV();

    int textureCol, textureRow;

    float repeatU = intersectionPoint.getShape().primitive.material.textureMap.repeatU;
    float repeatV = intersectionPoint.getShape().primitive.material.textureMap.repeatV;

    int shapeIndex = intersectionPoint.getShapeIndex();
    TextureMap textureMap = textureMaps[shapeIndex];
    std::vector<RGBA> *textureImage = textureMap.getTextureMapsRGBA();
    int textureHeight = textureMap.getTextureMapHeight();
    int textureWidth = textureMap.getTextureMapWidth();

    if (intersectionPoint.getShape().primitive.material.textureMap.isUsed) {
        textureCol = int(floor(u * repeatU * textureWidth)) % textureWidth;
        textureRow = int(floor((1-v) * repeatV * textureHeight)) % textureHeight;
    } else {
        return glm::vec4{0,0,0, 1.f};
    }

    if (u == 1.f) {
        textureCol = textureWidth - 1;
    }

    if (v == 0.f) {
        textureRow = textureHeight - 1;
    }
    RGBA textureColor = (*textureImage)[textureCol + textureWidth * textureRow];

    glm::vec4 textureColorFloatNew = glm::vec4(textureColor.r / 255.f, textureColor.g / 255.f, textureColor.b / 255.f, 1.f);
    return textureColorFloatNew;
}

