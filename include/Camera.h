#ifndef CAMERA_H
#define CAMERA_H

#include "ft_minecraft.h"

class Camera
{
	public:
		Camera();

		glm::vec3 up;
		glm::vec3 front;
		glm::vec3 pos;
		float speed;
		float sensetivity;

	private:
};

#endif