#include "../include/ft_minecraft.h"
#include "../include/stb_image.h"
#include "../include/Shader.h"
#include "../include/Camera.h"
#include "../include/Input.h"
#include "../include/Chunk.h"
#include "../include/ChunkLoader.h"

float deltaTime = 0.0f;

void framebuffer_size_callback(GLFWwindow*, int width, int height);
void processInput(GLFWwindow *window, Camera *camera, Input *input);
GLFWwindow* initGLFW();
int initGLAD(GLFWwindow* window);
unsigned int genTexture(std::string texturePath);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void putBenchmarkToTerminal(float deltaTime, unsigned int chunksNumberm, float lastChunkLoaderTime);
void DrawChunks(std::list<Chunk*> chunks, int modelLoc, Camera* camera, glm::mat4 vp);

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
	// anti-aliasing
	glEnable(GL_MULTISAMPLE);

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

	ChunkLoader* chunkLoader = new ChunkLoader();

	float lastframe = 0.0f;
	//Render loop
	while(!glfwWindowShouldClose(window))
	{
		deltaTime = glfwGetTime() - lastframe;
		lastframe = glfwGetTime();
		putBenchmarkToTerminal(deltaTime, chunkLoader->chunks.size(), chunkLoader->elapsedTime);

		processInput(window, camera, input);
		input->UpdateKeys();

		// Clear screen
		glClearColor(0.15f, 0.2f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// camera aka view matrix
		view = glm::lookAt(camera->pos, camera->pos + camera->front, camera->up);
		
		// update cam pos
		camera->UpdateChunkPos();

		// run chunkLoader. Gen new chunks, delete old chunks.
		chunkLoader->Update(camera->curChunkPos, camera->isInNewChunk);

		// Draw
		glBindTexture(GL_TEXTURE_2D, grassSideTexture);
		shader->Use();

		DrawChunks(chunkLoader->chunks, mvpLoc, camera, projection * view);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// clear terminal
	std::cout << std::endl;

	delete(chunkLoader);
	glfwTerminate();
	std::cout << "exited successfully!\n";
	return (0);
}

void DrawChunks(std::list<Chunk*> chunks, int mvpLoc, Camera* camera, glm::mat4 vp)
{
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mvp = glm::mat4(1.0f);

	for (auto& chunk : chunks)
	{
		if (chunk->indices.size() == 0 && chunk->isReady == true)
			{chunk++; continue;}
		glBindVertexArray(chunk->VAO);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(chunk->pos.x, chunk->pos.y, chunk->pos.z));

		mvp = vp * model;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

		glDrawElements(GL_TRIANGLES, chunk->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
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
	glfwWindowHint(GLFW_SAMPLES, MSAA_SAMPLES);						// anti-aliasing

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

void putBenchmarkToTerminal(float deltaTime, unsigned int chunksNumber, float lastChunkLoaderTime)
{
	// update benchmark every second
	static int wholeSecPassed = 0;
	float curTime = glfwGetTime();

	if ((int)curTime > wholeSecPassed)
	{
		wholeSecPassed = (int)curTime;
		std::cout << "\r                                          \r"; // clear output line
		std::cout << "\r" << (float)((int)((deltaTime * 1000) * 100)) / 100 << "ms | "
			<< (float)((int)((1 / deltaTime) * 100)) / 100 << "fps | "
			<< chunksNumber << " chunks loaded | "
			<< (float)((int)((lastChunkLoaderTime * 1000) * 100)) / 100 << "ms last chunkLoader took"
			<< std::flush;
	}
}
