#pragma once


#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>


#include "common.hpp"
#include "Team.hpp"



class Match
{
public:
	Match();
	~Match();
	void MatchPositionsToPlayers(int frameNmr);


	// structure of player positions, id 0 team1, id 1 team2, id 2 person object with unresolved identification
	std::vector<std::vector<glm::vec2>> playerPositions;

	bool firstInsertion = true;
	std::vector<Team> teams;
	std::string filePathName;


private:
	void SetAllPlayersForProcessing(int frameNmr);

	unsigned int matchId;
	int bestMatchThreshold = 10;


};

