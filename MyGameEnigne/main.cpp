#define GLEW_STATIC
#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include "Game.h";

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


//	GLFW function declerations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

//	The Width of the screen
const GLuint SCREEN_WIDTH = 1280;
const GLuint SCREEN_HEIGHT = 720;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

int main() {

	/*初始化；设置版本；*/
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//	明确告诉GLFW我们要使用openGL的核心配置文件，也就是较小的功能子集

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GAL GAME", nullptr, nullptr);
	
	glfwMakeContextCurrent(window);	//	make "window" as current context

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	//	close cursor
	glfwSetKeyCallback(window, key_callback);

	//	Init GLEW				glew:OpenGL SDK
	glewExperimental = true;
	glewInit();
	glGetError(); //	Call it once to catch glewInit() bug, all other errors are now from our application.

	//	OpenGL configuration
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);	//因为是2D， 所以不需要Z-buffer-test
	//	acbout blend https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/03%20Blending/
	glEnable(GL_BLEND);	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//	Initialize game
	Breakout.Init();

	//	DeletaTime variables
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	//	Start Game within Menu State
	Breakout.State = GAME_ACTIVE;

	while (!glfwWindowShouldClose(window)) {
		//	Calculate delta time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//deltaTime = 0.001f;
		//	Manage user input
		Breakout.ProcessInput(deltaTime);

		//Render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);		//没有z-buffer 所以不需要 GL_DEPTH_BUFFER_BIT
		Breakout.Render();

		glfwSwapBuffers(window);

		glfwPollEvents();	//	检测用户输入
	}

	glfwTerminate();
	return 0;
}


#pragma region Input Declarations
//key: which key; scancode:??; action:PRESS/RELASE... ; mode:??
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			Breakout.Keys[key] = GL_TRUE;
		else if (action == GLFW_RELEASE)
			Breakout.Keys[key] = GL_FALSE;
	}
}
#pragma endregion