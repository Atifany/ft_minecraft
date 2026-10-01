#include "../include/ChunkLoader.h"

// parallel thread
void GenChunks(std::list<Chunk*> chunks, std::list<Chunk*>* chunksBuf, glm::vec3 curCameraChunkCoord, bool* isBusy, std::list<unsigned int>* chunksToDelete);
Chunk* FindChunkAtChunkPos(const std::list<Chunk*>& chunks, glm::vec3 _chunkPos);

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
	// Add chunk->chunkPos to bring curCameraChunkCoord and chunkPos to one unit of measurment

	if (this->isBusy == false && this->chunksToDelete.size() > 0)
		this->DeleteChunks();
	if (this->isBusy == false && this->chunksBuf.size() != 0)
		this->JoinGeneratedChunks();
	
	// continue to exit if no triggers to start chunkLoader are set.
	if (cameraIsInNewChunk == false && this->shouldRunAgain == false)
		return ;
	
	// if chunkLoader isBusy and the trigger is set, set shouldRunAgain to true.
	if (this->isBusy == true)
	{
		this->shouldRunAgain = true;
		return ;
	}

	// ChunkLoader is idle and a trigger to start is present - run worker thread.
	this->shouldRunAgain = false;
	this->startedTime = glfwGetTime();
	this->isBusy = true;
	worker = std::jthread(GenChunks, this->chunks, &(this->chunksBuf), curCameraChunkPos, &(this->isBusy), &(this->chunksToDelete));
}

void ChunkLoader::JoinGeneratedChunks()
{
	this->elapsedTime = glfwGetTime() - this->startedTime;
	for (auto& chunk : this->chunksBuf)
		chunk->GenBuffers();
	// std::cout << "ChunkLoader: joined " << this->chunksBuf.size() << " chunks.\n";
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
	//std::cout << "ChunkLoader: Freed " << chunksSize << " chunks.\n";
}

void GenChunks(std::list<Chunk*> chunks, std::list<Chunk*>* chunksBuf, glm::vec3 curCameraChunkCoord, bool* isBusy, std::list<unsigned int>* chunksToDelete)
{
	// unload chunks outside CHUNK_RENDER_DIST from camera
	for (auto& chunk : chunks)
	{
		if (chunk->chunkPos.x < curCameraChunkCoord.x - CHUNK_RENDER_DIST || chunk->chunkPos.x > curCameraChunkCoord.x + CHUNK_RENDER_DIST ||
			chunk->chunkPos.y < curCameraChunkCoord.y - CHUNK_RENDER_DIST || chunk->chunkPos.y > curCameraChunkCoord.y + CHUNK_RENDER_DIST ||
			chunk->chunkPos.z < curCameraChunkCoord.z - CHUNK_RENDER_DIST || chunk->chunkPos.z > curCameraChunkCoord.z + CHUNK_RENDER_DIST)
		{
			(*chunksToDelete).push_back(chunk->VAO);
		}
	}

	// Load chunks within CHUNK_RENDER_DIST from camera
	for (int x = curCameraChunkCoord.x - CHUNK_RENDER_DIST; x <= curCameraChunkCoord.x + CHUNK_RENDER_DIST; x++)
	{
		for (int y = curCameraChunkCoord.y - CHUNK_RENDER_DIST; y <= curCameraChunkCoord.y + CHUNK_RENDER_DIST; y++)
		{
			for (int z = curCameraChunkCoord.z - CHUNK_RENDER_DIST; z <= curCameraChunkCoord.z + CHUNK_RENDER_DIST; z++)
			{
				if (FindChunkAtChunkPos(chunks, glm::vec3(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE)) == NULL)
				{
					Chunk* chunk = new Chunk(glm::vec3(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE));
					chunk->GenVoxels();
					(*chunksBuf).push_back(chunk);
				}
			}
		}
	}

	// Load not yet loaded chunks inside render distance
	std::list<Chunk*> combinedChunks;
	combinedChunks.insert(combinedChunks.end(), chunks.begin(), chunks.end());
	combinedChunks.insert(combinedChunks.end(), (*chunksBuf).begin(), (*chunksBuf).end());
	for (auto chunk = (combinedChunks).begin(); chunk != (combinedChunks).end(); )
	{
		if ((*chunk)->isReady == false
			&& ((*chunk)->chunkPos.x >= curCameraChunkCoord.x - (CHUNK_RENDER_DIST - 1)
			&& (*chunk)->chunkPos.x <= curCameraChunkCoord.x + (CHUNK_RENDER_DIST - 1)
			&& (*chunk)->chunkPos.y >= curCameraChunkCoord.y - (CHUNK_RENDER_DIST - 1)
			&& (*chunk)->chunkPos.y <= curCameraChunkCoord.y + (CHUNK_RENDER_DIST - 1)
			&& (*chunk)->chunkPos.z >= curCameraChunkCoord.z - (CHUNK_RENDER_DIST - 1)
			&& (*chunk)->chunkPos.z <= curCameraChunkCoord.z + (CHUNK_RENDER_DIST - 1)))
				(*chunk)->GenMesh(combinedChunks);
		chunk++;
	}

	// requires to gen mesh for combinedChunks because there are genvoxeled but not genmeshed chunks.
	// but cannot add them after in renderer...
	//*chunksBuf = combinedChunks;
	*isBusy = false;
}

Chunk* FindChunkAtChunkPos(const std::list<Chunk*>& chunks, glm::vec3 _chunkPos)
{
	for (auto& chunk : chunks)
		if (chunk->chunkPos == _chunkPos)
			return chunk;
	return NULL;
}

