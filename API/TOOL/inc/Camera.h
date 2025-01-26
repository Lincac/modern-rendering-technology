#pragma once

#include <iostream>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

class Camera
{
public:

    Camera(glm::vec3 pos = glm::vec3(0,0,3));

    glm::mat4 getviewmatrix();

    glm::mat4 getprojmatrix(float aspect);

    glm::vec3 focal;    
    glm::vec3 position;
    glm::vec3 up;

    float fov;
    float near_clip;
    float far_clip;

};