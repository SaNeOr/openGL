#pragma once

#include <GL/glew.h>
#include <glm.hpp>

#include "Texture2D.h"
#include"SpriteRenderer.h"

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

