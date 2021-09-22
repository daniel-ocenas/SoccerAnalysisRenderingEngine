#include "Team.hpp"

Team::Team(unsigned int ID)
{
	teamId = ID;
}

Team::~Team()
{
}

void Team::CreatePlayer( glm::vec2 playerPosition, int frameNmr)
{
	players.push_back(Player(playersCnt++, playerPosition, frameNmr));
}
