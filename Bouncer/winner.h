#ifndef WINNER_H
#define WINNER_H

#include <DarkGDK.h>
#include "environment.h"

class Winner
{
public:
	Winner();
	~Winner();

	void init();
	void deInit();
	void draw();

private:
	bool initialized;
	float timer;
	int sprWinner;
	int sizeX, sizeY;
	int musWinner;
	float fCameraAngleX;
	float fCameraAngleY;
	float fCameraAngleZ;

	float dist;
	float posX;
	float posY;
	float posZ;
};

extern Winner winner;
#endif