#include "Match.hpp"


/**
* @brief Match constructor
*/
Match::Match()
{
	matchId = 0;

	teams.push_back(Team(0));
	teams.push_back(Team(1));

	// default settings
	teams[0].SetTeamColor(COLORS::Blue);
	teams[1].SetTeamColor(COLORS::White);
}

Match::~Match()
{
}


/**
* @brief Matching detected position to player class
*/
void Match::MatchPositionsToPlayers(int frameNmr)
{

	unsigned int bestMatchPosID = 0;
	unsigned int bestMatchPosValue = 1000;


	// loop teams
	for (int i = 0; i < 2; i++)
	{
		// loop found coordinates
		for (int j = 0; j < playerPositions[i].size(); j++)
		{
			double bestMatchX = 1000;
			double bestMatchY = 1000;
			int idx = 0;

			// inserts first player on startup
			if (teams[i].players.size() == 0 ) 
			{
				teams[i].CreatePlayer(playerPositions[i][j], frameNmr);
				continue;
			}
			
			// for every found coordinate loop previous frame coordinates
			for (int k = 0; k < teams[i].players.size(); k++)
			{
				/*if (teams[i].players[k].procesed || teams[i].players[k].positions.size() < k)
				{
					continue;
				}*/

				// find best match
				if (teams[i].players[k].positions.size() > 0)
				{
					float errx = abs(playerPositions[i][j].x - teams[i].players[k].positions[0].x);
					float erry = abs(playerPositions[i][j].y - teams[i].players[k].positions[0].y);

					//double err = sqrt(pow(errx, 2) + pow(erry, 2));
					if (errx < bestMatchX && erry < bestMatchY)
					{
						bestMatchX = errx;
						bestMatchY = erry;
						idx = k;
					}
				}
			}

			// if best match is higher than threshold and team is not full add a new member
			if (bestMatchX > bestMatchThreshold && 
				bestMatchY > bestMatchThreshold &&
				teams[i].players.size() < teams[i].GetPlayersSize()	 )
			{
				teams[i].CreatePlayer(playerPositions[i][j], frameNmr);
			}
			else
			{
				teams[i].players[idx].InsertFirst(playerPositions[i][j], frameNmr);
				teams[i].players[idx].procesed = true;
			}



		}
	}



	//// loop teams
	//for (int i = 0; i < 2; i++)
	//{
	//	// loop found coordinates
	//	for (int j = 0; j < playerPositions[2].size(); j++)
	//	{
	//		double bestMatchX = 1000;
	//		double bestMatchY = 1000;
	//		int idx = 0;

	//		// for every found coordinate loop previous frame coordinates
	//		for (int k = 0; k < teams[i].players.size(); k++)
	//		{
	//			if (teams[i].players[k].procesed && )
	//			{
	//				continue;
	//			}
	//			// TODO add if not to loop assigned positions

	//			// find best match
	//			float errx = abs(playerPositions[2][j].x - teams[i].players[k].positions[0].x);
	//			float erry = abs(playerPositions[2][j].y - teams[i].players[k].positions[0].y);

	//			//double err = sqrt(pow(errx, 2) + pow(erry, 2));
	//			if (errx < bestMatchX && erry < bestMatchY)
	//			{
	//				bestMatchX = errx;
	//				bestMatchY = erry;
	//				idx = k;
	//			}

	//		}
	//		//std::cout << "Best match X:" << bestMatchX << std::endl;
	//		//std::cout << "Best match Y:" << bestMatchY << std::endl;
	//		//std::cout << "Orig: " << playerPositions[i][idx].x << " " << playerPositions[i][idx].y << std::endl;
	//		//std::cout << "New : " << teams[i].players[idx].positions[0].x << " " << teams[i].players[idx].positions[0].y << std::endl;


	//		// if best match is higher than threshold and team is not full add a new member
	//		if (bestMatchX < bestMatchThreshold &&
	//			bestMatchY < bestMatchThreshold )
	//		{
	//			//teams[i].players[idx].InsertFirst(playerPositions[i][j],  frameNmr);
	//		}

	//	}
	//}

	
	// zober nove prvky a porovnaj s existujucimi
	// uspesne najdeny match = zhoda mensia ako xx tak pridaj suradnicu na index 0

	// neuspesny match 1. vektor hracov nieje zaplneny tak pridaj noveho
	// 2. vyber hraca s najnizsim rozdielom a daj na idx
	// po pridani suradnic nastav bool ze bol hrac spracovany - predpoklad najdeny vysledok je vzdy ok

	// spracuj pole neidentifikovanych obj. mozno hrac, brankari, rozhodcovia, mozno treneri a ine osoby

	// pridaj neobsadene polohy pomocou kalmanovho filtra


	
	// delete lost tracks
	if (frameNmr % 150 == 0)
	{
		SetAllPlayersForProcessing(frameNmr);
		// loop teams
		for (int i = 0; i < 2; i++)
		{
			std::vector<Player>::iterator it ;
			for (it = teams[i].players.begin(); it != teams[i].players.end();)
			{
				if (it->positions.size() == 0)
					it = teams[i].players.erase(it);
				else
					it++;
			}
		}
	}

}


void Match::SetAllPlayersForProcessing(int frameNmr)
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < teams[i].players.size(); j++)
		{
			teams[i].players[j].CheckPositionsValidity(frameNmr);
			//teams[i].players[j].procesed = false;
		}
	}
}