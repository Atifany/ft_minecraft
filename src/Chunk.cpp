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
				if (i > CHUNK_SIZE / 8 && i < CHUNK_SIZE * 7 / 8 && j > CHUNK_SIZE / 8 && j < CHUNK_SIZE * 7 / 8 && k > CHUNK_SIZE / 8 && k < CHUNK_SIZE * 7 / 8)
					this->voxels[i][j][k] = GRASSBLOCK;
				else
					this->voxels[i][j][k] = EMPTYBLOCK;
			}
		}
	}

	Vertex verticies[36] = {
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

	Vertex verticiesFaceTop[6] = {
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 0.0f)}
	};
	Vertex verticiesFaceBottom[6] = {
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)}
	};
	Vertex verticiesFaceFront[6] = {
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)}
	};
	Vertex verticiesFaceBack[6] = {
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)}
	};
	Vertex verticiesFaceLeft[6] = {
		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)}
	};
	Vertex verticiesFaceRight[6] = {
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)}
	};

	// InsertVerteciesToMesh(this->mesh, verticies, sizeof(verticies) / sizeof(verticies[0]), glm::vec3(0.0f, 0.0f, 0.0f));
	// InsertVerteciesToMesh(this->mesh, verticies, sizeof(verticies) / sizeof(verticies[0]), glm::vec3(1.0f, 1.0f, 1.0f));
	InsertVerteciesToMesh(this->mesh, verticies, sizeof(verticies) / sizeof(verticies[0]), glm::vec3(-1.0f, -1.0f, -1.0f));

	for (int y = 0; y < CHUNK_SIZE; y++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				if (this->voxels[y][z][x] == EMPTYBLOCK)
					continue;
				if (y == CHUNK_SIZE - 1 || y != CHUNK_SIZE - 1 && this->voxels[y+1][z][x] == EMPTYBLOCK)
					InsertVerteciesToMesh(this->mesh, verticiesFaceTop, sizeof(verticiesFaceTop) / sizeof(verticiesFaceTop[0]), glm::vec3(x, y, z));
				if (y == 0 || y != 0 && this->voxels[y-1][z][x] == EMPTYBLOCK)
					InsertVerteciesToMesh(this->mesh, verticiesFaceBottom, sizeof(verticiesFaceBottom) / sizeof(verticiesFaceBottom[0]), glm::vec3(x, y, z));
				if (z == 0 || z != 0 && this->voxels[y][z-1][x] == EMPTYBLOCK)
					InsertVerteciesToMesh(this->mesh, verticiesFaceBack, sizeof(verticiesFaceBack) / sizeof(verticiesFaceBack[0]), glm::vec3(x, y, z));
				if (z == CHUNK_SIZE - 1 || z != CHUNK_SIZE - 1 && this->voxels[y][z+1][x] == EMPTYBLOCK)
					InsertVerteciesToMesh(this->mesh, verticiesFaceFront, sizeof(verticiesFaceFront) / sizeof(verticiesFaceFront[0]), glm::vec3(x, y, z));
				if (x == 0 || x != 0 && this->voxels[y][z][x-1] == EMPTYBLOCK)
					InsertVerteciesToMesh(this->mesh, verticiesFaceLeft, sizeof(verticiesFaceLeft) / sizeof(verticiesFaceLeft[0]), glm::vec3(x, y, z));
				if (x == CHUNK_SIZE - 1 || x != CHUNK_SIZE - 1 && this->voxels[y][z][x+1] == EMPTYBLOCK)
					InsertVerteciesToMesh(this->mesh, verticiesFaceRight, sizeof(verticiesFaceRight) / sizeof(verticiesFaceRight[0]), glm::vec3(x, y, z));
			}
		}
	}

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
