#include "gameover.h"

Gameover::Gameover()
{
	initialized = false;
}

Gameover::~Gameover()
{
}

void Gameover::draw()
{
	timer += timeStep;

	dbInk(dbRGB(255, 150 + 105 * abs(sin(timer)), 0), dbRGB(0, 0, 0));
	dbCenterText(sizeX / 2, sizeY - 65, "Press Return key or A button to continue"); 
	dbCenterText(sizeX / 2, sizeY - 40, "Press Escape key or Home button to exit"); 

}

void Gameover::init()
{
	timer = 0.0f;
	timeStep = (1.0f / env.syncRate);

	//update screen size info
	sizeX = dbScreenWidth();
	sizeY = dbScreenHeight();

	//update text settings
	dbSetTextFont("Arial");
	dbSetTextSize(20); 
	dbSetTextTransparent();
	dbSetTextToBold(); 

	if (!initialized)
	{
		initialized = true;

		dbLoadImage ("gameover.tga", env.image_ctr, 1);
		dbSprite (env.sprite_ctr, sizeX / 2, sizeY / 2, env.image_ctr);
		dbOffsetSprite (env.sprite_ctr, 200, 100);
		sprGameOver = env.sprite_ctr;
		env.image_ctr++;
		env.sprite_ctr++;

		dbLoadSound ("gameover.wav", env.sound_ctr);
		sndGameOver = env.sound_ctr;
		env.sound_ctr++;

	}

	dbShowSprite(sprGameOver);
	dbPlaySound(sndGameOver);
}

void Gameover::deInit()
{
	dbHideSprite(sprGameOver);
}

//global instance of Gameover
Gameover gameover;