#Texture
1) 加载一个纹理
2) 纹理 * 颜色
3) 纹理 * 纹理

通过上一篇的shader, 我们可以绘制一个多边形, 并且给它每个顶点上个色, 但是如果要控制每个顶点的颜色从而去完成一个我们想要的效果, 实在是太麻烦了.
因此, texture就是一个理想的选择, 我们可以通过texutre和多边形相关联, 简单来说, 就是在多边形上做标记(纹理坐标(Texture Coordinate)), 对应着texture原图像的位置, 从而完成了纹理映射(Map).

### 0. 加载纹理之前
在加载纹理之前, 我们需要先了解一些纹理的必要知识.

- 基本过程:
    首先, 纹理坐标(2D)的x,y轴是 以左下角为原点的, 且范围都是[0,1], 而使用纹理坐标获取纹理颜色叫做采样(Sampling). 我们只需要给顶点着色器传递纹理坐标, 只有交给片段着色器去处理就行了, 听上去还是非常容易的.

- warp:
    如果纹理坐标在[0,1]之外的话, OpenGL默认是重复这个纹理图像, 然而事实上openGl提供了4种处理方式:
    
    |环绕方式|描述
    |-|-|-|
    |GL_REPEAT	|对纹理的默认行为。重复纹理图像。
    |GL_MIRRORED_REPEAT	|和GL_REPEAT一样，但每次重复图片是镜像放置的。
    |GL_CLAMP_TO_EDGE	|纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
    |GL_CLAMP_TO_BORDER	|超出的坐标为用户指定的边缘颜色。
- filter:
    纹理过滤是个很重要的话题了, 如果一个texture分辨率很高, 但是被缩小很多倍后, 那么那个很小的点, 到底该是什么颜色呢?  这里主要讨论两种过滤方式: GL_NEAREST和GL_LINEAR.
    GL_NEAREST: 它会显示离中心距离最近的像素颜色.
    GL_LINEAR : 它会根据中心点附近的像素, 通过计算距离中心点的距离做一个加权,得出一个插值.
    从效果上来说, NEAREST会显得比较有颗粒感, 而LINEAR则比较光滑.
    当纹理被放大缩小的时候, 就可以始终该参数, 经行设置.

    然而, 这里还是存在一个问题, 如果是一个很多东西的场景里面, 当我们把镜头拉远时, 不同物体之间可能就会串味, 其次, 每次都要去计算颜值, 也非常浪费性能, 因此Mipmap就是一个巧妙的解决办法.
    Mipmap: 多级渐远纹理, 也就是说, 我们会一个纹理, 创建一套纹理, 当对象变小的时候, 我们就偷偷更换掉纹理, 指的注意的是, 后面一个纹理图像边长是前一个的1/2, 也就是他们之间都是2的幂的关系. (这也就是 为什么有什么我们玩游戏的时候, 当离一个地方越来越远的时候, 突然一下就变模糊的原因啦) 当然, 两张纹理偷还的时候, 也可以选择过滤方式, 使得不那么生硬.

    FYI: 故名思意, 多级渐远纹理, 是用来处理缩小纹理时候的方法, 如果为放大texture使用它, 将会报错(GL_INVALID_ENUM).

### 1. 加载一个纹理
说了这么多, 终于到了加载纹理的实践阶段了.
如何把图片加载到应用中呢? 貌似是个很麻烦的事, 还好已经有人帮我们铺好了路---<a href="https://github.com/nothings/stb/blob/master/stb_image.h">stb_image.h</a>库.

下载好后, 在主程序中添加以下两行语句, 从而完成库的导入.
```
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
```
>通过定义STB_IMAGE_IMPLEMENTATION，预处理器会修改头文件，让其只包含相关的函数定义源码，等于是将这个头文件变为一个 .cpp 文件了。现在只需要在你的程序中包含stb_image.h并编译就可以了。

之后通过调用stbi_load函数来完成图片的加载.
```
int width, height, nrChannels;
unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
```
显然, 前四个参数是图片位置; 宽; 高; 通道数量; 至于第五个参数..默认设为0?

然后就是生成纹理, 这个和VBO, EBO什么的差不多, 都是先声明一个buffer ID, 然后glGenXXXX() 去指定下对象数量和ID, 然后就是绑定,最后就是生成, 这个过程并没有什么创新的地方..

```
unsigned int texture;
glGenTextures(1, &texture);

glBindTexture(GL_TEXTURE_2D, texture);  //VBO是GL_ARRAY_BUFFER

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
glGenerateMipmap(GL_TEXTURE_2D);
```
需要注意的是glTexImage2函数, 它是用来生成纹理的
- The first argument specifies the texture target; setting this to GL_TEXTURE_2D means this operation will generate a texture on the currently bound texture object at the same target (so any textures bound to targets GL_TEXTURE_1D or GL_TEXTURE_3D will not be affected).
- The second argument specifies the mipmap level for which we want to create a texture for if you want to set each mipmap level manually, but we'll leave it at the base level which is 0.
- The third argument tells OpenGL in what kind of format we want to store the texture. Our image has only RGB values so we'll store the texture with RGB values as well.
- The 4th and 5th argument sets the width and height of the resulting texture. We stored those earlier when loading the image so we'll use the corresponding variables.
- The next argument should always be 0 (some legacy stuff).
- The 7th and 8th argument specify the format and datatype of the source image. We loaded the image with RGB values and stored them as chars (bytes) so we'll pass in the corresponding values.
- The last argument is the actual image data.

>当调用glTexImage2D时，当前绑定的纹理对象就会被附加上纹理图像。然而，目前只有基本级别(Base-level)的纹理图像被加载了，如果要使用多级渐远纹理，我们必须手动设置所有不同的图像（不断递增第二个参数）。或者，直接在生成纹理之后调用glGenerateMipmap。这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。

最后是释放图像内存.
```
stbi_image_free(data);
```

一个完整的例子:
```
	unsigned int TexBufferA;
	glGenTextures(1, &TexBufferA);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexBufferA);

	int width, height, nrChannel;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannel, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		printf("load image failed.");
	}
	stbi_image_free(data); 
```

当然了, 之前说过了, 我们要在vertex shader传递纹理坐标. 因此, 我们需要在vertices中添加 纹理坐标:
```
float vertices[] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};
```
因此, 我们也需要修改对应的顶点attribute(3+3+2):
```
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

```

然后对应修改vertex shader:
```
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
```

而在fragment shader中, 利用自带的Sampler类型, 和texture函数, 去设置纹理对象和纹理坐标
```
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
```

最后在render loop中, 只需在glDrawElements, 去绑定纹理, 就完成了所有的操作.
```
glBindTexture(GL_TEXTURE_2D, texture);
//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```

### 2. 纹理 * 颜色
如果想要给纹理混上个颜色, 直选哟在fragment shader中, 利用乘号来修改FragColor就行了.
```
FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
```
### 3. 纹理 * 纹理
我们之前说过, 有array_buffer, element_bufferd等, 这样用来连接VAO的接口,  TEXTURE_2D和他们是一样的存在, 不同的是它是默认的激活单元, 且默认单元是0(GL_TEXTURE0), 所以, 在创建纹理的时候, 即使我们不去设置位置值, 它也会默认的传给0号位置. 但是倘若我们想绑定多个纹理, 这个时候就可以使用glUniform1i 了.

具体流程:
首先激活当前纹理单元(Texture Unit):
```
glActiveTexture(GL_TEXTURE0);               //激活0号位置单元
glBindTexture(GL_TEXTURE_2D, texture);
```
> openGL至少会提供16个Texutre unit, 0~15, 且, GL_TEXTURE8 = GL_TEXTURE0 + 8;

当然,如果要再绑定一个纹理, 我们的fragment shader 也得多添加一个 sampler了.
```
#version 330 core

in vec4 vertexColor;              
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D ourFace;


out vec4 FragColor;                                

void main(){                                       
	FragColor = texture(ourTexture, TexCoord) * texture(ourFace, TexCoord);
    FragColor = mix(texture(ourTexture, TexCoord), texture(ourFace, TexCoord), 0.2); //第三个值返回第二个的颜色所占比(0.2, 则是第二个占了20%, 剩下的都是第一个)
}
```

最后就是设置uniform了:
```
glActiveTexture(GL_TEXTURE0);               //先激活0 texture unit
glBindTexture(GL_TEXTURE_2D, texture1);
glActiveTexture(GL_TEXTURE3);               //接着激活3 texture unit
glBindTexture(GL_TEXTURE_2D, texture2);
```

之后再render loop中设置uniform
```
    glUniform1i(glGetUniformLocation(testShader->ID, "ourTexture"), 0);
    glUniform1i(glGetUniformLocation(testShader->ID, "ourFace"), 3);
```

当然了, 因为这个不会不停的改变, 也就是说只需要设置一次就行了, 所以再render loop外面设置也行, 不过别忘了, 在涉及到改变shader之前, 调用glUseProgram();

最后, 因为在开头说过了, texture是以左下角为原点, 因此直接传进来的图y轴将会是反的. 所以翻转下y轴就好了
```
stbi_set_flip_vertically_on_load(true);
```

FYI: uniform的设置里面的参数是我们的shader, 所以, 我们可在shader类中去创建一个调用uniform的方法
```
void setInit1(const char* path, int unit ) const
{
    glUniform(glGetUniformLocation(ID, path), unit);
}
```
