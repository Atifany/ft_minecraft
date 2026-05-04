#include "../include/ft_minecraft.h"
#include "../include/stb_image.h"
#include "../include/Shader.h"
#include "../include/Camera.h"

const unsigned int SRC_WIDTH = 800;
const unsigned int SRC_HEIGHT = 600;
const float RENDER_DIST = 100.0f;
const unsigned int CUBE_VERTICIES_COUNT = 36;

// TMP
float lastMouseX = 400;
float lastMouseY = 300;
float yaw = -90.0f;
float pitch = 0.0f;
Camera *camera = new Camera();
bool isMouseFirstEntered = true;

float deltaTime = 0.0f;

void framebuffer_size_callback(GLFWwindow*, int width, int height);
void processInput(GLFWwindow *window, Camera *camera);
GLFWwindow* initGLFW();
int initGLAD(GLFWwindow* window);
unsigned int genTexture(std::string texturePath);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

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
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	Shader *shader = new Shader(vertexShaderPath, fragmentShaderPath);
	if (shader->ID == 0)
	{
		glfwTerminate();
		return (-1);
	}
	shader->Use();

	unsigned int VBO, VAO, EBO;
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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // capture and hide cursor
	glfwSetCursorPosCallback(window, mouse_callback);

	float lastframe = 0.0f;
	//Render loop
	while(!glfwWindowShouldClose(window))
	{
		deltaTime = glfwGetTime() - lastframe;
		lastframe = glfwGetTime();
		processInput(window, camera);

		// Clear screen
		glClearColor(0.15f, 0.2f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// camera aka view matrix
		view = glm::lookAt(camera->pos, camera->pos + camera->front, camera->up);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Draw
		glBindTexture(GL_TEXTURE_2D, grassSideTexture);
		shader->Use();
		glBindVertexArray(VAO);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, CUBE_VERTICIES_COUNT);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, CUBE_VERTICIES_COUNT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return (0);
}

// Input
void processInput(GLFWwindow *window, Camera *camera)
{
	// exit program
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	// move camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->pos += camera->front * camera->speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->pos -= camera->front * camera->speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->pos -= glm::normalize(glm::cross(camera->front, camera->up)) * camera->speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->pos += glm::normalize(glm::cross(camera->front, camera->up)) * camera->speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera->pos += glm::vec3(0.0f, 1.0f, 0.0f) * camera->speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
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
	if (isMouseFirstEntered)
	{
		lastMouseX = xpos;
		lastMouseY = ypos;
		isMouseFirstEntered = false;
	}

	float xoffset = (xpos - lastMouseX) * camera->sensetivity;
	float yoffset = (lastMouseY -ypos) * camera->sensetivity;
	lastMouseX = xpos;
	lastMouseY = ypos;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	camera->front = glm::normalize(direction);
}
