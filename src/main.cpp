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
#include "UIManager.hpp"

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
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

	gladLoadGL();

	// unsigned int shaderProgram = create_shader_program("shaders/shader.vert", "shaders/shader.frag");
	

	ShaderProgram ui_shader_program = ShaderProgram();
	ui_shader_program.add_shader(GL_VERTEX_SHADER, "shaders/shader.vert");
	ui_shader_program.add_shader(GL_FRAGMENT_SHADER, "shaders/shader.frag");
	ui_shader_program.link();


	ShaderProgram text_shader_program = ShaderProgram();
	text_shader_program.add_shader(GL_VERTEX_SHADER, "shaders/text.vert");
	text_shader_program.add_shader(GL_FRAGMENT_SHADER, "shaders/text.frag");
	text_shader_program.link();

	UIManager ui = UIManager(ui_shader_program.program);
	TextRenderer textRenderer = TextRenderer(&text_shader_program,"include/Roboto-VariableFont_wdth,wght.ttf", 18u);

	glViewport(0, 0, 800, 800);
	double x, y;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// Очистка буфера
		glClearColor(0.17f, 0.57f, 0.24f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwGetCursorPos(window, &x, &y);
		
		ui_shader_program.use();
		ui.draw_calendar();

		text_shader_program.use();
		textRenderer.render_text("Suck some cock", 0.5, 0.6, 1.0, Color(0, 0, 0));
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}