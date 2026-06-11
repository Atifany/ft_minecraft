#ifndef FT_MINECRAFT_H
#define FT_MINECRAFT_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define VOXELS_IN_CHUNK 32768
#define CHUNK_SIZE 32

#define EMPTYBLOCK 0
#define GRASSBLOCK 1

const unsigned int SRC_WIDTH = 1920;
const unsigned int SRC_HEIGHT = 1080;
const float RENDER_DIST = 200.0f;
const unsigned int CUBE_VERTICIES_COUNT = 36;

#endif