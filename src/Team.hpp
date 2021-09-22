#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include "Player.hpp"
#include "common.hpp"


class Team
{
public:
	Team(unsigned int ID);
	~Team();

	void SetTeamColor(unsigned int color) { jerseyColor = color; };
	unsigned int GetTeamColor() { return jerseyColor; };
	unsigned int GetPlayersSize() { return playersSize; };

	void CreatePlayer( glm::vec2 playerPosition, int frameNmr);
	std::vector<Player> players;


private:
	unsigned int teamId;
	// player size only of size 10, goalie has different jersey
	unsigned int playersSize = 10;
	unsigned int playersCnt = 0;



	unsigned int jerseyColor;


};
