#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <DarkGDK.h>
#include <newton.h>
#include <OpenGlUtil.h>
#include "gdkwiimote.h"
#include "gdknewton.h"
#include "main.h"

//adjust define for number of Physic objects
#define NEWTON_MAXBODIES 10

class Environment
{
public:
	Environment();
	~Environment();
	void init();
	void cleanUp();

	//Darkbasic instance counters
	int object_ctr;
	int image_ctr;
	int sprite_ctr;
	int mesh_ctr;
	int memblock_ctr;
	int particle_ctr;
	int sound_ctr;
	int music_ctr;

	int syncRate;
	float timeStep;

	//Wiimote
	gdkWiiMote* wm;
	bool wiimote_active;

	//Newton physics
	NewtonWorld* nWorld;
	NewtonBody* body[NEWTON_MAXBODIES];
	//rotated gravity used to simulate rotated world
	dVector rgravity;
	//real gravity
	dVector gravity;

private:
};

extern Environment env;

#endif