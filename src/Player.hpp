#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "common.hpp"



class Player
{

struct Position {
	float x;
	float y;
	int frame;
};

public:
	Player(int id,  glm::vec2 pos, int frameNmr);
	~Player();



	void InsertFirst(glm::vec2 pos, int frameNmr);
	void CheckPositionsValidity(int frameNmr);

	std::vector<Position> positions;
	bool procesed = false;

private:
	int playerId = 0;
	int playerNumber = 0;

	int positionsMaxSize = 5;
	int maxAllowedTime = 30;

	bool selected = false;


};

