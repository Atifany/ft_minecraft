#include "../include/Camera.h"

Camera::Camera()
{
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->front = glm::vec3(0.0f, 0.0f, -1.0f);
	this->pos = glm::vec3(0.0f, 0.0f, 3.0f);
	this->speed = 10.0f;
	this->sensetivity = 0.1f;
}
