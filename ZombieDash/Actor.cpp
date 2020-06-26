#include "Actor.h"
#include "StudentWorld.h"





void Penelope::doSomething()
{
	if (!isAlive())
		return;			 // If Penelope died, doSomething() method must return immediately
	if (isInfected())	
	{
		increaseInfection();	// this is allowing increasing the number
		
		if (checkInfectioncount() == 500)
		{
			setDie();
			getWorld()->playSound(SOUND_PLAYER_DIE);	
			getWorld()->decLives();
			return;		// If Penelope died, doSomething() method must return immediately
		}
	}
	// from graphicobject
 	double x = getX();	
 	double y = getY();
 	int key;
 	// from GameWorld.h (StudentWorld's base class)
	if (getWorld()->getKey(key)) 		// if pressing the key
	 switch(key)
	 {
		 // setDirection : allowing a user to change the direction
		 // (whether she can move or not)  as long as there is key hit, she would change direction.
		 // isBlocking	 : checking out whether Penelope can go not(checking out wall or other things later)
		 //					this is from StudentWorld.
	    case KEY_PRESS_SPACE:
			if (getWorld()->getNumFlameCharges() > 0)
			{
				int tempDir = getDirection();
				getWorld()->usingFlame();		// decrease 1 in total frame
				getWorld()->playSound(SOUND_PLAYER_FIRE); 
				getWorld()->shootFire(x, y, tempDir);		// it is supposed to be working but not!
			}
			break;

 		case KEY_PRESS_RIGHT:
			setDirection(right);						
			if (!getWorld()->isBlocking(x + 4, y, this))		
				moveTo(x + 4, y);
 			break;
 		case KEY_PRESS_LEFT:
			setDirection(left);
			if (!getWorld()->isBlocking(x - 4, y, this))
				moveTo(x - 4, y);
 			break;
 		case KEY_PRESS_UP:
			setDirection(up);
			if (!getWorld()->isBlocking(x, y + 4, this))
				moveTo(x, y + 4);
			break;
 		case KEY_PRESS_DOWN:
			setDirection(down);
			if (!getWorld()->isBlocking(x, y - 4, this))
				moveTo(x, y - 4);
 			break;
		case KEY_PRESS_ENTER:
			if (getWorld()->getNumVaccines() >= 1)
			{
				setNonInfection();
				getWorld()->usingVaccine();
				infectionCured();
			}
			break;
		case KEY_PRESS_TAB:
			if (getWorld()->getNumLandmines() >= 1)
			{
				getWorld()->usingLandmines();
				getWorld()->installLandmind(x,y);
			}

	 }	
}



void Exit::doSomething()
{
	if(getWorld()->goExit(getX(), getY()))
		getWorld()->increaseScore(200);
}

void Vaccine_Goodies::doSomething()
{
	if (!isAlive())
		return;
	if (getWorld()->getGoodies(getX(), getY()))	//checking out overlapping with Penelope
	{
		getWorld()->increaseScore(50);
		setDie();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseVaccines();
	}
}

void Gas_can_Goodies::doSomething()
{
	if (!isAlive())
		return;
	if (getWorld()->getGoodies(getX(), getY()))	//checking out overlapping with Penelope
	{
		getWorld()->increaseScore(50);
		setDie();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseFlameCharges();

	}
}

void Flame::doSomething()
{
	if (!isAlive())
		return;
	if (checkCount_Two() == 0)	//flame is remaining for two ticks
	{
		setDie();		// make falme die and delete it
		return;
	}
	else
	{
	count_Two();		// 2 --  
	getWorld()->FlameObjects(getX(), getY());
	}
}


void Landmine_Goodies::doSomething()
{
	if (!isAlive())
		return;
	if (getWorld()->getGoodies(getX(), getY()))	//checking out overlapping with Penelope
	{
		getWorld()->increaseScore(50);	// decrease score
		setDie();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseLandmines();
	}
}


void Landmine::doSomething()
{
	if (!isAlive())
		return;
	if (checkTime() == 0)
		setActive();
	if (!isActive()) // start from 30
		countDown();

	if (isActive()) // only works when landmine is activated
	{
		if (getWorld()->overlap_Landmine(getX(), getY())) // when overlaping
		{
			setDie();
			getWorld()-> playSound(SOUND_LANDMINE_EXPLODE);
			getWorld()-> flameOfLandmine(getX(), getY());

		}
	}
}


void Pit::doSomething()
{
	(getWorld()->overlap_Landmine(getX(), getY()));  // if step on it, it(human, zombie, or penelope) dies
}

void Citizen::doSomething()
{
	if (!isAlive())
	{
		return;
	}
	if (isInfected())
	{
		increaseInfection();	// this is allowing increasing the number

		if (checkInfectioncount() == 500)
		{
			setDie();
			getWorld()->playSound(SOUND_ZOMBIE_BORN);
			getWorld()->increaseScore(-1000);
			getWorld()->SmartZ_OR_DumbZ(getX(),getY());  //70% dumb  zombie    30% smart Zombie
			return;
		}
	}
	if (EvenNum() && isAlive())
	{
		

		int temp = getWorld()->dis_z(getX(), getY());   // temp=1 means there is no Zombie
		if (getWorld()->dis_p(getX(), getY()) < getWorld()->dis_z(getX(), getY()) ||
			temp == 1)
		{
			getWorld()->comingToPenelope(getX(), getY(), this);    //coming to Penelope
		}
		else if(getWorld()->dis_p(getX(), getY()) > getWorld()->dis_z(getX(), getY()) )
		{
			getWorld()->apartingFromZombie(getX(), getY(), this);  // be apart from Zombie
		}
		else
		{
			Incr_count();
			return;
		}
	}
	Incr_count();
}



////////////////


void Vomit::doSomething()
{
	if (!isAlive())
		return;
	else// isAlive()
	{
		if (checkCount_Two() == 0)		//vomit object deleted after 2
		{
			setDie();
			return;
		}
		else
		{
			getWorld()->doSt_InVomit(getX(), getY());
			count_Two();
		}
	}
	return;
}



void Zombie::decideDistance()
{
	m_checker = randInt(3, 10);
}


Direction Zombie::makeDirection()
{
	return (90 * (randInt(0, 3)));
}


bool Zombie::MoveWithDir(Direction dir)
{
	if (dir == right)
	{
		if (!getWorld()->isBlocking(getX() + 1, getY(), this))
		{
			moveTo(getX() + 1, getY());
			Distance_Counter();
			return true;
		}
	}
	else if (dir == up)
	{
		if (!getWorld()->isBlocking(getX(), getY() + 1, this))
		{
			moveTo(getX(), getY() + 1);
			Distance_Counter();
			return true;
		}
	}
	else if (dir == left)
	{
		if (!getWorld()->isBlocking(getX() - 1, getY(), this))
		{
			moveTo(getX() - 1, getY());
			Distance_Counter();
			return true;
		}
	}
	else
	{
		if (!getWorld()->isBlocking(getX(), getY() - 1, this))
		{
			moveTo(getX(), getY() - 1);
			Distance_Counter();
			return true;
		}
	}
	return false;
}

// vomiting
void Zombie::doSomething(){}


void DumbZombie::doSomething()
{
	if (!isAlive())
		return;
	else
	{
		if (EvenNum())
		{
			if (Distance_Checker() <= 0)
				decideDistance();
			getWorld()->ShootFrontWithVomit(getX(), getY(), getDirection());
			Direction dir = makeDirection();
			setDirection(dir);
			if (!MoveWithDir(dir))	// if cannot go
			{
				setZeroForDistance();	// set the distance plan to zero
			}
		}
		Incr_count();
	}
	
	return;
}

void SmartZombie::doSomething()
{
	if (!isAlive())
		return;
	else
	{
		if (EvenNum())
		{
			if (Distance_Checker() <= 0)
				decideDistance();
			getWorld()->ShootFrontWithVomit(getX(), getY(), getDirection());
			Direction dir;
			
			if(getWorld()->Targeting_Human_by_Zombie(getX(), getY()) > 80)
			{ 
				dir = makeDirection();
				setDirection(dir);
				if (!MoveWithDir(dir))	// if cannot go
				{
					setZeroForDistance();	// set the distance plan to zero
				}
			}
			else
			{
				getWorld()->comingToHumanByS_Zombie(getX(), getY(), this);
			}
			
			

		}
	}
	Incr_count();
	return;
}
