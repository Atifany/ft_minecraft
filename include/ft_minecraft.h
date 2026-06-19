#ifndef FT_MINECRAFT_H
#define FT_MINECRAFT_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define CHUNK_SIZE 32
#define VERTICIES_IN_FACE_RAW 4
#define VERTICIES_IN_FACE_DRAWN 6
#define ITEMS_IN_VERTEX 5
#define CHUNK_RENDER_DIST 16

#define EMPTYBLOCK 0
#define GRASSBLOCK 1

const unsigned int SRC_WIDTH = 1920;
const unsigned int SRC_HEIGHT = 1080;
const unsigned int CUBE_VERTICIES_COUNT = 36;

struct Vertex
{	
	glm::vec3 pos;
	glm::vec2 tex;
};

const Vertex verticies[36] = {
//  |----coordinate----| |-texture--|
	// Back face
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},
	{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},
	{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
	// Front face
	{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
	{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f)},
	{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
	// Left face
	{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
	{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
	{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
	// Right face
	{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
	{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
	{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
	// Bottom face
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
	{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
	// Top face
	{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
	{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
	{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 0.0f)}
};
const unsigned int indicesFace[6] = {
	0, 1, 2,
	2, 3, 0
};
const Vertex verticiesFaceTop[4] = {
	{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f)},	// top-right
	{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},	// top-left
	{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},	// bottom-left
	{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},	// bottom-right
};
const Vertex verticiesFaceBottom[4] = {
	{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 1.0f)},	// top-right
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},	// top-left
	{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},	// bottom-left
	{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},	// bottom-right
};
const Vertex verticiesFaceFront[4] = {
	{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f)},	// top-right
	{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},	// top-left
	{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},	// bottom-left
	{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},	// bottom-right
};
const Vertex verticiesFaceBack[4] = {
	{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},	// top-right
	{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},	// top-left
	{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},	// bottom-left
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},	// bottom-right
};
const Vertex verticiesFaceLeft[4] = {
	{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f)},	// top-right
	{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},	// top-left
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},	// bottom-left
	{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},	// bottom-right
};
const Vertex verticiesFaceRight[4] = {
	{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},	// top-right
	{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},	// top-left
	{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},	// bottom-left
	{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},	// bottom-right
};

#endif