#include "Game.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"

//Game-related State data
SpriteRenderer	*Renderer;
GameObject		*Player;


Game::Game(GLuint width, GLuint height)
	:State(GAME_ACTIVE), Keys(), Width(width),Height(height)
{
}

Game::~Game()
{
	delete Renderer;
}

void Game::Init()
{
	#pragma region Load Shader
		ResourceManager::LoadShader("vertexSource.vert", "fragmentSource.frag", "sprite");
	#pragma endregion
	//	Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, Width * 1.0f, Height * 1.0f , 0.0f, -1.0f, 1.0f);


	ResourceManager::GetShader("sprite").use().setUniform1i("image",0);
	ResourceManager::GetShader("sprite").setUniform4fv("projection", projection);
	#pragma region Load Texture 
	ResourceManager::LoadTexture("background.jpg", GL_RGB, "background");
	//ResourceManager::LoadTexture("awesomeface.png", GL_RGBA, "face");
	ResourceManager::LoadTexture("block.png", GL_RGB, "block");
	ResourceManager::LoadTexture("block_solid.png", GL_RGB, "block_solid");
	ResourceManager::LoadTexture("paddle.png", GL_RGBA, "paddle");
	#pragma endregion

	//	Set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	#pragma region Load Level
		GameLevel one; one.Load("one.lvl", Width, Height * 0.5);
		Levels.push_back(one);

		Level = 0;
	#pragma endregion

	//Configure game objects
		glm::vec2 playerPos = glm::vec2(Width / 2 - PLAYER_SIZE.x / 2, Height - PLAYER_SIZE.y);
		Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));


}

void Game::ProcessInput(GLfloat dt)
{
	if (State == GAME_ACTIVE) {
		float velocity = PLAYER_VELOCITY * dt;
		//	Move playerboard
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
}

void Game::Update(GLfloat dt)
{
}

void Game::Render()
{
	if (State == GAME_ACTIVE) {
		Renderer->DrawSprite(	ResourceManager::GetTexture("background"),
								glm::vec2(0, 0), glm::vec2(Width, Height), 0.0f);

		Levels[Level].Draw(*Renderer);
		Player->Draw(*Renderer);
	}

}