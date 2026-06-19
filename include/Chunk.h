#ifndef CHUNK_H
#define CHUNK_H

#include "ft_minecraft.h"

class Chunk
{
	public:
		Chunk();
		~Chunk();
		void GenVoxels();
		void GenMesh(std::vector<Chunk*> chunks);
		void GenBuffers();

		glm::vec3 pos;
		unsigned int voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
		std::vector<unsigned int> indices;
		unsigned int VAO;
		bool isReady;

	private:
		std::vector<float> mesh;
		unsigned int VBO;
		unsigned int EBO;
};

#endif