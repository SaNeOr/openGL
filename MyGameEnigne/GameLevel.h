#pragma once

#include<vector>

#include<GL/glew.h>
#include<glm.hpp>

#include"GameObject.h"
#include"SpriteRenderer.h"
#include"ResourceManager.h"

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

