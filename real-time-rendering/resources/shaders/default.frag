#version 330 core
/** fragment shader colors each vertex**/

//struct GlobalSceneData
//{
//  float k_a;
//  float k_d;
//  float k_s;
//  float shininess;
//  vec4 worldSpaceLightPosition;
//  vec4 worldSpaceCameraPosition;

//};

struct Light{
    vec3 function;
    vec3 color;
    vec4 position; // world space
    bool isDirectional;
    vec4 direction;
    float penumbra;
    float angle;
};

//uniform int numOfLights;
//uniform GlobalSceneData globalSceneData;
//uniform GlobalSceneData lights[numOfLights];

uniform Light lights[8];
int numOfLights = 8;
uniform int currentNumOfLights;

// variables I need to get from vertex shader. Variables that are different for each vertex
in vec4 worldSpacePosition;
in vec4 worldSpaceNormal;

uniform vec4 worldSpaceCameraPosition;
// not sure
// since, although k_a is the same across vertices, cAmbient for each objects is different. So can't sent this as uniform. should come from vertex shader
//

out vec4 fragColor;

/** uniform varialbes are the same for all vertices **/

// ambient lighting scene coefficient
uniform float k_a;

// diffuse lighting scene coefficient
uniform float k_d;

// specular lighting scene coefficient
uniform float k_s;

// vary by object. but same for all vertices
uniform float shininess;
uniform vec3 o_a;
uniform vec3 o_d;
uniform vec3 o_s;
uniform vec3 function;

// although it vaires for each light, it's the same for all vertices for each light
// there will be several uniform light position given
uniform vec4 worldSpaceLightPosition;
uniform vec4 directionLight; // direction of directional light

uniform vec4 worldspaceCameraPosition;


uniform bool isDirectionLight;
void main() {
    // Remember that you need to renormalize vectors here if you want them to be normalized

    // Task 10: set your output color to white (i.e. vec4(1.0)). Make sure you get a white circle!
    fragColor = vec4(1.0);
    // Task 11: set your output color to the absolute value of your world-space normals,
    //          to make sure your normals are correct.
    fragColor = abs(worldSpaceNormal);
    fragColor = vec4(0.0);
    // Task 12: add ambient component to output color
    fragColor = vec4(vec3(k_a),1.f) * vec4(o_a,1.f);


    vec4 fromSurfaceToLightWorldSpace;
    float distance;
    float ftt;

    for (int i = 0; i < numOfLights; ++i) {
        if (!lights[i].isDirectional) { // light is not directional light
            fromSurfaceToLightWorldSpace = lights[i].position - worldSpacePosition;

            // get distance from surface to light for attenuation
            distance = length(lights[i].position - worldSpacePosition);
            ftt = min(1.f, 1.f/(lights[i].function.x + distance * lights[i].function.y + pow(distance,2) * lights[i].function.z));
        } else { // light is directional light
            fromSurfaceToLightWorldSpace = -lights[i].direction;
            ftt = 1.f;
        }

        // Task 13: add diffuse component to output color
        // since light position of light direction is the same for all vertices,
        float diffuse = clamp(dot(normalize(worldSpaceNormal), normalize(fromSurfaceToLightWorldSpace)), 0.f, 1.f);
    //    fragColor += k_d * clamp(diffuse, 0.f, 1.f);
        fragColor += ftt * vec4(lights[i].color,1) * vec4(o_d,1) * vec4(vec3(k_d * diffuse), 0.f);

        // Task 14: add specular component to output color
    //    vec4 cameraPosition = inverse(viewMatrix) * vec4(0,0,0,1.f);
        vec4 fromSurfaceToCameraWorldSpace = normalize(worldSpaceCameraPosition - worldSpacePosition);
        // reflect(incident vector, surface normal vector).. incident vector = fromLightToSurface
        vec4 reflected = reflect(normalize(-fromSurfaceToLightWorldSpace), normalize(worldSpaceNormal));
        // remember to normalize when using dot product
        float RE = clamp(dot(normalize(reflected), normalize(fromSurfaceToCameraWorldSpace)), 0.f, 1.f);
        fragColor += ftt * vec4(lights[i].color,1) * vec4(o_s,1) * vec4(vec3(k_s * pow(RE, shininess)), 1.f);
    }
//    fragColor[3] = 1;
}
// Task 3: add the ambient term
//    int rgb = 3;
//    for (int i = 0; i < rgb; i++) {
//        illumination[i] += globalData.ka * material.cAmbient[i]; // ambient is vec4
//    }
