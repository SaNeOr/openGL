# Creating a Window
1. two libraries
    - GLFW
    - GLEW
3. how to create a window?


### 1.1. GLFW
>GLFW, it provides a simple API for creating  windows, contexts and surfaces, receving input and events.(openGL For creating Windows?2333)

目前而言， 通过GLFW这个库可以让我们快速、简单的利用openGL码出一个窗口。

### 1.2. GLEW(openGL Extension Wrangler)
>GLEW, it is a simpler tool that helps C/C++ developers initialize extensions and write protable applications.

### 3.how to create a window
- two steps
    1. Linking
    2. Hellow Window

1. Linking
我们都知道，C/C++中头文件只是用来宣告有那些变量或者函数我们可以使用，真正的实现是在lib/dll中。
因此，在项目中，先把我们下载好的两个新库的头文件附加进来，然后再把lib的目录和具体的
lib通过链接器链接进来，从而完成了第一步。

2. Hellow Window
```
#include<iostream>
#define GLEW_STATIC		//设置glew使用的是glew32s.lib

#include<GL/glew.h>
#include<GLFW/glfw3.h>
//glwe中包括了openGL的头文件， 因此必须要先加载glwe

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


	//render loop
	while (!glfwWindowShouldClose(window)) {

		//input
		processInput(window);	//下一帧执行Input

		//rendering commands here
		glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);	//利用颜色缓冲区 更换背景颜色

		//check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();	//检测用户输入
	}

	glfwTerminate();
	return 0;
}

```
