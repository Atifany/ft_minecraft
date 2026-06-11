#include "../include/Input.h"

Input::Input()
{
	this->keysPressed.insert(std::pair(-1, -1)); // init with one unused state to keep filled
	this->mousePos = std::make_pair(SRC_WIDTH / 2, SRC_HEIGHT / 2);
}

void Input::UpdateKeysFromInput(int key, int action)
{
	if (action == GLFW_PRESS)
	{
		if (this->keysPressed.count(key) == 0)
			this->keysPressed.insert(std::pair(key, I_KEY_SINGLE_PRESS));
	}
	if (action == GLFW_RELEASE)
	{
		if (this->keysPressed.count(key) != 0)
			this->keysPressed[key] = I_KEY_RELEASED;
	}
}

void Input::UpdateKeys()
{
	// Change single_press state to cont_press
	for (auto& x : this->keysPressed)
	{
		if (x.second == I_KEY_SINGLE_PRESS)
			x.second = I_KEY_HOLD;
	}

	// remove all keys marked with release state
	std::map<int, int>::iterator it = this->keysPressed.begin();
	while (it != this->keysPressed.end())
	{
		if ((*it).second == I_KEY_RELEASED)
			it = this->keysPressed.erase(it);
		else
			it++;
	}
}

int Input::GetKey(int key)
{
	for (auto& x : this->keysPressed)
	{
		if (x.first == key)
			return x.second;
	}
	return I_KEY_NOT_PRESSED;
}

void Input::UpdateMouseFromInput(std::pair<float, float> _mousePos)
{
	this->mousePos = _mousePos;
}

std::pair<float, float> Input::GetMousePos()
{
	return this->mousePos;
}