#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include<iostream>
#include <GL/glew.h>

Texture2D::Texture2D()
	:textureSlot(0)
{
	glGenTextures(1, &this->ID);
	glActiveTexture(GL_TEXTURE0 + textureSlot);

}




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

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, ID);
}

Texture2D::~Texture2D()
{
}
