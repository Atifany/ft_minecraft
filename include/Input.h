#ifndef INPUT_H
#define INPUT_H

#include <map>
#include "ft_minecraft.h"


#define I_KEY_NOT_PRESSED	0
#define I_KEY_SINGLE_PRESS	1
#define I_KEY_HOLD			2
#define I_KEY_RELEASED		3

class Input
{
	public:
		Input();

		void UpdateKeysFromInput(int key, int action); // updates keys from user input
		void UpdateKeys(); // automatically updates states of all remembered keys
		int GetKey(int key); // returns the state of a desired key

		void UpdateMouseFromInput(std::pair<float, float> _mousePos);
		std::pair<float, float> GetMousePos();

	private:
		std::map<int, int> keysPressed; // remembers all keys pressed with a state
		std::pair<float, float> mousePos;
};

#endif