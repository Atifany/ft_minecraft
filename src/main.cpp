#include "../include/ft_minecraft.h"
#include "../include/stb_image.h"
#include "../include/Shader.h"
#include "../include/Camera.h"
#include "../include/Input.h"
#include "../include/Chunk.h"

float deltaTime = 0.0f;

void framebuffer_size_callback(GLFWwindow*, int width, int height);
void processInput(GLFWwindow *window, Camera *camera, Input *input);
GLFWwindow* initGLFW();
int initGLAD(GLFWwindow* window);
unsigned int genTexture(std::string texturePath);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void putBenchmarkToTerminal(float deltaTime, unsigned int chunksNumber);
void DrawChunks(std::list<Chunk*> chunks, int modelLoc, Camera* camera, glm::mat4 vp);
void DeleteChunks(std::list<Chunk*> chunks);
Chunk* FindChunkAtPos(std::list<Chunk*> chunks, glm::vec3 _pos);
void UpdateChunks(std::list<Chunk*> chunks, std::list<Chunk*>* chunksBuf, glm::vec3 curCameraChunkCoord, bool* isWorking);

const std::string vertexShaderPath = "shaders/vertex_shader.shader";
const std::string fragmentShaderPath = "shaders/fragment_shader.shader";

const std::string grassSideTexturePath = "textures/Grass_Side.png";

int main()
{
	// Init GLFW and window
	GLFWwindow* window = initGLFW();
	if (window == NULL)
		return (-1);

	// Init GLAD
	if (initGLAD(window))
		return (-1);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	// cull back faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	Shader *shader = new Shader(vertexShaderPath, fragmentShaderPath);
	if (shader->ID == 0)
	{
		glfwTerminate();
		return (-1);
	}
	shader->Use();

	// Textures
	unsigned int grassSideTexture = genTexture(grassSideTexturePath);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// get uniforms for transform matricies
	int mvpLoc = glGetUniformLocation(shader->ID, "mvp");

	// Projection matrix shouldn't change each frame so its set before the main loop
	projection = glm::perspective(glm::radians(45.0f), (float)(SRC_WIDTH) / (float)(SRC_HEIGHT), 0.1f, (float)(CHUNK_RENDER_DIST * CHUNK_SIZE));
	
	Camera *camera = new Camera();
	Input *input = new Input();
	glfwSetWindowUserPointer(window, input);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // capture and hide cursor
	glfwSetCursorPosCallback(window, mouse_callback);

	std::list<Chunk*> chunks;		// actual chunks drawn
	std::list<Chunk*> chunksBuf;	// buffer for chunkLoader
	std::jthread chunkLoader;

	bool isWorking = false;
	chunkLoader = std::jthread(UpdateChunks, chunks, &chunksBuf, camera->curChunkCoord, &isWorking);

	float lastframe = 0.0f;
	//Render loop
	while(!glfwWindowShouldClose(window))
	{
		deltaTime = glfwGetTime() - lastframe;
		lastframe = glfwGetTime();
		putBenchmarkToTerminal(deltaTime, chunks.size());

		processInput(window, camera, input);
		input->UpdateKeys();

		// Clear screen
		glClearColor(0.15f, 0.2f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// camera aka view matrix
		view = glm::lookAt(camera->pos, camera->pos + camera->front, camera->up);

		glm::vec3 prevCameraChunkCoord = camera->curChunkCoord;
		// update chunk coordinate of a camera
		if (camera->pos / glm::vec3(CHUNK_SIZE) != camera->curChunkCoord)
			camera->curChunkCoord = glm::vec3((int)(camera->pos.x / CHUNK_SIZE), (int)(camera->pos.y / CHUNK_SIZE), (int)(camera->pos.z / CHUNK_SIZE));

		// load and unload chunks within renderdistance
		if (prevCameraChunkCoord != camera->curChunkCoord && isWorking == false)
			chunkLoader = std::jthread(UpdateChunks, chunks, &chunksBuf, camera->curChunkCoord, &isWorking);

		// join chunks generated in a parallel thread to main chunk list
		if (isWorking == false && chunksBuf.size() != 0)
		{
			for (auto& chunk : chunksBuf)
				chunk->GenBuffers();
			chunks.splice(chunks.end(), chunksBuf);
		}

		// Draw
		glBindTexture(GL_TEXTURE_2D, grassSideTexture);
		shader->Use();

		DrawChunks(chunks, mvpLoc, camera, projection * view);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// clear terminal
	std::cout << std::endl;

	DeleteChunks(chunks);
	glfwTerminate();
	std::cout << "exited successfully!\n";
	return (0);
}

void DrawChunks(std::list<Chunk*> chunks, int mvpLoc, Camera* camera, glm::mat4 vp)
{
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mvp = glm::mat4(1.0f);

	for(const auto& chunk : chunks)
	{
		if (chunk->indices.size() == 0 && chunk->isReady == true)
			continue;
		glBindVertexArray(chunk->VAO);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(chunk->pos.x, chunk->pos.y, chunk->pos.z));

		mvp = vp * model;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

		glDrawElements(GL_TRIANGLES, chunk->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

// Load new chunks and unload chunks within render distance
void UpdateChunks(std::list<Chunk*> chunks, std::list<Chunk*>* chunksBuf, glm::vec3 curCameraChunkCoord, bool* isWorking)
{
	*isWorking = true;
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
	std::list<Chunk*> combinedChunks;
	combinedChunks.insert(combinedChunks.end(), chunks.begin(), chunks.end());
	combinedChunks.insert(combinedChunks.end(), (*chunksBuf).begin(), (*chunksBuf).end());
	// Load not yet loaded chunks inside render distance and unload chunks outside CHUNK_RENDER_DIST from camera
	for (auto chunk = (*chunksBuf).begin(); chunk != (*chunksBuf).end(); )
	{
		// load not yet loaded chunks
		if ((*chunk)->isReady == false)
			(*chunk)->GenMesh(combinedChunks);
		// unload all chunks outside rander distance
		// if ((*chunk)->pos.x < (curCameraChunkCoord.x - CHUNK_RENDER_DIST / 2) * CHUNK_SIZE || (*chunk)->pos.x > (curCameraChunkCoord.x + CHUNK_RENDER_DIST / 2) * CHUNK_SIZE ||
		// 	(*chunk)->pos.y < (curCameraChunkCoord.y - CHUNK_RENDER_DIST / 2) * CHUNK_SIZE || (*chunk)->pos.y > (curCameraChunkCoord.y + CHUNK_RENDER_DIST / 2) * CHUNK_SIZE ||
		// 	(*chunk)->pos.z < (curCameraChunkCoord.z - CHUNK_RENDER_DIST / 2) * CHUNK_SIZE || (*chunk)->pos.z > (curCameraChunkCoord.z + CHUNK_RENDER_DIST / 2) * CHUNK_SIZE)
		// {
		// 	delete *chunk;
		// 	chunk = (*chunks).erase(chunk);
		// }
		// else
		chunk++;
	}
	*isWorking = false;
}

void DeleteChunks(std::list<Chunk*> chunks)
{
	for (auto chunk = chunks.begin(); chunk != chunks.end(); )
	{
		delete *chunk;
		chunk = chunks.erase(chunk);
	}
}

Chunk* FindChunkAtPos(std::list<Chunk*> chunks, glm::vec3 _pos)
{
	for (auto& chunk : chunks)
		if (chunk->pos == _pos)
			return chunk;
	return NULL;
}

// Input
void processInput(GLFWwindow *window, Camera *camera, Input *input)
{
	// translate mouse input to camera rotation
	camera->MouseInput(input->GetMousePos());

	// exit program
	if (input->GetKey(GLFW_KEY_ESCAPE) == I_KEY_SINGLE_PRESS)
		glfwSetWindowShouldClose(window, true);
	// wireframe mode
	if (input->GetKey(GLFW_KEY_R) == I_KEY_SINGLE_PRESS)
	{
		if (camera->isWireFrameModeOn == false)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		camera->isWireFrameModeOn = !(camera->isWireFrameModeOn);
	}
	
	// move camera
	if (input->GetKey(GLFW_KEY_W) == I_KEY_HOLD)
		camera->pos += camera->front * camera->speed * deltaTime;
	if (input->GetKey(GLFW_KEY_S) == I_KEY_HOLD)
		camera->pos -= camera->front * camera->speed * deltaTime;
	if (input->GetKey(GLFW_KEY_A) == I_KEY_HOLD)
		camera->pos -= glm::normalize(glm::cross(camera->front, camera->up)) * camera->speed * deltaTime;
	if (input->GetKey(GLFW_KEY_D) == I_KEY_HOLD)
		camera->pos += glm::normalize(glm::cross(camera->front, camera->up)) * camera->speed * deltaTime;
	if (input->GetKey(GLFW_KEY_SPACE) == I_KEY_HOLD)
		camera->pos += glm::vec3(0.0f, 1.0f, 0.0f) * camera->speed * deltaTime;
	if (input->GetKey(GLFW_KEY_LEFT_SHIFT) == I_KEY_HOLD)
		camera->pos -= glm::vec3(0.0f, 1.0f, 0.0f) * camera->speed * deltaTime;
}

//  Adjust viewport on window resize
void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Init GLFW and create window
GLFWwindow* initGLFW()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Init window
	GLFWwindow* window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "ft_minecraft", NULL, NULL);
	if (window == NULL)
	{
		printf("Failed to create a window.");
		glfwTerminate();
		printf("Error: failed to create window.");
		return (NULL);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);			// Vsync
	return (window);
}

// Init GLAD lib
int initGLAD(GLFWwindow* window)
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Error: Failed to initialize GLAD");
		return (1);
	}
	glViewport(0, 0, SRC_WIDTH, SRC_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	return (0);
}

unsigned int genTexture(std::string texturePath)
{
	// stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cout << "Error: Failed to load texture from:\n" << texturePath << "\n";
		return (0);
	}

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// clip parametrs
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	return (textureID);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	Input *input = static_cast<Input*>(glfwGetWindowUserPointer(window));
	input->UpdateMouseFromInput(std::make_pair(static_cast<float>(xpos), static_cast<float>(ypos)));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	Input *input = static_cast<Input*>(glfwGetWindowUserPointer(window));
	input->UpdateKeysFromInput(key, action);
}

void putBenchmarkToTerminal(float deltaTime, unsigned int chunksNumber)
{
	// update benchmark every second
	static int wholeSecPassed = 0;
	float curTime = glfwGetTime();

	if ((int)curTime > wholeSecPassed)
	{
		wholeSecPassed = (int)curTime;
		std::cout << "\r                              \r"; // clear output line
		std::cout << "\r" << (float)((int)((deltaTime * 1000) * 100)) / 100 << "ms "
			<< (float)((int)((1 / deltaTime) * 100)) / 100 << "fps "
			<< chunksNumber << " chunks loaded"
			<< std::flush;
	}
}
