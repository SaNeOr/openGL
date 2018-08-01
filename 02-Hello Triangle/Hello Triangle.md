# Hello Triangle
1. obj-info into vertices
2. vertices into VBO
3. VAO
4. shader program
5. draw

##### About graphics pipeline
- two parts:
    1. 3d coordinates into 2d coordinates
    2. 2d coordis into actual pixels

(a 2d coordinate and a pixel are difference, 一个2d坐标是指一个具体的点，而一个像素取决于屏幕的分辨率)
>graphics pipeline是通过GPU上的small programs实现的， 这些small programs就叫做shaders。(gpu原来只用来跑阴影，历史名词)而其所用的language:GLSL。

------------
##### 如何把obj数据传到vertx shader？

把.OBJ的数据 parse成一个数组,然后传到gpu的VBO中， 然后用VAO(一个模型一个vao)去挖vbo， 最后把VAO传到vertex shader

### 1. obj-info into vertices
用建模软件(如blender)所导出的waverfront是.obj格式的文件，其中包含了模型的顶点、uv、normals等数据，从中parse我们所需要的数据作为vertices(也就是一个数组啦)。
```
假设这就是我们从.OBJ中获得的数组
float vertices[] = {
	0.5f,  0.5f, 0.0f,  
	0.5f, -0.5f, 0.0f,  
	-0.5f, -0.5f, 0.0f
};
```

### 2. vertices into VBO
其实，VBO就是定点缓冲区,that can store a large number of vertices in the GPU's memory.  因此，这一步就是将cpu的数据传到gpu上而已啦。
如何做呢? 其实，很简单， 只用声明一个VBO， 然后用glGenBuffers生成它，接着声明我们所需要绑定的buffer的类型(因为openGL有很多种buffer对象)，然后将其和最后在把我们已经得到的vertices用glBufferData填充进去就好了。
```
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);		
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```
### 3. VAO
然而，VBO说到底都还只是一些数据而已，GPU怎么知道哪些是哪些？ 因此，我们需要一个数据结构来给GPU解释下这些数据的内涵。VAO会将VBO的数据读入VAO中，但是VAO也不是法力无边，它也不知道这些数据是什么，不过我们可以通过glVertexAttribPointer来调教它如何看待(attribute)这些数据，之后，这些数据就会作为VAO的参数(attribute)...(难怪attribute动词意思是"看待"，名词是"属性"...)
```
    unsigned int VAO;
    glGenVertexArrays(1,&VAO); 
    glBindVertexArray(VAO);
    /*...*/
    //glVertexAttribPointer
```
>arrays，buffers，这两个方法都有个s, 因为他们是可以处理数组的啦!

### 4.shader program
这是一个大坑，下章来填，这里直接抄最简单的shader来完成我们三角形的绘制。
首先要明白，shader program需要vertexShader和fragmentShader两个大步骤，因此这两个GLSL分别是
```
const char* vertexShaderSource =
"#version 330 core                                            \n"
"layout(location = 0) in vec3 aPos;                           \n"	//location :off-set
"void main(){                                                 \n"
"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);}            \n";


const char* fragmentShaderSource =
"#version 330 core                                  \n"
"out vec4 FragColor;                                \n"
"void main(){                                       \n"
"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);}         \n";
``` 

>FYI:这里直接把GLSL当字符串来处理了， 就省去了读入的部分了(当然，这只是权宜之计)，有一个语法细节， 字符串换行，以前写macro function的时候会常用到， 然而一般都是在 行末加上"\\",而这里是另外一种方法——每行都标上双引号。 这么写最大的好处可能就是， 可以写上行注释了。

有了这两个source之后，我们就可以创建对应的两个shader了(很直白)
```
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
```

最后将这两个shader attach到program中，并将shaderProgram link到程序中。
```
	unsigned shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);	
```
从而算是完成了shader的部分了。

Right now we sent the input vertex data to the GPU and instructed the GPU how it should process the vertex data within a vertex and fragment shader. We're almost there, but not quite yet. OpenGL does not yet know how it should interpret the vertex data in memory and how it should connect the vertex data to the vertex shader's attributes. We'll be nice and tell OpenGL how to do that.

最后我们再补充上调教VAO看待VBO的部分
```
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
```

### 5. draw


```
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 3);           //从vertices中第0个vertex开始， 一共三个顶点
```
然而，当我们使用VBO作为buffer去绘制四边形的时候，就比较奇怪了， 由于我们实际上一直都是在画三角形，因此在面对四边形的时候，实际上是画了两个三角形，那么此时VAO所对应的vertices就得要6个顶点了，这个时候我们就可以使用EBO。

### EBO
其实，openGL实际上是一个状态机，每次只有一个context，这个context里面会有一个VAO,而这个VAO是可以通过array buffer绑定多个VBO的，当然这里的重要是介绍EBO，Element Buffer Object，索引缓冲对象， 通过它我们只需要去填写每个三角形对应的索引值就好了，而不需要在vertices上写重复的定点了。
```
float vertices[] = {
	0.5f,  0.5f, 0.0f,  // top right
	0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
};

unsigned int indices[] = {
	0,1,3,
	1,2,3
};
```
>需要注意的是openGL是逆时针绘制的， 如果定点的顺序是顺时针那么绘制的将是图形的背面。 通过glEnable开启cull face 就显而易见了。
```	
    /*....*/
    glViewport(0, 0, 800, 600);

    glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glCullFace(GL_FRONT);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //使用这个函数会使图形只绘制边框
```

当然，使用了EBO后，draw的方法就会有点小区别了
```
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    //glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glUseProgram(shaderProgram);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```
