# Shader
1) 变色
2) 定点色
3) shader读取器
之前在三角形的绘制中，使用了openGL文档中给出的shader通过字符数组的方式完成绘制。所以,现在我们要通过几个简单的实例去了解shder到底写了什么。

### 1. 变色
上次我们绘制的三角形是一个橘色(大橘已定?)的，然而这个橘色是怎么喷上去的呢? 程序中显然没有颜色绘制的代码嘛， 所以, 真相应该就是在shader中了。
``` 
const char* fragmentShaderSource =
"#version 330 core                                  \n"
"out vec4 FragColor;                                \n"
"void main(){                                       \n"
"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);}         \n";
``` 
在fragmentShdaerSource中， FragColor就是用来设置颜色的，(1.0f, 0.5f, 0.2f, 1.0f)就是橘色啦。 那如果我们想改变颜色只用把这里的颜色换成其它的就行了(R,G,B,A)。 然而如果我们想让它不停的变色呢？ 很可惜, GPU并没有时间的概念， 因此没办法直接通过Shader去完成， 然而CPU是有CPU的概念, 因此我们要想办法通过CPU传数据给GPU， 这个时候就可以利用uniform了。

在介绍uniform之前应该要先稍微了解下GLSL的语法...


```
//A shader typically has the following structure:
#version version_number
in type in_variable_name;
in type in_variable_name;

out type out_variable_name;
  
uniform type uniform_name;
  
void main()
{
  // process input(s) and do some weird graphics stuff
  ...
  // output processed stuff to output variable
  out_variable_name = weird_stuff_we_processed;
}
```


```
//因此triangle中的shader的意思就是:
const char* vertexShaderSource =
"#version 330 core                                            \n"   //#version类似于define， 这里是用来宣告使用的shader版本 "330 core" 指的就是3.3版
"layout(location = 0) in vec3 aPos;                           \n"   //layout(location = x) 是用来设置 off-set的， 也就是VAO中attribute的起始位置
                                                                    //vec2;vec3;vec4 是变量类型(vector) aPos就是变量名
                                                                    //这条语句来实现把VAO中从0byte的位置开始， 写入到一个vec3 aPos中。
"void main(){                                                 \n"
"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);}            \n";  //gl_Position是已有的变量， 用来设置图像的vertex。

const char* fragmentShaderSource =
"#version 330 core                                  \n"
"out vec4 FragColor;                                \n"     //out是输出到graphic pipeline； 如果要接受上一个流程的某个变量， 则用in， 变量名要保持一致
"void main(){                                       \n"
"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);}         \n";
```

OK, 感觉很容易对吧？ 接下里就可以去完成实现triangle的变色了，fragmentShdaer会负责喷色，因此我们要在fragmentShdaer添加一个uniform变量，由于颜色是个4维变量， 因此其类型就是vec4了，最后再把out的对象在mian()中把赋值位uniform对象。
```
#version 330 core
out vec4 FragColor;
uniform vec4 ourColor;
void main(){
    FragColor = ourColor;
}
```

因为uniform是CPU和GPU交流的信鸽， 因此我们在程序中去完成传递， 其实主要就是两步: <b>标记</b>下uniform位置，然后<b>设置</b>uniform的值。

但是，如何让它不停的变色呢？ 我们仅仅知道和时间有关欸~  不过，了解到RGB中每个颜色的值域是[0,1]， 我们因该需要一个以时间为自变量，值域在[0,1]变换的函数， 于是..sin成为了一个非常好的候选， (sin(time) + 1)/2, 完美的完成了我们的需求。

```
float timeValue = glfwGetTime();        //获得当前时间
float greenValue = sin(timeValue) / 2.0 + 0.5;  //以时间为自变量, 值域在[0,1]变换

//定位
int vertexColorLocation  = glGetUniformLocation(shaderProgram, "ourColor");
//设置
glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);     //故名思意, 4f就是4个float， 同理还有1f,2f,3f, 1i,2i,3i,4i(i: GLint)
```
所以,这样就好了? 没错, 就是这么简单... 这样就能实现变色了.

summary, 通过这个例子, 我们知道了如果通过cpu向gpu传送数据(通过uniform).


### 2. 定点色
第一个例子我们知道了如何通过fragmentShader修改对象颜色, 然而事实上fragmentShader是可以控制每个vertex的color的, 由于我们只穿了三个vertices, 因此中间部分会被做均匀化处理(很智能?). 

- 第一步:
给三个顶点添加color info
```
        float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 

    };
```

- 第二步:
增加有关color info 的attribute; 修改关于postion info的attribute的stride


由于添加了color info, 因此,VBO不再是一个顶点接着一个顶点了(tightly packed), 而是开始夹杂中有关color的info了. 因此glVertexAttribPointer要换新的了, 当然设置完atrribute后别忘了enable.

>glVertexAttribPointer:
第一个参数设置attribute的索引值, 和vertexShdaer中的location对应. (范围:[0,16)  )
第二个参数设置每个attribute的size, 这里vertex postion和color都是3个float, 因此都设置为3.
第三个参数设置attribute类型(GL_FLOAT,GL_BYTE,GL_SHORT...)
第四个参数设置是否normalized, (GL_TRUE或GL_FALSE)
第五个参数设置stride, 这里也就是指第一个vertex positon的起点和第二个的距离, 即6个float.
第六个参数设置offset, postion的offset是0, 而第一个color的offset是过了三个float, 因此就是3 * sizeof(float).
```
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
```
- 第三步:
增加有关color info的vertexShader
```
  #version 330 core                                         
  layout(location = 0) in vec3 aPos;                        //location :off-set
  layout(location = 1) in vec3 aColor;                      //location :off-set
  out vec4 vertexColor;                                     
  void main(){                                              
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);          
    vertexColor = vec4(aColor.x, aColor.y, aColor.z, 1.0);
  }
  //vec的赋值也有另外一种写法:
  gl_Position = vec4(aPos, 1.0);
  vertexColor = vec4(aColor, 1.0);
```
之前在GLSL语法中简单说过了,  out的变量会传递给下一个流程, 因此, 到fragmentShdaer要设置好in.
```
#version 330 core                            
in vec4 vertexColor;                         
uniform vec4 ourColor;
out vec4 FragColor;                                
void main(){                                       
	FragColor = vertexColor;
}
```

summary, 通过设置顶点色, 让我们对fragmentShaderd的处理方式更了解了, 也同时更了解了GLSL的out和in, 同时也对VAO的attribute更为了解.


### 3. shader读取器
然而, 不管怎么样, 通过char array去写shader实在是太丑了, 而且很麻烦, 因此我们需要一个可以从文件中读取shader code的function, 从而让我们可以直接在txt中完成shader code.

当然, 既然把shader的读取写到一个函数里了, 为什么不直接把与shader有关的写到一个class里呢?

首先第一个目的,我们得把文件内容写到char array中, 一个明显的区别, 文件包括了很多信息,比如其所在位置, 创建时间,等等.  明显不等于 char array, 因此我们无法直接把文件内容赋值给一个char array, 因此在c++中, 我们可以利用 strem 和 string 作为媒介, 为什么呢? 因为stream可以用来读取文件内容, 而stream可以转换成string, 而string 是可以 转换成 char array的.

由于我们将其封装成一个类, 因此, 首先声明下头文件
```
//Shdader.h
class Shader{
  public:
    Shdaer(const char* vertexPath, const char* fragmentPath);
  /*TODO*/
    const char* vertexSource;
    const char* fragmentSource;
    unsigned int ID;    //shader program handle
  /*TODO*/
};

```

然后在cpp中去完成定义.
```
#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

#define GLEW_STATIC		//设置glew使用的是glew32s.lib
#include<GL/glew.h>

using namespace std;

//Shader.cpp
ifstream vertexFile;
ifstream fragmentFile;
stringStream vertexSStream;
stringStream fragmentSStream;

//exception check
vertexFile.Exception(ifstream::failbit || ifstream::badbit);
fragmentFile.Exception(ifstream::failbit || ifstream::badbit);
try{
  vertexFile.open(vertexPath);
  vertexFile.open(fragmentPath);
  if(!vertexFile.is_open() || !vertexFragment.is_open()){
    throw exception("open file error"); 
  }
  //fileStream to stringStream
  vertexSStream << vertexFile.rdbuf(); //filestream to stringstream
  fragmentSStream << fragmentFile.rdbuf(); //rdbuf() 真是好用,,
  vertexFile.close();
  fragmentFile.close();
  
  //stringSteam to string
  vertexString = vertexSStream.str();
  fragmentString = fragmentSStream.str();

  //string to char array
  vertexSource = vertexString.c_str();
  fragmentSouce = fragmentString.c_str();

  //create shdader
  unsigned int vertex, fragment;
  
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertexSource, NULL);
  glCompileShader(vertex);		//binary ocde
  //checkCompileErrors(vertex, "VERTEX");
  
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragmentSource, NULL);
  glCompileShader(fragment);
  //checkCompileErrors(fragment, "FRAGMENT");

  ID = glCreateProgram();
  glAttachShader(ID,vertex);		//excute code
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  //checkCompileErrors(ID, "PROGRAM");

  glDeleteShader(vertex);
  glDeleteShader(fragment);

}

cath(const std::exception e){
  printf(e.what()); //what() 会抛出 throw exception的内容
}
``` 

在main()中, glViewport()后, 我们不再需要那么长的createa shader program了, 噢, 当然, 开头的char array也不再需要了, 可以把fragmentSource 和 vertexSource 写到两个文本里. 我们现在只需要在render loop前, 创建一个Shader 对象就好了.

```
Shader* testShader =new Shader("vertexSource.txt", "fragmentSource.txt"); //arguments 就是两个source的path
```

当然, 这样的话在render loop中 glUseProgram() 也可以写到我们的类中.
```
//Shader.h
#pragma once
#include<string>
class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);

	const char* vertexSource;
	const char* fragmentSource;
	unsigned int ID;					//Shader Program ID
	void use();
};

//Shader.cpp
void Shader::use() {
	glUseProgram(ID);
}
```

关于glUseProgram不得不说下,gluseprogram: Installs a program object as part of current rendering state, 它就是用来修改当前正在active的shader的, 换而言之, 就是一旦修改了shader, 就得在调用一次它. 所以呢, 把它放在render loop里, 确实再方便不过了, 这也说明了, 其实对于目前我们写的shader, glUseProgram放到 render loop外面也是可以的.

> Then we query for the location of the ourColor uniform using glGetUniformLocation. We supply the shader program and the name of the uniform (that we want to retrieve the location from) to the query function. If glGetUniformLocation returns -1, it could not find the location. Lastly we can set the uniform value using the glUniform4f function. Note that finding the uniform location does not require you to use the shader program first, but updating a uniform does require you to first use the program (by calling glUseProgram), because it sets the uniform on the currently active shader program.

###### debug
不过, 这里有个很大的问题, 如果shader 中有任何问题, 程序还是会正常跑起来, 只是不显示出绘制的对象罢了, 但是哪里有问题, 是没有任何提示的, 这就很尴尬了, 因此我们需要和vs对c++程序出错误, 一样的报错信息, glGetProgramInfoLog会帮我get错误的info.
```
//Shader.h
class Shader
{
  //...
private:
	void checkCompileErrors(unsigned int ID, std::string type);

};

//shader.cpp
//chek compile or link error
void Shader::checkCompileErrors(unsigned int ID, std::string type) {
	int success;
	char infoLog[512];

	if (type != "PROGRAM") {
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(ID, 512, NULL, infoLog);
			cout << "shader compile error" << infoLog << endl;
		}
	}
	else {
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			cout << "program linking error" << infoLog << endl;
		}
		
	}
}

//因此, 也是在shader compile或者link的时候去check

  glCompileShader(vertex);		
  checkCompileErrors(vertex, "VERTEX");

  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");

  glLinkProgram(ID);
  checkCompileErrors(ID, "PROGRAM");

//FYI: iv什么意思?
```
