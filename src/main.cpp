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
#include "raphics.h"
#include "ShaderLoader.h"

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void ChangeBckgColor(Button *btn, Color color)
{
	btn->BackgroundColor = color;
}

void SomeShitFunc(GLFWwindow *window, int button, int action, int mods)
{
	std::cout << "CLICK MAFFACKA \n";
}
void AnotherUselessFucntion(Button *btn)
{
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
}

int main()
{

	// Инициализация GLFW
	glfwInit();

	// Указываем GLFW, какую версию OpenGL используем
	// В данном случае версия OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Указываем GLFW на использование CORE-профиля
	// Который содержит только современные поддерживаемые функции
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Создаём GLFWwindow объект 800х800 пикселей, с названием "YoutubeOpenGL"
	GLFWwindow *window = glfwCreateWindow(800, 800, "YoutubeOpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Загружаем GLAD
	gladLoadGL();

	// GLuint a;
	// glGenBuffers(1, &a);
	// glBindBuffer(GL_ARRAY_BUFFER, a);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW);

	unsigned int shaderProgram = create_shader_program("shaders/shader.vert", "shaders/shader.frag");

	// GLuint vao;
	// glGenVertexArrays(1, &vao);

	// glBindVertexArray(vao);
	// glBindBuffer(GL_ARRAY_BUFFER, a);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	// glEnableVertexAttribArray(0);

	Button myButton(
		vector3(0, 0, 0.0f),
		vector3(0.25f, 0.1f, 0.0f)
		// Color(0, 132, 240), // Цвет фона
		// Color(128, 0, 128)	// Цвет границы
	);

	// myButton.OnHover = ChangeBckgColor;
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, 800, 800);
	glfwSetMouseButtonCallback(window, SomeShitFunc);
	double x, y;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// Очистка буфера
		glClearColor(0.17f, 0.57f, 0.24f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwGetCursorPos(window, &x, &y);
		// myButton.MoveToCursor(x, y, 800, 800);
		// ChangeBckgColor(&myButton, Color(x / 256, y / 256, (x * y) / 256));
		if (myButton.IsHovered(x, y, 800, 800))
		{
			ChangeBckgColor(&myButton, Color(10, 10, 0));
		}
		else
		{
			ChangeBckgColor(&myButton, Color());
		}

		glUseProgram(shaderProgram);
		myButton.Draw(shaderProgram);

		// std::cout << x << " " << y << '\n';
		// myButton.UpdateGeometry();
		// Обновляем позиции вершин
		// rotate(points, 0.00125f);

		// ОБНОВЛЯЕМ данные в буфере
		// glBindBuffer(GL_ARRAY_BUFFER, a);
		// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

		// Рисуем треугольник

		// glBindVertexArray(vao);
		// glLineWidth(5);
		// glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Delete all the objects we've created

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}