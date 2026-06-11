#ifndef CAMERA_H
#define CAMERA_H

#include "ft_minecraft.h"
#include "cmath"

class Camera
{
	public:
		Camera();
		void MouseInput(std::pair<float, float> mousePos);

		glm::vec3 up;
		glm::vec3 front;
		glm::vec3 pos;
		float speed;
		float sensetivity;
		bool isWireFrameModeOn;

	private:
		float yaw;
		float pitch;
		float lastMouseX;
		float lastMouseY;
};

#endif