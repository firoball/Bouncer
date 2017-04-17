#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <DarkGDK.h>
#include "environment.h"

class Gameover
{
public:
	Gameover();
	~Gameover();

	void init();
	void deInit();
	void draw();

private:
	bool initialized;
	float timer;
	float timeStep;
	int sprGameOver;
	int sizeX, sizeY;
	int sndGameOver;
};

extern Gameover gameover;
#endif