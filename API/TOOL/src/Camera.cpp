#include "Camera.h"

Camera::Camera(glm::vec3 pos)
    : position(pos)
{
    focal = glm::vec3(0);
    up = glm::vec3(0,1,0);

    fov = 60.0f;    
    near_clip = 0.1f;
    far_clip = 1000.0f;
}

glm::mat4 Camera::getviewmatrix()
{
    return glm::lookAt(position, focal, up);
}

glm::mat4 Camera::getprojmatrix(float aspect)
{
    return glm::perspective(glm::radians(fov), aspect, near_clip, far_clip);
}
