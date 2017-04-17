#ifndef LOGO_H
#define LOGO_H

#include <DarkGDK.h>
#include "environment.h"

class Logo
{
public:
	Logo();
	~Logo();

	void init();
	void deInit();
	void draw();
	bool isFinished();

private:
	bool initialized;
	bool finished;
	int sprDragon;
	int sprNewton;
	int sndDragon;
	int sndNewton;
	int step;
	float timer;
	float lastTime;
	float timeStep;
};

extern Logo logo;

#endif