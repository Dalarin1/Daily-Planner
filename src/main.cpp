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

class InputManager
{
	UIManager *m_ui_manager;

public:
	InputManager(UIManager *ui_manager) : m_ui_manager(ui_manager) {}

	// Статические методы для GLFW коллбэков
	static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		// Получаем экземпляр InputManager из пользовательских данных окна
		auto *inputManager = static_cast<InputManager *>(glfwGetWindowUserPointer(window));
		if (inputManager && inputManager->m_ui_manager->m_active_textfield)
		{
			inputManager->m_ui_manager->m_active_textfield->handle_key_input(key, action);
		}
	}

	static void char_callback(GLFWwindow *window, unsigned int codepoint)
	{
		auto *inputManager = static_cast<InputManager *>(glfwGetWindowUserPointer(window));
		if (inputManager && inputManager->m_ui_manager->m_active_textfield)
		{
			inputManager->m_ui_manager->m_active_textfield->handle_char_input(codepoint);
		}
	}

	static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
	{
		auto *inputManager = static_cast<InputManager *>(glfwGetWindowUserPointer(window));
		if (!inputManager || button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS)
			return;

		double x, y;
		glfwGetCursorPos(window, &x, &y);
		inputManager->m_ui_manager->handle_click(x, y, 800, 800);
	}
};

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

	GLFWwindow *window = glfwCreateWindow(800, 800, "DailyPlannerG", NULL, NULL);

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

	TextRenderer textRenderer = TextRenderer(&text_shader_program, "include/arial.ttf", 32u, 800, 800);

	UIManager ui = UIManager(std::make_shared<ShaderProgram>(ui_shader_program), std::make_shared<TextRenderer>(textRenderer));
	ui._calendar.add_task(Task("Next item", "decs", "", Task::Priority::Medium, std::chrono::floor<date::days>(std::chrono::system_clock::now())));
	ui._calendar.add_task(Task());
	ui._calendar.add_task(Task());
	ui._calendar.add_task(Task("text task", "desc", "", Task::Priority::Low, date::year_month_day(date::year{2025}, date::month{7}, date::day{1})));
	ui._calendar.set_view_mode(Calendar::ViewMode::Day);
	ui._calendar.navigate_to_date(std::chrono::floor<date::days>(std::chrono::system_clock::now()));
	// ui.update_tasks();
	ui.crupdate_day_mode();
	ui.crupdate_week_mode();
	ui.crupdate_month_mode();
	ui.crupdate_date_switch_elements();

	InputManager inputManager = InputManager(&ui);

	glfwSetWindowUserPointer(window, &inputManager);

	glfwSetCharCallback(window, &InputManager::char_callback);
	glfwSetKeyCallback(window, &InputManager::key_callback);
	glfwSetMouseButtonCallback(window, &InputManager::mouse_button_callback);

	// glfwSetWindowUserPointer(window, window);

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

		glfwGetCursorPos(window, &x, &y);
		ui.update(x, y, 800, 800);
		// if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		// {
		// 	ui.handle_click(x, y, 800, 800);
		// }
		ui.draw_calendar();
		ui_shader_program.use();
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