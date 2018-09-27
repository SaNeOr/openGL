#include "SpriteRenderer.h"







SpriteRenderer::SpriteRenderer(Shader shader)
{
	this->shader = shader;
	this->initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
	glDeleteVertexArrays(1, &quadVAO);
}

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

void SpriteRenderer::initRenderData()
{
	//	Configure VAO/VBO
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos     // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	//	VAO
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

		
	//	VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//	set attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

}
