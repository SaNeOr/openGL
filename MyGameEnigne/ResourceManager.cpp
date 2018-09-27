#include "ResourceManager.h"

// Instantiate static variables
std::map<std::string, Shader>       ResourceManager::Shaders;
std::map<std::string, Texture2D>    ResourceManager::Textures;


ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}

Shader ResourceManager::LoadShader(	const char * vertexPath, 
									const char * fragmentPath,
									std::string name)
{

	Shaders[name] = loadShaderFromFile(vertexPath, fragmentPath);
	return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char * file, int alpha, std::string name)
{
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
	return Textures[name];
}

void ResourceManager::Clear()
{
}



Shader ResourceManager::loadShaderFromFile(const char * vertexPath, const char * fragmentPath)
{

	Shader shader(vertexPath, fragmentPath);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char * filePath, int alpha)
{

	Texture2D texture;
	texture.loadImageToGPU(filePath, alpha);
	return texture;
}

