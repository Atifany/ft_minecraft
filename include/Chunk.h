#ifndef CHUNK_H
#define CHUNK_H

#include "ft_minecraft.h"

class Chunk
{
	public:
		Chunk(glm::vec3 _pos);
		~Chunk();
		void GenVoxels();
		void GenMesh(const std::list<Chunk*>& chunks);
		void GenBuffers();

		glm::vec3 pos;
		glm::vec3 chunkPos; // rounded by CHUNK_SIZE position
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