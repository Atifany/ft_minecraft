#include "../include/Chunk.h"

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
}
