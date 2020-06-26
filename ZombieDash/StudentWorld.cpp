#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Level.h"
#include <iostream>
#include "Actor.h"
#include <math.h>
#include <sstream>
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), end_Stage(false), m_Vaccine(0), m_Flame(0), m_LandMine(0)
	// initialize Penelope
{
}

int StudentWorld::init()
{

	int tempLevel = getLevel();
	std::ostringstream oss;
	if (tempLevel < 10)
		oss << "level0" << tempLevel << ".txt";
	else
		oss << "level" << tempLevel << ".txt";
	std::string levelFile = oss.str();

	Level lev(assetPath());
	Level::LoadResult result = lev.loadLevel(levelFile);
	if (result == Level::load_fail_file_not_found)
		cerr << "Cannot find level01.txt data file" << endl;
	else if (result == Level::load_fail_bad_format)
		cerr << "Your level was improperly formatted" << endl;
	else if (result == Level::load_success)
	{
		for (int i = 0; i < SPRITE_WIDTH; i++)			// for X-axis
			for (int j = 0; j < SPRITE_HEIGHT; j++)		// for Y-axis
			{
				Level::MazeEntry ge = lev.getContentsOf(i, j);
				switch (ge)
				{
					case Level::empty:
						break;
					case Level::smart_zombie:
						myActors.push_back(new SmartZombie(this, SPRITE_HEIGHT*i, SPRITE_HEIGHT* j));
						break;
					case Level::dumb_zombie:
						myActors.push_back(new DumbZombie(this, SPRITE_HEIGHT*i, SPRITE_HEIGHT* j));
						break;
					case Level::player:		
						myPlayer =  (new Penelope(this, SPRITE_HEIGHT*i, SPRITE_HEIGHT* j));
						break;	
					case Level::citizen:
						myActors.push_back(new Citizen(this, SPRITE_HEIGHT*i, SPRITE_HEIGHT* j));
						break;
					case Level::exit:
						myActors.push_back(new Exit(this, SPRITE_HEIGHT*i, SPRITE_HEIGHT* j));
						break;
					case Level::wall:
						myActors.push_back(new Wall(this, SPRITE_HEIGHT*i, SPRITE_HEIGHT* j));
						break;		
					case Level::pit:
						myActors.push_back(new Pit(this, SPRITE_HEIGHT*i, SPRITE_HEIGHT* j));
						break;
					case Level::vaccine_goodie:
						myActors.push_back(new Vaccine_Goodies(this, SPRITE_HEIGHT*i, SPRITE_HEIGHT* j));
						break;
					case Level::gas_can_goodie:
						myActors.push_back(new Gas_can_Goodies(this, SPRITE_HEIGHT*i, SPRITE_HEIGHT* j));
						break;
					case Level::landmine_goodie:
						myActors.push_back(new Landmine_Goodies(this, SPRITE_HEIGHT*i, SPRITE_HEIGHT* j));
						break;
					default:
						break;
				}
			}
	}
	myPlayer->setNonInfection();
	allItemsSetZero();
    return GWSTATUS_CONTINUE_GAME;
}


 int StudentWorld::move()
{
	setGameStatText(Info(getScore()));

	vector<Actor*>::iterator ActorsItr = myActors.begin();
	while (ActorsItr != myActors.end())
	{			// let all objects in Vector to do 'doSomething()'
		
			(*ActorsItr)->doSomething();

			if (end_Stage == true)	// if there is not citizen
			{
				end_Stage = false;	// ready for the next level
				playSound(SOUND_LEVEL_FINISHED);
				return GWSTATUS_FINISHED_LEVEL;
			}
			++ActorsItr;
	}
	myPlayer->doSomething();
	if (!myPlayer->isAlive())
	{
		return GWSTATUS_PLAYER_DIED;
	}

	ActorsItr = myActors.begin();
	while (ActorsItr != myActors.end())
	{
		if (!(*ActorsItr)->isAlive())		//if an object is dead
		{
			delete (*ActorsItr);
			ActorsItr = myActors.erase(ActorsItr);	// remove dangling pointer
		}
		else
		{
			++ActorsItr;
		}
	}


	// This code is here merely to allow the game to build, run, and terminate after you hit enter.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	//decLives();
	return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::cleanUp()
{
	if(myPlayer != nullptr)
	{
		delete myPlayer;
		myPlayer = nullptr;
	}
	
	for (auto it = myActors.begin(); it != myActors.end(); ++it)
	{
		delete (*it);
	}
	myActors.clear();
}


// In each level.txt file, it shows minized map version as text version.
//			Each object are one character in the text file.
// And based on that information, each object in game are disposed and each of them * 16.
// also, it allows checking out the distance for overlap object (with isPassible checker)
// Also, I also used absoulute value because the value can be negative.
bool StudentWorld::isBlocking(double x, double y, Actor* myself)
{
	vector<Actor*>::iterator myVlist;
	for (myVlist = myActors.begin(); myVlist != myActors.end(); ++myVlist)
	{
		if ((*myVlist) != myself && !(*myVlist)->isPassable())		// for wall
			//if (abs((*myVlist)->getX() - x) + abs((*myVlist)->getY() - y) < 15)	
			//if(Euclidean_distance(((*myVlist)->getX() - x), ((*myVlist)->getY() - y)) < 16)
			if(abs((*myVlist)->getX() - x) < 16 && abs((*myVlist)->getY() - y) < 16)
				return true;
	}
	return false;
}

bool StudentWorld::GapOfAgentandPenelope(double x, double y)
{	
	if((abs(myPlayer->getX() - x) < 16 && abs(myPlayer->getY() - y) < 16))
		return true;
	return false;
}
 
// The iterator only passes myActors. Only Citizens can be passable but not other.
// Penelope is not in the vector so that do not need to concern of it.

bool StudentWorld::goExit(double x, double y)
{
	vector<Actor*>::iterator myVlist;
	int count = 0;
	for (myVlist = myActors.begin(); myVlist != myActors.end(); ++myVlist)
	{
		if ((*myVlist)->passable_of_Exit())			// i set it only for citizen
		{
			count++;
			double temp = Euclidean_distance(((*myVlist)->getX() - x), ((*myVlist)->getY() - y));
			if (temp <= 10)
			{
				increaseScore(500);   // Inform the StudentWorld object that the user is to receive 500 points.
				(*myVlist)->setDie();
				playSound(SOUND_CITIZEN_SAVED);
			}
		}
	}
	if (count == 0)	// if there is no more citizen, then Penelope can go in
	{
		double temp_player = Euclidean_distance((myPlayer->getX() - x), (myPlayer->getY() - y));
		if (temp_player <= 10)
		{
			end_Stage = true;
			return true;
		}
		else
			end_Stage = false;

	}
	return false;

}
// check the distance between goodie and a player. IF a player acquires the goodie, return true.
bool StudentWorld::getGoodies(double x, double y)
{
			double temp = Euclidean_distance((myPlayer->getX() - x), (myPlayer->getY() - y));
			if (temp <= 10)
				return true;
			return false;
}


double StudentWorld::Euclidean_distance(double x, double y)
{
	double temp = x * x + y * y;
	temp = sqrt(temp);
	return temp;
}


// this is inserting score and number of  items
string StudentWorld::Info(int score)		
{
	string str;
	std::ostringstream oss;
	if (score < 0)
	{
		int value = abs(score);
		oss << "-" << setfill('0') << setw(5) << to_string(value);
		str = oss.str();
	}
	else
	{
		oss << setfill('0') << setw(6) << to_string(score);
		str = oss.str();
	}
	oss << "Score: " << str << "  Level: " << getLevel() << "  Lives: " << getLives() << "  Vaccines: " << getNumVaccines() << "  Flames: " <<getNumFlameCharges() << "  Mines: " << getNumLandmines() << "  Infected: " << myPlayer->checkInfectioncount();
	string value = "Score: " + str + "  Level: " + to_string(getLevel()) + "  Lives: " + to_string(getLives()) + "  Vaccines: " + to_string(getNumVaccines()) +  "  Flames: " + to_string(getNumFlameCharges()) + "  Mines: " + to_string(getNumLandmines()) + "  Infected: " + to_string(myPlayer->checkInfectioncount());

	return (value);
}


// When shooting fire and its direction
void StudentWorld::shootFire(double x, double y, Direction dir)
{
	if (dir == 90) // up						something weird need to check
		for (int i = 1; i <= 3; i++)
		{
			if (wall_or_exit(x, y + i * SPRITE_HEIGHT))
				return;
			myActors.push_back(new Flame(this, x, y + i * SPRITE_HEIGHT, dir));
		}
	if (dir == 270) // down
		for (int i = 1; i <= 3; i++)
		{
			if (wall_or_exit(x, y - i * SPRITE_HEIGHT))
				return;
			myActors.push_back(new Flame(this, x, y - i * SPRITE_HEIGHT, dir));
		}
	if (dir == 0) // right
		for (int i = 1; i <= 3; i++)
		{
			if (wall_or_exit(x + i * SPRITE_WIDTH, y))
				return;
			myActors.push_back(new Flame(this, x + i * SPRITE_WIDTH, y, dir));
		}
	if(dir == 180) // left
		for (int i = 1; i <= 3; i++)
		{
			if (wall_or_exit(x - i * SPRITE_WIDTH, y))
				return;
			myActors.push_back(new Flame(this, x - i * SPRITE_WIDTH, y, dir));
		}
}

// there is no deleting, but it is only for showing fire objects
// return true, when passing through fire. if not false (as an example, wall, exit)
bool StudentWorld::wall_or_exit(double x, double y)
{
	vector<Actor*>::iterator myVlist;
	for (myVlist = myActors.begin(); myVlist != myActors.end(); ++myVlist)
	{
		// if it wall or exit   and close to the object
		if(!(*myVlist)->flameOverlapable() && (Euclidean_distance((*myVlist)->getX() - x, (*myVlist)->getY() - y)) <= 10)
			return true;
	}
	return false;
}



// flame is setting die for human, zombie, penelope,
// and activating Landmine
void StudentWorld::FlameObjects(double x, double y)
{
	// distance with Penelope
	if ((Euclidean_distance(myPlayer->getX() - x, myPlayer->getY() - y)) <= 10)
	{
		decLives();
		playSound(SOUND_PLAYER_DIE);
		myPlayer->setDie();
	}
		


	vector<Actor*>::iterator myVlist;
	for (myVlist = myActors.begin(); myVlist != myActors.end(); ++myVlist)
	{
		if ((*myVlist)->flameable())// all flameable except wall, pit, exit, and fire itself
		{
			if ((Euclidean_distance((*myVlist)->getX() - x, (*myVlist)->getY() - y)) <= 10)
				if ((*myVlist)->isAlive())
				{
					if ((*myVlist)->isInfectable())
					{
						playSound(SOUND_CITIZEN_DIE);
						increaseScore(-1000);
					}
					if ((*myVlist)->Vomitable())
					{
						playSound(SOUND_ZOMBIE_DIE);
						increaseScore(1000);
					}
					(*myVlist)->setDie();
					return;
				}
		}
	}
}

void StudentWorld::installLandmind(double x, double y)
{
	myActors.push_back(new Landmine(this, x , y));
}


// for landmine and pit
bool StudentWorld::overlap_Landmine(double x, double y) // whether fire can be overlapable or not
{
	// if penelope in the fire, set die
	if (Euclidean_distance(myPlayer->getX() - x, (myPlayer->getY() - y)) <= 10)
	{
		myPlayer->setDie();
		decLives();
		playSound(SOUND_PLAYER_DIE);
		return true;
	}
	vector<Actor*>::iterator myVlist;
	for (myVlist = myActors.begin(); myVlist != myActors.end(); ++myVlist)
	{

		if ((*myVlist)->FallInPit())
		{
			if ((Euclidean_distance((*myVlist)->getX() - x, (*myVlist)->getY() - y)) <= 10)
			{
				if ((*myVlist)->isInfectable())
				{
					increaseScore(-1000);
					playSound(SOUND_CITIZEN_DIE);
				}
				if ((*myVlist)->Vomitable())
				{
					increaseScore(1000);
					playSound(SOUND_ZOMBIE_DIE);
				}
				(*myVlist)->setDie();
				return true;
			}
		}

		if ((*myVlist)->StepOnLandmine()) // true only for human, zombie ,penelope, and flame
			if ((Euclidean_distance((*myVlist)->getX() - x, (*myVlist)->getY() - y)) <= 10)
			{
				//(*myVlist)->setDie();
				return true;
			}
	}
	return false;
}


// fire from (9)the landmine 
void StudentWorld::flameOfLandmine(double x, double y)
{
	//   (x-1,y+1,d)   (x,y+1,l)  (x+1,y+1,l)
	//   (x-1, y ,d)   (x,y,r)    (x+1,y,u)
	//	 (x-1,y-1,r)   (x,y-1,r)  (x+1,y-1,u)
	myActors.push_back(new Pit(this, x , y ));
	int dir = 90;
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if(overlap_Landmine(x + i, y + j))
				if(!wall_or_exit(x + i * SPRITE_WIDTH, y + j * SPRITE_HEIGHT))
					myActors.push_back(new Flame(this, x + i* SPRITE_WIDTH, y + j*SPRITE_HEIGHT, dir));
		}
	}
	
}

void StudentWorld::makingPit(double x, double y)
{
	myActors.push_back(new Pit(this, x, y));

}

void StudentWorld::SmartZ_OR_DumbZ(double x, double y)
{
	int value = randInt(1, 100);
	if(value <= 30)   //1~30 smart
		myActors.push_back(new SmartZombie(this, x,y));
	else               // 31~100 Dumb
		myActors.push_back(new DumbZombie(this, x,y));
	return;
}


double StudentWorld::dis_p(double x, double y)
{
	double xx = Euclidean_distance(myPlayer->getX() - x, myPlayer->getY() - y);
	return xx;
}

double StudentWorld::dis_z(double x, double y)
{
	double Small = -1;
	double New;
	int count = 0;
	for (auto itr = (myActors).begin(); itr != (myActors).end(); ++itr)
	{
		if ((*itr)->Vomitable())
		{
			New = Euclidean_distance((*itr)->getX() - x, (*itr)->getY() - y);
			if (count == 0 || Small > New) // 1 6 3
			{
				Small = New;
			}
			count++;
		}
		
	}
	return Small;
}



bool StudentWorld::encountableInfectable(double x, double y, Direction& dir)
{
	for (auto itr = (myActors).begin(); itr != (myActors).end(); ++itr)
	{
		if ((*itr)->isInfectable())
			if (Euclidean_distance((*itr)->getX() - x, (*itr)->getY() - y) == 16)
			{
				if (((*itr)->getY() - y) == 16)
				{
					dir = 90; // zombie should see up
					return true;
				}
				else if (((*itr)->getY() - y) == -16)	//when zombie position is hier than human
				{
					dir = 270; // see down
					return true;
				}
				else if (((*itr)->getX() - x) == 16)
				{
					dir = 0; // see right
					return true;
				}
				else if (((*itr)->getX() - x) == -16)
				{
					dir = 180;
					return true;
				}
			}
	}
	if (Euclidean_distance(myPlayer->getX() - x, myPlayer->getY() - y) == 16)
	{
		if ((myPlayer->getY() - y) == 16)
		{
			dir = 90; // zombie should see up
			return true;
		}
		else if ((myPlayer->getY() - y) == -16)	//when zombie position is hier than human
		{
			dir = 270; // see down
			return true;
		}
		else if ((myPlayer->getX() - x) == 16)
		{
			dir = 0; // see right
			return true;
		}
		else if ((myPlayer->getX() - x) == -16)
		{
			dir = 180;
			return true;
		}
	}
	return false;
}




/* 
When x is moving and coming to close to 'o'


   1 2 3 4 5
1  X
2
3
4    O

gapX < gapY   in this case go right

   1 2 3 4 5
1  X
2        O
3
4    
gapX > gapY   in this case go down
*/


void StudentWorld::apartingFromZombie(double x, double y, Actor* citizen)
{
	for (auto itr = (myActors).begin(); itr != (myActors).end(); ++itr)
	{
		double temp = Euclidean_distance((*itr)->getX() - x, (*itr)->getY() - y);
		if ((*itr)->Vomitable() && temp <= 80 )
		{
			double gapX = x - (*itr)->getX();
			double gapY = y - (*itr)->getY();


			if (gapX == 0 && gapY == 0)
				return;
			else if (gapX == 0)
			{
				if (gapY < 0)
				{
					if (!isBlocking(citizen->getX(), citizen->getY() - 2, citizen))
					{
						citizen->setDirection(270);
						citizen->moveTo(citizen->getX(), citizen->getY() - 2);
						return;
					}
				}
				else
				{
					if (!isBlocking(citizen->getX(), citizen->getY() + 2, citizen))
					{
						citizen->setDirection(90);
						citizen->moveTo(citizen->getX(), citizen->getY() + 2);
						return;
					}
				}
			}
			else if (gapY == 0)
			{
				if (gapX < 0)
				{
					if (!isBlocking(citizen->getX() - 2, citizen->getY(), citizen))
					{
						citizen->setDirection(180);
						citizen->moveTo(citizen->getX() - 2, citizen->getY());
						return;
					}
				}
				else
				{
					if (!isBlocking(citizen->getX() + 2, citizen->getY(), citizen))
					{
						citizen->setDirection(0);
						citizen->moveTo(citizen->getX() + 2, citizen->getY());
						return;
					}
				}
			}
			else if (abs(gapX) > abs(gapY))
			{
				if (gapX < 0)
				{
					if (!isBlocking(citizen->getX() - 2, citizen->getY(), citizen))
					{
						citizen->setDirection(180);
						citizen->moveTo(citizen->getX() - 2, citizen->getY());
						return;
					}
				}
				else
				{
					if (!isBlocking(citizen->getX() + 2, citizen->getY(), citizen))
					{
						citizen->setDirection(0);
						citizen->moveTo(citizen->getX() + 2, citizen->getY());
						return;
					}
				}
				if (gapY < 0)
				{
					if (!isBlocking(citizen->getX(), citizen->getY() - 2, citizen))
					{
						citizen->setDirection(270);
						citizen->moveTo(citizen->getX(), citizen->getY() - 2);
						return;
					}
				}
				else
				{
					if (!isBlocking(citizen->getX(), citizen->getY() + 2, citizen))
					{
						citizen->setDirection(90);
						citizen->moveTo(citizen->getX(), citizen->getY() + 2);
						return;
					}
				}
			}
			else
			{
				if (gapY < 0)
				{
					if (!isBlocking(citizen->getX(), citizen->getY() - 2, citizen))
					{
						citizen->setDirection(270);
						citizen->moveTo(citizen->getX(), citizen->getY() - 2);
						return;
					}
				}
				else
				{
					if (!isBlocking(citizen->getX(), citizen->getY() + 2, citizen))
					{
						citizen->setDirection(90);
						citizen->moveTo(citizen->getX(), citizen->getY() + 2);
						return;
					}
				}
				if (gapX < 0)
				{
					if (!isBlocking(citizen->getX() - 2, citizen->getY(), citizen))
					{
						citizen->setDirection(180);
						citizen->moveTo(citizen->getX() - 2, citizen->getY());
						return;
					}
				}
				else
				{
					if (!isBlocking(citizen->getX() + 2, citizen->getY(), citizen))
					{
						citizen->setDirection(0);
						citizen->moveTo(citizen->getX() + 2, citizen->getY());
						return;
					}
				}
				
			}


		}

	}
	return;
}


void StudentWorld::comingToPenelope(double x, double y, Actor* citizen)
{
		double temp = Euclidean_distance(x-(myPlayer)->getX(), y-(myPlayer)->getY());

		if ( temp <= 80)
		{
			double gapX = citizen->getX() - (myPlayer)->getX();
			double gapY = citizen->getY() - (myPlayer)->getY();
			cout << gapX << " : " << gapY << endl;
			if (gapX == 0 && gapY == 0)	
				return;
			else if (gapX == 0)
			{
				if (gapY < 0)
				{
					if (!isBlocking(citizen->getX(), citizen->getY() + 2, citizen))
					{
						if (!GapOfAgentandPenelope(citizen->getX(), citizen->getY() + 2)) // dis 15
						{
							citizen->setDirection(90);
							citizen->moveTo(citizen->getX(), citizen->getY() + 2);
							return;
						}
					}
				}
				else
				{
					if (!isBlocking(citizen->getX(), citizen->getY() - 2, citizen))
					{
						if (!GapOfAgentandPenelope(citizen->getX(), citizen->getY() - 2))
						{
							citizen->setDirection(270);
							citizen->moveTo(citizen->getX(), citizen->getY() - 2);
							return;
						}
					}
				}
			}
			else if (gapY == 0)
			{
				cout << "7" << endl;
				if (gapX < 0)
				{
					if (!isBlocking(citizen->getX() + 2, citizen->getY(), citizen))
					{
						if (!GapOfAgentandPenelope(citizen->getX() + 2, citizen->getY()))
						{
							citizen->setDirection(0);
							citizen->moveTo(citizen->getX() + 2, citizen->getY());
							return;
						}
					}
				}
				else
				{
					if (!isBlocking(citizen->getX() - 2, citizen->getY(), citizen))
					{
						if (!GapOfAgentandPenelope(citizen->getX() - 2, citizen->getY()))
						{
							citizen->setDirection(180);
							citizen->moveTo(citizen->getX() - 2, citizen->getY());
							return;
						}
					}
				}
			}
			else if (abs(gapX) > abs(gapY))
			{
				if (gapX < 0 && gapY > 0)
				{
					if (!isBlocking(citizen->getX(), citizen->getY() - 2, citizen))
						if (!GapOfAgentandPenelope(x, y - 2))
						{
							citizen->setDirection(270);
							citizen->moveTo(citizen->getX(), citizen->getY() - 2);
							return;
						}
				}
				else if (gapX > 0 && gapY > 0)
				{
					if (!isBlocking(citizen->getX(), citizen->getY() - 2, citizen))
						if (!GapOfAgentandPenelope(x, y - 2))
						{
							citizen->setDirection(270);
							citizen->moveTo(citizen->getX(), citizen->getY() - 2);
							return;
						}
				}
				else if (gapX > 0 && gapY < 0)
				{
					if (!isBlocking(citizen->getX(), citizen->getY() + 2, citizen))
						if (!GapOfAgentandPenelope(x, y + 2))
						{
							citizen->setDirection(90);
							citizen->moveTo(citizen->getX(), citizen->getY() + 2);
							return;
						}
				}
				else if (gapX > 0 && gapY == 0)
				{
					if (!isBlocking(citizen->getX() - 2, citizen->getY(), citizen))
						if (!GapOfAgentandPenelope(x - 2, y))
						{
							citizen->setDirection(180);
							citizen->moveTo(citizen->getX() - 2, citizen->getY());
							return;
						}
				}
				else if (gapX < 0 && gapY == 0)
				{
					if (!isBlocking(citizen->getX() + 2, citizen->getY(), citizen))
						if (!GapOfAgentandPenelope(x + 2, y))
						{
							citizen->setDirection(0);
							citizen->moveTo(citizen->getX() + 2, citizen->getY());
							return;
						}
				}
				else
				{
					if (!isBlocking(citizen->getX(), citizen->getY() - 2, citizen))
						if (!GapOfAgentandPenelope(x, y - 2))
						{
							citizen->setDirection(270);
							citizen->moveTo(citizen->getX(), citizen->getY() - 2);
							return;
						}
				}
				return;
			}
			else
			{
				if (gapX < 0 && gapY > 0)
				{
					if (!isBlocking(x+2, y, citizen))
						if (!GapOfAgentandPenelope(x+2, y))
						{
							citizen->setDirection(0);
							citizen->moveTo(x+2, y);
							return;
						}
					return;
				}
				else if (gapX > 0 && gapY > 0)
				{
					if (!isBlocking(x-2, y, citizen))
						if (!GapOfAgentandPenelope(x-2, y))
						{
							citizen->setDirection(180);
							citizen->moveTo(x-2, y);
							return;
						}
					return;
				}
				else if (gapX > 0 && gapY < 0)
				{
					if (!isBlocking(x - 2, y, citizen))
						if (!GapOfAgentandPenelope(x - 2, y))
						{
							citizen->setDirection(180);
							citizen->moveTo(x - 2, citizen->getY());
							return;
						}
				}
				else
				{
					if (!isBlocking(citizen->getX() + 2, citizen->getY(), citizen))
					{
						if (!GapOfAgentandPenelope(citizen->getX() + 2, citizen->getY()))
						{
							citizen->setDirection(0);
							citizen->moveTo(citizen->getX() + 2, citizen->getY());
							return;
						}
					}
					return;
				}

			}

			return;
		}

		return;
}


// this is vomiting fomr zombie
void StudentWorld::doSt_InVomit(double x, double y)
{
	for (auto itr = (myActors).begin(); itr != (myActors).end(); ++itr)
	{
		if ((*itr)->isInfectable() && (*itr)->isAlive() && !(*itr)->isInfected() )
		{
			if (Euclidean_distance((*itr)->getX() - x, (*itr)->getY() - y) <= 10)
			{
				(*itr)->setInfected();
				return;
			}
		}
	}
	if (myPlayer->isAlive() && !myPlayer->isInfected())
	{
		if (Euclidean_distance(myPlayer->getX() - x, myPlayer->getY() - y) <= 10)
		{
			myPlayer->setInfected();
			return;
		}
	}
	return;
}


void StudentWorld::ShootFrontWithVomit(double x, double y, Direction dir)
{


		if (myPlayer->getY() == y &&
			Euclidean_distance(myPlayer->getX() - x, myPlayer->getY() - y) <= SPRITE_WIDTH)
		{
			if (dir == 0 && myPlayer->getX() - x >= 0) //right
			{

				playSound(SOUND_ZOMBIE_VOMIT);
				myActors.push_back(new Vomit(this, x + SPRITE_WIDTH, y, dir));
				return;
			}
			else if (dir == 180 && myPlayer->getX() - x < 0)  // left
			{

				playSound(SOUND_ZOMBIE_VOMIT);
				myActors.push_back(new Vomit(this, x - SPRITE_WIDTH, y, dir));
				return;
			}
		}

		else if (myPlayer->getX() == x &&
				Euclidean_distance(myPlayer->getY() - y, myPlayer->getX() - x) <= SPRITE_HEIGHT)
		{
			if (dir == 90 && myPlayer->getY() - y >= 0) // above
			{

				playSound(SOUND_ZOMBIE_VOMIT);
				myActors.push_back(new Vomit(this, x , y + SPRITE_HEIGHT, dir));
				return;
			}
			else if (dir == 270 && myPlayer->getY() - y < 0) //down
			{

				playSound(SOUND_ZOMBIE_VOMIT);
				myActors.push_back(new Vomit(this, x, y- SPRITE_HEIGHT, dir));
				return;
			}
		}

	for (auto itr = (myActors).begin(); itr != (myActors).end(); ++itr)
	{
		if ((*itr)->isInfectable() && (*itr)->isAlive())
		{
			if ((*itr)->getY() == y &&
				Euclidean_distance((*itr)->getX() - x, (*itr)->getY() - y) <= SPRITE_WIDTH)
			{
				if (dir == 0 && (*itr)->getX() - x >= 0) //right
				{

					playSound(SOUND_ZOMBIE_VOMIT);
					myActors.push_back(new Vomit(this, x + SPRITE_WIDTH, y, dir));
					return;
				}
				else if (dir == 180 && (*itr)->getX() - x < 0)  // left
				{

					playSound(SOUND_ZOMBIE_VOMIT);
					myActors.push_back(new Vomit(this, x - SPRITE_WIDTH, y, dir));
					return;
				}
			}
			else if ((*itr)->getX() == x &&
				Euclidean_distance((*itr)->getY() - y, (*itr)->getX() - x) <= SPRITE_HEIGHT){
				if (dir == 90 && (*itr)->getY() - y >= 0) // above
				{
					playSound(SOUND_ZOMBIE_VOMIT);
					myActors.push_back(new Vomit(this, x, y + SPRITE_HEIGHT, dir));
					return;
				}
				else if (dir == 270 && (*itr)->getY() - y < 0) //down
				{

					playSound(SOUND_ZOMBIE_VOMIT);
					myActors.push_back(new Vomit(this, x, y - SPRITE_HEIGHT, dir));
					return;
				}
			}


		}
		
	}
	return;
}






// find the human object which is the most close to zombie
double StudentWorld::Targeting_Human_by_Zombie(double x, double y)
{
	double Small = Euclidean_distance((myPlayer)->getX() - x, (myPlayer)->getY() - y);
	double New;
	for (auto itr = (myActors).begin(); itr != (myActors).end(); ++itr)
	{
		if ((*itr)->isInfectable() && (*itr)->isAlive())
		{
			New = Euclidean_distance((*itr)->getX() - x, (*itr)->getY() - y);
			if (Small > New) // 1 6 3
			{
				Small = New;
			}
		}

	}
	return Small;
}





void StudentWorld::comingToHumanByS_Zombie(double x, double y, Actor* sZom)
{
	Actor* temp  = myPlayer;
	double Small = Euclidean_distance((myPlayer)->getX() - x, (myPlayer)->getY() - y);
	double New;
	for (auto itr = (myActors).begin(); itr != (myActors).end(); ++itr)
	{
		if ((*itr)->isInfectable() && (*itr)->isAlive())
		{
			New = Euclidean_distance((*itr)->getX() - x, (*itr)->getY() - y);
			if (Small > New) // 1 6 3
			{
				Small = New;
				temp = (*itr);
			}
		}

	}
	if (Euclidean_distance(temp->getX() - x, temp->getY() - y)
		>= Euclidean_distance(myPlayer->getX() - x, myPlayer->getY() - y))
	{
		temp = myPlayer;
	}
	

			if(Euclidean_distance(x - (temp)->getX(), y - temp->getY()) <= 80)
			{ 
				double gapX = sZom->getX() - (temp)->getX();
				double gapY = sZom->getY() - (temp)->getY();
				//cout << gapX << " : " << gapY << endl;
				if (gapX == 0 && gapY == 0)
					return;
				else if (gapX == 0)
				{
					if (gapY < 0) 
					{
						if (!isBlocking(x, y + 1, sZom))
							if (!GapOfAgentandPenelope(x, y + 1)) // dis 15
							{
								sZom->setDirection(90);
								sZom->moveTo(x, y + 1);
								return;
							}
					}
					else {
						if (!isBlocking(x, y - 1, sZom))

							if (!GapOfAgentandPenelope(x, y - 1))
							{
								sZom->setDirection(270);
								sZom->moveTo(x, y - 1);
								return;
							}
					}
				}
				else if (gapY == 0)
				{
					if (gapX < 0)
					{
						if (!isBlocking(x + 1, y, sZom))
						{
							if (!GapOfAgentandPenelope(x + 1,y))
							{
								sZom->setDirection(0);
								sZom->moveTo(x + 1, y);
								return;
							}
						}
					}
					else
					{
						if (!isBlocking(x - 1, y, sZom))
						{
							if (!GapOfAgentandPenelope(x - 1, y))
							{
								sZom->setDirection(180);
								sZom->moveTo(x - 1, y);
								return;
							}
						}
					}
				}
				else if (abs(gapX) > abs(gapY))
				{
						if (gapX < 0)
						{
							if (!isBlocking(x + 1, y, sZom))
							{
								if (!GapOfAgentandPenelope(x + 1, y))
								{
									sZom->setDirection(0);
									sZom->moveTo(x + 1, y);
									return;
								}
							}
						}
						else
						{
							if (!isBlocking(x - 1, y, sZom))
								if (!GapOfAgentandPenelope(x - 1, y))
								{
									sZom->setDirection(180);
									sZom->moveTo(x - 1, y);
									return;
								}
						}
						if (gapY < 0)
						{
							if (!isBlocking(x, y + 1, sZom))
							{
								if (!GapOfAgentandPenelope(x, y + 1))
								{
									sZom->setDirection(90);
									sZom->moveTo(x, y + 1);
									return;
								}
							}
						}
						else
						{
							if (!isBlocking(x, y - 1, sZom))
								if (!GapOfAgentandPenelope(x, y - 1))
								{
									sZom->setDirection(270);
									sZom->moveTo(x, y - 1);
									return;
								}
						}
				}
				else
				{
						if (gapY < 0)
						{
							if (!isBlocking(x, y + 1, sZom))
							{
								if (!GapOfAgentandPenelope(x, y + 1))
								{
									sZom->setDirection(90);
									sZom->moveTo(sZom->getX(), sZom->getY() + 1);
									return;
								}
							}
						}
						else
						{
							if (!isBlocking(sZom->getX(), sZom->getY() - 1, sZom))
							{
								if (!GapOfAgentandPenelope(x, y - 1))
								{
									sZom->setDirection(270);
									sZom->moveTo(sZom->getX(), sZom->getY() - 1);
									return;
								}
							}
						}

						if (gapX < 0)
						{
							if (!isBlocking(sZom->getX() + 1, sZom->getY(), sZom))
								if (!GapOfAgentandPenelope(sZom->getX() + 1, sZom->getY()))
								{
									sZom->setDirection(0);
									sZom->moveTo(sZom->getX() + 1, sZom->getY());
									return;
								}
						}

						else
						{
							if (!isBlocking(sZom->getX() - 1, sZom->getY(), sZom))
								if (!GapOfAgentandPenelope(sZom->getX() - 1, sZom->getY()))
								{
									sZom->setDirection(180);
									sZom->moveTo(sZom->getX() - 1, sZom->getY());
									return;
								}
						}
				}
			}
			

	return;
	





}
