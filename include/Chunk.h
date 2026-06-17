#ifndef CHUNK_H
#define CHUNK_H

#include "ft_minecraft.h"

class Chunk
{
	public:
		Chunk();
		~Chunk();
		void Load();
		void SetActive(std::vector<Chunk*> chunks);

		glm::vec3 pos;
		unsigned int voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
		std::vector<float> mesh;
		std::vector<unsigned int> indices;
		unsigned int VAO;
		bool isActive;

	private:
		unsigned int VBO;
		unsigned int EBO;
};

#endif