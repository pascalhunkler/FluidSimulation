#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <vector>
#include "Particle.h"

class GUI
{
private:
	GLFWwindow* window;
	Shader* shader;
	unsigned int vao;
	unsigned int vbo;
	
public:
	/**
	 *	Create a new simulation window and initialize OpenGL, GLFW and GLAD
	 *	@param width the width of the window
	 *	@param height the height of the window
	 */
	GUI(int width, int height, float particleSize);

	/**
	 *	poll events and return true iff window should not close yet
	 *	@return true if window should not close, false if window should close
	 */
	bool update();

	/**
	 *	draw all particles whose positions are given
	 *	@param particles particles which are drawn
	 */
	void draw(const std::vector<Particle>& particles) const;

	char* get_picture_data() const;

	// callback which is called by glfw when a key is pressed
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	// callback which is called by glfw when window size changes
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	
	~GUI();
};

