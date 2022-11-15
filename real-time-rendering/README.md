# Projects 5 & 6: Lights, Camera & Action!

All project handouts can be found [here](https://cs1230.graphics/projects).


# Extra credits.
Distance from the object to the camera: search for "//extra credit 2"
I used settings.extracredit2.
I calculated distance using camera's position and the center of object, both in the world space. As the range of paramters (0~25) and that of distance (0~100) are significantly different, I rescaled distance by mulitplying (5 / 100). So, I divide ths parameters by (distance * 5 / 100).
    
The number of objects in the scene: search for "//extra credit 1"
I used settings.extracredit1.
To make sure the difference is visible, I multiplied parameters by 2 and then divided them by the number of objects in the scene.

