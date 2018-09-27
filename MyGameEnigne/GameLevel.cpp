#include "GameLevel.h"

#include <fstream>
#include <sstream>




void GameLevel::Load(const char * file, int levelWidth, int levelHeight)
{
	//	Clear old data
	Bricks.clear();

	//	Load from file
	std::vector<std::vector<int> > tileData;


	int tileCode;
	std::string line;
	std::ifstream fstream(file);


	if (fstream) {

		while (std::getline(fstream, line)) {

			std::istringstream sstream(line);
			std::vector<int> row;
			while (sstream >> tileCode) {
				row.push_back(tileCode);
			}
			tileData.push_back(row);
		}
	}

	if (tileData.size()) {
		init(tileData, levelWidth, levelHeight);
	}

}

void GameLevel::Draw(SpriteRenderer & renderer)
{
	for (GameObject &tile : Bricks) {
		if (!tile.Destrotyed)
			tile.Draw(renderer);
	}
}

bool GameLevel::IsGG()
{
	for (GameObject &tile : Bricks) {
		if (!tile.IsSolid && !tile.Destrotyed) {
			return false;
		}
	}
	return true;
}


GameLevel::~GameLevel()
{

}

void GameLevel::init(std::vector<std::vector<int>> tileData, int levelWidth, int levelHeight)
{
	//	Calculate dimensions
	int height = tileData.size();
	int width = tileData[0].size();
	float unit_width = levelWidth* 1.0f / width, unit_height = levelHeight / height ;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			//	Check block type from  level data	//1 solid,
			if (tileData[y][x] == 1) {
				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
				obj.IsSolid = true;
				Bricks.push_back(obj);

			}
			else if (tileData[y][x] > 1) {
				glm::vec3 color = glm::vec3(1.0f); // original: white
				if (tileData[y][x] == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tileData[y][x] == 3)
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tileData[y][x] == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tileData[y][x] == 5)
					color = glm::vec3(1.0f, 0.5f, 0.0f);

				
				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
			}

		}
	}
}
