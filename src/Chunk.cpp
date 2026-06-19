#include "../include/Chunk.h"

void InsertVerteciesToMesh(std::vector<unsigned int> &indices, std::vector<float> &mesh, const Vertex verticies[], size_t verticiesSize, glm::vec3 offset);
Chunk* _FindChunkAtPos(std::vector<Chunk*> chunks, glm::vec3 _pos);

Chunk::Chunk()
{
	this->pos = glm::vec3(0.0f, 0.0f, 0.0f);
	this->VAO = 0;
	this->VBO = 0;
	this->EBO = 0;
	this->isReady = false;
}

Chunk::~Chunk()
{
	// delete active mesh
	glDeleteVertexArrays(1, &(this->VAO)); this->VAO = 0;
	glDeleteBuffers(1, &(this->VBO)); this->VBO = 0;
	glDeleteBuffers(1, &(this->EBO)); this->EBO = 0;
	this->mesh.clear();
}

void Chunk::GenVoxels()
{
	for(int y = 0; y < CHUNK_SIZE; y++)
	{
		for(int z = 0; z < CHUNK_SIZE; z++)
		{
			for(int x = 0; x < CHUNK_SIZE; x++)
			{
				if (y + this->pos.y + (this->pos.z + this->pos.x) / CHUNK_SIZE <= 0)
					this->voxels[y][z][x] = GRASSBLOCK;
				else
					this->voxels[y][z][x] = EMPTYBLOCK;
			}
		}
	}
}

void Chunk::GenMesh(std::vector<Chunk*> chunks)
{
	// if no mesh loaded then load new mesh
	if (this->VAO != 0 || this->mesh.size() != 0)
		return ;

	Chunk* topChunkNeighbour	 = _FindChunkAtPos(chunks, glm::vec3(this->pos.x, this->pos.y + CHUNK_SIZE, this->pos.z));
	Chunk* bottomChunkNeighbour	 = _FindChunkAtPos(chunks, glm::vec3(this->pos.x, this->pos.y - CHUNK_SIZE, this->pos.z));
	Chunk* leftChunkNeighbour	 = _FindChunkAtPos(chunks, glm::vec3(this->pos.x - CHUNK_SIZE, this->pos.y, this->pos.z));
	Chunk* rightChunkNeighbour	 = _FindChunkAtPos(chunks, glm::vec3(this->pos.x + CHUNK_SIZE, this->pos.y, this->pos.z));
	Chunk* frontChunkNeighbour	 = _FindChunkAtPos(chunks, glm::vec3(this->pos.x, this->pos.y, this->pos.z + CHUNK_SIZE));
	Chunk* backChunkNeighbour	 = _FindChunkAtPos(chunks, glm::vec3(this->pos.x, this->pos.y, this->pos.z - CHUNK_SIZE));
	for (int y = 0; y < CHUNK_SIZE; y++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				if (this->voxels[y][z][x] == EMPTYBLOCK)
					continue;
					
				if (y == 0 && bottomChunkNeighbour != NULL && bottomChunkNeighbour->voxels[CHUNK_SIZE - 1][z][x] == EMPTYBLOCK ||
					y != 0 && this->voxels[y-1][z][x] == EMPTYBLOCK)
					InsertVerteciesToMesh(this->indices, this->mesh, verticiesFaceBottom, sizeof(verticiesFaceBottom) / sizeof(verticiesFaceBottom[0]), glm::vec3(x, y, z));
				
				if (y == CHUNK_SIZE - 1 && topChunkNeighbour != NULL && topChunkNeighbour->voxels[0][z][x] == EMPTYBLOCK ||
				y != CHUNK_SIZE - 1 && this->voxels[y+1][z][x] == EMPTYBLOCK)
					InsertVerteciesToMesh(this->indices, this->mesh, verticiesFaceTop, sizeof(verticiesFaceTop) / sizeof(verticiesFaceTop[0]), glm::vec3(x, y, z));
				
				if (z == 0 && backChunkNeighbour != NULL && backChunkNeighbour->voxels[y][CHUNK_SIZE - 1][x] == EMPTYBLOCK ||
				z != 0 && this->voxels[y][z-1][x] == EMPTYBLOCK)
					InsertVerteciesToMesh(this->indices, this->mesh, verticiesFaceBack, sizeof(verticiesFaceBack) / sizeof(verticiesFaceBack[0]), glm::vec3(x, y, z));
				
				if (z == CHUNK_SIZE - 1 && frontChunkNeighbour != NULL && frontChunkNeighbour->voxels[y][0][x] == EMPTYBLOCK ||
				z != CHUNK_SIZE - 1 && this->voxels[y][z+1][x] == EMPTYBLOCK)
					InsertVerteciesToMesh(this->indices, this->mesh, verticiesFaceFront, sizeof(verticiesFaceFront) / sizeof(verticiesFaceFront[0]), glm::vec3(x, y, z));
				
				if (x == 0 && leftChunkNeighbour != NULL && leftChunkNeighbour->voxels[y][z][CHUNK_SIZE - 1] == EMPTYBLOCK ||
				x != 0 && this->voxels[y][z][x-1] == EMPTYBLOCK)
					InsertVerteciesToMesh(this->indices, this->mesh, verticiesFaceLeft, sizeof(verticiesFaceLeft) / sizeof(verticiesFaceLeft[0]), glm::vec3(x, y, z));
				
				if (x == CHUNK_SIZE - 1 && rightChunkNeighbour != NULL && rightChunkNeighbour->voxels[y][z][0] == EMPTYBLOCK ||
				x != CHUNK_SIZE - 1 && this->voxels[y][z][x+1] == EMPTYBLOCK)
					InsertVerteciesToMesh(this->indices, this->mesh, verticiesFaceRight, sizeof(verticiesFaceRight) / sizeof(verticiesFaceRight[0]), glm::vec3(x, y, z));
			}
		}
	}
}

void Chunk::GenBuffers()
{
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

	this->isReady = true;
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

Chunk* _FindChunkAtPos(std::vector<Chunk*> chunks, glm::vec3 _pos)
{
	for (Chunk* chunk : chunks)
		if (chunk->pos == _pos)
			return chunk;
	return NULL;
}
