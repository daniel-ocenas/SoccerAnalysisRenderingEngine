#include "Player.hpp"



Player::Player(int id, glm::vec2 pos, int frameNmr)
{
	positions.push_back(Position{ pos.x, pos.y,frameNmr });
	playerId = id;
}

Player::~Player()
{
}



void Player::InsertFirst(glm::vec2 pos, int frameNmr)
{
	positions.insert(positions.begin(), Position{ pos.x, pos.y,frameNmr });
	if (positions.size() > positionsMaxSize)
	{
		positions.pop_back();
	}
}


void Player::CheckPositionsValidity(int frameNmr)
{
	if (positions.size() > 0 && positions[positions.size() - 1].frame < frameNmr - maxAllowedTime)
	{
		positions.pop_back();
	}
}
