#ifndef CHUNK_H
#define CHUNK_H

#include "ft_minecraft.h"

class Chunk
{
	public:
		Chunk();
		glm::vec3 pos;
		unsigned int voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

		std::vector<float> mesh;
		unsigned int VAO;
		

		void Load();

	private:
};

#endif