#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "GameWorld.h"
#include <string>
#include "Actor.h"
#include <vector>
using namespace std;

// The vector is used in the code

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
	~StudentWorld() { cleanUp(); }
	// When going out the program, it would need clearing up
    virtual int init();
	virtual int move();
	
    virtual void cleanUp();
	// This function is used in Actor and 
	// checking out whether some actor blocked or not.
	// Actor* in parameter is not really necessary in part1 but
	// I believe that it must be needed in section2
	bool isBlocking(double x, double y, Actor* myself);
	bool goExit(double x, double y);
	bool getGoodies(double x, double y); // checking out if a player get goodie or not (distance)

	double Euclidean_distance(double x, double y);
	string Info(int score);	// data about penelope information


	// maybe it would be wrong
	int getNumVaccines() const { return m_Vaccine; }
	void increaseVaccines() { m_Vaccine += 1; }
	void usingVaccine() { m_Vaccine--; }
	void allItemsSetZero() { m_Vaccine = 0;  m_Flame = 0; m_LandMine = 0; }
	int getNumFlameCharges() const { return m_Flame; }
	void increaseFlameCharges() { m_Flame += 5; }
	void usingFlame() { m_Flame--; }
	int getNumLandmines() const { return m_LandMine; }
	void increaseLandmines() { m_LandMine += 2; }
	void usingLandmines() { m_LandMine--; }

	void installLandmind(double x, double y);
	bool overlap_Landmine(double x, double y);


	void shootFire(double x, double y, Direction dir);
	bool wall_or_exit(double x, double y);
	void FlameObjects(double x, double y);
	void makingPit(double x, double y);
	void flameOfLandmine(double x, double y);
	void SmartZ_OR_DumbZ(double x, double y);
	double dis_p(double x, double y);
	double dis_z(double x, double y);
	bool GapOfAgentandPenelope(double x, double y);
	bool encountableInfectable(double x, double y, Direction& dir);

	void apartingFromZombie(double x, double y, Actor* myself);
	void comingToPenelope(double x, double y, Actor* citizen);
	void ShootFrontWithVomit(double x, double y, Direction dir);
	void doSt_InVomit(double x, double y);
	double Targeting_Human_by_Zombie(double x, double y);
	void comingToHumanByS_Zombie(double x, double y, Actor* sZom);
private:
	// for vector.

	int m_Vaccine, m_Flame, m_LandMine;

	Penelope* myPlayer;
	vector<Actor*> myActors;



	bool end_Stage;

};

#endif // STUDENTWORLD_H_
