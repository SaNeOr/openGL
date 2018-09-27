#pragma once

#include <map>
#include <string>

#include<GL/glew.h>

#include "Texture2D.h"
#include "Shader.h"

class ResourceManager
{
public:
	//Resource storage
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Texture2D>	Textures;
	

	//
	static Shader		LoadShader(	const char * vertexPath, 
									const char * fragmentPath,
									std::string name);

	static Shader		GetShader(std::string name);


	static Texture2D	LoadTexture(const char* file,
									int alpha,
									std::string name);

	static Texture2D	GetTexture(std::string name);

	static void Clear();



private:
	ResourceManager();
	~ResourceManager();

	static Shader	loadShaderFromFile(	const char* vertexPath, 
										const char* fragmentPath);
	
	
	static Texture2D	loadTextureFromFile(const char *filePath, 
											int alpha);

};

