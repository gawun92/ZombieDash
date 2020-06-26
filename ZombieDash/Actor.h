#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"
#include "vector"
class StudentWorld;


// Actor is the base class and (Penelope and Wall class) will be derived from it and used.
// Actor is also a derived class from GraphObject.
class Actor : public GraphObject
{
	public:
			// constructor of Actor class
		Actor(StudentWorld* world, int imageID, double startX, double startY, Direction dir, int depth) :
			GraphObject(imageID, startX, startY, right, depth, 1),
			m_world(world), m_passable(false), m_alive(true), m_passable_of_Exit(false), m_flameable(true), m_stepOnMine(false), m_FireOverLap(true), m_Fallinpit(false),  m_vomitable(false), m_infectable(false), m_infected(false)
			{}
			virtual ~Actor() {}
			virtual void doSomething() = 0;	// member function
			StudentWorld* getWorld() const { return m_world; }


			bool isPassable()			{ return m_passable; }
			void setPassable()			{ m_passable = true; }


			bool isAlive()				{ return m_alive;}
			void setDie()				{ m_alive = false; }

			bool passable_of_Exit()		{ return m_passable_of_Exit; }
			void set_exit_passible()	{ m_passable_of_Exit = true; }

			bool flameable() {return m_flameable;}
			void setNotFlameable() { m_flameable = false; }
			bool StepOnLandmine() { return m_stepOnMine; }
			void setStepOnMineTrue() { m_stepOnMine = true; }
			//void getAttack()			{m_infected = true;}



			// This is for wall and exit, this is only for using the fire object motion 
			// it it not related to killing or remove something with fire
			bool flameOverlapable() { return m_FireOverLap; }
			void setNot_flameOverlapable() { m_FireOverLap = false; }


			bool FallInPit() { return m_Fallinpit; }
			void setFallInPit() { m_Fallinpit = true; }



			

			bool Vomitable() { return m_vomitable; }
			void setVomitable() { m_vomitable = true; }

			bool isInfectable() { return m_infectable; }
			void setInfectable() { m_infectable = true; }

			bool isInfected() { return m_infected; }
			void setInfected() { m_infected = true; }
			void infectionCured() { m_infected = false; }
	private:
			StudentWorld* m_world;
			bool m_infected;
			bool m_infectable;
			bool m_vomitable;
			
			bool m_Fallinpit;
			bool m_FireOverLap;
			bool m_stepOnMine;
			bool m_flameable;
			bool m_passable, m_alive, m_passable_of_Exit;

};

class Wall : public Actor
{
	public:
			Wall(StudentWorld* world,double startX, double startY) :
				Actor(world,IID_WALL, startX, startY, right, 0)
			{
				setNot_flameOverlapable();
				setNotFlameable();	
			}
			virtual ~Wall() {}
			virtual void doSomething() {}
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Initially, the depth is 1 and all heading to right side
class ActivatingObject : public Actor
{
public:
	ActivatingObject(StudentWorld* world, int imageID, double startX, double startY, Direction dir,int depth) :
		Actor(world, imageID, startX, startY, dir ,1), m_count(2) {}
	virtual ~ActivatingObject() {};
	virtual void doSomething() {}
	void count_Two() { m_count--; }		// this is for flame
	int checkCount_Two() { return m_count; }
private:
	int m_count;
};
			class Exit : public ActivatingObject
			{
			public:
				Exit(StudentWorld* world, double startX, double startY) :
					ActivatingObject(world, IID_EXIT, startX, startY,right,1)
				{
					setPassable();
					setNotFlameable();
					setNot_flameOverlapable();
				}
				virtual ~Exit() {}
				virtual void doSomething();
			private:
			};

			class Pit : public ActivatingObject   // no die
			{
			public:
				Pit(StudentWorld* world, double startX, double startY) :
					ActivatingObject(world, IID_PIT, startX, startY,right,0)
				{
					setNotFlameable();
					setPassable();
				}
				virtual ~Pit() {}
				virtual void doSomething();
			};

			class Flame : public ActivatingObject	
			{
			public:
				Flame(StudentWorld* world, double startX, double startY, Direction dir) :
					ActivatingObject(world, IID_FLAME, startX, startY, dir, 0)
				{
					setStepOnMineTrue();  // to burn landmine  this is problem since it is penetrating the wall 
					setPassable();
					setNotFlameable();
				}
				virtual ~Flame() {}
				virtual void doSomething();

			private:
		
			};

			class Vomit : public ActivatingObject
			{
			public:
				Vomit(StudentWorld* world, double startX, double startY, Direction dir) :
					ActivatingObject(world, IID_VOMIT, startX, startY, dir,0)
				{
					setPassable();
				}
				virtual ~Vomit() {}
				virtual void doSomething();
			};
			// count is starting from 30 and decreasing
			class Landmine : public ActivatingObject
			{
			public:
				Landmine(StudentWorld* world, double startX, double startY) :
					ActivatingObject(world, IID_LANDMINE, startX, startY,right,1),count(30),m_checker(false), m_active(false)
				{
					setNotFlameable();
					setPassable();
				}
				virtual ~Landmine() {}
				virtual void doSomething();


			private:
				void setActive() { m_active = true; }
				bool isActive() { return m_active; }
				bool m_active;
				void countDown() { count -= 1; }
				int checkTime() { return count; }
				int count;
				bool m_checker;
			};


			class Goodie : public ActivatingObject
			{
			public:
				Goodie(StudentWorld* world, int imageID, double startX, double startY) :
					ActivatingObject(world, imageID, startX, startY,right,1)		
				{
					setPassable();
				}
				virtual ~Goodie() {}
				virtual void doSomething() {}
			};

						class Vaccine_Goodies : public Goodie
						{
						public:
							Vaccine_Goodies(StudentWorld* world, double startX, double startY) :
								Goodie(world, IID_VACCINE_GOODIE, startX, startY)
							{
								
							}
							virtual ~Vaccine_Goodies() {}
							virtual void doSomething();
						};
						class Gas_can_Goodies : public Goodie
						{
						public:
							Gas_can_Goodies(StudentWorld* world, double startX, double startY) :
								Goodie(world, IID_GAS_CAN_GOODIE, startX, startY)
							{
							}
							virtual ~Gas_can_Goodies() {}
							virtual void doSomething();
						};

						class Landmine_Goodies : public Goodie
						{
						public:
							Landmine_Goodies(StudentWorld* world, double startX, double startY) :
								Goodie(world, IID_LANDMINE_GOODIE, startX, startY)
							{
							}
							virtual ~Landmine_Goodies() {}
							virtual void doSomething();
						};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// depth=0   size = 1  dir = right
class Agent : public Actor
{
public:
	Agent(StudentWorld* world, int imageID, double startX, double startY) :
		Actor(world, imageID, startX, startY, right, 0), m_count(1)
	{
		setStepOnMineTrue();// for landmine
		setFallInPit();
	}
	virtual ~Agent() {} 
	virtual void doSomething() {}
	 bool EvenNum() { return (m_count % 2 == 0); }
	 void Incr_count() { m_count += 1; }
private:
	int m_count;
};
			class Human : public Agent
			{
			public:
				Human(StudentWorld* world, int imageID, double startX, double startY) :
					Agent(world, imageID, startX, startY),  m_infection_count(0)
				{
					setInfectable();
				}
				virtual ~Human() {}
				virtual void doSomething() {}
				
				
				void increaseInfection() { m_infection_count += 1; }
				void setNonInfection() { m_infection_count = 0; }
				int  checkInfectioncount() { return m_infection_count; }
			private:
				int  m_infection_count;

			};

						class Penelope : public Human
						{
							public:
								Penelope(StudentWorld* world, double startX, double startY) :
									Human(world, IID_PLAYER, startX, startY)
								{}
								virtual ~Penelope() {}
								virtual void doSomething();
							private:

						};

						class Citizen : public Human
						{
							public:
								Citizen(StudentWorld* world, double startX, double startY) :
									Human(world, IID_CITIZEN, startX, startY)
								{
									set_exit_passible();		// setting true
								}
								virtual ~Citizen() {}
								virtual void doSomething();
								
						};
			class Zombie : public Agent
			{
			public:
				Zombie(StudentWorld* world, double startX, double startY) :
					Agent(world, IID_ZOMBIE, startX, startY)
				{
					setVomitable();
				}
				virtual ~Zombie() {}
				virtual void doSomething();

				void decideDistance();
				void setZeroForDistance() { m_checker = 0; }
				double Distance_Checker() { return m_checker; }
				void   Distance_Counter() { m_checker--; }
				bool MoveWithDir(Direction dir);
				Direction makeDirection();
			private:
				double m_checker;
			};
						class DumbZombie : public Zombie
						{
						public:
							DumbZombie(StudentWorld* world, double startX, double startY) :
								Zombie(world, startX, startY)
							{
							}
							virtual ~DumbZombie() {}
							virtual void doSomething();
						private:
							// nothing
						};

						class SmartZombie : public Zombie
						{
						public:
							SmartZombie(StudentWorld* world, double startX, double startY) :
								Zombie(world, startX, startY)
							{
							}
							virtual ~SmartZombie() {}
							virtual void doSomething();
						private:
							// nothing
						};


#endif // ACTOR_H_
