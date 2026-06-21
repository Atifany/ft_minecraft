#include "../include/ChunkLoader.h"

// parallel thread
void GenChunks(std::list<Chunk*> chunks, std::list<Chunk*>* chunksBuf, glm::vec3 curCameraChunkCoord, bool* isBusy, std::list<unsigned int>* chunksToDelete);
Chunk* FindChunkAtPos(std::list<Chunk*> chunks, glm::vec3 _pos);

ChunkLoader::ChunkLoader()
{
	this->isBusy = false;
	this->shouldRunAgain = false;
	this->elapsedTime = 0.0f;
	this->startedTime = 0.0f;
}

ChunkLoader::~ChunkLoader()
{
	if (this->isBusy == true)
	{
		std::cout << "ChunkLoader: waiting for worker thread to finish.\n";
		this->worker.join();
	}
	this->FreeChunks();
}

void ChunkLoader::Update(glm::vec3 curCameraChunkPos, bool cameraIsInNewChunk)
{
	if (this->isBusy == false && this->chunksToDelete.size() > 0)
		this->DeleteChunks();
	if (this->isBusy == false && this->chunksBuf.size() != 0)
		this->JoinGeneratedChunks();
	
	// tell chunkLoader to run again if more chunks need to be loaded
	if (cameraIsInNewChunk == true && this->isBusy == true)
		this->shouldRunAgain = true;	
	// load and unload chunks within renderdistance
	if ((cameraIsInNewChunk == true || this->shouldRunAgain == true) && this->isBusy == false)
	{
		this->shouldRunAgain = false;
		this->startedTime = glfwGetTime();
		this->isBusy = true;
		worker = std::jthread(GenChunks, this->chunks, &(this->chunksBuf), curCameraChunkPos, &(this->isBusy), &(this->chunksToDelete));
	}
}

void ChunkLoader::JoinGeneratedChunks()
{
	this->elapsedTime = glfwGetTime() - this->startedTime;
	for (auto& chunk : this->chunksBuf)
		chunk->GenBuffers();
	std::cout << "ChunkLoader: joined " << this->chunksBuf.size() << " chunks.\n";
	this->chunks.splice(this->chunks.end(), this->chunksBuf);
}

// delete chunks outside render distance
void ChunkLoader::DeleteChunks()
{
	for (auto chunk = this->chunks.begin(); chunk != this->chunks.end(); )
	{
		if (std::find(this->chunksToDelete.begin(), this->chunksToDelete.end(), (*chunk)->VAO) != this->chunksToDelete.end())
		{
			delete *chunk;
			chunk = this->chunks.erase(chunk);
		}
		else
			chunk++;
	}
	std::cout << "ChunkLoader: deleted " << this->chunksToDelete.size() << " chunks.\n";
	this->chunksToDelete.clear();
}

void ChunkLoader::FreeChunks()
{
	unsigned int chunksSize = chunks.size();
	for (auto chunk = this->chunks.begin(); chunk != this->chunks.end(); )
	{
		delete *chunk;
		chunk = this->chunks.erase(chunk);
	}

	chunksSize += chunksBuf.size();
	for (auto chunk = this->chunksBuf.begin(); chunk != this->chunksBuf.end(); )
	{
		delete *chunk;
		chunk = this->chunksBuf.erase(chunk);
	}
	std::cout << "ChunkLoader: Freed " << chunksSize << " chunks.\n";
}

void GenChunks(std::list<Chunk*> chunks, std::list<Chunk*>* chunksBuf, glm::vec3 curCameraChunkCoord, bool* isBusy, std::list<unsigned int>* chunksToDelete)
{
	// unload chunks outside CHUNK_RENDER_DIST from camera
	for (auto& chunk : chunks)
	{
		if (chunk->pos.x < (curCameraChunkCoord.x - CHUNK_RENDER_DIST / 2) * CHUNK_SIZE || chunk->pos.x > (curCameraChunkCoord.x + CHUNK_RENDER_DIST / 2) * CHUNK_SIZE ||
			chunk->pos.y < (curCameraChunkCoord.y - CHUNK_RENDER_DIST / 2) * CHUNK_SIZE || chunk->pos.y > (curCameraChunkCoord.y + CHUNK_RENDER_DIST / 2) * CHUNK_SIZE ||
			chunk->pos.z < (curCameraChunkCoord.z - CHUNK_RENDER_DIST / 2) * CHUNK_SIZE || chunk->pos.z > (curCameraChunkCoord.z + CHUNK_RENDER_DIST / 2) * CHUNK_SIZE)
		{
			(*chunksToDelete).push_back(chunk->VAO);
		}
	}

	// Load chunks within CHUNK_RENDER_DIST from camera
	for (int x = curCameraChunkCoord.x - CHUNK_RENDER_DIST / 2; x < curCameraChunkCoord.x + CHUNK_RENDER_DIST / 2; x++)
	{
		for (int y = curCameraChunkCoord.y - CHUNK_RENDER_DIST / 2; y < curCameraChunkCoord.y + CHUNK_RENDER_DIST / 2; y++)
		{
			for (int z = curCameraChunkCoord.z - CHUNK_RENDER_DIST / 2; z < curCameraChunkCoord.z + CHUNK_RENDER_DIST / 2; z++)
			{
				if (FindChunkAtPos(chunks, glm::vec3(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE)) == NULL)
				{
					Chunk* chunk = new Chunk();
					chunk->pos = glm::vec3(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE);
					chunk->GenVoxels();
					(*chunksBuf).push_back(chunk);
				}
			}
		}
	}

	// Load not yet loaded chunks inside render distance and unload chunks outside CHUNK_RENDER_DIST from camera
	std::list<Chunk*> combinedChunks;
	combinedChunks.insert(combinedChunks.end(), chunks.begin(), chunks.end());
	combinedChunks.insert(combinedChunks.end(), (*chunksBuf).begin(), (*chunksBuf).end());
	for (auto chunk = (*chunksBuf).begin(); chunk != (*chunksBuf).end(); )
	{
		// load not yet loaded chunks
		if ((*chunk)->isReady == false)
			(*chunk)->GenMesh(combinedChunks);
		chunk++;
	}
	*isBusy = false;
}

Chunk* FindChunkAtPos(std::list<Chunk*> chunks, glm::vec3 _pos)
{
	for (auto& chunk : chunks)
		if (chunk->pos == _pos)
			return chunk;
	return NULL;
}
