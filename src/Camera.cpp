#include "../include/Camera.h"

Camera::Camera()
{
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->front = glm::vec3(0.0f, 0.0f, -1.0f);
	this->right = glm::cross(this->up, this->front);
	this->pos = glm::vec3(0.0f, 0.0f, 3.0f);
	this->curChunkPos = glm::vec3(INT_MAX, INT_MAX, INT_MAX);
	this->speed = 10.0f;
	this->sensetivity = 0.1f;
	this->isWireFrameModeOn = false;
	this->isInNewChunk = false;

	this->yaw = -90.0f;
	this->pitch = 0.0f;
	this->lastMouseX = SRC_WIDTH / 2;
	this->lastMouseY = SRC_HEIGHT / 2;
}

void Camera::MouseInput(std::pair<float, float> mousePos)
{
	if (std::abs(mousePos.first - this->lastMouseX) > 100.0f)
	{
		this->lastMouseX = mousePos.first;
		this->lastMouseY = mousePos.second;
		return ;
	}

	float xoffset = (mousePos.first - this->lastMouseX) * this->sensetivity;
	float yoffset = (this->lastMouseY - mousePos.second) * this->sensetivity;
	this->lastMouseX = mousePos.first;
	this->lastMouseY = mousePos.second;

	this->yaw += xoffset;
	this->pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	this->front = glm::normalize(direction);
	this->right = glm::cross(this->up, this->front);
}

void Camera::UpdateChunkPos()
{
	this->isInNewChunk = false;
	glm::vec3 prevCameraChunkPos = this->curChunkPos;
	glm::vec3 tmpPos = glm::vec3(std::floor(this->pos.x / CHUNK_SIZE),
								std::floor(this->pos.y / CHUNK_SIZE),
								std::floor(this->pos.z / CHUNK_SIZE));
	if (tmpPos.x != this->curChunkPos.x ||
		tmpPos.y != this->curChunkPos.y ||
		tmpPos.z != this->curChunkPos.z)
	{
		std::cout << "Update curChunkPos from " << this->curChunkPos.x << "x " << this->curChunkPos.z << "z to ";
		this->curChunkPos = tmpPos;
		std::cout << this->curChunkPos.x << "x " << this->curChunkPos.z << "z\n";
		this->isInNewChunk = true;
	}
}
