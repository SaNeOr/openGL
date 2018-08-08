#include "Shader.h"

#include<iostream>
#define GLEW_STATIC		//设置glew使用的是glew32s.lib

#include<GL/glew.h>
#include<GLFW/glfw3.h>
//glew中包括了openGL的头文件， 因此必须要先加载glew

float vertices[] = {
	0.5f,  0.5f, 0.0f,			// top right
	0.5f, -0.5f, 0.0f,			// bottom right
	-0.5f, -0.5f, 0.0f,			// bottom left
	-0.5f,  0.5f, 0.0f ,		// top left 
};

unsigned int indices[] = {
	0,1,3,
	1,2,3
};


void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main() {
	

	/*初始化；设置版本；*/
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//明确告诉GLFW我们要使用openGL的核心配置文件，也就是较小的功能子集


	//创建窗口对象
	GLFWwindow* window = glfwCreateWindow(800, 600, "GalGame", NULL, NULL);
	if (window == NULL) {
		printf("Open window failed");
		glfwTerminate();	//终止glfw
		return -1;
	}
	glfwMakeContextCurrent(window);	//确认window为当前线程

	//Init GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		printf("Init GLEW failed");
		glfwTerminate();
		return -1;
	}

	//告诉OpenGL窗口的大小
	glViewport(0, 0, 800, 600);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glCullFace(GL_FRONT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	

	Shader* testShader =new Shader("vertexSource.txt", "fragmentSource.txt");


	unsigned int VAO;
	glGenVertexArrays(1,&VAO); 
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);		//array_buffer||element buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);




	//render loop
	while (!glfwWindowShouldClose(window)) {
		testShader->use();

		//input
		processInput(window);	//下一帧执行Input

		//rendering commands here
		glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);	//利用颜色缓冲区 更换背景颜色

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);

		float timeValue = glfwGetTime();
		float greenValue = sin(timeValue) / 2.0 + 0.5;
		testShader->fragmentSource;
		int vertexLocation = glGetUniformLocation(testShader->ID, "ourColor");
		glUniform4f(vertexLocation, 0, greenValue, 0,1.0f);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();	//检测用户输入
	}

	glfwTerminate();
	return 0;
}


