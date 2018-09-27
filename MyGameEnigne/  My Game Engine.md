# Game Engine
>leran openGL文档中，最后的2D game istance， 作为像我这样noob来说， 感觉写的挺好的， 甚至可以当作一个简单的2D游戏引擎来用， 所以... 这里mark下自己实现这个简单的engine的过程。 可能，表述过程会是 自顶向下的吧。

[1. main()](#1)

[2. game()](#2)

[3. 资源管理器](#3)

[4. shader](#4.1) & [texture](#4.2)

[5. 渲染精灵](#5)

[6. level & Game object](#6)


### <div id = "1">1. main()</div>
主程序主要是按照顺序去调用各个功能模块，所以嘛， 是比较流程的， 就像降龙十八掌， 两船兵一样， 按照流程打一遍就好了。所以, 这个流程大体如下:
<del>18人口下2矿</del>
```
OPENGL INIT:

    Init openGL Version;

    Create window;

    Make window as current context

    Set key callback

GLEW INIT

OPENGL CONFIGURATION
    
    Viewport

    //CULL FACE? DEPTH TEST?

GAME INIT

//Update Game State(ACTIVE? MENU? WIN?)

RENDER LOOP{

    Process Input

    RENDER:
        Clear Screen

        Game render

    Swap Buffers

    PollEvents
    
}

```
基本来说， 就是这样吧， 初始化openGL: 比如说版本呀， 窗口大小呀， 设置为context， 设置下key的回调函数；
因为用了glew, 这个东西差不多就是个OpenGL的sdk吧， 所以也要初始化下它；
接着是设置下opengl的配置， 比如说显示的大小， 要不要开启反面不画， 深度测试等等；
然后就是游戏的初始化工作， 这个是大头， 等会在第二点"2. game()"里面细说；
然后就是 渲染循环了， 处理下 输入， 设置好render， 调用swap buffers， 都是老调重弹了， 就没什么好说的了。 进入下一点吧！

当然, 这里还有一个有意思的地方--Set key callback
用起来挺容易的， 

```
glfwSetKeyCallback(*Window, Function);
```
*window就不用多说了， 关于Function:
>5个参数， scancode和mode 不知道有啥用... 其实也挺简单的， 就是检测下action的状态， 如果PRESS就把对应的Keys[key]设置为TRUE就行了， RELEASE后再改为FALSE就行了。
```
//eg:
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
```
当然， 其实还有一个挺有意思的地方， glew的初始化， catch bug可以直接调用glGetError();
>Call it once to catch glewInit() bug, all other errors are now from our application.

### <div id = "2">2. game()</div>
开门见山的说， 主要分两部分:
1). 初始化游戏状态， 比如说加载shaders， textures, levels。
2). GameLoop, 主要就是 输入(操作) 和 渲染。

每个都是大头， 不过还好，很多东西都是封装好的，所以具体的实现细节得要到第三点之后才会谈， 这里主要还是流程。

#### 1). 初始化游戏状态， 比如说加载shaders， textures, levels
```
LOAD SHADER(vertex & fragment)

CONFIGURE SHADERS:
    Set texture slot
    
    MVP matrix


LOAD TEXTURE

LOAD LEVEL

GAME OBJECT(PLAYER) CONFIGURE
```
- Load shader， 也就是加载shader， 具体的实现会在第三、四点提到。
  

- Set  texture slot， 设置texture的槽位，texture的值虽然是uniform， 但是我们并不用glUniform给它赋值，而是使用glUniform1i，我们可以给纹理采样器(sampler)分配一个槽位(slot)，这样的话我们能够在一个片段着色器中设置多个纹理。一个纹理的位置值通常称为一个纹理单元(Texture Unit)。一个纹理的默认纹理单元是0，它是默认的激活纹理单元。


- MVP martix， 由于这里是2D game， 所以我们只需要M和P就行了， 因为我们不存在什么摄像头， 所以world to view就不要了， 我们只需要把model的crood转到world坐标， 然后做一个正交的projection就行了。
    ```
        glm::mat4 projection = glm::ortho(0.0f, Width * 1.0f, Height * 1.0f , 0.0f, -1.0f, 1.0f);
    ```
    关于最后两个参数，zNear zFar, zNear指的是摄像头到 视锥体近裁剪平面的距离， zFar则是到远裁剪平面的距离。 由于我们这里是正交状态， 所以两个平面大小是一样的， 因此 两个值一个设置成负数，一个设置成正数就行了，设多少都无所谓， 因为是正交， 所以远近不影响成像大小。

- Load Texture，也就是加载texture， 具体的实现会在第三、四点提到。

- Load level， 用来加载关卡， 具体的实现会在第五点提到。

- GAME OBJECT(PLAYER) CONFIGURE，主要是来介绍设置player的形象的， 细节同放到第五点里。

#### 2). GameLoop, 主要就是 输入(操作) 和 渲染
>输入这里会谈论其细节， 渲染则是等到下一点再涉及。
```
Judge Game State:
    Control Player Move
```
在玩家控制object移动之前， 先判断下游戏当前的状态， 至于是什么是游戏的状态？ 指的就是当前游戏是否是进行中呀， 还是打开了菜单， 还是怎么地了， 如果是打开了菜单， 那么这个时候游戏是否应该暂停等等...

openGL中， keyboard controller只需要创建一个bool Keys[1024]和上节谈论到的key call back就可以了，具体使用:
```
		if (Keys[GLFW_KEY_A]) {
			if (Player->Position.x >= 0) {
				Player->Position.x -= velocity;
			}
		}

		if (Keys[GLFW_KEY_D]) {
			if (Player->Position.y <= Width - Player->Size.x) {
				Player->Position.x += velocity;
			}
		}
	}
``` 
这里有个有意思的地方，如果 X>=0 就允许object往左移， 右移亦然，反观以前都是到了边界，就让object-position为一个定值，显然这个思路显然更合理， 不过...也容易出现bug.. 比如说object边缘卡到了屏幕外... 当然， 你会说在比较的时候 + 上velocity不就行了？ 然而... 如果这么做了， 虽然不会卡到屏幕外， 但是可能就移动不到屏幕边缘了。

渲染的话，当然就是 加载背景呀， 渲染关卡呀， player-object呀
```
Judege Game State:
    Draw Background
    Draw Levels
    Draw player
```



### <div id = "3">3.资源管理器</div>
>真香。

从设计模式来说， 这是一个单例模式(Singleton)， 也就是说这个类只有一个实例。 为什么说它真香呢？ 当然得先说说，"我就是从这跳下去， 就是死不写资源管理器"。 当写完shader和texture后， 那么直接调用它们就行了嘛， 要什么资源管理器？ 真是麻烦啊! 不过看到 GAME CLASS中 resourceManager的使用后， 不得不感觉, 真香！

ResourceManager 其实挺容易的， 就是用个map， 去给Shader/texutre取个名字， 这样的话， 调用的时候只需要ResourceManager::GetShader("shaderName") 就行。

先来看看Resource Manager的使用("来一口，老弟"):
```
ResourceManager::LoadShader("vertexSource.vert", "fragmentSource.frag", "sprite");
ResourceManager::GetShader("sprite").use().setUniform1i("image",0);
ResourceManager::LoadTexture("background.jpg", GL_RGB, "background");
```

首先，看看Resourece storage
```
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Texture2D>	Textures;
```
string的作用在于Load的时候，可以给resource设置一个name， 这样在get的时候可以根据name来调用， 真是超方便。 因此， Load 和Get 基本都是这样
```
static TYPE Loadsm(filePath, string name);
static TYPE Get(string name);
```

### <div id = "4.1">4.shader & texture</div>
刚刚的那些便利， 让我们只需要关注逻辑而不用关系底层的原因， 关键就是这里的shader和texture封装的好呀~~

先来说说shader吧。

与其说shader， 倒不如说是shader load， 至于具体的fragment shader和vertex shader，暂时并没有涉及到什么新的东西， vertex shader仅仅是处理下物体的position(mvp),给fragment传递下texcoords罢了， 而fragment shader也只是根据texture生成个颜色罢了。
```
//vertex shader
#version 330 core
layout(location = 0) in vec4 vertex;	//<vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 modelMat;
uniform mat4 projection;

void main(){
	TexCoords = vertex.zw;
	gl_Position = projection * modelMat * vec4(vertex.xy, 0.0, 1.0);	//dont need view-matrix

}


//fragment shader
#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;

void main(){
	color = vec4(spriteColor,1.0f) * texture(image, TexCoords);
}
```

所以， 回到重点--shader load。
通过编写shader class， 我们通过其实例来加载vertex shader和fragment shader， 以及通过它的方法， 来完成cpu和gpu的交互， 也就是设置uniform。

```
class Shader
{
public：
    //...
    Shader(const char* vertexPath, const char* fragmentPath);
    unsigned ID;

	const char* vertexSource;
	const char* fragmentSource;
    //...
    Shader& use();

    void setUniform4fv(const char* paramNameString,glm::mat4 param);
	void setUniform3f(const char* paramNameString, glm::vec3 param);
	void setUniform1i(const char* paraNameString, int slot);

private:
	void checkCompileErrors(unsigned int ID, std::string type);
}
```

这里的两个key是构造函数， 和check_errors。

构造函数的参数是两个shader的filePath。 然后通过fstream转到stringstream，之后再通过stringstream转到string，最后将string传给char array。

>之所以这么做，因为这么操作很方便，通过文件流来读取文件的内容在c++stream的加持下十分容易，而为什么不直接把fstream传给char*， 是因为stream只能传给stream， stringstream则是负责将 stream传给string的， 就像fstream是把file传给stream一样的， 一进一出。

在写fstream遇到两个有意思的地方。
一个是exceptions， 一个则是 rdbuf():

从形式上来看， 先设置好exceptions， 然后就是try中去调用fstream::open()， 之后便是catch。 所以不能发现有两个看上去不对劲的地方:
```
    1. 为什么在open之前去执行？ 
    2. 为什么设置了它后， 就不用设置throw了？
```
查看c++reference便可清楚， 这家伙设置的是stream的internal key， 所以...设置了它， 就相当对给stream对象开了光， 设置好了后，相对于启动了catch bug，一旦stream遇到了我们设置好的错误状态时， 变回throw一个exception。

>void exceptions (iostate except);
The second form (2) sets a new exception mask for the stream and clears the stream's error state flags (as if member clear() was called).

至于rdbuf()， 其实就是读取file stream的内容， 可以直接把file的内容传递给string stream。
```
ifstream vertexFile;
stringstream vertexSStream;

vertexSStream << vertexFile.rdbuf();
```

所以还是如之前说的一样， stream确实很方便呀。
```
	ifstream vertexFile;
	ifstream fragmentFile;
	stringstream vertexSStream;
	stringstream fragmentSStream;
	string vertexString;
	string fragmentString;

	vertexFile.exceptions(ifstream::failbit || ifstream::badbit);
	fragmentFile.exceptions(ifstream::failbit || ifstream::badbit);
	try {
		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);
		//badbit/failbit == throw exception; 

		vertexSStream << vertexFile.rdbuf();
		fragmentSStream << fragmentFile.rdbuf();
		vertexFile.close();
		fragmentFile.close();

		vertexString = vertexSStream.str();
		fragmentString = fragmentSStream.str();

		vertexSource = vertexString.c_str();
		fragmentSource = fragmentString.c_str();
    }
    catch(ifstream::failure e){         //failure就是fstream的exception
		cout << e.what() << endl;
	}
```

说完读档之后， 便是将shader和program连起来了。

这个过程也是很流程的。
```
COMPILE:

    COMPILE VERTEX

    COMPILE FRAGMENT

LINK:

    ATTACH VERTEX

    ATTACH FRAGMENT

    LINK TO PROGRAM
```
当然， 如果compile或者link出现了bug怎么办了？ 所以这里就会用的 glGetShaderiv() & glGetProgramiv(),和glGetShaderInfoLog() & glGetProgramInfoLog()来检查错误。

故名意思， 一个是检测shader的错误， 而一个是检测program的错误。
因此， 我们可以封装好一个方法， 作为shader的私类， 来debug。
```
void Shader::checkCompileErrors(unsigned int ID, std::string type)
{
	int success;
	char infoLog[512];

	if (type != "PROGRAM") {
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(ID, 512, NULL, infoLog);
			cout << "shader compiler error" << infoLog << endl;
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
```

之后， 只需要在compile shader或者link program后调用checkCompilerErrors就行。
```
//...
glCompileShader(vertex);
checkCompileErrors(vertex, "VERTEX");
//...
glCompileShader(fragment);
checkCompileErrors(fragment, "FRAGMENT");
//...
glLinkProgram(ID);
checkCompileErrors(ID, "PROGRAM");
```

关于use()， 是一个简单的调用glUseProgram()的方法，之所以设计个它， 是因为一旦要修改shdaer的时候， 得需要先激活shader。

leranOpenGL中则是将use的返回值设置成了*this， 这样的话在使用完后use便可以直接后接一个方法， 挺cooool的。  

```
//给shader的texture设置slot
ResourceManager::GetShader("sprite").use().setUniform1i("image",0);
```

最后就是简单的封装一些setunifrom。
```
void Shader::setUniform4fv(const char * paramNameString, glm::mat4 param)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, paramNameString), 1, GL_FALSE, glm::value_ptr(param));
}

void Shader::setUniform3f(const char * paramNameString, glm::vec3 param)
{
	glUniform3f(glGetUniformLocation(ID, paramNameString), param.x, param.y, param.z);
}

void Shader::setUniform1i(const char * paraNameString, int slot)
{
	glUniform1i(glGetUniformLocation(ID, paraNameString), slot);
}
```

### <div id = "4.2"></div>
说完shader， 之后就是texture了。
texture, 虽然对于code来说， 看上去挺容易的， 但是实际上的内部原理还是非常麻烦的， 比如说高分辨率的图放到小的空间中， 抗锯齿的问题，小分辨率的图放到大分辨率中模糊的问题，纹理坐标范围[0,1]外的texture的环绕方式 等等。 所以texture还是挺复杂的， 毕竟...你不想你的游戏看上去都是马赛克嘛。 （当然，你也可以说这是你的这个游戏的风格)

Texture class 主要就是一个方法， load image to GPU(), 当我就是这么native以为的时候，直到后来加载了多个texture， 我才意识， 我对texture的理解有欠缺呀~ 至于到底哪里有问题呢？ 等第5点， 关于渲染的部分再说吧。 这里就是 谈论下 load image to gpu的部分。
```
void Texture2D::loadImageToGPU(const char * filePath, int format)
{

    glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glActiveTexture(GL_TEXTURE + textureSlot);

	int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true);

	data = stbi_load(filePath, &width, &height, &nrChannels,0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

	}
	else {
		printf("loat image failed!");
	}
	stbi_image_free(data);
	//glBindTexture(GL_TEXTURE_2D, ID); // if sec para is 0, means unbind;
    glBindTexture(GL_TEXTURE_2D, 0);
}

```
基本流程和生成VBO什么的差不多， 都是先声明一个buffer ID， 然后glGenXXXX()去指定对象数量和ID, 然后就是bind， 最后是生成。

glTexImage2D， 就是用来生成纹理的。
>当调用glTexImage2D时，当前绑定的纹理对象就会被附加上纹理图像。然而，目前只有基本级别(Base-level)的纹理图像被加载了，如果要使用多级渐远纹理，我们必须手动设置所有不同的图像（不断递增第二个参数）。或者，直接在生成纹理之后调用glGenerateMipmap。这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。

但是这里有个问题， 我们这里texture只有一个槽位， 而我们要渲染的有多个objcet，而且他们的texture都不一样， 该怎么办呢？ 其实...这个我是理解的， 因为opegGL的渲染过程是一个状态机， 每次只渲染一个VAO， 因此即使我们把所有texture设置在一个槽位上其实并没关系， 但是这就有个地方要注意了， 在render loop中， 在glDrawElements()前， 必须得要去告诉vao， "嘿嘿嘿， 你现在要重新绑定一个texture啦"，如果我们在Draw之前没有去告诉VAO换了新的texture， 那么我们观察下， 最后一次调用 glbinTexture()是在loadImageToGPU中， GenTextures()之后，这里的作用是来给fragment传递texture的， 因此...如果没有在render loop中重新绑定， 那么最终的效果就是所有的object的texture都是最后一个被加载的texture的效果。

不要问我 为什么这里的感悟这么深... 

### <div id = "5">5. 渲染精灵</div>
渲染精灵的作用主要是就是负责初始化渲染的data， 和开启渲染。

初始化依旧是很流程...
```
CONFIGURE VAO/VBO

SET VERTICES[]

GEN/BIND VAO

GEN/BIND VBO

SET ATTRIBUTE
```

渲染部分:
```
void SpriteRenderer::DrawSprite(Texture2D texture, glm::vec2 position, 
								glm::vec2 size, GLfloat rotate, glm::vec3 color)
{
	shader.use();
	
	glm::mat4 modelMat;
	modelMat = glm::translate(modelMat, glm::vec3(position, 0.0f));

	modelMat = glm::translate(modelMat, glm::vec3(0.5f * size.x, 0.5 * size.y, 0.0f));
	modelMat = glm::rotate(modelMat, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMat = glm::translate(modelMat, glm::vec3(-0.5f*size.x, -0.5f * size.y, 0.0f));

	modelMat = glm::scale(modelMat, glm::vec3(size, 1.0f));

	shader.setUniform4fv("modelMat", modelMat);
	shader.setUniform3f("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();

	// Set Model
	glBindVertexArray(quadVAO);

	// Drawcall
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glBindVertexArray(0);

}
```

通过给DrawSprite传递texture， 以及缩放、位置、旋转、颜色， 来决定sprite的最终效果。

texture.bind(), 则是在设置texture前， 重新bind下texture。(如果有多个texture公用一个slot)， 需要在每次draw之前都bind下， 当然， 如果不需要改texture， 不bind也行。最后便是喜闻乐见的Drawcall啦。


### <div id = "6">6. level & Game object</div>
好啦， 终于到了最后一点了。

这里的Game level很简单， 就是负责 保存每个level的state， 以及一个用来绘制当前窗口的接口， 最后便是检测游戏是否结束的判断方法。
```
class GameLevel
{
public:

	//	Level state
	std::vector<GameObject> Bricks;

	//	Constructor
	GameLevel(){}

	//	Loads level from file
	void Load(const char* file, int levelWidth, int levelHeight);

	//	Render level
	void Draw(SpriteRenderer &renderer);

	//	Check gg?
	bool IsGG();

	~GameLevel();

private:
	//	Initialize level from tile data
	void init(std::vector<std::vector<int> > tileData, int levelWidth, int levelHeight);
};

```

这里的Load的具体实现，只能说提供了一定的思路， 因为这是一个打砖块的游戏, 所以可以直接在文本用数字中保存关卡的状态， 然后读取到二位数组中。 最后通过init计算出每个砖块的大小和位置加载到Bricks中。 

所以,GameObject长啥样呢？ 不难想象， 我们肯定是需要一个用来保存每块砖的状态的， 那这个游戏里砖的形态无非就是 存在/不存在， 是不是不朽的？
```
class GameObject
{
public:

	//	Object state
	glm::vec2 Position, Size, Velocity;
	glm::vec3 Color;

	float Rotation;
	bool IsSolid;
	bool Destrotyed;

	//	Render state
	Texture2D Sprite;

	//	Constructor(s)
	GameObject();
	GameObject(	glm::vec2 pos, glm::vec2 size, 
				Texture2D sprite, glm::vec3 color = glm::vec3(1.0f),
				glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	//Drawa sprite
	virtual void Draw(SpriteRenderer &renderer);


	~GameObject();
};
```
所以， 就如同上面一样， 并没有什么很特别的啦。

最后来看看封装好Level 以及gameobject后， 渲染是多方便。
```
void Game::Render()
{
	if (State == GAME_ACTIVE) {
		Renderer->DrawSprite(	ResourceManager::GetTexture("background"),
								glm::vec2(0, 0), glm::vec2(Width, Height), 0.0f);

		Levels[Level].Draw(*Renderer);
		Player->Draw(*Renderer);
	}

}
```


