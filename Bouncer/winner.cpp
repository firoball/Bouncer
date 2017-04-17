#include "winner.h"

Winner::Winner()
{
	initialized = false;
}

Winner::~Winner()
{
}

void Winner::draw()
{
	timer += env.timeStep;

	dbInk(dbRGB(255, 150 + 105 * abs(sin(timer)), 0), dbRGB(0, 0, 0));
	dbCenterText(sizeX / 2, sizeY - 65, "Press Return key or A button to continue"); 
	dbCenterText(sizeX / 2, sizeY - 40, "Press Escape key or Home button to exit"); 

	fCameraAngleX = dbWrapValue ( 25.0f + 5 * sin(timer * 0.2));
	fCameraAngleY = dbWrapValue ( timer * 10.0f );
	dbXRotateCamera ( fCameraAngleX );
	dbYRotateCamera ( fCameraAngleY );
	dbZRotateCamera ( fCameraAngleZ );
	posX = -dist * dbSIN(fCameraAngleY);
	posY = -dist * dbSIN(fCameraAngleX) - 170;
	posZ = -dist * dbCOS(fCameraAngleY);
	dbPositionCamera (posX,posY,posZ); 
}

void Winner::init()
{
	timer = 0.0f;
	fCameraAngleX = 25.0f;
	fCameraAngleY = 0.0f;
	fCameraAngleZ = 0.0f;
	dist = 40.0f;

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

		dbLoadImage ("welldone.tga", env.image_ctr, 1);
		dbSprite (env.sprite_ctr, sizeX / 2, sizeY / 2, env.image_ctr);
		dbOffsetSprite (env.sprite_ctr, 200, 100);
		sprWinner = env.sprite_ctr;
		env.image_ctr++;
		env.sprite_ctr++;

		dbLoadMusic("win.mp3", env.music_ctr);
		musWinner = env.music_ctr;
		env.music_ctr++;

	}

	dbShowSprite(sprWinner);
	dbLoopMusic(musWinner);
}

void Winner::deInit()
{
	dbHideSprite(sprWinner);
	dbStopMusic(musWinner);
}

//global instance of Winner
Winner winner;