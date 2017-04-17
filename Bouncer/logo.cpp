#include "logo.h"

Logo::Logo()
{
	initialized = false;
}

Logo::~Logo()
{
}

void Logo::init()
{
	timer = 0.0f;
	timeStep = (1.0f / env.syncRate);
	finished = false;
	step = 0;

	if (!initialized)
	{
		initialized = true;

		//get screen size info
		int posX = dbScreenWidth() / 2;
		int posY = dbScreenHeight() / 2;

		dbLoadImage ("dragon.bmp", env.image_ctr, 1);
		dbSprite (env.sprite_ctr, posX, posY, env.image_ctr);
		dbOffsetSprite (env.sprite_ctr, 128, 128);
		sprDragon = env.sprite_ctr;
		env.image_ctr++;
		env.sprite_ctr++;

		dbLoadImage ("newton.bmp", env.image_ctr, 1);
		dbSprite (env.sprite_ctr, posX, posY, env.image_ctr);
		dbOffsetSprite (env.sprite_ctr, 128, 128);
		dbScaleSprite (env.sprite_ctr, 0);
		sprNewton = env.sprite_ctr;
		env.image_ctr++;
		env.sprite_ctr++;

		dbLoadSound ("dragon.wav", env.sound_ctr);
		sndDragon = env.sound_ctr;
		env.sound_ctr++;

		dbLoadSound ("newton.wav", env.sound_ctr);
		sndNewton = env.sound_ctr;
		env.sound_ctr++;

	}
	dbHideSprite(sprNewton);
	dbShowSprite(sprDragon);

	dbSetTextFont("Arial");
	dbSetTextSize(18); 
	dbSetTextTransparent();
	dbSetTextToBold(); 

}

void Logo::deInit()
{
	dbHideSprite(sprDragon);
	dbHideSprite(sprNewton);
}

void Logo::draw()
{
	static float fTemp;
	dbInk(dbRGB(240,209,67), dbRGB(0,0,0));
	dbBox (0, 0, dbScreenWidth(), dbScreenHeight());
	if (!env.wiimote_active)
	{
		dbInk(dbRGB(209,47,14), dbRGB(0,0,0));
		dbCenterText(dbScreenWidth() / 2, dbScreenHeight() - 50, 
		"WARNING: In order to fully enjoy this game, a Wiimote is required!");
		dbCenterText(dbScreenWidth() / 2, dbScreenHeight() - 30, 
		"Please connect Wiimote and restart game."); 
	}

	switch (step)
	{
	case 0:	//rotating dragon
		fTemp = __min(timer * 40, 100);
		dbScaleSprite(sprDragon, fTemp);

		fTemp = __min(timer * 100, 255);
		dbSetSpriteAlpha(sprDragon, fTemp);

		fTemp = timer * 360;
		if (timer >= 3.0f)
		{
			dbRotateSprite(sprDragon, 0);
			dbPlaySound(sndDragon);
			lastTime = timer;
			step++;
		}
		else
			dbRotateSprite(sprDragon, fTemp);
		break;

	case 1: //wait 1.5 seconds
		if (timer >= lastTime + 1.0f)
		{
			lastTime = timer;
			step++;
		}
		break;

	case 2:	//zoom in and fade out dragon quickly
		fTemp = __min(100 + (timer - lastTime) * 200, 400);
		dbScaleSprite(sprDragon, fTemp);

		fTemp = __min((timer - lastTime) * 200, 255);
		dbSetSpriteAlpha(sprDragon, 255 - fTemp);
		if (timer >= lastTime + 1.6f)
		{
			dbHideSprite(sprDragon);
			dbShowSprite(sprNewton);
			lastTime = timer;
			step++;
		}
		break;

	case 3:	//zoom in Newton logo
		fTemp = __min((timer - lastTime) * 80, 100);
		dbScaleSprite(sprNewton, fTemp);

		fTemp = __min((timer - lastTime) * 200, 255);
		dbSetSpriteAlpha(sprNewton, fTemp);
		if (timer >= lastTime + 1.5f)
		{
			dbPlaySound(sndNewton);
			lastTime = timer;
			step++;
		}
		break;

	case 4: //zoom out Newton logo
		fTemp = __min((timer - lastTime) * 80, 100);
		dbScaleSprite(sprNewton, 100 - fTemp);

		fTemp = __min((timer - lastTime) * 200, 255);
		dbSetSpriteAlpha(sprNewton, 255 - fTemp);
		if (timer >= lastTime + 2.0f)
		{
			finished = true;
			step++;
		}
		break;

	default:
		//do nothing
		break;
	}
	timer += timeStep;
}

bool Logo::isFinished()
{
	return finished;
}

//global instance of Logo
Logo logo;