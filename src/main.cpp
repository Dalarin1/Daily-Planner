/*
106. Beginner: Daily Planner with Calendar View
Description: A daily planner that allows users to schedule tasks, view upcoming events, and set reminders.
Tech Stack:
Features:
	Add, edit, and delete tasks
	Calendar view for daily, weekly, and monthly plans
	Reminders for tasks and events
	Color-coded categories for tasks
Learning Path: Local storage, calendar UI, and reminder notifications.
Open-source Focus: Contributors can add recurring tasks, theme customization, and more notification options.
*/
#include <iostream>
#include "raphics.hpp"
#include "ShaderLoader.hpp"
#include "UIManager.hpp"

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void mouse_click_callback(GLFWwindow *window, int button, int action, int mods){
	if(current_hovered_object == nullptr){
		return;
	}
	current_hovered_object->OnClick(current_hovered_object);
}

void button_callback(Button *btn)
{
	btn->current_bkg_color = btn->base_bkg_color.Lerp(Color(255, 0, 200), 60.0f);
}

int main()
{

	// Инициализация GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 800, "DailyPlannerGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, mouse_click_callback);
	gladLoadGL();

	unsigned int shaderProgram = create_shader_program("shaders/shader.vert", "shaders/shader.frag");
	UIManager ui = UIManager(shaderProgram);
	glViewport(0, 0, 800, 800);
	double x, y;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// Очистка буфера
		glClearColor(0.17f, 0.57f, 0.24f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwGetCursorPos(window, &x, &y);
		ui.check_buttons_bover(x,y,800,800);
		glUseProgram(shaderProgram);
		ui.draw_calendar();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}