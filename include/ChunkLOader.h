#ifndef CHUNKLOADER_H
#define CHUNKLOADER_H

#include "ft_minecraft.h"
#include "Chunk.h"

class ChunkLoader
{
	public:
		ChunkLoader();
		~ChunkLoader();

		bool isBusy;
		bool shouldRunAgain;		// if new chunks need to be generated, but chunkloader is busy, then tell him to run UpdateChunks again when finished
		float elapsedTime;			// time it took chunkLoader to load last batch of chunks
		std::list<Chunk*> chunks;

		void Update(glm::vec3 curCameraChunkPos, bool cameraIsInNewChunk);

	private:
		float startedTime;
		std::jthread worker;
		std::list<Chunk*> chunksBuf;
		std::list<unsigned int> chunksToDelete;		// stores VAOs of chunks to delete

		void JoinGeneratedChunks();
		void DeleteChunks();
		void FreeChunks();
};

#endif