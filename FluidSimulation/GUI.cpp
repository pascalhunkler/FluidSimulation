#include "GUI.h"
#include <iostream>
#include <exception>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include <vector>

GUI::GUI(int width, int height, float particleSize)
{
	// Initialize GLFW and create new GLFW window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Fluid Simulation", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		throw std::exception("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		glfwTerminate();
		throw std::exception("Failed to initialize GLAD");
	}

	glViewport(0, 0, width, height);

	// Set the callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	// Activate blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create and link new shader program
	shader = new Shader("VertexShader.glsl", "FragmentShader.glsl");

	// data for the vertex buffer object
	const float right = particleSize / 2;
	const float top = particleSize / 2;
	const float left = -particleSize / 2;
	const float bottom = -particleSize / 2;
	const float vertexBuffer[] = {
		 right, bottom, 0.f, 1.f, -1.f,
		 right, top, 0.f, 1.f, 1.f,
		 left, top, 0.f, -1.f, 1.f,
		 left, bottom, 0.f, -1.f, -1.f
	};

	// Create and bind vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and bind vertex buffer object and copy vertex data into it
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffer), vertexBuffer, GL_STATIC_DRAW);

	// Set the attributes of the vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Activate the shader program so that we can draw later
	shader->use();
}


bool GUI::update()
{
	if(glfwWindowShouldClose(window))
	{
		return false;
	}
	glfwPollEvents();
	return true;
}


void GUI::draw(const std::vector<Particle>& particles) const
{
	// Clear framebuffer with a black color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBindVertexArray(vao);
	
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	glm::mat4 projectionMatrix = glm::mat4(
		2.f / float(width), 0.f, 0.f, 0.f,
		0.f, 2.f / float(height), 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		-1.f, -1.f, 0.f, 1.f);
	int projectionLoc = glGetUniformLocation(shader->id, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	for (auto& particle : particles)
	{
		// Draw each particle
		glm::mat4 model = glm::mat4(1.0f);
		glm::vec3 position = glm::vec3(particle.position, 0.f);
		model = glm::translate(model, position);
		int modelLoc = glGetUniformLocation(shader->id, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int colorLoc = glGetUniformLocation(shader->id, "color");
		glUniform3fv(colorLoc, 1, glm::value_ptr(particle.color));
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	glfwSwapBuffers(window);
}

char* GUI::get_picture_data() const
{
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	char* buffer = new char[width * height * 3];
	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, buffer);
	return buffer;
}


void GUI::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// update the gl viewport if the window size changes
	glViewport(0, 0, width, height);
}

void GUI::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, key) == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close window if escape key is pressed
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		}
	}
}

GUI::~GUI()
{
	glfwTerminate();
	delete shader;
}
