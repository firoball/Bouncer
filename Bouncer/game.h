#ifndef GAME_H
#define GAME_H

#include <DarkGDK.h>
#include <newton.h>
#include <OpenGlUtil.h>

#include "environment.h"
#include "hud.h"
#include "gdknewton.h"
#include "gdkwiimote.h"
#include "mdl5loader.h"
#include "item.h"

//#define DEBUG

#define BOOST_FAC 20.0f
#define PLAYER_MASS 40
#define NUM_ENVOBJECTS 7
#define INDEX_ITEM 6
#define INDEX_TREE 4

class Game
{
public:
	Game();
	~Game();

	void init();
	void deInit();
	void loop();
	bool died();
	bool won();

private:
	void moveCamera();
	void attachCamera(int ctr, float tilt, float roll);
	void setOrientation();
	void applyImpulse(int obj, dVector *vel);
	void checkDeath();
	void checkWinner();
	void checkBooster();
	void loadObjects();

#ifdef DEBUG
public:	//ugly - required for adding items to list -- item.cpp
	int obj[1000][3];	//should be enough
	float obj2[1000][3];
	int objSelector;
	int objCreated;
	int objCtrOffs;
private:
	void placeObjects();
#endif

	float fCameraAngleX;
	float fCameraAngleY;
	int oPlayer;
	int oSkybox;
	int oLevel;
	int oEnv[NUM_ENVOBJECTS];
	int oScale[NUM_ENVOBJECTS];
	int oTree;
	int sndDie;
	int sndWin;
	int sndBoost;
	int musGame;
	bool initialized;
	bool dead;	//player died
	bool deadTimeout;	//timeout after death occured
	bool winner;	//player won the game
	bool winnerTimeout;	//timeout after winning occured
	float delayTimer; //restart delay after timeout/death
	int boostTemp;
	float boostTimer;
	
	//temp vars for level orientation
	float pan, tilt, roll;

	//temp vars for Wiimote orientation calculation
	dVector wmAng;	//current Wiimote angle
	dMatrix matrix;	//matrix for rotation operations
	dVector wmG;	//Wiimote 'g' Vector
	dVector wmFrc;	//Wiimote force Vector
	float wX, wY, wZ;	//'g' corrected force values
};

extern Game game;
#endif