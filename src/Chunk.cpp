#include "../include/Chunk.h"

struct Vertex
{
	glm::vec3 pos;
	glm::vec2 tex;
};

void InsertVerteciesToMesh(std::vector<float> &mesh, Vertex verticies[], size_t verticiesSize, glm::vec3 offset);

Chunk::Chunk()
{
	this->pos = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Chunk::Load()
{
	for(int i = 0; i < CHUNK_SIZE; i++)
	{
		for(int j = 0; j < CHUNK_SIZE; j++)
		{
			for(int k = 0; k < CHUNK_SIZE; k++)
			{
				if (i < CHUNK_SIZE / 2)
					this->voxels[i][j][k] = GRASSBLOCK;
				else
					this->voxels[i][j][k] = EMPTYBLOCK;
			}
		}
	}

	Vertex verticies[36] = {
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},

		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},

		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},

		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},

		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},

		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 0.0f)}
	};

	InsertVerteciesToMesh(this->mesh, verticies, sizeof(verticies) / sizeof(verticies[0]), glm::vec3(0.0f, 0.0f, 0.0f));
	InsertVerteciesToMesh(this->mesh, verticies, sizeof(verticies) / sizeof(verticies[0]), glm::vec3(1.0f, 1.0f, 1.0f));
	std::cout << this->mesh.size() << "\n";

	unsigned int VBO;
	glGenVertexArrays(1, &(this->VAO));
	glGenBuffers(1, &VBO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, this->mesh.size() * sizeof(this->mesh[0]), this->mesh.data(), GL_STATIC_DRAW);

	// setting position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// setting texture coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void InsertVerteciesToMesh(std::vector<float> &mesh, Vertex verticies[], size_t verticiesSize, glm::vec3 offset)
{
	for (int i = 0; i < verticiesSize; i++)
	{
		mesh.insert(mesh.end(), verticies[i].pos.x + offset.x);
		mesh.insert(mesh.end(), verticies[i].pos.y + offset.y);
		mesh.insert(mesh.end(), verticies[i].pos.z + offset.z);
		mesh.insert(mesh.end(), verticies[i].tex.x);
		mesh.insert(mesh.end(), verticies[i].tex.y);
	}
}
