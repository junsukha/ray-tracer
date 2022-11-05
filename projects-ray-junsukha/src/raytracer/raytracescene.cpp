#include <stdexcept>
#include "raytracescene.h"


// why do I have to initialize camera in initialization list
RayTraceScene::RayTraceScene(int width, int height, const RenderData &metaData) /*:*/ /*camera(cameraData.pos,
                                                                                         cameraData.look,
                                                                                         cameraData.up,
                                                                                         cameraData.heightAngle,
                                                                                         cameraData.aperture,
                                                                                         cameraData.focalLength,
                                                                                         width/float(height))*/{
    // Optional TODO: implement this. Store whatever you feel is necessary.

    imageWidth = width;
    imageHeight = height;
    cameraData = metaData.cameraData;
    globalData = metaData.globalData;
    lights = metaData.lights;
    shapes = metaData.shapes;

    // make camera instance here?
//    float aspectRatio = imageWidth / float(imageHeight);
//    camera = Camera(cameraData.pos,
//                    cameraData.look,
//                    cameraData.up,
//                    cameraData.heightAngle,
//                    cameraData.aperture,
//                    cameraData.focalLength,
//                    aspectRatio);

//    Camera(glm::vec4 pos, glm::vec4 look, glm::vec4 up, float heightAngle, float aperture, float focalLength, int width, int height)
}

const std::vector<SceneLightData> RayTraceScene::getLights() const {
    return lights;
}

const std::vector<RenderShapeData> RayTraceScene::getShapes() const {
    return shapes;
}

const int& RayTraceScene::width() const {
    // Optional TODO: implement the getter or make your own design
    return imageWidth;
//    throw std::runtime_error("not implemented");
}

const int& RayTraceScene::height() const {
    // Optional TODO: implement the getter or make your own design
    return imageHeight;
//    throw std::runtime_error("not implemented");
}

const SceneGlobalData& RayTraceScene::getGlobalData() const {
    // Optional TODO: implement the getter or make your own design
    return globalData;
//    throw std::runtime_error("not implemented");
}

const SceneCameraData &RayTraceScene::getCameraData() const {
    // Optional TODO: implement the getter or make your own design
    return cameraData;
//    throw std::runtime_error("not implemented");
}


//const Camera& RayTraceScene::getCamera() const {
//    /*
//    // Optional TODO: implement the getter or make your own design
//    // 맨처음 function에서 Camera instance 만들어서 camera 정보 저장한다음에 private variable 만들어서 여기서 그걸 가져오는건가
//    // 아니면 예를 들어 getViewMatrix() 경우, Camera camera에서 camera.getViewMatrix(view matrix 구하기 위해 필요한 정보 parameters) 넣고 바로 return.
//    // 그러면 Camera class 안에 private variables 만들필요 x
//    */

////    throw std::runtime_error("not implemented");
//    return camera;
//}
