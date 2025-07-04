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

	ShaderProgram ui_shader_program = ShaderProgram();
	ui_shader_program.add_shader(GL_VERTEX_SHADER, "shaders/shader.vert");
	ui_shader_program.add_shader(GL_FRAGMENT_SHADER, "shaders/shader.frag");
	ui_shader_program.link();

	ShaderProgram text_shader_program = ShaderProgram();
	text_shader_program.add_shader(GL_VERTEX_SHADER, "shaders/text.vert");
	text_shader_program.add_shader(GL_FRAGMENT_SHADER, "shaders/text.frag");
	text_shader_program.link();

	TextRenderer textRenderer = TextRenderer(&text_shader_program);
	textRenderer.load_font("arial32", "include/arial.ttf", 32u);

	UIManager ui = UIManager(&ui_shader_program, &text_shader_program, &textRenderer);
	// ui._font = textRenderer.fonts["arial32"];
	ui._calendar.add_task(Task(L"Next item", L"decs", L"", Task::Priority::Medium, std::chrono::floor<date::days>(std::chrono::system_clock::now())));
	ui._calendar.add_task(Task());
	ui._calendar.add_task(Task());
	ui._calendar.add_task(Task(L"Колупаторо", L"Описание", L"", Task::Priority::Low, std::chrono::floor<date::days>(std::chrono::system_clock::now())));
	ui._calendar.add_task(Task(L"text task", L"desc", L"", Task::Priority::Low, date::year_month_day(date::year{2025}, date::month{7}, date::day{1})));
	ui._calendar.set_view_mode(Calendar::ViewMode::Day);
	ui._calendar.navigate_to_date(std::chrono::floor<date::days>(std::chrono::system_clock::now()));
	ui.update_tasks();

	glViewport(0, 0, 800, 800);
	double x, y;
	const int targetFPS = 30; // Достаточно для интерфейса
	const double frameDelay = 1.0 / targetFPS;
	// ОПТИМИЗЕЙШЕСТВО, ВСРАТО, НО ЖЕСТКО ЗАНИЖАЕТ CPU с 50% до 3%
	while (!glfwWindowShouldClose(window))
	{
		auto frameStart = glfwGetTime();
		processInput(window);
		// Рендеринг
		glClearColor(0.17f, 0.57f, 0.24f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		textRenderer.render_text("arial32", L"Понедельник", vector2(200, 200), Color(0,0,0), 1.0f);

		glfwGetCursorPos(window, &x, &y);

		ui.update(x, y, 800, 800);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			ui.handle_click(x, y, 800, 800);
		}
		ui.draw_calendar();

		glfwSwapBuffers(window);

		// Ограничение FPS
		double frameTime = glfwGetTime() - frameStart;
		if (frameTime < frameDelay)
		{
			glfwWaitEventsTimeout(frameDelay - frameTime); // Экономит CPU
		}
		else
		{
			glfwPollEvents(); // Если кадр занял больше времени
		}
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}