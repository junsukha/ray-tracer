# Project 3: Intersect

Welcome to Project 3! The handout for the project is located [here](https://browncsci1230.github.io/projects/ray/1).

For each pixel in 2D image, I do this:
1. Find a corresponding ray and loop through all shapes.
2. Compare T values starting from INFINITY. So you get only one T from each shape at maximum.
3. After loop is done, compare T values from all objjets and pick smallest T again. 
4. With that T, use intersection point, normal to find lights on that point.

All tasks for  normal credits work.


# Project 4: Illumination
All tasks for  normal credits work.

Extra credit.
- created my own scenefile. snowman.xml
- adaptive super sampling: Search for m_config.enableSuperSample. I shoot rays at for corners of a pixel. I get color from each of them. I get fiver colors per pixel. I average them and set the average as my pixel's final color. But if colors of four corners are significantly different, I divide the pixel into quadrants and shoot 4 rays for each quadrant and do the same thing. I used "goto:" and "again" to send the process back to the beginning to add more coordinates. To calculte the differences of colors of four corners, I used Euclidean distance of r,g,b.

- BVH tree: bvhtree.cpp, bvhtree.h

- Refraction: Code in between /** Refraction  Start **/ ~ /** Refraction  end **/ in raytracer.cpp. 
              I implemented refraction (and transparency) algorithm using Snell's law. I used glm::refract twice for two intersections on one shape. 
              First one handles first intersection with the shape from the outside towards it, and the second handles from inside of the shape towards going                  
              outside. I used the opposite surface normal for the second one.
              
- soft Shadow: code in between /** soft shadow start **/ ~ /** soft shadow end **/ in raytracer.cpp. Added myLight to lights vector before looping lights in    
              "phong" function. I used rand() function to sample random point in my rectangle light. I used LightType.Area for this. At the shadow point 
              (doesn't know if it's shadow or not yet), I shoot a ray to a light point where this point is randomly chosen out of my rectangle area light.
              I added
              rtConfig.enableSoftShadow  = settings.value("Feature/softshadow").toBool();  in main.cpp
              and
              softshadow = true in QSettings.ini (didn't submit this one) 
            
