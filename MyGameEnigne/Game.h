#pragma once


#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include <vector>

#include "GameLevel.h"

//	Represents the current state of the game
//	According to the current state decide to render or process different items
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

//Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(200, 40);
//Inital velocity of the player paddle
const float PLAYER_VELOCITY(1000.0f);


class Game
{
public:
	//	Game state
	GameState			State;
	GLboolean			Keys[1024];
	GLuint				Width, Height;
	//
	Game(GLuint width, GLuint height);
	~Game();

	//	Game Level
	std::vector<GameLevel>	Levels;
	int						Level;


	//	Initialize game state(load all shaders/textures/levels)
	void Init();

	//	GameLoop
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
};

