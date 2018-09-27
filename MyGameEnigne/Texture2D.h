#pragma once

class Texture2D
{
public:

	unsigned int ID;		//handle
	int textureSlot;
	unsigned char* data;


	//int width, height, nrChannels;


	Texture2D();

	void loadImageToGPU(const char* filePath, int format);

	void Bind() const;
	~Texture2D();
};

