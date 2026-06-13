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
void putBenchmarkToTerminal(float deltaTime);
void DrawChunks(std::vector<Chunk*> chunks, int modelLoc);
void DeleteChunks(std::vector<Chunk*> chunks);

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

	// cube verticies
	float verticies[] = {
	//  |----coordinate----| |-texture--|
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,	// Bottom-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	// top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,	// bottom-right         
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	// top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,	// bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	// top-left
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	// bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	// bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,	// top-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,	// top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,	// top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	// bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	// top-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	// top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	// bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	// bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	// bottom-right
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	// top-right
		// Right face
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	// top-left
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	// bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	// top-right         
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	// bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	// top-left
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	// bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	// top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,	// top-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	// bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	// bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	// bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	// top-right
		// Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	// top-left
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	// bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	// top-right     
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	// bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	// top-left
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f 	// bottom-left
	};

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

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

	// setting position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// setting texture coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Textures
	unsigned int grassSideTexture = genTexture(grassSideTexturePath);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// get uniforms for transform matricies
	int modelLoc = glGetUniformLocation(shader->ID, "model");
	int viewLoc = glGetUniformLocation(shader->ID, "view");
	int projectionLoc = glGetUniformLocation(shader->ID, "projection");

	// Projection matrix shouldn't change each frame so its set before the main loop
	projection = glm::perspective(glm::radians(45.0f), (float)(SRC_WIDTH) / (float)(SRC_HEIGHT), 0.1f, RENDER_DIST);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	Camera *camera = new Camera();
	Input *input = new Input();
	glfwSetWindowUserPointer(window, input);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // capture and hide cursor
	glfwSetCursorPosCallback(window, mouse_callback);

	std::vector<Chunk*> tstChunks;
	for(int i = 0; i < 216; i++)
	{
		Chunk* chunk = new Chunk();
		chunk->Load();
		chunk->SetActive(true);
		chunk->pos = glm::vec3((i % 6) * CHUNK_SIZE, ((int)(i / 6) % 6) * CHUNK_SIZE, ((int)(i / 36) % 6) * CHUNK_SIZE);
		tstChunks.push_back(chunk);
	}

	float lastframe = 0.0f;
	//Render loop
	while(!glfwWindowShouldClose(window))
	{
		deltaTime = glfwGetTime() - lastframe;
		lastframe = glfwGetTime();
		putBenchmarkToTerminal(deltaTime);

		processInput(window, camera, input);
		input->UpdateKeys();

		// Clear screen
		glClearColor(0.15f, 0.2f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// camera aka view matrix
		view = glm::lookAt(camera->pos, camera->pos + camera->front, camera->up);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Draw
		glBindTexture(GL_TEXTURE_2D, grassSideTexture);
		shader->Use();

		DrawChunks(tstChunks, modelLoc);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// clear terminal
	std::cout << std::endl;

	DeleteChunks(tstChunks);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	std::cout << "exited successfully!\n";
	return (0);
}

void DrawChunks(std::vector<Chunk*> chunks, int modelLoc)
{
	glm::mat4 model = glm::mat4(1.0f);

	for(const auto& chunk : chunks)
	{
		glBindVertexArray(chunk->VAO);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(chunk->pos.x, chunk->pos.y, chunk->pos.z));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, chunk->mesh.size());
		glBindVertexArray(0);
	}
}

void DeleteChunks(std::vector<Chunk*> chunks)
{
	for(int i = 0; i < chunks.size(); i++)
		delete chunks[i];
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

void putBenchmarkToTerminal(float deltaTime)
{
	// update benchmark every second
	static int wholeSecPassed = 0;
	float curTime = glfwGetTime();

	if ((int)curTime > wholeSecPassed)
	{
		wholeSecPassed = (int)curTime;
		std::cout << "\r                              \r"; // clear output line
		std::cout << "\r" << (float)((int)((deltaTime * 1000) * 100)) / 100 << "ms " << (float)((int)((1 / deltaTime) * 100)) / 100 << "fps" << std::flush;
	}
}
