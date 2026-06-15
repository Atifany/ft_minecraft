#include "../include/Chunk.h"

void InsertVerteciesToMesh(std::vector<unsigned int> &indices, std::vector<float> &mesh, const Vertex verticies[], size_t verticiesSize, glm::vec3 offset);

Chunk::Chunk()
{
	this->pos = glm::vec3(0.0f, 0.0f, 0.0f);
	this->VAO = 0;
	this->VBO = 0;
	this->EBO = 0;
}

Chunk::~Chunk()
{
	this->SetActive(false);
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

	this->voxels[0][0][0] = GRASSBLOCK;
}

void Chunk::SetActive(bool isActive)
{
	// if no mesh loaded then load new mesh
	if (isActive == true && this->VAO == 0 && this->mesh.size() == 0)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				for (int x = 0; x < CHUNK_SIZE; x++)
				{
					if (this->voxels[y][z][x] == EMPTYBLOCK)
						continue;
					if (y == 0 || y != 0 && this->voxels[y-1][z][x] == EMPTYBLOCK)
						InsertVerteciesToMesh(this->indices, this->mesh, verticiesFaceBottom, sizeof(verticiesFaceBottom) / sizeof(verticiesFaceBottom[0]), glm::vec3(x, y, z));
					if (y == CHUNK_SIZE - 1 || y != CHUNK_SIZE - 1 && this->voxels[y+1][z][x] == EMPTYBLOCK)
						InsertVerteciesToMesh(this->indices, this->mesh, verticiesFaceTop, sizeof(verticiesFaceTop) / sizeof(verticiesFaceTop[0]), glm::vec3(x, y, z));
					if (z == 0 || z != 0 && this->voxels[y][z-1][x] == EMPTYBLOCK)
						InsertVerteciesToMesh(this->indices, this->mesh, verticiesFaceBack, sizeof(verticiesFaceBack) / sizeof(verticiesFaceBack[0]), glm::vec3(x, y, z));
					if (z == CHUNK_SIZE - 1 || z != CHUNK_SIZE - 1 && this->voxels[y][z+1][x] == EMPTYBLOCK)
						InsertVerteciesToMesh(this->indices, this->mesh, verticiesFaceFront, sizeof(verticiesFaceFront) / sizeof(verticiesFaceFront[0]), glm::vec3(x, y, z));
					if (x == 0 || x != 0 && this->voxels[y][z][x-1] == EMPTYBLOCK)
						InsertVerteciesToMesh(this->indices, this->mesh, verticiesFaceLeft, sizeof(verticiesFaceLeft) / sizeof(verticiesFaceLeft[0]), glm::vec3(x, y, z));
					if (x == CHUNK_SIZE - 1 || x != CHUNK_SIZE - 1 && this->voxels[y][z][x+1] == EMPTYBLOCK)
						InsertVerteciesToMesh(this->indices, this->mesh, verticiesFaceRight, sizeof(verticiesFaceRight) / sizeof(verticiesFaceRight[0]), glm::vec3(x, y, z));
				}
			}
		}

		glGenVertexArrays(1, &(this->VAO));
		glGenBuffers(1, &(this->VBO));
		glGenBuffers(1, &(this->EBO));

		glBindVertexArray(this->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->mesh.size() * sizeof(this->mesh[0]), this->mesh.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(this->indices[0]), this->indices.data(), GL_STATIC_DRAW);

		// setting position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// setting texture coordinate attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	// delete active mesh
	else if (isActive == false)
	{
		glDeleteVertexArrays(1, &(this->VAO)); this->VAO = 0;
		glDeleteBuffers(1, &(this->VBO)); this->VBO = 0;
		glDeleteBuffers(1, &(this->EBO)); this->EBO = 0;
		this->mesh.clear();
	}
}

void InsertVerteciesToMesh(std::vector<unsigned int> &indices, std::vector<float> &mesh, const Vertex verticies[], size_t verticiesSize, glm::vec3 offset)
{
	for (int i = 0; i < verticiesSize; i++)
	{
		mesh.insert(mesh.end(), verticies[i].pos.x + offset.x);
		mesh.insert(mesh.end(), verticies[i].pos.y + offset.y);
		mesh.insert(mesh.end(), verticies[i].pos.z + offset.z);
		mesh.insert(mesh.end(), verticies[i].tex.x);
		mesh.insert(mesh.end(), verticies[i].tex.y);
	}
	// gen indices
	int indiceOffset = (((mesh.size() / ITEMS_IN_VERTEX) / VERTICIES_IN_FACE_RAW) - 1) * VERTICIES_IN_FACE_RAW;
	for (int j = 0; j < VERTICIES_IN_FACE_DRAWN; j++)
		indices.insert(indices.end(), indicesFace[j] + indiceOffset);
}
